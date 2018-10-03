/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="http://bbserver.ficosa.com:27990/projects/FAL/repos/
*mmw013_datadownloadmng/browse/applicableDocuments/requirements/
*MMW013_DataDownloadMng_20161215.doc">MMW013_DataDownloadMng</a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains DataDownloadMng supporting function definitions
*           and internal global variables.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  19/12/16 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMngFunctions.c
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

/* -------------------------------- Includes -------------------------------- */
/* LEGATO Dependencies */
#include "legato.h"
#include "le_thread.h"
#include "le_eventLoop.h"
#include "le_mutex.h"

/* Local Component Dependencies */
#include "DataDownloadMngFunctions.h"
#include "DataDownloadMngDbUtil.h"
#include "fi_mdlw_intercomp.h"
#include "acp245_alarm_notification.h"
#include "acprn_source_destination.h"
#include "acprn_application_header.h"
#include "acprn_remote_vehicle_function.h"
#include "acprn_service_provisioning_function.h"
#include "acprn_probe_application_data.h"

/* ------------------------------ Defines ----------------------------------- */

/**
 * GET_LIST_ITEM macro is defined to get item from legato doubly linked list
 * where le_dls_Link_t member name should be list.
 */
#define DATADOWNLOADMNG_GET_LIST_ITEM(link_address, node_structure_name)  ({   \
    node_structure_name *item = NULL;                                          \
    item = ((link_address) != NULL) ? ((node_structure_name *)CONTAINER_OF(    \
           (link_address), node_structure_name,list)) : NULL;                  \
    item;})

/**
 * DATADOWNLOADMNG_PERSISTENT_MEMORY_DIR macro is defined for a directory path
 * to save the any data related to DataDownloadMng in persistent (non-volatile)
 * memory.
 */
#define DATADOWNLOADMNG_PERSISTENT_MEMORY_DIR  ((UI_8 *)"/data/DataDownloadMng")

/**
 * DATADOWNLOADMNG_DATA_BACKUP_FILE macro is defined for file path to save the
 * data backup of DataDownloadMng in persistent (non-volatile) memory.
 */
#define DATADOWNLOADMNG_DATA_BACKUP_FILE   \
                                    ((UI_8 *)"/data/DataDownloadMng/dbkup.tcu")

/**
 * DATADOWNLOADMNG_NAMED_DATA_TABLE macro is defined to specify the name of the
 * table in which to store received raw data in persistent memory.
 */
#define DATADOWNLOADMNG_NAMED_DATA_TABLE       ((UI_8 *)"named_data")

/**
 * DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE macro is defined to specify the name
 * of the table in which to backup the data of download list in persistent
 * memory.
 */
#define DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE  ((UI_8 *)"downloadlist")

/**
 * DATADOWNLOADMNG_TASK_LIST_DATA_TABLE macro is defined to specify the name of
 * the table in which to backup the data of task list in persistent memory.
 */
#define DATADOWNLOADMNG_TASK_LIST_DATA_TABLE      ((UI_8 *)"tasklist")

/**
 * DATADOWNLOADMNG_CONF_LIST_DATA_TABLE macro is defined to specify the name of
 * the table in which to backup the configuration installation data list
 * in persistent memory.
 */
#define DATADOWNLOADMNG_CONF_LIST_DATA_TABLE      ((UI_8 *)"conflist")

/**
 * DATADOWNLOADMNG_DOWNLOADED_DATA_TABLE macro is defined to specify the name of
 * the table in which to backup the downloaded configuration data list
 * in persistent memory.
 */
#define DATADOWNLOADMNG_DOWNLOADED_DATA_TABLE      ((UI_8 *)"downloadeddata")


/**
 * DATADOWNLOADMNG_TASK_LIST_DATA_TABLE macro is defined to specify the name of
 * the table in which to backup the data of task list in persistent memory.
 */
#define DATADOWNLOADMNG_SENT_ITEM_DATA_TABLE      ((UI_8 *)"sentitem")

/**
 * DATADOWNLOADMNG_DOWNLOAD_LIST_MAX_SIZE macro is defined to specify the
 * maximum no. of records in a Download List.
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_89
 */
#define DATADOWNLOADMNG_DOWNLOAD_LIST_MAX_SIZE   ((UI_8)(15))

/**
 * DATADOWNLOADMNG_TASK_LIST_MAX_SIZE macro is defined to specify the maximum
 * no. of records in a SMS TASK List.
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_139
 */
#define DATADOWNLOADMNG_TASK_LIST_MAX_SIZE   ((UI_8)(15))


/**
 * DATADOWNLOADMNG_CONF_LIST_MAX_SIZE macro is defined to specify the maximum
 * no. of records in a configuration installation List.
 */
#define DATADOWNLOADMNG_CONF_LIST_MAX_SIZE   ((UI_8)(100))

/**
 * DATADOWNLOADMNG_INIT_DATA_ROUTE_INFO  macro is defined to specify the
 * initialize value for data route info structure.
 */
#define DATADOWNLOADMNG_DATA_ROUTE_INFO_INIT    \
 {DATADOWNLOADMNG_ACP245, {FALSE, (UI_32)0}, (UI_8)0, (UI_8)0}


/**
 * DATADOWNLOADMNG_DOWNLOAD_LIST_MAX_SIZE macro is defined to specify the
 * maximum no. of nodes in a Priority List.
 */
#define DATADOWNLOADMNG_PRIORITY_LIST_MAX_SIZE   ((UI_8)(35))


/* ----------------------------- Data Types --------------------------------- */

/**
 * This is a user defined data type which desribes the sequential
 * executable task priorities (SETP) value range.
 */
typedef enum {
    SETP_INSTALL_CONFIG_FILE = 1, ///< Config file installation priority(Highest)
    SETP_SMS_TASK_LIST,           ///< SMS Task List's task execution priority
    SETP_SMS_SENDING,             ///< SMS Sending Priority
    SETP_DATA_UPLOAD_PRU0,        ///< Data block upload priority PRU0
    SETP_DATA_UPLOAD_PRU1,        ///< Data block upload priority PRU1
    SETP_DATA_UPLOAD_PRU2,        ///< Data block upload priority PRU2
    SETP_DATA_UPLOAD_PRU3,        ///< Data block upload priority PRU3
    SETP_DATA_UPLOAD_PRU4,        ///< Data block upload priority PRU4
    SETP_DATA_UPLOAD_PRU5,        ///< Data block upload priority PRU5
    SETP_DATA_UPLOAD_PRU6,        ///< Data block upload priority PRU6
    SETP_DATA_UPLOAD_PRU7,        ///< Data block upload priority PRU7
    SETP_DATA_UPLOAD_PRU8,        ///< Data block upload priority PRU8
    SETP_DATA_UPLOAD_PRU9,        ///< Data block upload priority PRU9
    SETP_DOWNLOAD_FILE,           ///< File Download Priority
    SETP_DATA_UPLOAD_PRU10,       ///< Data block upload priority PRU10
    SETP_DATA_UPLOAD_PRU11,       ///< Data block upload priority PRU11
    SETP_DATA_UPLOAD_PRU12,       ///< Data block upload priority PRU12
    SETP_DATA_UPLOAD_PRU13,       ///< Data block upload priority PRU13
    SETP_DATA_UPLOAD_PRU14,       ///< Data block upload priority PRU14
    SETP_DATA_UPLOAD_PRU15,       ///< Data block upload priority PRU15
    SETP_DATA_UPLOAD_PRU16,       ///< Data block upload priority PRU16
    SETP_DATA_UPLOAD_PRU17,       ///< Data block upload priority PRU17
    SETP_DATA_UPLOAD_PRU18,       ///< Data block upload priority PRU18
    SETP_DATA_UPLOAD_PRU19,       ///< Data block upload priority PRU19
    SETP_DATA_UPLOAD_PRU20,       ///< Data block upload priority PRU20
    SETP_DATA_UPLOAD_PRU21,       ///< Data block upload priority PRU21
    SETP_DATA_UPLOAD_PRU22,       ///< Data block upload priority PRU22
    SETP_DATA_UPLOAD_PRU23,       ///< Data block upload priority PRU23
    SETP_DATA_UPLOAD_PRU24,       ///< Data block upload priority PRU24
    SETP_DATA_UPLOAD_PRU25,       ///< Data block upload priority PRU25
    SETP_DATA_UPLOAD_PRU26,       ///< Data block upload priority PRU26
    SETP_DATA_UPLOAD_PRU27,       ///< Data block upload priority PRU27
    SETP_DATA_UPLOAD_PRU28,       ///< Data block upload priority PRU28
    SETP_DATA_UPLOAD_PRU29,       ///< Data block upload priority PRU29
    SETP_DATA_UPLOAD_PRU30,       ///< Data block upload priority PRU30
    SETP_LOWEST_PRIORITY = 0xff   ///< Lowest Priority
} t_ddm_task_priority;


