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


#define BLOCK_SIZE					N25Q_SUBSECTOR_SIZE
#define NUM_BLOCKS					N25Q_NUM_SUBSECTS

#define CBW_SIGNATURE				(0x43425355U)
#define CSW_SIGNATURE				(0x53425355U)
#define CBW_SIZE					31
#define CSW_SIZE					13

/* SCSI commands */
#define SCSI_TESTUNITRDY			0x00
#define SCSI_INQUIRY				0x12
#define SCSI_READFMTCPTY			0x23
#define SCSI_READCPTY10				0x25
#define SCSI_MODESNS6				0x1A
#define SCSI_STRTSTPUNIT			0x1B
#define SCSI_REQSENSE				0x03
#define SCSI_READ10					0x28
#define SCSI_WRITE10				0x2A

/* SCSI command response lengths */
#define INQUIRY_DATA_LEN			36
#define READFMTCPTY_DATA_LEN		12
#define READCPTY10_DATA_LEN			8
#define MODESNS6_DATA_LEN			4
#define REQSENSE_DATA_LEN			20

/* BOT return values */
#define BOT_CBW_INVALID				-1
#define BOT_DATAOUTSTAGE			-2

#define BOT_NO_STALL				0
#define BOT_IN_STALL				1
#define BOT_OUT_STALL				2
#define BOT_BOTH_STALL				3

/* External function declarations */
int32_t BOT_process(const uint8_t* buf, uint8_t len, uint8_t* stallReqd);
uint8_t BOT_isRdyForNxtCmd();
void BOT_setCmdDone();

#endif /* __BOT_H */
