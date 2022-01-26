/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  anirudhr
  * @brief   Header file for usb_desc.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define SIZ_DEVICE_DESC              	  18
#define SIZ_CONFIG_DESC              	  32

#define SIZ_STRING_LANGID            	  4
#define SIZ_STRING_MANUFAC            	  38
#define SIZ_STRING_PRODUCT           	  38
#define SIZ_STRING_SERIAL                 26
#define SIZ_STRING_INTERFACE              16

extern const uint8_t deviceDescriptor[SIZ_DEVICE_DESC];
extern const uint8_t configDescriptor[SIZ_CONFIG_DESC];
extern const uint8_t stringLangID[SIZ_STRING_LANGID];
extern const uint8_t stringManufac[SIZ_STRING_MANUFAC];
extern const uint8_t stringProduct[SIZ_STRING_PRODUCT];
extern const uint8_t stringSerial[SIZ_STRING_SERIAL];
extern const uint8_t stringInterface[SIZ_STRING_INTERFACE];

#endif /* __USB_DESC_H */