/**
 * This is a user defined data type which describes the type of incoming data
 * packet format.
 */
typedef enum
{
    DATADOWNLOADMNG_ACP245 = 0,         ///< ACP standard format(ACP245).
    DATADOWNLOADMNG_ACPRN  = 1          ///< ACP extended format(ACPRN).
} t_ddm_acp_type;

/**
 * This is a user defined data type which describes the
 * route information of data to deliver to appropriate service and process.
 */
typedef struct
{
    t_ddm_acp_type acp_type;             ///< ACP frame type.
    t_acp_application_session_id session; ///< Host-client session id.
    UI_8 app_id;                         ///< App ID for ACP245 data.
    UI_8 message_type;                   ///< Data message type.
} t_ddm_data_route_info;

/**
 * This is a user defined data type which describes a sequential
 * executable tasks priority(SETP) List (doubly linked list) item.
 */
typedef struct
{
    t_ddm_task_priority priority;      ///< pending task priority
    le_dls_Link_t list;                ///< Doubly link list to link nodes
} t_ddm_setp_list_item;


/* ---------------------------- Global Variables ---------------------------- */

static UI_8 datadownload_data_hndl_buf[4 * DATADOWNLOADMNG_DATABLOCK_MAX_LEN] = {0};

/**
 * A static variable for mdlwServices supported data packet to communicate with
 * mdlwServices.
 */
static t_mdlw_msg datadownload_mdlw_msg = {0};

/**
 * A static variable for a database utility object to perform persistent memory
 * storage task.
 */
static t_ddm_util_db datadownload_backup_db = DATADOWNLOADMNG_DB_UTIL_INIT;

/**
 * A global variable which shows the status of pending installation of new
 * configuration files.
 * @a Values:
 *  - True: Pending config file installation tasks are available.
 *  - False: No any pending config file installation task is available.
 */
static BOOL datadownload_conf_instl_flag = FALSE;

/**
 * A global variable for mutex lock to protect the
 * "datadownload_conf_instl_flag" updation, from race condition.
 */
static le_mutex_Ref_t datadownload_conf_instl_lck_ref = NULL;


/**
 * A global variable which shows the status of pending download request in
 * a Download List.
 * @a Values:
 *  - True: Pending download requests are available in Download List.
 *  - False: No any pending download request is available in Download List.
 */
static BOOL datadownload_dwnld_rqst_flag = FALSE;

/**
 * A static variable for mutex lock to protect the download request flag
 * variable updation from race condition.
 */
static le_mutex_Ref_t datadownload_dwnld_rqst_lck_ref = NULL;

/**
 * A global variable which shows the status of pending action request in
 * a SMS Task List.
 * @a Values:
 *  - True: Pending action requests are available in SMS Task List.
 *  - False: No any pending action request is available in SMS Task List.
 */
static BOOL datadownload_actn_rqst_flag = FALSE;

/**
 * A static variable for mutex lock to protect the action request flag
 * variable updation from race condition.
 */
static le_mutex_Ref_t datadownload_actn_rqst_lck_ref = NULL;

/**
 * A static variable for mutex lock to protect the sequential executable task
 * priority List updation from race condition.
 */
static le_mutex_Ref_t datadownload_setp_list_lck_ref = NULL;

/**
 * A static variable for memory pool reference which is allocated for
 * Sequential executable task priority(SETP) List.
 */
static le_mem_PoolRef_t datadownload_setp_list_pl_ref = NULL;

/**
 * A static variable for Sequential executable task priority(SETP) List doubly
 * linked list object with initialization to use legato doubly linked list API
 * like insert, add, delete, push and pop operation.
 */
static le_dls_List_t datadownload_setp_list = LE_DLS_LIST_INIT;


#if 0
/**
 * A static variable for local data buffer for DataDownloadMng to send the
 * data block.
 */
static t_ddm_datablock datadownload_snd_buf = DATADOWNLOADMNG_DATABLOCK_INIT;
#endif
/**
 * A static variable for mutex lock to protect the datadownload_snd_buf
 * updation from race condition.
 */
static le_mutex_Ref_t datadownload_snd_buf_lck_ref = NULL;

/**
 * A static variable for reference of thread required for monitoring the events
 * reported by any function of DataDownloadMng.
 */
static le_thread_Ref_t datadownload_evnts_thread_ref = NULL;

/**
 * A static variable for declaring legato event id to capture the event
 * corresponding to assign value in this variable.
 */
static le_event_Id_t datadownload_rcv_data_evnt_id = NULL;

/* --------------------------- Routine prototypes --------------------------- */


/* -----------------------------Static Routines ----------------------------- */


/**
 * @private Set status of pending installation of config file.
 * @par Operations contract:
 *  This function sets the status of pending installation of config file as
 *  TRUE or FALSE.
 *
 * @param[in] pending_status Pending status to be set (TRUE/FALSE).
 *
 * @return
 *      None
 */
static void DataDownloadMngSetConfInstlFlag (BOOL pending_status)
{
    DATADOWNLOADMNG_LOCK(datadownload_conf_instl_lck_ref);
    datadownload_conf_instl_flag = pending_status;
    DATADOWNLOADMNG_UNLOCK(datadownload_conf_instl_lck_ref);
} /* DataDownloadMngSetConfInstlFlag  */

/**
 * @private Get status of pending installation of config file.
 * @par Operations contract:
 *  This function returns the status of pending installation of config file.
 *
 * @return
 *  - TRUE: At least one pending installation of config file.
 *  - FALSE: No any installation of config file is pending.
 */
static BOOL DataDownloadMngGetConfInstlFlag (void)
{
    BOOL ret_val = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_conf_instl_lck_ref);
    ret_val = datadownload_conf_instl_flag;
    DATADOWNLOADMNG_UNLOCK(datadownload_conf_instl_lck_ref);

    return ret_val;
} /* DataDownloadMngGetConfInstlFlag */

/**
 * @private Set status of pending download request.
 * @par Operations contract:
 *  This function sets the status of pending download request as TRUE or FALSE.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_130
 *  - REU_MMW013_DDM_66
 *  - REU_MMW013_DDM_77
 *
 * @param[in] pending_status Pending status to be set (TRUE/FALSE).
 *
 * @return
 *      None
 */
