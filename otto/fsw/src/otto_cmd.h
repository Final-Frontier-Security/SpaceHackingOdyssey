/**
 * @file
 *
 * CF command processing function declarations
 */

#ifndef OTTO_CMD_H
#define OTTO_CMD_H

#include "cfe.h"
#include "otto_app.h"

CFE_Status_t OTTO_NoopCmd(void);

void OTTO_ResetCounters(void);

void OTTO_Disable(void);
void OTTO_Enable(void);
void OTTO_Execute(void);
void OTTO_Config(void);
int32 OTTO_OpenFile(char *Filename);

int32 OTTO_ExecuteCmd(const CFE_MSG_Message_t *MsgPtr);


void OTTO_DownloadFromSatellite(const CFE_MSG_Message_t *MsgPtr);
void OTTO_UploadToSatellite(const CFE_MSG_Message_t *MsgPtr);

#endif