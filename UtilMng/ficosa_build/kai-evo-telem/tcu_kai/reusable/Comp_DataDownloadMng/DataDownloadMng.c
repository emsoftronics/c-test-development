/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="http://bbserver.ficosa.com:27990/projects/FAL/repos/
*mmw013_datadownloadmng/browse/applicableDocuments/requirements/
*MMW013_DataDownloadMng_20161215.doc">MMW013_DataDownloadMng</a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains all shared functions definition which are shared
*           for another components for integration purpose and some internal
*           global variables and static functions definition.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  17/11/16 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMng.c
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

/* -------------------------------- Includes -------------------------------- */

#include "DataDownloadMng.h"
#include "DataDownloadMngFunctions.h"
#include "Debug.h"

/* -------------------------------- Defines --------------------------------- */

/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */


/**
 * A static variable for local received data buffer for DataDownloadMng to store
 * the received data block.
 */
static t_ddm_datablock datadownload_data_buf = DATADOWNLOADMNG_DATABLOCK_INIT;

/**
 * A global variable which shows the status of pending SMS to send.
 * @a Values:
 *  - True: Pending SMSs to send are available.
 *  - False: No any pending SMS to send is available.
 */
static BOOL datadownload_sms_send_flag = FALSE;

/**
 * A global variable for mutex lock to protect the flag of pending SMS to send
 * from race condition.
 */
static le_mutex_Ref_t datadownload_sms_send_lck_ref = NULL;


/**
 * A global set of flags for different priorities of uploading data block and
 * their mutex lock to protect the flags updation from race condition.
 */
static t_ddm_data_upld_flags datadownload_data_upld_flags = {{FALSE}, {NULL}};

/**
 * A global variable which shows the status of waiting for response.
 * @a Values:
 *  - True: Waiting for response of sent data.
 *  - False: Not waiting for any responce.
 */
static BOOL datadownload_resp_wait_flag = FALSE;

/**
 * A global variable for mutex lock to protect the
 * "datadownload_resp_wait_flag" updation, from race condition.
 */
static le_mutex_Ref_t datadownload_resp_wait_lck_ref = NULL;

/* --------------------------- Routine prototypes --------------------------- */

#ifdef TEST_FDDM_CMD
/* Register to command manager. */
static SI_32 DataDownloadMngRegisterToCmdMng (void);
#endif

/* -----------------------------Static Routines ----------------------------- */


#ifdef TEST_FDDM_CMD
/**
 * @private Register to command manager.
 * @par Operations contract:
 *  This function registers DataDownloadMng to command manager such that its
 *  function can be tested using command line.
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
static SI_32 DataDownloadMngRegisterToCmdMng (void)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;
    command_errno_t command = COMMAND_INVALID;
    extern command_t command_FDDM;

    command = CommandRegister (&command_FDDM);
    if (command == COMMAND_OK) {
        SendDebug (DEBUG_INFO, DEBUG_ID,
        "@FDDM DataDownloadMng: Registered with the CmdMng.\n");
    }
    else {
        status = DATADOWNLOADMNG_FAILURE;
        SendDebug (DEBUG_ERROR, DEBUG_ID,
        "@FDDM DataDownloadMng: Registeration with the CmdMng is Failed.\n");
    }
    return status;
}

#endif

/* -------------------------------- Routines -------------------------------- */

/**
 * @brief Set response waiting status.
 * @par Operations contract:
 *  This function sets the response waiting status of sent data.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_186
 *
 * @param[in] waiting_status Waiting status of sent data (TRUE/FALSE).
 *
 * @return
 *      None
 */
void DataDownloadMngSetRespWaitFlag (BOOL waiting_status)
{
    DATADOWNLOADMNG_LOCK(datadownload_resp_wait_lck_ref);
    datadownload_resp_wait_flag = waiting_status;
    DATADOWNLOADMNG_UNLOCK(datadownload_resp_wait_lck_ref);
} /* DataDownloadMngSetRespWaitFlag */


