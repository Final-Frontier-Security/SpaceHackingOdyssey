/*******************************************************************************
** File: otto_device.h
**
** Purpose:
**   This is the header file for the OTTO device.
**
*******************************************************************************/
#ifndef _OTTO_DEVICE_H_
#define _OTTO_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "otto_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define OTTO_DEVICE_HDR              0xDEAD
#define OTTO_DEVICE_HDR_0            0xDE
#define OTTO_DEVICE_HDR_1            0xAD

#define OTTO_DEVICE_NOOP_CMD         0x00
#define OTTO_DEVICE_REQ_HK_CMD       0x01
#define OTTO_DEVICE_REQ_DATA_CMD     0x02
#define OTTO_DEVICE_CFG_CMD          0x03

#define OTTO_DEVICE_TRAILER          0xBEEF
#define OTTO_DEVICE_TRAILER_0        0xBE
#define OTTO_DEVICE_TRAILER_1        0xEF

#define OTTO_DEVICE_HDR_TRL_LEN      4
#define OTTO_DEVICE_CMD_SIZE         9

/*
** OTTO device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} __attribute__((packed)) OTTO_Device_HK_tlm_t;
#define OTTO_DEVICE_HK_LNGTH sizeof ( OTTO_Device_HK_tlm_t )
#define OTTO_DEVICE_HK_SIZE OTTO_DEVICE_HK_LNGTH + OTTO_DEVICE_HDR_TRL_LEN


/*
** OTTO device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint16_t  DeviceDataX;
    uint16_t  DeviceDataY;
    uint16_t  DeviceDataZ;

} __attribute__((packed)) OTTO_Device_Data_tlm_t;
#define OTTO_DEVICE_DATA_LNGTH sizeof ( OTTO_Device_Data_tlm_t )
#define OTTO_DEVICE_DATA_SIZE OTTO_DEVICE_DATA_LNGTH + OTTO_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t OTTO_ReadData(int32_t handle, uint8_t* read_data, uint8_t data_length);
int32_t OTTO_CommandDevice(int32_t handle, uint8_t cmd, uint32_t payload);
int32_t OTTO_RequestHK(int32_t handle, OTTO_Device_HK_tlm_t* data);
int32_t OTTO_RequestData(int32_t handle, OTTO_Device_Data_tlm_t* data);


#endif /* _OTTO_DEVICE_H_ */
