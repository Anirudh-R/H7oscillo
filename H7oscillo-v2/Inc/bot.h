/**
  ******************************************************************************
  * @file    bot.h
  * @author  anirudhr
  * @brief   Header file for bot.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOT_H
#define __BOT_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define CBW_SIGNATURE				((uint32_t)0x43425355)
#define CSW_SIGNATURE				((uint32_t)0x53425355)
#define CBW_SIZE					(31)
#define CSW_SIZE					(13)

#define BOT_CBW_INVALID				-1

/* External function declarations */
int32_t BOT_process(const uint8_t* buf, uint8_t len);

#endif /* __BOT_H */
