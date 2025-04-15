/*******************************************************************************
** File: otto_app.c
**
** Purpose:
**   This file contains the source code for the OTTO application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "otto_app.h"
#include "otto_cfdp.h"
#include "otto_cmd.h"
#include "otto_fcncodes.h"
// #include "cfs_utils.h"
// #include "cfe.h"
#include "otto_device.h"
// #include "otto_events.h"
#include "otto_platform_cfg.h"
#include "otto_perfids.h"
#include "otto_msg.h"
#include "otto_msgids.h"
#include "otto_version.h"
#include "hwlib.h"



/*
** Global Data
*/
OTTO_AppData_t OTTO_AppData;

static CFE_EVS_BinFilter_t  OTTO_EventFilters[] =
{   /* Event ID    mask */
    {OTTO_RESERVED_EID,           0x0000},
    {OTTO_STARTUP_INF_EID,        0x0000},
    {OTTO_LEN_ERR_EID,            0x0000},
    {OTTO_PIPE_ERR_EID,           0x0000},
    {OTTO_SUB_CMD_ERR_EID,        0x0000},
    {OTTO_SUB_REQ_HK_ERR_EID,     0x0000},
    {OTTO_PROCESS_CMD_ERR_EID,    0x0000},
    {OTTO_CMD_ERR_EID,            0x0000},
    {OTTO_CMD_NOOP_INF_EID,       0x0000},
    // {OTTO_CMD_EXECUTE_FILE_INF_EID,       0x0000},
    {OTTO_CMD_RESET_INF_EID,      0x0000},
    {OTTO_CMD_ENABLE_INF_EID,     0x0000},
    {OTTO_ENABLE_INF_EID,         0x0000},
    {OTTO_ENABLE_ERR_EID,         0x0000},
    {OTTO_CMD_DISABLE_INF_EID,    0x0000},
    {OTTO_DISABLE_INF_EID,        0x0000},
    {OTTO_DISABLE_ERR_EID,        0x0000},
    {OTTO_CMD_CONFIG_INF_EID,     0x0000},
    {OTTO_DEVICE_TLM_ERR_EID,     0x0000},
    {OTTO_REQ_HK_ERR_EID,         0x0000},
    {OTTO_REQ_DATA_ERR_EID,       0x0000},
    {OTTO_UART_INIT_ERR_EID,      0x0000},
    {OTTO_UART_CLOSE_ERR_EID,     0x0000},
    /* TODO: Add additional event IDs (EID) to the table as created */
};


