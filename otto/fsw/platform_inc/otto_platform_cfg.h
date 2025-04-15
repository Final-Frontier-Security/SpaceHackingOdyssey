/************************************************************************
** File:
**   $Id: otto_platform_cfg.h  $
**
** Purpose:
**  Define sample Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _OTTO_PLATFORM_CFG_H_
#define _OTTO_PLATFORM_CFG_H_

/*
** Default OTTO Configuration
*/
#ifndef OTTO_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define OTTO_CFG_STRING           "usart_29"
    #define OTTO_CFG_HANDLE           29 
    #define OTTO_CFG_BAUDRATE_HZ      115200
    #define OTTO_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define OTTO_CFG_DEBUG
#endif

#endif /* _OTTO_PLATFORM_CFG_H_ */
