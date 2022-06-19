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

/* Sense key, ASC, ASCQ codes */
#define SENSEKEY_ILLEGAL_REQUEST	0x05
#define SENSEKEY_NOT_READY			0x02
#define ASC_INVALID_CMD				0x20
#define ASCQ_INVALID_CMD			0x00
#define ASC_ADDR_OUTOFRANGE			0x21
#define ASCQ_ADDR_OUTOFRANGE		0x00
#define ASC_INV_CDB_FIELD			0x24
#define ASCQ_INV_CDB_FIELD			0x00
#define ASC_NOTRDY_BECOMINGRDY		0x04
#define ASCQ_NOTRDY_BECOMINGRDY		0x01

/* BOT return values */
#define BOT_CBW_INVALID				-1
#define BOT_DATAOUTSTAGE			-2

/* External function declarations */
int32_t BOT_process(const uint8_t* buf, uint8_t len);
uint8_t BOT_isRdyForNxtCmd();
void BOT_setCmdDone();

#endif /* __BOT_H */