/**
 * @brief Get response waiting status.
 * @par Operations contract:
 *  This function returns the response waiting status of sent data.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_186
 *
 * @return
 *  - TRUE: Waiting for sent data response.
 *  - FALSE: Not waiting for any response.
 */
BOOL DataDownloadMngGetRespWaitFlag (void)
{
    BOOL ret_val = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_resp_wait_lck_ref);
    ret_val = datadownload_resp_wait_flag;
    DATADOWNLOADMNG_UNLOCK(datadownload_resp_wait_lck_ref);

    return ret_val;
} /* DataDownloadMngGetRespWaitFlag */

/**
 * @brief Set status of pending SMS to be send.
 * @par Operations contract:
 *  This function sets the status of pending SMS to be send as TRUE or FALSE.
 *
 * @param[in] pending_status Pending status to be set (TRUE/FALSE).
 *
 * @return
 *      None
 */
void DataDownloadMngSetSmsSendFlag (BOOL pending_status)
{
    DATADOWNLOADMNG_LOCK(datadownload_sms_send_lck_ref);
    datadownload_sms_send_flag = pending_status;
    DATADOWNLOADMNG_UNLOCK(datadownload_sms_send_lck_ref);
} /* DataDownloadMngSetSmsSendFlag */


/**
 * @brief Get status of pending SMS to be send.
 * @par Operations contract:
 *  This function returns the status of pending SMS to be send.
 *
 * @return
 *  - TRUE: At least one SMS to be send is pending.
 *  - FALSE: No any SMS to be send is pending.
 */
BOOL DataDownloadMngGetSmsSendFlag (void)
{
    BOOL ret_val = FALSE;

    DATADOWNLOADMNG_LOCK(datadownload_sms_send_lck_ref);
    ret_val = datadownload_sms_send_flag;
    DATADOWNLOADMNG_UNLOCK(datadownload_sms_send_lck_ref);

    return ret_val;
} /* DataDownloadMngGetSmsSendFlag */

/**
 * @brief Set status of pending prioritized upload data block.
 * @par Operations contract:
 *  This function sets the status of pending prioritized upload data block as
 *  TRUE or FALSE.
 *
 * @param[in] upload_priority_no  Priority no.(0 to 30) for data block upload.
 * @param[in] pending_status Pending status to be set (TRUE/FALSE).
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngSetDataUpldFlag (UI_8 upload_priority_no, BOOL pending_status)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if (upload_priority_no < DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES) {
        DATADOWNLOADMNG_LOCK(datadownload_data_upld_flags
                .data_upld_lck_ref[upload_priority_no]);
        datadownload_data_upld_flags.data_upld_flag[upload_priority_no]
            = pending_status;
       DATADOWNLOADMNG_UNLOCK(datadownload_data_upld_flags
                .data_upld_lck_ref[upload_priority_no]);
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : %u: Invalid priority number.\n",
                (int)upload_priority_no);
        status = DATADOWNLOADMNG_FAILURE;
    }

    return status;
} /* DataDownloadMngSetDataUpldFlag */

/**
 * @brief Get status of pending prioritized upload data block.
 * @par Operations contract:
 *  This function returns the status of pending prioritized upload data block.
 *
 * @return
 *  - TRUE: At least one prioritized upload data block is pending.
 *  - FALSE: No any prioritized upload data block is pending.
 */
BOOL DataDownloadMngGetDataUpldFlag (UI_8 upload_priority_no)
{
    BOOL ret_val = FALSE;

    if (upload_priority_no < DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES) {
        DATADOWNLOADMNG_LOCK(datadownload_data_upld_flags
                .data_upld_lck_ref[upload_priority_no]);
        ret_val = datadownload_data_upld_flags
                    .data_upld_flag[upload_priority_no];
        DATADOWNLOADMNG_UNLOCK(datadownload_data_upld_flags
                .data_upld_lck_ref[upload_priority_no]);
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : %u: Invalid priority number.\n",
                (int)upload_priority_no);
    }

    return ret_val;
} /* DataDownloadMngGetDataUpldFlag */

