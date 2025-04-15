/*******************************************************************************
** File: otto_app.h
**
** Purpose:
**   This is the main header file for the OTTO application.
**
*******************************************************************************/
#ifndef _OTTO_APP_H_
#define _OTTO_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "otto_device.h"
#include "otto_events.h"
#include "otto_msg.h"
#include "otto_msgids.h"
#include "otto_version.h"
#include "otto_cfdp.h"
#include "hwlib.h"


#define OTTO_SUCCESS            (0)  //codes for success and error
#define OTTO_ERROR              (-1)


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define OTTO_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define OTTO_DEVICE_DISABLED       0
#define OTTO_DEVICE_ENABLED        1


/*
** OTTO global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `otto_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    OTTO_Hk_tlm_t   hk;   /* OTTO Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
    ** Device data 
    ** TODO: Make specific to your application
    */
    OTTO_Device_tlm_t DevicePkt;      /* Device specific data packet */

    OTTO_CFDP_PDU_t fileInfo;
    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t SampleUart;             /* Hardware protocol definition */

} OTTO_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern OTTO_AppData_t OTTO_AppData; /* OTTO App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (OTTO_AppMain), these
**       functions are not called from any other source module.
*/
void  OTTO_AppMain(void);
int32 OTTO_AppInit(void);
void  OTTO_ProcessCommandPacket(void);
void  OTTO_ProcessGroundCommand(const CFE_SB_Buffer_t *msg);
void  OTTO_ProcessTelemetryRequest(void);
void  OTTO_ReportHousekeeping(void);
void  OTTO_ReportDeviceTelemetry(void);

#endif /* _OTTO_APP_H_ */
