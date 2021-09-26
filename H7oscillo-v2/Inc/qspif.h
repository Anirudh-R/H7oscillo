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


/* Micron N25Q128A parameters */
#define N25Q_SUBSECTOR_SIZE				4096
#define N25Q_PAGE_SIZE					256
#define N25Q_NUM_SUBSECTS				4096	/* number of subsectors in the flash */

#define QSPI_USE_NONBLK_WRITES			1		/* do non-blocking flash program and erase */

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

/* states of the QSPI flash interface */
#define QSPI_STATE_IDLE					0		/* idle */
#define QSPI_STATE_PIP					1		/* program in progress */
#define QSPI_STATE_EIP					2		/* erase in progress */

typedef struct{
	uint8_t* pageDataPtr;		/* pointer to the data for the current page */
	uint32_t pageAddr;			/* address of the current page */
	uint32_t pagesLeft;			/* no. of remaining pages to be programmed */
} QSPIWriteState_t;

extern __IO uint8_t qspiState;

extern void QSPI_init(void);
extern void QSPI_flash_init(void);
extern uint8_t QSPI_flash_read(__IO uint8_t buf[], uint32_t addr, uint32_t len);
#if !QSPI_USE_NONBLK_WRITES
extern uint8_t QSPI_flash_write(const uint8_t buf[], uint32_t addr, uint32_t len);
#else
extern uint8_t QSPI_flash_write_nb(const uint8_t buf[], uint32_t addr, uint32_t secCnt);
#endif
extern uint8_t QSPI_is_busy(void);

#endif /* __QSPIF_H */