static void DataDownloadMngSetDwnldRqstFlag (BOOL pending_status)
{
    DATADOWNLOADMNG_LOCK(datadownload_dwnld_rqst_lck_ref);
    datadownload_dwnld_rqst_flag = pending_status;
    DATADOWNLOADMNG_UNLOCK(datadownload_dwnld_rqst_lck_ref);
}


/**
 * @private Get status of pending download request.
 * @par Operations contract:
 *  This function returns the status of pending download request.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_130
 *  - REU_MMW013_DDM_66
 *  - REU_MMW013_DDM_77
 *
 * @return
 *  - TRUE: At least one download requests pending.
 *  - FALSE: No any download request is pending.
 */
static BOOL DataDownloadMngGetDwnldRqstFlag (void)
{
    BOOL ret_val = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_dwnld_rqst_lck_ref);
    ret_val = datadownload_dwnld_rqst_flag;
    DATADOWNLOADMNG_UNLOCK(datadownload_dwnld_rqst_lck_ref);

    return ret_val;
}


/**
 * @private Set status of pending action request.
 * @par Operations contract:
 *  This function sets the status of pending action request as TRUE or FALSE.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_141
 *  - REU_MMW013_DDM_142
 *  - REU_MMW013_DDM_143
 *
 * @param[in] pending_status Pending status to be set (TRUE/FALSE).
 *
 * @return
 *      None
 */
static void DataDownloadMngSetActnRqstFlag (BOOL pending_status)
{
    DATADOWNLOADMNG_LOCK(datadownload_actn_rqst_lck_ref);
    datadownload_actn_rqst_flag = pending_status;
    DATADOWNLOADMNG_UNLOCK(datadownload_actn_rqst_lck_ref);
}


/**
 * @private Get status of pending action request.
 * @par Operations contract:
 *  This function returns the status of pending action request.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_141
 *  - REU_MMW013_DDM_142
 *  - REU_MMW013_DDM_143
 *
 * @return
 *  - TRUE: At least one action requests pending.
 *  - FALSE: No any action request is pending.
 */
static BOOL DataDownloadMngGetActnRqstFlag (void)
{
    BOOL ret_val = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_actn_rqst_lck_ref);
    ret_val = datadownload_actn_rqst_flag;
    DATADOWNLOADMNG_UNLOCK(datadownload_actn_rqst_lck_ref);

    return ret_val;
}


/**
 * @private Insert data block to SETP List.
 * @par Operations contract:
 *  This function inserts new priority to SETP List in sorted order.
 *
 * @param[in] priority  Verticle priority of sequential executable task·
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngInsertToSetpList (t_ddm_task_priority priority)
{
    t_ddm_setp_list_item *new_item = NULL, *current_item = NULL;
    le_dls_Link_t *current_link = NULL;
    SI_32 status = DATADOWNLOADMNG_FAILURE;
    BOOL loop_break = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_setp_list_lck_ref);
    if (le_dls_NumLinks(&datadownload_setp_list)
            < DATADOWNLOADMNG_PRIORITY_LIST_MAX_SIZE) {

        new_item = le_mem_ForceAlloc(datadownload_setp_list_pl_ref);
        if (new_item != NULL) {
            new_item->priority = priority;
            new_item->list = LE_DLS_LINK_INIT;
            current_link = le_dls_Peek(&datadownload_setp_list);
            if (!current_link) {
                le_dls_Queue(&datadownload_setp_list, &(new_item->list));
            }
            else {
                /* Sort according to priority */
                loop_break = FALSE;
                do {
                    current_item = DATADOWNLOADMNG_GET_LIST_ITEM(current_link,
                            t_ddm_setp_list_item);
                    if (priority < current_item->priority) {
                        le_dls_AddBefore(&datadownload_setp_list, current_link,
                                &(new_item->list));
                        loop_break = TRUE;
                    }
                    else if (priority == current_item->priority) {
                        loop_break = TRUE;
                    }
                    else {
                        current_link = le_dls_PeekNext(&datadownload_setp_list,
                                current_link);
                        loop_break = (current_link == NULL) ? TRUE : FALSE;
                    }
                } while (loop_break != TRUE);
                if (current_link == NULL) {
                    le_dls_Queue(&datadownload_setp_list, &(new_item->list));
                }
            }
            status = DATADOWNLOADMNG_SUCCESS;
        }
        else {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Priority List memory pool"
                    " is out of memory!!\n");
        }
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Priority List is full!!\n");
    }
    DATADOWNLOADMNG_UNLOCK(datadownload_setp_list_lck_ref);

    return status;
} /* DataDownloadMngInsertToSetpList */



/**
 * @private Get priority from SETP List.
 * @par Operations contract:
 *  This function gives a pointer to first priority in SETP List.
 *
 * @param[out] return_priority_ref  Pointer of a priority variable to store
 *                                  priority.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngGetFromSetpList (t_ddm_task_priority *return_priority_ref)
{
    t_ddm_setp_list_item *listobj = NULL;
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if (return_priority_ref == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Return priority is assign to NULL pointer!!\n");
        status = DATADOWNLOADMNG_FAILURE;
    }
    else {
        DATADOWNLOADMNG_LOCK(datadownload_setp_list_lck_ref);
        if (le_dls_IsEmpty(&datadownload_setp_list) == FALSE) {
            listobj = DATADOWNLOADMNG_GET_LIST_ITEM(le_dls_Peek(
                        &datadownload_setp_list), t_ddm_setp_list_item);
        }
        DATADOWNLOADMNG_UNLOCK(datadownload_setp_list_lck_ref);

        if ((SI_64)listobj <= 0) {
            *return_priority_ref = SETP_LOWEST_PRIORITY;
        }
        else {
            *return_priority_ref = listobj->priority;
        }
    }
    return status;
} /* DataDownloadMngGetFromSetpList */


/**
 * @private Delete priority from SETP List.
 * @par Operations contract:
 *  This function removes specified priority from Sequential Executable Task
 *  Priority List.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngDelFromSetpList (t_ddm_task_priority priority)
{
    t_ddm_setp_list_item *current_item = NULL;
    le_dls_Link_t *current_link = NULL;
    SI_32 status = DATADOWNLOADMNG_FAILURE;
    BOOL loop_break = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_setp_list_lck_ref);
    if (le_dls_IsEmpty(&datadownload_setp_list) == FALSE) {
            current_link = le_dls_Peek(&datadownload_setp_list);
            if (current_link != NULL) {
                loop_break = FALSE;
                do {
                    current_item = DATADOWNLOADMNG_GET_LIST_ITEM(current_link,
                            t_ddm_setp_list_item);
                    if (priority == current_item->priority) {
                        le_dls_Remove(&datadownload_setp_list, current_link);
                        le_mem_Release(current_item);
                        loop_break = TRUE;
                    }
                    else {
                        current_link = le_dls_PeekNext(&datadownload_setp_list,
                                current_link);
                        loop_break = (current_link == NULL) ? TRUE : FALSE;
                    }
                } while (loop_break != TRUE);
                if (current_link != NULL) {
                    status = DATADOWNLOADMNG_SUCCESS;
                }
            }
    }
    DATADOWNLOADMNG_UNLOCK(datadownload_setp_list_lck_ref);

    return status;
} /* DataDownloadMngDelFromSetpList  */


/**
 * @private Update the SETP List according to pending task status change.
 * @par Operations contract:
 *  This function updates the priority of pending task in sequential executable
 *  task priority according to pending task status change.
 *
 * @param[in] pending_status    Pending status of particular priority task.
 * @param[in] priority          Priority of particular task.
 *
 * @rerurn
 *      - None
 */
