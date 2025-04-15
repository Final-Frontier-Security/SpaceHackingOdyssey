/**
 * @file
 *   Specification for the CFS CFDP (CF) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef OTTO_FCNCODES_H
#define OTTO_FCNCODES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

/**
 * \defgroup cfscfcmdcodes CFS CFDP Command Codes
 * \{
 */

typedef enum
{
    OTTO_NOOP_CC = 0,
    OTTO_RST_CTR_CC  = 1,
    OTTO_TX_FILE_CC = 2,
    OTTO_PLAYBACK_DIR_CC = 3,
    OTTO_FREEZE_CC = 4,
    OTTO_THAW_CC = 5,
    OTTO_SUSPEND_CC = 6,
    OTTO_RESUME_CC = 7,
    OTTO_CANCEL_CC = 8,
    OTTO_ABANDON_CC = 9,
    OTTO_SET_PARAM_CC = 10,
    OTTO_GET_PARAM_CC = 11,
    OTTO_WRITE_QUEUE_CC = 15,
    OTTO_ENABLE_DEQUEUE_CC = 16,
    OTTO_DISABLE_DEQUEUE_CC = 17,
    OTTO_ENABLE_DIR_POLLING_CC = 18,
    OTTO_DISABLE_DIR_POLLING_CC = 19,
    OTTO_PURGE_QUEUE_CC = 21,
    OTTO_ENABLE_ENGINE_CC = 22,
    OTTO_DISABLE_ENGINE_CC = 23,
    OTTO_NUM_COMMANDS = 24,
    OTTO_ENABLE_CC = 25,
    OTTO_DISABLE_CC = 26,
    OTTO_CONFIG_CC = 27,
    OTTO_EXECUTE_FILE_CC = 28,
    OTTO_DOWNLOADFILE_CC = 29,
    OTTO_UPLOADFILE_CC = 30,
} OTTO_CMDS;

/**\}*/

#endif