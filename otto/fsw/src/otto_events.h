/************************************************************************
** File:
**    otto_events.h
**
** Purpose:
**  Define OTTO application event IDs
**
*************************************************************************/

#ifndef _OTTO_EVENTS_H_
#define _OTTO_EVENTS_H_

/* Standard app event IDs */
#define OTTO_RESERVED_EID              0
#define OTTO_STARTUP_INF_EID           1
#define OTTO_LEN_ERR_EID               2
#define OTTO_PIPE_ERR_EID              3
#define OTTO_SUB_CMD_ERR_EID           4
#define OTTO_SUB_REQ_HK_ERR_EID        5
#define OTTO_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define OTTO_CMD_ERR_EID               10
#define OTTO_CMD_NOOP_INF_EID          11
#define OTTO_CMD_RESET_INF_EID         12
#define OTTO_CMD_ENABLE_INF_EID        13
#define OTTO_ENABLE_INF_EID            14
#define OTTO_ENABLE_ERR_EID            15
#define OTTO_CMD_DISABLE_INF_EID       16
#define OTTO_DISABLE_INF_EID           17
#define OTTO_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define OTTO_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define OTTO_DEVICE_TLM_ERR_EID        30
#define OTTO_REQ_HK_ERR_EID            31


/* Device specific telemetry event IDs */
#define OTTO_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define OTTO_UART_INIT_ERR_EID         40
#define OTTO_UART_CLOSE_ERR_EID        41

#define OTTO_CMD_UPLOAD_EID			42

#define OTTO_INIT_INF_EID (20)

#endif