static void DataDownloadMngUpdateSetpList (BOOL pending_status, t_ddm_task_priority priority)
{
    if (pending_status == TRUE) {
        DataDownloadMngInsertToSetpList(priority);
    }
    else {
        DataDownloadMngDelFromSetpList(priority);
    }
}



/**
 * @private SETP List updating thread.
 * @par Operations contract:
 *  This thread function is specially design to update the sequential
 *  executable task priority list by monitoring the pending task flags.
 *
 * @param[in] context  Reference to any input context to process
 *                     (default behavior).
 *
 * @return
 *  - Pointer to any necessary modified buffer by this thread.
 */
static void *DataDownloadMngSetpListUpdtThrd (void *context)
{
    UI_8 loop_counter = (UI_8)0;

    while (TRUE) { /* Infinite loop */

        /* Updation of priority of configuration file installation */
        DataDownloadMngUpdateSetpList(
                DataDownloadMngGetConfInstlFlag(),
                SETP_INSTALL_CONFIG_FILE);

        /* Updation of priority of SMS Task List Processing */
        DataDownloadMngUpdateSetpList(
                DataDownloadMngGetActnRqstFlag(),
                SETP_SMS_TASK_LIST);

        /* Updation of priority of SMS send task */
        DataDownloadMngUpdateSetpList(
                DataDownloadMngGetSmsSendFlag(),
                SETP_SMS_SENDING);

        /* Updation of priorities of high priority upload data blocks
         * (from PRU0 to PRU9) */
        for (loop_counter = (UI_8)0; loop_counter < (UI_8)10; loop_counter++) {
            DataDownloadMngUpdateSetpList(
                    DataDownloadMngGetDataUpldFlag(loop_counter),
                    (t_ddm_task_priority)((UI_8)SETP_DATA_UPLOAD_PRU0
                        + loop_counter));
        }

        /* Updation of priority of SMS send task */
        DataDownloadMngUpdateSetpList(
                DataDownloadMngGetDwnldRqstFlag(),
                SETP_DOWNLOAD_FILE);

        /* Updation of priorities of low priority upload data blocks
         * (from PRU10 to PRU30) */
        for (loop_counter = (UI_8)10;
                loop_counter < DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES;
                loop_counter++) {
            DataDownloadMngUpdateSetpList(
                    DataDownloadMngGetDataUpldFlag(loop_counter),
                    (t_ddm_task_priority)((UI_8)SETP_DATA_UPLOAD_PRU10
                        + loop_counter - (UI_8)10));
        }
    } /* End of Infinite loop */

    return NULL;
}  /* End of DataDownloadMngSetpListUpdtThrd */


/**
 * @private Sequentially executing thread for task.
 * @par Operations contract:
 *  This thread function executes the various task in sequential manner
 *  according to priority.
 *
 * @param[in] context  Reference to any input context to process
 *                     (default behavior).
 *
 * @return
 *  - Pointer to any necessary modified buffer by this thread.
 */
static void *DataDownloadMngSqntlExcTaskThrd (void *context)
{
    t_ddm_task_priority top_priority = SETP_LOWEST_PRIORITY;

    while (TRUE) { /* Infinite loop */
        if (DataDownloadMngGetFromSetpList(&top_priority)) {

            switch (top_priority) {
            case SETP_LOWEST_PRIORITY:
                break;
            case SETP_INSTALL_CONFIG_FILE:
                /* TODO Call ConfigurationMng API to install config file */
                break;
            case SETP_SMS_TASK_LIST:
                /* TODO Call Task List processing function */
                break;
            case SETP_SMS_SENDING:
                /* TODO Call DataUploadMng API to send sms */
                break;
            case SETP_DOWNLOAD_FILE:
                /* TODO Call Download processing function*/
                break;
            default:
                if (top_priority <= SETP_DATA_UPLOAD_PRU9) {
                    /* TODO Call DataUploadMng API to upload Data block with
                     * priority from PRU0 to PRU9 */
                } else {
                    /* TODO Call DataUploadMng API to upload Data block with
                     * priority from PRU10 to PRU10 */
                }
            }
        }
    }
    return NULL;
} /* DataDownloadMngSqntlExcTaskThrd */


/*
 * @private Initialize Sequential Executable Priority Task Parameters.
 * @par Operations contract:
 *  This thread function initializes all parameter used in sequential
 *  execution of tasks according to priorities.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngInitSETPParams (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Global variable initialization */
    datadownload_dwnld_rqst_flag = FALSE;
    datadownload_actn_rqst_flag = FALSE;
    datadownload_conf_instl_flag = FALSE;


    /* Mutex lock initialization */
    DATADOWNLOADMNG_INIT_LOCK(datadownload_conf_instl_lck_ref);
    DATADOWNLOADMNG_INIT_LOCK(datadownload_dwnld_rqst_lck_ref);
    DATADOWNLOADMNG_INIT_LOCK(datadownload_actn_rqst_lck_ref);
    DATADOWNLOADMNG_INIT_LOCK(datadownload_setp_list_lck_ref);


    datadownload_setp_list_pl_ref = le_mem_CreatePool("SETP_list_pool",
            sizeof(t_ddm_setp_list_item));
    if (datadownload_setp_list_pl_ref == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Unable to create memory pool for SETP List!!\n");
        status = DATADOWNLOADMNG_FAILURE;
    } else {
        le_mem_ExpandPool(datadownload_setp_list_pl_ref,
                DATADOWNLOADMNG_PRIORITY_LIST_MAX_SIZE);
    }

    return status;
} /* DataDownloadMngInitSETPParams */


/**
 * @private Check source ID existence.
 * @par Operations contract:
 *  This function checks the existence of incoming data block source id in TCU
 *  side by interacting with ServerMng.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_119
 *  - REU_MMW013_DDM_123
 *
 * @param[in] src_id    Source id of received data block.
 * @param[in] dst_id    Destination id of received data block.
 *
 * @return
 *  - TRUE: If source ID exists.
 *  - FALSE: If source ID does not exist.
 */
static BOOL DataDownloadMngCheckSrcIdExist (UI_8 src_id, UI_8 dest_id)
{
    /* TODO Call ServerMng function to check existence of source id. */
    return TRUE;
} /* DataDownloadMngCheckSrcIdExist */


/**
 * @private Parse received data block.
 * @par Operations contract:
 *  This function parses the received data block and extract information to
 *  identify service by which received data block to be processed and type of
 *  message request.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_71
 *  - REU_MMW013_DDM_124
 *  - REU_MMW013_DDM_64
 *  - REU_MMW013_DDM_81
 *
 * @param[in] msgptr         Reference to received data block data structure.
 * @param[out] parsed_info   Reference to routing information data structure.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngParseDataBlk (const t_ddm_datablock *msgptr, t_ddm_data_route_info *parsed_info)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS ;
    t_acp_application_header acp245_app_info = {0};
    t_acprn_application_header acprn_app_info = {0};

    if ((msgptr->length < (UI_16)8) || (parsed_info == NULL)) {
        status = DATADOWNLOADMNG_FAILURE;
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Either null pointer assign to store parsed data or "
                "received data is not in ACP format.\n");
    }
    else {
        parsed_info->acp_type = (t_ddm_acp_type)Acp245GetBit((UI_8)1,
                (UI_8)(msgptr->bytes[4]));

        if (parsed_info->acp_type == DATADOWNLOADMNG_ACPRN) {
            /* Parsing ACPRN packet */
            memset(&(parsed_info->session),0,sizeof(parsed_info->session));
            AcpRNApplicationHeaderUnpack((UI_8 *)(&(msgptr->bytes[4])),
                &(acprn_app_info));
            parsed_info->message_type = (UI_8)acprn_app_info.message_type;
        }
        else {
            /* Parsing ACP245 packet */
            Acp245ApplicationSessionIdUnpack((UI_8 *)(&(msgptr->bytes[0])),
                &(parsed_info->session));
            Acp245ApplicationHeaderUnpack((UI_8 *)(&(msgptr->bytes[4])),
                &(acp245_app_info));
            parsed_info->app_id = (UI_8)acp245_app_info.application_id;
            parsed_info->message_type = (UI_8)acp245_app_info.message_type;
        }
    }

    return status;
} /* DataDownloadMngParseDataBlk */


