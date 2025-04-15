#ifndef OTTO_CFDP_H
#define OTTO_CFDP_H

#include "cfe.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define OTTO_CHUNK_SIZE 256

typedef enum {
	OTTO_CFDP_CLASS_1 = 0,
	OTTO_CFDP_CLASS_2 = 1
} OTTO_CFDP_Class_t;

typedef struct
{
	const char *source;
	const char *destination;
	OTTO_CFDP_Class_t cfdp_class;
	uint32 chunk_size;	
	char buffer[OTTO_CHUNK_SIZE];
} OTTO_CFDP_Transaction_t;

void OTTO_CFDP_SendFile(const char *source, const char *destination);
void OTTO_CFDP_GetFile(uint8_t pdu_type, char *buffer, const char *destination, uint16_t length);
void OTTO_CFDP_StartSendFileTransaction(OTTO_CFDP_Transaction_t *transaction);
void OTTO_CFDP_StartGetTransaction(OTTO_CFDP_Transaction_t *transaction, char* buffer);
int32 OTTO_CFDP_SendPDU();
void OTTO_PrintMe(CFE_SB_Buffer_t *buf);
int32_t OTTO_CFDP_SendEOF();
int32_t OTTO_CFDP_IsEOFPDU(CFE_SB_Buffer_t *buffer);

#endif