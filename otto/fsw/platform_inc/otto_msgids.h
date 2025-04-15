/************************************************************************
** File:
**   $Id: otto_msgids.h  $
**
** Purpose:
**  Define OTTO Message IDs
**
*************************************************************************/
#ifndef _OTTO_MSGIDS_H_
#define _OTTO_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define OTTO_CMD_MID              0x185A
/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define OTTO_REQ_HK_MID           0x185B 

/** \brief Message ID for waking up the processing cycle */
#define OTTO_WAKE_UP_MID 0x185C
/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define OTTO_HK_TLM_MID           0x085A 
#define OTTO_DEVICE_TLM_MID       0x085B 
#define OTTO_FILEDOWNLOAD_TLM_MID 0x085C

#endif /* _OTTO_MSGIDS_H_ */