/**
 * @private Push data block to Download List.
 * @par Operations contract:
 *  This function appends new data block item to Download List.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_76
 *  - REU_MMW013_DDM_148
 *
 * @param[in] db_data_rec      Reference to database data record to be push.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPushToDwnldList (t_ddm_util_db_data *db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    SI_32 block_count = (SI_32)0;

    block_count = DataDownloadMngCountDbRows(&datadownload_backup_db,
            DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
    if (db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer assigned to %s "
                "function as input.\n", __func__);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < (SI_32)0) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read record count "
                "from table \"%s\".\n",
                DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < DATADOWNLOADMNG_DOWNLOAD_LIST_MAX_SIZE) {
        status = DataDownloadMngPutRecDataToDb(&datadownload_backup_db,
                DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE, db_data_rec,
                DATADOWNLOADMNG_DOWNLOAD_LIST_MAX_SIZE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to write in table "
                    "\"%s\".\n", DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
        }
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Download List is full!!\n");
    }

    return status;
} /* DataDownloadMngPushToDwnldList */


/**
 * @private Get data block pointer from Download List.
 * @par Operations contract:
 *  This function gives a pointer to first data block in Download List without
 *  removing from Download List.
 *
 * @param[out] ret_db_data_rec  Reference to database data record output buffer.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_159
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngGetFromDwnldList (t_ddm_util_db_data *ret_db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if (ret_db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer given for return"
                " data block from download list.\n");
        status = DATADOWNLOADMNG_FAILURE;
    }
    else {
        status = DataDownloadMngGetRecDataFrmDb(&datadownload_backup_db,
                DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE, ret_db_data_rec);

        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read data from "
                    "table \"%s\".\n", DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
        }
    }

    return status;
} /* DataDownloadMngGetFromDwnldList */


/**
 * @private Delete data block from Download List.
 * @par Operations contract:
 *  This function removes first data block from Download List.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngDelFromDwnldList (void)
{
    SI_32 status = DATADOWNLOADMNG_FAILURE;

    status = DataDownloadMngDelDataFromDb(&datadownload_backup_db,
            DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE, NULL);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to delete from "
                "table \"%s\".\n", DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
    }
    return status;
} /* DataDownloadMngDelFromDwnldList */


/**
 * @private Push data block to Task List.
 * @par Operations contract:
 *  This function appends new data block item to SMS Task List.
 *
 * @param[in] db_data_rec      Reference to database data record to be push.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_138
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPushToTaskList (t_ddm_util_db_data *db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    SI_32 block_count = (SI_32)0;

    block_count = DataDownloadMngCountDbRows(&datadownload_backup_db,
            DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
    if (db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer assigned to %s "
                "function as input.\n", __func__);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < (SI_32)0) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read record count "
                "from table \"%s\".\n",
                DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < DATADOWNLOADMNG_TASK_LIST_MAX_SIZE) {
        status = DataDownloadMngPutRecDataToDb(&datadownload_backup_db,
                DATADOWNLOADMNG_TASK_LIST_DATA_TABLE, db_data_rec,
                DATADOWNLOADMNG_TASK_LIST_MAX_SIZE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to write in table "
                    "\"%s\".\n", DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
        }
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Task List is full!!\n");
    }

    return status;
} /* DataDownloadMngPushToTaskList */


/**
 * @private Get data block pointer from SMS Task List.
 * @par Operations contract:
 *  This function gives a pointer to first data block in Task List without
 *  removing from Task List.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_138
 *
 * @param[out] ret_db_data_rec  Reference to database data record output buffer.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngGetFromTaskList (t_ddm_util_db_data *ret_db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if (ret_db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer given for return"
                " data block from task list.\n");
        status = DATADOWNLOADMNG_FAILURE;
    }
    else {
        status = DataDownloadMngGetRecDataFrmDb(&datadownload_backup_db,
                DATADOWNLOADMNG_TASK_LIST_DATA_TABLE, ret_db_data_rec);

        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read data from "
                    "table \"%s\".\n", DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
        }
    }

    return status;
} /* DataDownloadMngGetFromTaskList */


/**
 * @private Delete data block from Task List.
 * @par Operations contract:
 *  This function removes first data block from Task List and deallocate
 *  it.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_179
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngDelFromTaskList (void)
{
    SI_32 status = DATADOWNLOADMNG_FAILURE;

    status = DataDownloadMngDelDataFromDb(&datadownload_backup_db,
            DATADOWNLOADMNG_TASK_LIST_DATA_TABLE, NULL);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to delete from "
                "table \"%s\".\n", DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
    }
    return status;
} /* DataDownloadMngDelFromTaskList */


/**
 * @private Push data block to Configuration installation List.
 * @par Operations contract:
 *  This function appends new data block item to Configuration installation List.
 *
 * @param[in] db_data_rec      Reference to database data record to be push.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPushToConfList (t_ddm_util_db_data *db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    SI_32 block_count = (SI_32)0;

    block_count = DataDownloadMngCountDbRows(&datadownload_backup_db,
            DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
    if (db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer assigned to %s "
                "function as input.\n", __func__);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < (SI_32)0) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read record count "
                "from table \"%s\".\n",
                DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
        status = DATADOWNLOADMNG_FAILURE;
    }
    else if (block_count < DATADOWNLOADMNG_CONF_LIST_MAX_SIZE) {
        status = DataDownloadMngPutRecDataToDb(&datadownload_backup_db,
                DATADOWNLOADMNG_CONF_LIST_DATA_TABLE, db_data_rec,
                DATADOWNLOADMNG_CONF_LIST_MAX_SIZE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to write in table "
                    "\"%s\".\n", DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
        }
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Task List is full!!\n");
    }

    return status;
} /* DataDownloadMngPushToConfList */



/**
 * @private Get data block pointer from configuration installation List.
 * @par Operations contract:
 *  This function gives a pointer to first data block in configuration
 *  installation List without removing from configuration installation List.
 *
 * @param[out] ret_db_data_rec  Reference to database data record output buffer.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngGetFromConfList (t_ddm_util_db_data *ret_db_data_rec)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if (ret_db_data_rec == NULL) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Null pointer given for return"
                " data block from configuration installation list.\n");
        status = DATADOWNLOADMNG_FAILURE;
    }
    else {
        status = DataDownloadMngGetRecDataFrmDb(&datadownload_backup_db,
                DATADOWNLOADMNG_CONF_LIST_DATA_TABLE, ret_db_data_rec);

        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to read data from "
                    "table \"%s\".\n", DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
        }
    }

    return status;
} /* DataDownloadMngGetFromConfList */


