#include "otto_app.h"
#include "otto_cmd.h"
#include "otto_msg.h"
#include "otto_cfdp.h"

#include <string.h>

CFE_Status_t OTTO_NoopCmd()
{
    CFE_EVS_SendEvent(OTTO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: No-Op received, Version %d.%d.%d.%d",
                      OTTO_MAJOR_VERSION, OTTO_MINOR_VERSION, OTTO_REVISION, OTTO_MISSION_REV);

    ++OTTO_AppData.hk.cmdCount;

    return CFE_SUCCESS;
}

int32 OTTO_OpenFile(char *Filename)
{
    char line[1000];
    memset(line, '\0', sizeof(line));
    char path[300] = "/home/jstar/Desktop/github-nos3/fsw/build/exe/cpu1/data/";
    strcat(path, Filename);
    FILE *inFile  = fopen(path, "r");

    if (inFile == NULL)
    {  
        CFE_EVS_SendEvent(OTTO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: EXECUTE FILE command file doesn't exist: %s", path);
        return OTTO_ERROR;
    }

    while (fgets(line, sizeof(line), inFile)) {
        printf("Line: %s", line); 
        system(line);
        memset(line, '\0', sizeof(line));
    }
    fclose(inFile);
    CFE_EVS_SendEvent(OTTO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: EXECUTE FILE command file exists!: %s", path);
    return OTTO_SUCCESS;
}

int32 OTTO_ExecuteCmd(const CFE_MSG_Message_t *MsgPtr)
{
    const OTTO_ExecuteCmd_t *CmdPtr = (const OTTO_ExecuteCmd_t *)MsgPtr;
    int status;

    status = system(CmdPtr->cmd);

    if(status == -1)
    {
        CFE_EVS_SendEvent(OTTO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Failed to execute command: %s", CmdPtr->cmd);
        return;
    }
    CFE_EVS_SendEvent(OTTO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Executed command SUCCESS");
    return OTTO_SUCCESS;
}

/*
** Reset all global counter variables
*/
void OTTO_ResetCounters()
{
    OTTO_AppData.hk.cmdErrorCount = 0;
    OTTO_AppData.hk.cmdCount = 0;
    OTTO_AppData.hk.deviceErrorCount = 0;
    OTTO_AppData.hk.DeviceCount = 0;
    return;
} 

CFE_Status_t OTTO_SendHkCmd(const OTTO_SendHkCmd_t *msg)
{
    CFE_MSG_SetMsgTime(CFE_MSG_PTR(OTTO_AppData.hk.TlmHeader), CFE_TIME_GetTime());
    /* return value ignored */ CFE_SB_TransmitMsg(CFE_MSG_PTR(OTTO_AppData.hk.TlmHeader), true);

    /* This is also used to check tables */
    //OTTO_CheckTables();

    return CFE_SUCCESS;
}

void OTTO_Config(void)
{
    int32 status = OS_SUCCESS;
    uint32_t config = ntohl(((OTTO_Config_cmd_t*) OTTO_AppData.MsgPtr)->DeviceCfg); // command is defined as big-endian... need to convert to host representation
    CFE_EVS_SendEvent(OTTO_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Configuration command received: %u", config);
    /* Command device to send HK */
    status = OTTO_CommandDevice(OTTO_AppData.SampleUart.handle, OTTO_DEVICE_CFG_CMD, config);
}
/*
** Execute Script
*/
void OTTO_Execute(void)
{

    return;
} 

/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void OTTO_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (OTTO_AppData.hk.DeviceEnabled == OTTO_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols available
        */ 
        OTTO_AppData.SampleUart.deviceString = OTTO_CFG_STRING;
        OTTO_AppData.SampleUart.handle = OTTO_CFG_HANDLE;
        OTTO_AppData.SampleUart.isOpen = PORT_CLOSED;
        OTTO_AppData.SampleUart.baud = OTTO_CFG_BAUDRATE_HZ;
        OTTO_AppData.SampleUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&OTTO_AppData.SampleUart);
        if (status == OS_SUCCESS)
        {
            OTTO_AppData.hk.DeviceCount++;
            OTTO_AppData.hk.DeviceEnabled = OTTO_DEVICE_ENABLED;
            CFE_EVS_SendEvent(OTTO_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Device enabled");
        }
        else
        {
            OTTO_AppData.hk.deviceErrorCount++;
            CFE_EVS_SendEvent(OTTO_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: UART port initialization error %d", status);
        }
    }
    else
    {
        OTTO_AppData.hk.deviceErrorCount++;
        CFE_EVS_SendEvent(OTTO_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: Device enable failed, already enabled");
    }
    return;
}

/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void OTTO_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (OTTO_AppData.hk.DeviceEnabled == OTTO_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(OTTO_AppData.SampleUart.handle);
        if (status == OS_SUCCESS)
        {
            OTTO_AppData.hk.DeviceCount++;
            OTTO_AppData.hk.DeviceEnabled = OTTO_DEVICE_DISABLED;
            CFE_EVS_SendEvent(OTTO_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Device disabled");
        }
        else
        {
            OTTO_AppData.hk.deviceErrorCount++;
            CFE_EVS_SendEvent(OTTO_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: UART port close error %d", status);
        }
    }
    else
    {
        OTTO_AppData.hk.deviceErrorCount++;
        CFE_EVS_SendEvent(OTTO_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "OTTO: Device disable failed, already disabled");
    }
    return;
}

void OTTO_DownloadFromSatellite(const CFE_MSG_Message_t *MsgPtr)
{
    const OTTO_FileTxSatellite_t *CmdPtr = (const OTTO_FileTxSatellite_t *)MsgPtr;

    OTTO_CFDP_SendFile(CmdPtr->Source, CmdPtr->Destination);
}

void OTTO_UploadToSatellite(const CFE_MSG_Message_t *MsgPtr)
{
    const OTTO_FileSatellite_t *CmdPtr = (const OTTO_FileSatellite_t *)MsgPtr;
    //OTTO_PrintMe(MsgPtr);

    OTTO_CFDP_GetFile(CmdPtr->pdu_type, CmdPtr->buffer, CmdPtr->destination, CmdPtr->length);
}