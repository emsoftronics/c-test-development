/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="http://bbserver.ficosa.com:27990/projects/FAL/repos/
*mmw013_datadownloadmng/browse/applicableDocuments/requirements/
*MMW013_DataDownloadMng_20161215.doc">MMW013_DataDownloadMng</a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains all DataDownloadMng supporting functions
*           prototypes, data structures and macros.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  19/12/16 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMngFunctions.h
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

#ifndef __DATADOWNLOADMNGFUNCTIONS_H__
#define __DATADOWNLOADMNGFUNCTIONS_H__

/* -------------------------------- Includes -------------------------------- */

/* Local Component Dependencies */
#include "DataDownloadMng.h"

/* ------------------------------ Defines ----------------------------------- */

/**
 * DATADOWNLOADMNG_INIT_LOCK macro is defined for initializing the mutex lock of
 * type le_mutex_ref_t.
 */
#define DATADOWNLOADMNG_INIT_LOCK(lock_ref)   \
            ({ lock_ref = le_mutex_CreateRecursive( STRINGIZE(lock_ref) );})

/**
 * DATADOWNLOADMNG_LOCK macro is defined for acquiring the mutex lock of type
 * le_mutex_ref_t.
 */
#define DATADOWNLOADMNG_LOCK(lock_ref)         le_mutex_Lock(lock_ref)

/**
 * DATADOWNLOADMNG_UNLOCK macro is defined for releasing the acquired mutex lock
 * of type le_mutex_ref_t.
 */
#define DATADOWNLOADMNG_UNLOCK(lock_ref)       le_mutex_Unlock(lock_ref)

/**
 * DATADOWNLOADMNG_DEL_LOCK macro is defined for deleting the mutex lock of type
 * le_mutex_ref_t and freeing the le_mutex acquired memory.
 */
#define DATADOWNLOADMNG_DEL_LOCK(lock_ref)     le_mutex_Delete(lock_ref)


/**
 * DATADOWNLOADMNG_DATABLOCK_MAX_LEN macro is defined to specify the maximum
 * limit of no. of bytes in a data block.
 */
#define DATADOWNLOADMNG_DATABLOCK_MAX_LEN        ((UI_32)(512))


/**
 * DATADOWNLOADMNG_INIT_DATABLOCK  macro is defined to specify the initialize
 * value for data block structure.
 */
#define DATADOWNLOADMNG_DATABLOCK_INIT        {(SI_32)0, (UI_16)0, {0}}


/**
 * DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES macro is defined to specify
 * the maximum no. of Data block upload priorities.
 */
#define DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES   ((UI_8)(31))

/**
 * DATADOWNLOADMNG_LBL_RECEIVED_DATABLOCK macro is defined to specify the name of
 * received data block for reference purpose in persistent memory.
 */
#define DATADOWNLOADMNG_LBL_RECEIVED_DATABLOCK   ((UI_8 *)"receiveddatablock")

/* ------------------------------- Data Types ------------------------------- */

/**
 * This is the user defined data type which describes a data block
 * with data block buffer and no. of bytes available in it.
 */
typedef struct
{
    t_ddm_msg_service message_service; ///< Service by which message received.
    UI_16 length;                      ///< Available no. of bytes in data block.
    UI_8 bytes[DATADOWNLOADMNG_DATABLOCK_MAX_LEN];
                                       ///< Data block bytes buffer memory.
} t_ddm_datablock;

/**
 * This is a user defined data type which describes an item in
 * a doubly linked list with data block.
 */
typedef struct
{
    t_ddm_datablock datablock;         ///< Data bytes buffer memory.
    le_dls_Link_t list;	               ///< Doubly link list to link nodes.
} t_ddm_datalist_item;

/**
 * This is a user defined data type which describes the flags for pending data
 * block to be upload of all different priority.
 */
typedef struct {
    BOOL data_upld_flag[DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES];
                                 ///< Possible priorities for Data Upload
    le_mutex_Ref_t data_upld_lck_ref[DATADOWNLOADMNG_MAX_NO_OF_DATA_UPLOAD_PRIORITIES];
                                 ///< Mutex locks for all possible priorities
} t_ddm_data_upld_flags;

/* --------------------------- Routine prototypes --------------------------- */

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
LE_SHARED void DataDownloadMngRaiseRcvdEvnt(void *data);

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
LE_SHARED SI_32 DataDownloadMngPutDataToNV (UI_8 *data_label, void *data, UI_32 data_size);

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
LE_SHARED SI_32 DataDownloadMngGetDataFromNV (UI_8 *data_label, void *output_buf, UI_32 buf_size);

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
LE_SHARED SI_32 DataDownloadMngDelDataFromNV (UI_8 *data_label);

/**
 * @brief Initialize DataDownloadMng.
 * @par Operations contract:
 *  This function initializes the events, threads, Global variables and memory.
 *
 * @return
 *  - DATADOWNLOADMNG_SUCCESS: Function execution Succeeded.
 *  - DATADOWNLOADMNG_FAILURE: Function execution Failed.
 */
LE_SHARED SI_32 DataDownloadMngInit (void);

#endif /* END OF __DATADOWNLOADMNGFUNCTIONS_H__ */