/**
 * @private Delete data block from configuration installation List.
 * @par Operations contract:
 *  This function removes first data block from configuration installation List
 *  and deallocate
 *  it.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngDelFromConfList (void)
{
    SI_32 status = DATADOWNLOADMNG_FAILURE;

    status = DataDownloadMngDelDataFromDb(&datadownload_backup_db,
            DATADOWNLOADMNG_CONF_LIST_DATA_TABLE, NULL);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to delete from "
                "table \"%s\".\n", DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
    }
    return status;
} /* DataDownloadMngDelFromConfList */


/**
 * @private Check the data integrity of given received data.
 * @par Operations contract:
 *  This function checks the data integrity of given ACP packet.
 *
 * @param[in] datablock     Reference to input ACP datablock.
 *
 * @return
 *  - TRUE: If data integrity is OK.
 *  - FALSE: If data integrity is failed.
 */
static BOOL DataDownloadMngCheckDataIntgrty (t_ddm_datablock *datablock)
{
    BOOL ret_val = FALSE;

    /* Checking the data integrity */
    ret_val = TRUE;

    return ret_val;
}


/**
 * @private Pack configuration download acknowledgement.
 * @par Operations contract:
 *  This functions packs configuration download acknowledgement into ACP packet.
 *
 * @param[out] acp_out_buf  Reference to output buffer for returned ACP packet.
 * @param[in] ack_type      Acknowledgement type negative or positive.
 *                          - TRUE: Positive Acknowledgement.
 *                          - FALSE: Negative Acknowledgement.
 *
 * @return
 *  - Return length of return ACP packet.
 */
static UI_32 DataDownloadMngPackConfDwnldAck (UI_8 *acp_out_buf, BOOL ack_type)
{
    UI_32 acp_pack_len = (UI_8)0;

    if (ack_type == TRUE) {
        /* Assign Ack_Flag = 1 i.e. positive acknowledgement. */
    }
    else {
        /* Assign Ack_Flag = 0 i.e. negative acknowledgement. */
    }

    return acp_pack_len;
}


/**
 * @private Send data block to mdlwServices.
 * @par Operations contract:
 *  This function delivers the received message to mdlwServices.
 *
 * @param[in] datablock     Data block to be sent.
 *
 * @return
 *      None
 */
static void DataDownloadMngSendToMdlwr(t_ddm_datablock *datablock)
{
    memset(&datadownload_mdlw_msg, 0, sizeof(datadownload_mdlw_msg));
    datadownload_mdlw_msg.gdc_source
        = (t_source_gdc)datablock->message_service;
    datadownload_mdlw_msg.event = UNSOL_EVENT_MILON;
    memcpy(datadownload_mdlw_msg.data, datablock->bytes, datablock->length);
    datadownload_mdlw_msg.data_len = datablock->length;
    fi_mdlw_mdlwIntercompSendMessage(datadownload_mdlw_msg);
}

/**
 * @private Route received remote function command.
 * @par Operations contract:
 *  This function routes the received remote function command from server to
 *  mdlwService or Task list.
 *
 * @param[in] datablock         Data block to be routed.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRouteRmtCmd (t_ddm_datablock *datablock)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    t_ddm_util_db_data db_data = DATADOWNLOADMNG_DB_DATA_INIT;
    t_acprn_auth_confirm_request *auth_req = NULL;
    t_acprn_auth_conf_req_ack *unpacked_acp
        = (t_acprn_auth_conf_req_ack *)datadownload_data_hndl_buf;

    AcpRNAuthenticationConfirmReqAckUnpack(&(datablock->bytes[4]), unpacked_acp);
    if (DataDownloadMngCheckSrcIdExist((UI_8)unpacked_acp->source_id,
                (UI_8)unpacked_acp->destination_id) == TRUE) {
        db_data.id1 = (SI_32)unpacked_acp->source_id;
        db_data.id2 = (SI_32)unpacked_acp->destination_id;
        if (datablock->message_service == DATADOWNLOADMNG_MSG_SMS) {
            /* Framing of authentication & Confirmation ACP packet to send */
            auth_req = (t_acprn_auth_confirm_request *)datadownload_data_hndl_buf;
            fi_acp_AcpRNAuthenticationConfirmMessage(auth_req);
            auth_req->source_id = (UI_8)db_data.id1;
            datablock->length = (UI_16)AcpRNAuthenticationConfirmRequestPack(
                    auth_req, datablock->bytes);
            datablock->message_service = DATADOWNLOADMNG_MSG_TCP;
        }

        switch(db_data.id2) {
        case FI_ACPRN_DESTINATION_CCS_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_RLU_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_PROBE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_TCU_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SIM_1_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SIM_2_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_EV_SERVICE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_PROBE_SERVICE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SVT_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_ECALL_CONFIGURATION_UPDATE:
            if (auth_req != NULL) {

                db_data.id2 = FI_ACPRN_DESTINATION_COMMON;
                db_data.data_length = datablock->length;
                db_data.data_buffer = datablock->bytes;
                status =DataDownloadMngPushToDwnldList(&db_data);
                if (status == DATADOWNLOADMNG_SUCCESS) {
                    DataDownloadMngSetDwnldRqstFlag(TRUE);
                }
                else {
                SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to push "
                        "download request in download list.\n");
                }
            }
            else {
                /* Send received data to mdlwService  */
                DataDownloadMngSendToMdlwr(datablock);
            }
            break;
        default:
            if (auth_req != NULL) {
                db_data.id2 = FI_ACPRN_DESTINATION_COMMON;
                db_data.data_length = datablock->length;
                db_data.data_buffer = datablock->bytes;
                status = DataDownloadMngPushToTaskList(&db_data);
                if (status == DATADOWNLOADMNG_SUCCESS) {
                    DataDownloadMngSetActnRqstFlag(TRUE);
                }
                else {
                SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to push "
                        "action request in task list.\n");
                }
            }
            else {
                /* Send received data to mdlwService  */
                DataDownloadMngSendToMdlwr(datablock);
            }
            break;
        }
    }
    else {
        SendDebug(DEBUG_INFO, DEBUG_ID,
                "@FDDM : Received message source id not found so this "
                "message is discarded\n");
    }

    return status;
}

/**
 * @private Delete data is waiting for response.
 * @par Operations contract:
 *  This function deletes the data from sent item table if it is waiting for
 *  response.
 *
 * @param[in] source_id     Source ID of received acknowledgement ACP Packet.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngDelDataIfwaiting (SI_16 source_id)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Delete the data if it is in waiting */

    /* End message sequence */
    return status;
}

/**
 * @private Route received acknowledgement from server.
 * @par Operations contract:
 *  This function routes received acknowledgement from server to mdlwService.
 *
 * @param[in] datablock         Data block to be routed.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRouteRcvdAck (t_ddm_datablock *datablock)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    t_acprn_probe_upload_ack_msg *unpacked_acp
        = (t_acprn_probe_upload_ack_msg *)datadownload_data_hndl_buf;

    AcpRNProbeUploadAckMessageUnpack(&(datablock->bytes[4]), unpacked_acp);
    if (DataDownloadMngCheckSrcIdExist((UI_8)unpacked_acp->source_id,
                (UI_8)unpacked_acp->destination_id) == TRUE) {
        status = DataDownloadMngDelDataIfwaiting((SI_32)unpacked_acp->source_id);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID,
                "@FDDM : Unexpected response received from server.\n");
        }
        else {
            /* Send received data to mdlwService  */
            DataDownloadMngSendToMdlwr(datablock);
        }
        DataDownloadMngSetRespWaitFlag(FALSE);
    }
    return status;
}




