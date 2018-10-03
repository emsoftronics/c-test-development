/************  Copyright 2004-2014 FICOSA CORPORATIVE ELECTRONICS  ************
| Language:       |  MISRA C
| Controller:     |
| Spec. Document: |  legato.io
|                 |
|-----------------|------------------------------------------------------------
| Project:        | TCU E4.0
| Reference:      |
|------------------------------------------------------------------------------
| MODIFICATIONS HISTORY
| Date       -     Coder      -    Description
| 27/12/16  Jagdish Prajapati     Creation of the file.
|------------------------------------------------------------------------------
| DESCRIPTION:
|  File contains Command client interface for DataDownloadMng APIs.
 ******************************************************************************/
/* ------------------------------- includes --------------------------------- */

#include "legato.h"
#include "Global.h"
#include "Debug.h"
#include "HalCfg.h"

#include "CmdMng.h"
#include "DataDownloadMng.h"
#include "DataDownloadMngFunctions.h"

/* -------------------------- function prototypes --------------------------- */
static void Command_FDDM_REPORT_DATA(SI_32 parameters, SI_32 argc, arguments_t argv);
static void Command_FDDM_SET_FLAG_SENDSMS(SI_32 parameters, SI_32 argc, arguments_t argv);
static void Command_FDDM_SET_FLAG_UPLOAD(SI_32 parameters, SI_32 argc, arguments_t argv);
static void Command_FDDM_GET_FLAG_SENDSMS(SI_32 parameters, SI_32 argc, arguments_t argv);
static void Command_FDDM_GET_FLAG_UPLOAD(SI_32 parameters, SI_32 argc, arguments_t argv);

/* --------------------------- global variables ----------------------------- */

command_variant_t fddm_report_data = {
    .function = &Command_FDDM_REPORT_DATA,
    .parameters = 1,
    .is_variadic = 0,
    .invocation = { "REPORT", "DATA", NULL }
};

command_variant_t fddm_set_flag_sendsms = {
    .function = &Command_FDDM_SET_FLAG_SENDSMS,
    .parameters = 1,
    .is_variadic = 0,
    .invocation = { "SET", "SENDSMSFLAG", NULL }
};

command_variant_t fddm_set_flag_upload = {
    .function = &Command_FDDM_SET_FLAG_UPLOAD,
    .parameters = 2,
    .is_variadic = 0,
    .invocation = { "SET", "UPLOADFLAG", NULL }
};

command_variant_t fddm_get_flag_sendsms = {
    .function = &Command_FDDM_GET_FLAG_SENDSMS,
    .parameters = 0,
    .is_variadic = 0,
    .invocation = { "GET", "SENDSMSFLAG", NULL }
};

command_variant_t fddm_get_flag_upload = {
    .function = &Command_FDDM_GET_FLAG_UPLOAD,
    .parameters = 1,
    .is_variadic = 0,
    .invocation = { "GET", "UPLOADFLAG", NULL }
};

char *fddm_help[] = {
    "Usage: FDDM [COMMAND]",
    "Available commands:",
	"   REPORT DATA <bytes in hex pair (0 to F) without space>  "
        "Set SMS to DatadDownloadMng buffer and report it.",
	"   SET SENDSMSFLAG <TRUE/FALSE>    Set status of pending SMS to be sent to its flag.",
	"   SET UPLOADFLAG <priority no.(0 to 30)> <TRUE/FALSE>   "
        "Set status of pending data to be uploaded to its flag.",
	"   GET SENDSMSFLAG <TRUE/FALSE>    Get status of pending SMS to be sent.",
	"   GET UPLOADFLAG <priority no.(0 to 30)>  Set status of pending data to be uploaded.",
    NULL
};


command_t command_FDDM = {
    .name = "FDDM",
    .brief = "Manage Data Download component",
    .help = (char **)&fddm_help,
    .variant = {
                &fddm_report_data,
                &fddm_set_flag_sendsms,
                &fddm_set_flag_upload,
                &fddm_get_flag_sendsms,
                &fddm_get_flag_upload,
                NULL
    }
};


/* ------------------------------- functions -------------------------------- */


static void Command_FDDM_REPORT_DATA(SI_32 parameters, SI_32 argc, arguments_t argv)
{
    SI_32 param_id;
    t_ddm_datablock tmp_datablock;

    param_id = argc - parameters;

    tmp_datablock.length = le_hex_StringToBinary((const char *)argv[param_id],
            strlen((char *)argv[param_id]),(uint8_t *)tmp_datablock.bytes,
            DATADOWNLOADMNG_DATABLOCK_MAX_LEN);
    DataDownloadMngReportRcvdData(tmp_datablock.message_service,
            tmp_datablock.bytes, tmp_datablock.length);

    SendDebug(DEBUG_INFO, DEBUG_ID , "@FDDM : Command: DataDownloadMngSetSMSBuffer compeleted\n");
} /* DataDownloadMngSetActnRqstFlag */


static void Command_FDDM_SET_FLAG_SENDSMS(SI_32 parameters, SI_32 argc, arguments_t argv)
{
    SI_32 param_id;

    param_id = argc - parameters;

    if (strcmp((char *)argv[param_id], "TRUE") == 0) {
        DataDownloadMngSetSmsSendFlag(TRUE);
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: TRUE is set to SMS sending flag\n");
    }
    else if (strcmp((char *)argv[param_id], "FALSE") == 0) {
        DataDownloadMngSetSmsSendFlag(FALSE);
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: FALSE is set to SMS sending flag\n");
    }
    else {
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: Invalid Value assignment!! "
                "Flag is not affected. Please use \"TRUE\" or \"FALSE\" as argument.\n");
    }

} /* Command_FDDM_SET_FLAG_SENDSMS */

static void Command_FDDM_SET_FLAG_UPLOAD(SI_32 parameters, SI_32 argc, arguments_t argv)
{
    SI_32 param_id;
    SI_8 *endptr = NULL;
    UI_8 priority = 0xff;

    param_id = argc - parameters;
    priority = (UI_8)strtol((const char *)argv[param_id], (char **)&endptr, 10);

    if (strcmp((char *)argv[param_id + 1], "TRUE") == 0) {
        DataDownloadMngSetDataUpldFlag(priority, TRUE);
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: TRUE is set to data upload flag for priority %d.\n", priority);
    }
    else if (strcmp((char *)argv[param_id + 1], "FALSE") == 0) {
        DataDownloadMngSetDataUpldFlag(priority, FALSE);
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: FALSE is set to data upload flag for priority %d.\n", priority);
    }
    else {
        SendDebug(DEBUG_INFO, DEBUG_ID ,
                "@FDDM : Command: Invalid Value assignment!! "
                "Flag is not affected. Please use \"TRUE\" or \"FALSE\" as argument.\n");
    }

} /* Command_FDDM_SET_FLAG_UPLOAD */


static void Command_FDDM_GET_FLAG_SENDSMS(SI_32 parameters, SI_32 argc, arguments_t argv)
{
}

static void Command_FDDM_GET_FLAG_UPLOAD(SI_32 parameters, SI_32 argc, arguments_t argv)
{
}


