#include "cfe.h"

#include "otto_app.h"
#include "otto_cfdp.h"
#include "otto_events.h"
#include "otto_msgids.h"
#include <stdio.h>
#include <errno.h>

void OTTO_CFDP_SendFile(const char *source, const char *destination)
{
	OTTO_CFDP_Transaction_t transaction;

	memset(&transaction, 0, sizeof(transaction));
	transaction.source = source;
	strncpy(OTTO_AppData.fileInfo.destination, destination, sizeof(OTTO_AppData.fileInfo.destination)-1);
	OTTO_AppData.fileInfo.destination[sizeof(OTTO_AppData.fileInfo.destination)-1] = 0;
	transaction.cfdp_class = OTTO_CFDP_CLASS_1;
	transaction.chunk_size = OTTO_CHUNK_SIZE;

	OTTO_CFDP_StartSendFileTransaction(&transaction);
}

void OTTO_CFDP_GetFile(uint8_t pdu_type, char *buffer, const char *destination, uint16_t length)
{
	if(pdu_type == 2)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: EOF Reached");
		return;
	}
	OTTO_CFDP_Transaction_t transaction;

	memset(&transaction, 0, sizeof(transaction));
	transaction.destination = destination;
	transaction.chunk_size = length;

	OTTO_CFDP_StartGetTransaction(&transaction, buffer);
}

void OTTO_CFDP_StartSendFileTransaction(OTTO_CFDP_Transaction_t *transaction)
{
	int32_t status;
	osal_id_t fd;

	status = OS_OpenCreate(&fd, transaction->source, OS_FILE_FLAG_NONE, OS_READ_ONLY);
	if(status != OS_SUCCESS)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to open source: %s with error: %d", transaction->source, (int)status);
		return;
	}

	size_t size;

	while(1)
	{
		size = OS_read(fd, OTTO_AppData.fileInfo.buffer, transaction->chunk_size);

		if(size < 0)
		{
			CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to read from source file. Read %d bytes", size);
			OS_close(fd);
			return;
		}
		else
		{
			if(size == 0)
			{
				CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Reached EOF");
				break;
			}
			status = OTTO_CFDP_SendPDU();
			if(status != CFE_SUCCESS)
			{
				CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to send file data PDU");
				OS_close(fd);
				return;
			}

		}
		sleep(2);	
	}

	status = OTTO_CFDP_SendEOF();
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to send EOF");
	}
	else
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: File send complete");
	}

	OS_close(fd);
}

void OTTO_CFDP_StartGetTransaction(OTTO_CFDP_Transaction_t *transaction, char* buffer)
{
	int32 status;
	osal_id_t fd;

	status = OS_OpenCreate(&fd, transaction->destination, OS_FILE_FLAG_CREATE , OS_READ_WRITE);
	if(status != OS_SUCCESS)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to open destination: %s", transaction->destination);
		return;
	}
	status = OS_lseek(fd, 0, OS_SEEK_END);
	if(status < OS_SUCCESS)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to seek end of file");
		OS_close(fd);
		return;
	}

	//OTTO_PrintMe(buffer);
	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: About to write %d bytes to file", transaction->chunk_size);
	status = OS_write(fd, buffer, transaction->chunk_size);
	if(status < OS_SUCCESS)
	{
		CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Failed to write to destination file: Error: %d", (int)status);
		OS_close(fd);
		return;
	}
	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_ERROR, "OTTO: Wrote to file with status: %d", status);
	OS_close(fd);
}

int32_t OTTO_CFDP_SendEOF()
{
	int32 status;
	OTTO_AppData.fileInfo.pdu_type = 2;
	OTTO_AppData.fileInfo.direction = 1;

	CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &OTTO_AppData.fileInfo);
    status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &OTTO_AppData.fileInfo, true);
	if (status == CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Successfully sent telemetry packet");
        return CFE_SUCCESS;
    }
    else
    {
    	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Error sending tlm packet: %d", status);
    	return status;
    }
}

int32 OTTO_CFDP_SendPDU()
{
	int32 status;
	OTTO_AppData.fileInfo.pdu_type = 0;
	OTTO_AppData.fileInfo.direction = 1;

	CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &OTTO_AppData.fileInfo);
    status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &OTTO_AppData.fileInfo, true);
    if (status == CFE_SUCCESS)
    {
    	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Successfully sent telemetry packet");
        return CFE_SUCCESS;
    }
    else
    {
    	CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: Error sending tlm packet: %d", status);
    	return status;
    }

	
}

void OTTO_PrintMe(CFE_SB_Buffer_t *buf)
{
    size_t            len = 0; 
    CFE_MSG_GetSize(&buf, &len);
    uint8_t *bytePtr = (uint8_t *)buf;
    CFE_EVS_SendEvent(OTTO_CMD_UPLOAD_EID, CFE_EVS_EventType_INFORMATION, "OTTO: length of buffer: %d", len);

    for(uint16_t i = 0; i < len; i++)
    {
        printf("%02X ", bytePtr[i]);
    }
    printf("\n");
}