/**
 * @private Route received configuration update request from server.
 * @par Operations contract:
 *  This function routes the received configuration request from server to
 *  ConfigrationMng or Download List.
 * @param[in] datablock         Data block to be routed.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRouteConfUpdt (t_ddm_datablock *datablock)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    t_ddm_util_db_data db_data = DATADOWNLOADMNG_DB_DATA_INIT;
    t_acprn_auth_conf_req_ack *unpacked_acp
        = (t_acprn_auth_conf_req_ack *)datadownload_data_hndl_buf;

    AcpRNAuthenticationConfirmReqAckUnpack(&(datablock->bytes[4]), unpacked_acp);
    if (DataDownloadMngCheckSrcIdExist((UI_8)unpacked_acp->source_id,
                (UI_8)unpacked_acp->destination_id) == TRUE) {

        switch(unpacked_acp->destination_id) {
        case FI_ACPRN_DESTINATION_CCS_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_RLU_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_PROBE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_TCU_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SIM_1_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SIM_2_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_EV_SERVICE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_PROBE_SERVICE_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_SVT_CONFIGURATION_UPDATE:
        case FI_ACPRN_DESTINATION_ECALL_CONFIGURATION_UPDATE:
            db_data.id1 = (SI_32)unpacked_acp->source_id;
            db_data.id2 = (SI_32)unpacked_acp->destination_id;
            db_data.data_length = datablock->length;
            db_data.data_buffer = datablock->bytes;
            /* Saving to persistent memory */
            status = DataDownloadMngPushToConfList(&db_data);
            if ( DataDownloadMngCheckDataIntgrty(datablock) == TRUE) {
                /* if check is succeeded then send +ve Acknowledge for data*/
                DataDownloadMngPackConfDwnldAck(datablock->bytes, TRUE);
            }
            else {
                /* if check is failed then send -ve Acknowledge for data*/
                DataDownloadMngPackConfDwnldAck(datablock->bytes, FALSE);

                /* Delete the data block */
                /* TODO status = DataDownloadMngDelFromConfList(&db_data); */
            }
            /* TODO Send Ack/Nack to DataUploadMng */
            break;
        default:
            /* Discard Configuration */
            break;
        }
    }
    else {
        SendDebug(DEBUG_INFO, DEBUG_ID,
                "@FDDM : Received message source id not found so this "
                "message is discarded\n");
    }

    return status;
}

/**
 * @private Route the ACPRN data block to appropriate list or service.
 * @par Operations contract:
 * This function routes the ACPRN data to appropriate list or service according
 * to application id and message type.
 *
 * @param[in] datablock         Data block to be routed.
 * @param[in] route_info        Reference to received data route information.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRouteAcprnBlk (t_ddm_datablock *datablock, t_ddm_data_route_info *route_info)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    switch(route_info->message_type) {
    case FI_ACPRN_MESSAGETYPE_REMOTE_FUNCTION_COMMAND:
        DataDownloadMngRouteRmtCmd(datablock);
        break;
    case FI_ACPRN_MESSAGETYPE_CONFIGURATION_REQUEST:
        break;
    case FI_ACPRN_MESSAGETYPE_ACKNOWLEDGEMENT_MESSAGE:
        DataDownloadMngRouteRcvdAck(datablock);
        break;
    case FI_ACPRN_MESSAGETYPE_CONFIGURATION_UPDATE:
        DataDownloadMngRouteConfUpdt(datablock);
        break;
    default:
        DataDownloadMngDelDataIfwaiting((SI_32)(-1));
        break;
    }
    return status;
} /* DataDownloadMngRouteAcprnBlk  */

/**
 * @private Route the ACP245 data block to appropriate list or service.
 * @par Operations contract:
 * This function routes the ACP245 data to appropriate list or service according
 * to application id and message type.
 *
 * @param[in] datablock         Data block to be routed.
 * @param[in] route_info        Reference to received data route information.
 *
 * @return
 *      None
 */
static void DataDownloadMngRouteAcp245Blk (t_ddm_datablock *datablock, t_ddm_data_route_info *route_info)
{
    switch(route_info->app_id) {
    case FI_ACP_APPLICATION_MILON:
        memset(&datadownload_mdlw_msg, 0, sizeof(datadownload_mdlw_msg));
        datadownload_mdlw_msg.gdc_source
            = (t_source_gdc)datablock->message_service;
        datadownload_mdlw_msg.event = UNSOL_EVENT_MILON;
        memcpy(datadownload_mdlw_msg.data, datablock->bytes, datablock->length);
        datadownload_mdlw_msg.data_len = datablock->length;
        /* Send received data to mdlwService  */
        fi_mdlw_mdlwIntercompSendMessage(datadownload_mdlw_msg);
        break;
    default:
        break;
    }
} /* DataDownloadMngRouteAcp245Blk  */

/**
 * @private Process received data block.
 * @par Operations contract:
 *  This function processes the received data block.
 *
 * @par Info Ref.:
 *  - REU_MMW013_DDM_189
 *  - REU_MMW013_DDM_88
 *  - REU_MMW013_DDM_191
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPrcssRcvdDataBlk (t_ddm_datablock *datablock)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    t_ddm_data_route_info data_route_info = DATADOWNLOADMNG_DATA_ROUTE_INFO_INIT;

    /* Parse received Data Block */
    status = DataDownloadMngParseDataBlk(datablock, &data_route_info);
        status = DATADOWNLOADMNG_FAILURE;
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug(DEBUG_INFO, DEBUG_ID,
                "@FDDM : Data parsing is failed!!\n");
    }
    else {
        /* Identify ACP frame */
        if (data_route_info.acp_type == DATADOWNLOADMNG_ACPRN) {
            status = DataDownloadMngRouteAcprnBlk(datablock, &data_route_info);
        }
        else {
            DataDownloadMngRouteAcp245Blk(datablock, &data_route_info);
        }
    }
    return status;
} /* DataDownloadMngPrcssRcvdDataBlk */


/**
 * @private Process Download request.
 * @par Operations contract:
 *   This function processes the download request from Download List.
 *
 * @par Info Ref.:
 *  - REU_MMW013_DDM_199
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPrcssDwnldList (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    DATADOWNLOADMNG_LOCK(datadownload_snd_buf_lck_ref);
    /* status = DataDownloadMngGetFromDwnldList(&datadownload_snd_buf); */
    if (status == DATADOWNLOADMNG_SUCCESS) {
        /* TODO call HttpsClientMng function to send download request. */
    }
    DATADOWNLOADMNG_UNLOCK(datadownload_snd_buf_lck_ref);

    return status;
} /* DataDownloadMngPrcssDwnldList */

/**
 * @private Process action request.
 * @par Operations contract:
 *   This function processes the action request from Task List.
 *
 * @par Info Ref.:
 *  - REU_MMW013_DDM_176
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngPrcssActnList (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    DATADOWNLOADMNG_LOCK(datadownload_snd_buf_lck_ref);
    /* status = DataDownloadMngGetFromTaskList(&datadownload_snd_buf);*/
    if (status == DATADOWNLOADMNG_SUCCESS) {
        /* TODO call HttpsClientMng function to send download request. */
    }
    DATADOWNLOADMNG_UNLOCK(datadownload_snd_buf_lck_ref);

    return status;
} /* DataDownloadMngPrcssActionList */


/**
 * @private DataDownloadMng events monitoring Thread.
 * @par Operations contract:
 *  This function is a thread routine which monitors the reporting events in
 *  DataDownloadMng.
 *
 * @param[in] contextptr:   Pointer to a thread context buffer which is shared
 *                          one.
 * @return
 *  - Pointer to any necessary modified buffer by this thread.
 */
