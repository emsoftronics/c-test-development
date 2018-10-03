/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="http://bbserver.ficosa.com:27990/projects/FAL/repos/
*mmw013_datadownloadmng/browse/applicableDocuments/requirements/
*MMW013_DataDownloadMng_20161215.doc">MMW013_DataDownloadMng</a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains all shared function prototypes, macros and data
*           types which are shared for another components for integration
*           purpose.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  17/11/16 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMng.h
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

/**
 *      \mainpage The Data Download Manager Component Documentation
 *
 *      @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
 *      @version 1.0
 *      @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
 *
 *      <HR>
 *
 *      \section Description
 *      DataDownloadMng is implemented to route the all incoming data to proper
 *      service or component. And it handles sequential execution of task
 *      according to their Priorities.
 *
 *      \section How_to_use How to use
 *      DataDownloadMng provides some APIs for other components and some APIs
 *      are required from other components.\n
 *
 *      In order to pass an incoming data block through DataDownloadMng, SmsMng
 *      or HttpsClientMng has to to call DataDownloadMngReportRcvdData() API
 *      using inclusion of DataDownloadMng.h as shown below:\n
 *
 *      @code
 *      LE_SHARED SI_32 DataDownloadMngReportRcvdData (
 *              t_ddm_msg_service msg_service, UI_8 *data_packet,
 *              UI_16 packet_length);
 *      @endcode
 *
 *      The above funtion copies the binary data block to DataDownloadMng local
 *      buffer. There has to be provide the following input arguments:
 *          - <i>msg_service</i> is the type of message service by which it is
 *          received. The possible values are:
 *         <ol>
 *         <li><b>DATADOWNLOADMNG_MSG_SMS</b>: Received message by SmsMng.</li>
 *         <li><b>DATADOWNLOADMNG_MSG_TCP</b>: Received message by
 *         HttpsClientMng.</li>
 *         </ol>
 *
 *          - <i>data_packet</i> is the reference to received data packet.
 *          - <i>packet_length</i> is the length of data packet.\n
 *
 *      It will also expose the some APIs to update the flags. These APIs shall
 *      be used by DataUploadMng and ConfigurationMng. These APIs are exposed to
 *      monitor the pending task which are implemented by DataUploadMng and
 *      ConfigurationMng.\n
 *
 *      For DataUploadMng following APIs are exposed:
 *
 *      @code
 *
 *      LE_SHARED void DataDownloadMngSetSmsSendFlag (BOOL pending_status);
 *
 *      LE_SHARED BOOL DataDownloadMngGetSmsSendFlag (void);
 *
 *      LE_SHARED SI_32 DataDownloadMngSetDataUpldFlag (UI_8 upload_priority_no,
 *      BOOL pending_status);
 *
 *      LE_SHARED BOOL DataDownloadMngGetDataUpldFlag (UI_8 upload_priority_no);
 *
 *      @endcode
 *
 *      The above functions will be called in following condition:
 *      - Whenever SMS to be sent availability status will change then
 *      DataUploadMng calls DataDownloadMngSetSmsSendFlag() function to update
 *      the status.
 *      - If DataUploadMng wants to monitor the status of availability of SMS to
 *      be sent then it can call DataDownloadMngGetSmsSendFlag() function that
 *      will return true if available and false if not.
 *      - Whenever upload data block availability status will change then
 *      DataUploadMng calls DataDownloadMngSetDataUpldFlag() function to update
 *      the status with data upload block priority input.
 *      - If DataUploadMng wants to monitor the status of availability of upload
 *      data block then it can call DataDownloadMngGetDataUpldFlag() function
 *      with priority as input that will return true if available and false if
 *      not.\n
 *
 *      Flags value shall be TRUE if corresponding task is pending else it shall
 *      be FALSE.\n
 *
 *      DataDownloadMng needs following APIs:-
 *          - Acp245ProtocolStack API to unpack SMS ACP packet.
 *          - ServerMng API to validate source ID.
 *          - HttpsClientMng API to download the configuration from server.
 *          - EncryptionMng API to check data integrity of downloaded data block.
 *          - DataUploadMng API to send the data block and acknowledgement to
 *          the server.
 *          - ConfigurationMng API to install the configuraion.
 *          - mdlwService API to communicate with various services.
 *          - ServiceMng API to activate and deactivate the services.\n\n
 *
 *
 */

#ifndef __DATADOWNLOADMNG_H__
#define __DATADOWNLOADMNG_H__

/* -------------------------------- Includes -------------------------------- */

/* Local Component Dependencies */
#include "Global.h"


/* -------------------------------- Defines --------------------------------- */

/**
 * TEST_FDDM_CMD macro is defined to enable command line test mode for functions
 * of DataDownloadMng.
 */
#define TEST_FDDM_CMD


#ifdef TEST_FDDM_CMD
#include "CmdMng.h"
#endif

/**
 * DATADOWNLOADMNG_SUCCESS macro is defined the success constant return by any
 * function when it executes successfully.
 */
#define DATADOWNLOADMNG_SUCCESS          ((SI_32)0)

/**
 * DATADOWNLOADMNG_FAILURE macro is defined the failure constant return by any
 * function when its execution fails.
 */
#define DATADOWNLOADMNG_FAILURE          ((SI_32)(-1))



/* ------------------------------- Data Types ------------------------------- */

/**
 * This is user defined data type which describes the possible message service
 * type that it will use for downloading purpose.
 */
typedef enum {
    DATADOWNLOADMNG_MSG_SMS  = 0,    ///< Message via SMS service.
    DATADOWNLOADMNG_MSG_TCP  = 1     ///< Message via TCP service.
} t_ddm_msg_service;

/* ---------------------------- Global Variables ---------------------------- */


/* --------------------------- Routine prototypes --------------------------- */

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
LE_SHARED void DataDownloadMngSetRespWaitFlag (BOOL waiting_status);


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
LE_SHARED BOOL DataDownloadMngGetRespWaitFlag (void);

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
LE_SHARED void DataDownloadMngSetSmsSendFlag (BOOL pending_status);

/**
 * @brief Get status of pending SMS to be send.
 * @par Operations contract:
 *  This function returns the status of pending SMS to be send.
 *
 * @return
 *  - TRUE: At least one SMS to be send is pending.
 *  - FALSE: No any SMS to be send is pending.
 */
LE_SHARED BOOL DataDownloadMngGetSmsSendFlag (void);

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
LE_SHARED SI_32 DataDownloadMngSetDataUpldFlag (UI_8 upload_priority_no, BOOL pending_status);

/**
 * @brief Get status of pending prioritized upload data block.
 * @par Operations contract:
 *  This function returns the status of pending prioritized upload data block.
 *
 * @return
 *  - TRUE: At least one prioritized upload data block is pending.
 *  - FALSE: No any prioritized upload data block is pending.
 */
LE_SHARED BOOL DataDownloadMngGetDataUpldFlag (UI_8 upload_priority_no);


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
LE_SHARED SI_32 DataDownloadMngReportRcvdData (t_ddm_msg_service msg_service, UI_8 *data_packet, UI_16 packet_length);


#endif /* END OF __DWNLDMNG_H__ */

