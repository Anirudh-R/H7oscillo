/**
  ******************************************************************************
  * @file    usb_core.h
  * @author  anirudhr
  * @brief   Header file for usb_core.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CORE_H
#define __USB_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_desc.h"


#define MAX_PACKET_SZ			64		/* max packet size for all endpoints */

/* External function declarations */
extern void USB_init(void);
extern void USB_handle_event(void);

#endif /* __USB_CORE_H */