static void* DataDownloadMngEventsMainThread (void *contextptr)
{
    /* Start legato event monitoring thread */
    le_event_RunLoop();
}


/**
 * @private Event Handler for received data block.
 * @par Operations contract:
 *  This function will be automatically called when received data block will be
 *  reported with essential report information that will be captured in "report"
 *  pointer as argument. And it is defined to process received SMS.
 *
 * @param[in] report:   Reported parameter capturing pointer.
 */
static void DataDownloadMngRcvDataEvntHndlr (void *report)
{
    /* Received data block processing */
    if (DataDownloadMngPrcssRcvdDataBlk(report) == DATADOWNLOADMNG_FAILURE) {
        SendDebug(DEBUG_INFO, DEBUG_ID,
                "@FDDM : Recieved data block processing is failed!!\n");
    }
}

/**
 * @private Register DataDownloadMng events.
 * @par Operations contract:
 *  This function registers events supported by DataDownloadMng.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRegisterEvents (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    datadownload_evnts_thread_ref = le_thread_Create("DATADOWNLOADMNG_event_Thread",
            DataDownloadMngEventsMainThread, NULL);
    if (datadownload_evnts_thread_ref == NULL) {
        SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : Unable to allocate memory for "
                "event main thread!!\n");
        status = DATADOWNLOADMNG_FAILURE;
    }
    else {
        datadownload_rcv_data_evnt_id = le_event_CreateId("Data_Rcv_Event",
                sizeof(t_ddm_datablock));
        le_event_AddHandler("Rcv_Event_Handler", datadownload_rcv_data_evnt_id,
                DataDownloadMngRcvDataEvntHndlr);

        le_thread_Start(datadownload_evnts_thread_ref);
    }
    return status;
} /* DataDownloadMngRegisterEvents */


/**
 * @private Initialize database and create tables
 * @par Operations contract:
 *  This function initializes database and creates all DataDownloadMng required
 *  tables.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngInitTables(void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Initialize database to store data */
    status = DataDownloadMngInitDb(&datadownload_backup_db,
            DATADOWNLOADMNG_DATA_BACKUP_FILE, NULL);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Database initialization "
                "failed!!\n", DATADOWNLOADMNG_DATA_BACKUP_FILE);
    }
    else {
        status = DataDownloadMngCreateDbTable(&datadownload_backup_db,
                DATADOWNLOADMNG_NAMED_DATA_TABLE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Unable to create table"
                ": %s\n", DATADOWNLOADMNG_NAMED_DATA_TABLE);
        }

        status = DataDownloadMngCreateDbTable(&datadownload_backup_db,
                DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Unable to create table"
                ": %s\n", DATADOWNLOADMNG_DOWNLOAD_LIST_DATA_TABLE);
        }

        status = DataDownloadMngCreateDbTable(&datadownload_backup_db,
                DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Unable to create table"
                ": %s\n", DATADOWNLOADMNG_TASK_LIST_DATA_TABLE);
        }

        status = DataDownloadMngCreateDbTable(&datadownload_backup_db,
                DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Unable to create table"
                ": %s\n", DATADOWNLOADMNG_CONF_LIST_DATA_TABLE);
        }

        status = DataDownloadMngCreateDbTable(&datadownload_backup_db,
                DATADOWNLOADMNG_DOWNLOADED_DATA_TABLE);
        if (status == DATADOWNLOADMNG_FAILURE) {
            SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Unable to create table"
                ": %s\n", DATADOWNLOADMNG_DOWNLOADED_DATA_TABLE);
        }
    }

    return status;
}

/* -------------------------------- Routines -------------------------------- */

/**
 * @brief Raise event for received data.
 * @par Operations contract:
 *  This function raises the event which is registered for receiving of data and
 *  a handler is attached with this event for processing of received data.
 *
 * @param[in] data  Reference to received data block.
 *
 * @return
 *      None
 */
void DataDownloadMngRaiseRcvdEvnt(void *data)
{
    le_event_Report(datadownload_rcv_data_evnt_id, data,
                    sizeof(t_ddm_datablock));
}


/**
 * @brief Put the data block in persistent memory.
 * @par Operations contract:
 *  This function saves the any data block in persistent memory with a reference
 *  label to identify the data block in persistent memory.
 *
 * @param[in] data_label    Data label to identify the data block.
 * @param[in] data          Data block to be store.
 * @param[in] data_size     Size of data block.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngPutDataToNV (UI_8 *data_label, void *data, UI_32 data_size)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Writing to sqlite3 database */
    status = DataDownloadMngPutDataToDb(&datadownload_backup_db,
            DATADOWNLOADMNG_NAMED_DATA_TABLE, data_label, data, data_size);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Unable to write in database table \"%s\".\n",
                DATADOWNLOADMNG_NAMED_DATA_TABLE);
    }

    return status;
} /* DataDownloadMngPutDataToNV */


/**
 * @brief Get the data block from persistent memory.
 * @par Operations contract:
 *  This function retrieves the data block from persistent memory corresponding
 *  to reference label.
 *
 * @param[in] data_label    Data label to identify the data block.
 * @param[out] output_buf   Reference to buffer to store return data block.
 * @param[in] buf_size     Size of output buffer.
 *
 * @return
 *  - Data length read from persistent memory, if function execution succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngGetDataFromNV (UI_8 *data_label, void *output_buf, UI_32 buf_size)
{
    SI_32 ret_len = (SI_32)0;

    /* Reading from sqlite3 database */
    ret_len = DataDownloadMngGetDataFromDb(&datadownload_backup_db,
            DATADOWNLOADMNG_NAMED_DATA_TABLE, data_label, output_buf, buf_size);
    if (ret_len == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Unable to read from database table \"%s\".\n",
                DATADOWNLOADMNG_NAMED_DATA_TABLE);
    }

    return ret_len;
} /* DataDownloadMngGetDataToNV */


/**
 * @brief Delete data block from persistent memory.
 * @par Operations contract:
 *  This function deletes the data block from persistent memory which will be
 *  identified by unique reference label.
 *
 * @param[in] data_label    Data label to identify the data block.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngDelDataFromNV (UI_8 *data_label)
{
    UI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Deleting from sqlite3 database */
    status = DataDownloadMngDelDataFromDb(&datadownload_backup_db,
            DATADOWNLOADMNG_NAMED_DATA_TABLE, data_label);
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Unable to delete from database table \"%s\".\n",
                DATADOWNLOADMNG_NAMED_DATA_TABLE);
    }

    return status;
} /* DataDownloadMngDelDataFromNV */


/**
 * @brief Initialize DataDownloadMng.
 * @par Operations contract:
 *  This function initializes the events, threads, Global variables and memory.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngInit (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    /* Global variable initialization */

    /* Mutex lock initialization */
    DATADOWNLOADMNG_INIT_LOCK(datadownload_snd_buf_lck_ref);

    /* Initialize database to store data */
    status = DataDownloadMngInitTables();
    if (status == DATADOWNLOADMNG_FAILURE) {
        SendDebug(DEBUG_INFO, DEBUG_ID, "@FDDM : %s : Database initialization "
                " or table creation failed!!\n",
                DATADOWNLOADMNG_DATA_BACKUP_FILE);
    }

    /* Event registration */
    status |= DataDownloadMngRegisterEvents();

    /* Initialization of Priority and its execution related parameters */
    DataDownloadMngInitSETPParams();

    return status;
} /* DataDownloadMngInitDataDownloadMng  */


