/**
  ******************************************************************************
  * @file    qspif.h
  * @author  anirudhr
  * @brief   Header file for qspif.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QSPIF_H
#define __QSPIF_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_ll_utils.h"


#define N25Q_SUBSECTOR_SIZE				4096
#define N25Q_PAGE_SIZE					256
#define N25Q_NUM_SUBSECTS				4096	/* number of subsectors in flash */

#define FLASH_CMD_WR_ENABLE				0x06
#define FLASH_CMD_RD_STATUSREG			0x05
#define FLASH_CMD_RD_FLAGSTATUSREG		0x70
#define FLASH_CMD_CLR_FLAGSTATUSREG		0x50
#define FLASH_CMD_RD_VCONFREG			0x85
#define FLASH_CMD_WR_VCONFREG			0x81
#define FLASH_CMD_RD_EVCONFREG			0x65
#define FLASH_CMD_WR_EVCONFREG			0x61
#define FLASH_CMD_FASTRD_QUADIO			0xEB
#define FLASH_CMD_FASTPROG_QUADIEXT		0x12
#define FLASH_CMD_ERASE_SUBSECT			0x20

extern void QSPI_init(void);
extern void QSPI_flash_init(void);
extern uint8_t QSPI_flash_read(__IO uint8_t buf[], uint32_t addr, uint32_t len);
extern uint8_t QSPI_flash_write(const uint8_t buf[], uint32_t addr, uint32_t len);
extern void QSPI_config_memmapped(void);

#endif /* __QSPIF_H */