/*
** Application entry point and main process loop
*/
void OTTO_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(OTTO_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = OTTO_AppInit();
    if (status != CFE_SUCCESS)
    {
        OTTO_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&OTTO_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(OTTO_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&OTTO_AppData.MsgPtr,  OTTO_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(OTTO_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            OTTO_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(OTTO_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: SB Pipe Read Error = %d", (int) status);
            OTTO_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    OTTO_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(OTTO_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(OTTO_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 OTTO_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    OTTO_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(OTTO_EventFilters,
                              sizeof(OTTO_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                              CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("OTTO: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&OTTO_AppData.CmdPipe, OTTO_PIPE_DEPTH, "OTTO_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(OTTO_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(OTTO_CMD_MID), OTTO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(OTTO_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            OTTO_CMD_MID, (unsigned int) status);
        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(OTTO_WAKE_UP_MID), OTTO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(OTTO_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            OTTO_WAKE_UP_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(OTTO_REQ_HK_MID), OTTO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(OTTO_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            OTTO_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /*
    if (status == CFE_SUCCESS)
    {
        status = OTTO_TableInit(); 
    }

    if (status == CFE_SUCCESS)
    {
        status = OTTO_CFDP_InitEngine(); 
    }
    */

    /*
    ** TODO: Subscribe to any other messages here
    */


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the OTTO_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(OTTO_AppData.hk.TlmHeader),
                   CFE_SB_ValueToMsgId(OTTO_HK_TLM_MID),
                   OTTO_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(OTTO_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(OTTO_DEVICE_TLM_MID),
                   OTTO_DEVICE_TLM_LNGTH);

    CFE_MSG_Init(CFE_MSG_PTR(OTTO_AppData.fileInfo.TlmHeader),
                   CFE_SB_ValueToMsgId(OTTO_FILEDOWNLOAD_TLM_MID),
                   OTTO_CFDP_PDU_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */


    /* 
    ** Always reset all counters during application initialization 
    */
    OTTO_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    OTTO_AppData.hk.DeviceEnabled = OTTO_DEVICE_DISABLED;
    OTTO_AppData.hk.DeviceHK.DeviceCounter = 0;
    OTTO_AppData.hk.DeviceHK.DeviceConfig = 0;
    OTTO_AppData.hk.DeviceHK.DeviceStatus = 0;

    memset(OTTO_AppData.fileInfo.buffer, '\0', sizeof(OTTO_AppData.fileInfo.buffer));

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(OTTO_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "OTTO App Initialized. Version %d.%d.%d.%d",
                OTTO_MAJOR_VERSION,
                OTTO_MINOR_VERSION, 
                OTTO_REVISION, 
                OTTO_MISSION_REV); 
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("OTTO: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the OTTO command pipe
*/
void OTTO_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t msg = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(OTTO_AppData.MsgPtr, &msg);
    switch (CFE_SB_MsgIdToValue(msg))
    {
        /*
        ** Ground Commands with command codes fall under the OTTO_CMD_MID (Message ID)
        */
        case OTTO_CMD_MID:
            OTTO_ProcessGroundCommand(OTTO_AppData.MsgPtr);
            break;

         case OTTO_WAKE_UP_MID:
            //OTTO_WakeupCmd((const OTTO_WakeupCmd_t *)OTTO_AppData.MsgPtr);
            break;
        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case OTTO_REQ_HK_MID:
            OTTO_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            OTTO_AppData.hk.cmdErrorCount++;
            CFE_EVS_SendEvent(OTTO_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "OTTO: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(msg));
            break;
    }
    return;
} 

/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void OTTO_ProcessGroundCommand(const CFE_SB_Buffer_t *msg)
{
    int32 status = OS_SUCCESS;

    typedef void (*const handler_fn_t)(const void *);

    static handler_fn_t fns[] = {
        [OTTO_NOOP_CC]                = (handler_fn_t)OTTO_NoopCmd,
        [OTTO_RST_CTR_CC]          = (handler_fn_t)OTTO_ResetCounters,
        [OTTO_EXECUTE_FILE_CC]     = (handler_fn_t)OTTO_ExecuteCmd,
        [OTTO_ENABLE_CC]           = (handler_fn_t)OTTO_Enable,
        [OTTO_DISABLE_CC]          = (handler_fn_t)OTTO_Disable,
        [OTTO_CONFIG_CC]           = (handler_fn_t)OTTO_Config,
        [OTTO_DOWNLOADFILE_CC]     = (handler_fn_t)OTTO_DownloadFromSatellite,
        [OTTO_UPLOADFILE_CC]       = (handler_fn_t)OTTO_UploadToSatellite,
    };

    static const uint16 expected_lengths[] = {
        [OTTO_NOOP_CC]             = sizeof(OTTO_NoArgs_cmd_t),
        [OTTO_RST_CTR_CC]          = sizeof(OTTO_NoArgs_cmd_t),
        [OTTO_EXECUTE_FILE_CC]     = sizeof(OTTO_ExecuteCmd_t),
        [OTTO_ENABLE_CC]           = sizeof(OTTO_NoArgs_cmd_t),
        [OTTO_DISABLE_CC]          = sizeof(OTTO_NoArgs_cmd_t),
        [OTTO_CONFIG_CC]           = sizeof(OTTO_Config_cmd_t),
        [OTTO_DOWNLOADFILE_CC]     = sizeof(OTTO_FileTxSatellite_t),
        [OTTO_UPLOADFILE_CC]       = sizeof(OTTO_FileSatellite_t),
    };

    CFE_MSG_FcnCode_t cmd = 0;
    size_t            len = 0;  

    CFE_MSG_GetFcnCode(&msg->Msg, &cmd);

    if (cmd < (sizeof(expected_lengths) / sizeof(expected_lengths[0])))
    {
        CFE_MSG_GetSize(&msg->Msg, &len);

        /* first, verify command length */
        if (len == expected_lengths[cmd])
        {
            /* if valid, process command */
            if (fns[cmd])
            {
                fns[cmd](msg);
            }
        }
        else
        {
            CFE_EVS_SendEvent(OTTO_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "OTTO: invalid ground command length for command 0x%02x, expected %d got %zd", cmd,
                              expected_lengths[cmd], len);
            ++OTTO_AppData.hk.cmdErrorCount;
        }
    }
    else
    {
        CFE_EVS_SendEvent(OTTO_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: invalid ground command packet cmd_code=0x%02x",
                          cmd);
        ++OTTO_AppData.hk.cmdErrorCount;
    }
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void OTTO_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(OTTO_AppData.MsgPtr, &MsgId);   

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(OTTO_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case OTTO_REQ_HK_TLM:
            OTTO_ReportHousekeeping();
            break;

        case OTTO_REQ_DATA_TLM:
            OTTO_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            OTTO_AppData.hk.cmdErrorCount++;
            CFE_EVS_SendEvent(OTTO_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "OTTO: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void OTTO_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (OTTO_AppData.hk.DeviceEnabled == OTTO_DEVICE_ENABLED)
    {
        status = OTTO_RequestHK(OTTO_AppData.SampleUart.handle, (OTTO_Device_HK_tlm_t*) &OTTO_AppData.hk.DeviceHK);
        if (status == OS_SUCCESS)
        {
            OTTO_AppData.hk.DeviceCount++;
        }
        else
        {
            OTTO_AppData.hk.deviceErrorCount++;
            CFE_EVS_SendEvent(OTTO_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "OTTO: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &OTTO_AppData.hk);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &OTTO_AppData.hk, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void OTTO_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (OTTO_AppData.hk.DeviceEnabled == OTTO_DEVICE_ENABLED)
    {
        status = OTTO_RequestData(OTTO_AppData.SampleUart.handle, (OTTO_Device_Data_tlm_t*) &OTTO_AppData.DevicePkt.Sample);
        if (status == OS_SUCCESS)
        {
            OTTO_AppData.hk.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &OTTO_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &OTTO_AppData.DevicePkt, true);
        }
        else
        {
            OTTO_AppData.hk.deviceErrorCount++;
            CFE_EVS_SendEvent(OTTO_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "OTTO: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}