/**
 * @brief Report received data block to DataDownloadMng.
 * @par Operations contract:
 *  This function copies the data received from SmsMng or HttpsClientMng
 *  to DataDownloadMng local buffer memory. This function is design specially
 *  for SmsMng and HttpsClientMng to call at the time of receiving of data to
 *  report to DataDownloadMng.
 *
 * @par Requirement Ref.:
 *  - REU_MMW013_DDM_70
 *  - REU_MMW013_DDM_122
 *
 * @param[in] msg_service      Message service type by which message received.
 * @param[in] data_packet      Reference to received data block.
 * @param[in] packet_length    No. of bytes in received data block.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
SI_32 DataDownloadMngReportRcvdData (t_ddm_msg_service msg_service, UI_8 *data_packet, UI_16 packet_length)
{
    SI_32 status = DATADOWNLOADMNG_SUCCESS;

    if ((packet_length > 0) && (data_packet != NULL)) {
        if (packet_length <= DATADOWNLOADMNG_DATABLOCK_MAX_LEN) {
            /* Clearing the data buffer */
            memset(&datadownload_data_buf, 0, sizeof(datadownload_data_buf));

            /* Coping received data in local data buffer */
            datadownload_data_buf.message_service = msg_service;
            datadownload_data_buf.length = packet_length;
            memcpy(datadownload_data_buf.bytes, data_packet, packet_length);

            /* Taking backup of data in persistent memory */
            DataDownloadMngPutDataToNV(DATADOWNLOADMNG_LBL_RECEIVED_DATABLOCK,
                        &datadownload_data_buf, sizeof(datadownload_data_buf));

            /* Reporting the received data block with event */
            DataDownloadMngRaiseRcvdEvnt(&datadownload_data_buf);
        }
        else {
            SendDebug (DEBUG_INFO, DEBUG_ID,
                    "@FDDM : Received data packet length (%u) is out of "
                    "available data buffer size(%u).\n", packet_length,
                    DATADOWNLOADMNG_DATABLOCK_MAX_LEN);
            status = DATADOWNLOADMNG_FAILURE;
        }
    }
    else {
        SendDebug (DEBUG_INFO, DEBUG_ID,
                "@FDDM : Either empty data packet with length (%u) is "
                "received or NULL pointer is assigned as argument for data "
                "packet.\n", packet_length);
        status = DATADOWNLOADMNG_FAILURE;
    }

    return status;
} /* DataDownloadMngReportRcvdData */

/**
 * Data Download Manager component main thread.
 * <b>polyspace<MISRA-C:8.1:</b><i>Not a defect:</i>Justify with annotations>
 * COMPONENT_INIT legato specific function, Declaration not required.
 * <b>polyspace<MISRA-C:19.7:</b><i>Not a defect:</i>Justify with annotations>
 * legato specific component initialization function.
 */
COMPONENT_INIT
{
    UI_32 loop_counter = (UI_32)0;

#ifdef TEST_FDDM_CMD
    /* Registration to Commnad Manager*/
    DataDownloadMngRegisterToCmdMng();
#endif

    /* Global Variable initialization */

    memset(&datadownload_data_buf, 0, sizeof(datadownload_data_buf));

    DataDownloadMngSetSmsSendFlag(FALSE);
    DATADOWNLOADMNG_INIT_LOCK(datadownload_sms_send_lck_ref);

    for ( loop_counter = (UI_32)0;
            loop_counter < DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES;
            loop_counter++) {
        DataDownloadMngSetDataUpldFlag(loop_counter, FALSE);
        DATADOWNLOADMNG_INIT_LOCK(datadownload_data_upld_flags.data_upld_lck_ref
                [loop_counter]);
    }

    /* Initialization of another related data which are externally available. */
    DataDownloadMngInit();
}
