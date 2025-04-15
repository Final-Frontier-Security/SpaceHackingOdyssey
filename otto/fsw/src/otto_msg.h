/*******************************************************************************
** File:
**   otto_msg.h
**
** Purpose:
**  Define OTTO application commands and telemetry messages
**
*******************************************************************************/
#ifndef _OTTO_MSG_H_
#define _OTTO_MSG_H_

#include "cfe.h"
#include "otto_device.h"
#include "otto_cfdp.h"

/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define OTTO_REQ_HK_TLM              0
#define OTTO_REQ_DATA_TLM            1
#define OTTO_FILENAME_MAX_PATH      64
#define OTTO_CMD_MAX_LENGTH		128

typedef struct 
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} OTTO_SendHkCmd_t;

typedef struct
{
    /* Every command requires a header used to identify it */
    uint8    CmdHeader[sizeof(CFE_MSG_CommandHeader_t)];

} OTTO_NoArgs_cmd_t;


/*
** OTTO write configuration command
*/
typedef struct
{
    uint8    CmdHeader[sizeof(CFE_MSG_CommandHeader_t)];
    uint32   DeviceCfg;

} OTTO_Config_cmd_t;

typedef struct
{
    uint8   CmdHeader[sizeof(CFE_MSG_CommandHeader_t)]; 
    char    cmd[OTTO_CMD_MAX_LENGTH];

}OTTO_ExecuteCmd_t;


/*
** OTTO device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    OTTO_Device_Data_tlm_t Sample;

} __attribute__((packed)) OTTO_Device_tlm_t;
#define OTTO_DEVICE_TLM_LNGTH sizeof ( OTTO_Device_tlm_t )


/*
** OTTO housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   cmdErrorCount;
    uint8   cmdCount;
    uint8   deviceErrorCount;
    uint8   DeviceCount;
  
    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8   DeviceEnabled;
    OTTO_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) OTTO_Hk_tlm_t;
#define OTTO_HK_TLM_LNGTH sizeof ( OTTO_Hk_tlm_t )

typedef struct 
{
	CFE_MSG_TelemetryHeader_t TlmHeader;
	uint8_t pdu_type;
	uint8_t direction;
	char buffer[OTTO_CHUNK_SIZE];
	char destination[OTTO_FILENAME_MAX_PATH];
} OTTO_CFDP_PDU_t;
#define OTTO_CFDP_PDU_TLM_LNGTH sizeof ( OTTO_CFDP_PDU_t )

typedef struct 
{
	CFE_MSG_CommandHeader_t CmdHeader;
	char Destination[OTTO_FILENAME_MAX_PATH];
	char Source[OTTO_FILENAME_MAX_PATH];
} OTTO_FileTxSatellite_t;

typedef struct 
{
	CFE_MSG_CommandHeader_t CmdHeader;
	uint16_t length;
	uint8_t pdu_type;
	char destination[OTTO_FILENAME_MAX_PATH];
	char buffer[OTTO_CHUNK_SIZE];
} OTTO_FileSatellite_t;

#endif /* _OTTO_MSG_H_ */
