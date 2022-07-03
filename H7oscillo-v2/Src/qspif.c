/**
  ******************************************************************************
  * @file    qspif.c
  * @author  anirudhr
  * @brief   Implements QSPI interface towards Micron N25Q128A NOR flash.
  ******************************************************************************
  */

/* Includes */
#include "qspif.h"


#if QSPI_USE_NONBLK_WRITES
static uint8_t QSPI_flash_write_page_nb(const uint8_t buf[], uint32_t addr);
#endif
#if !QSPI_USE_NONBLK_WRITES
static uint8_t QSPI_flash_erase_subsector(uint32_t addr);
#else
static uint8_t QSPI_flash_erase_subsector_nb(uint32_t addr);
#endif
static void QSPI_flash_wren_single(void);
static void QSPI_flash_wren_quad(void);
static void QSPI_flash_statuspoll_single(uint8_t cmd, uint32_t mask, uint32_t match);
static void QSPI_flash_statuspoll_quad(uint8_t cmd, uint32_t mask, uint32_t match);
#if QSPI_USE_NONBLK_WRITES
static void QSPI_flash_statuspoll_quad_nb(uint8_t cmd, uint32_t mask, uint32_t match);
#endif
#ifdef DEBUG
static void QSPI_flash_prnt_reg_single(uint8_t cmd);
static void QSPI_flash_prnt_reg_quad(uint8_t cmd);
#endif
static uint8_t QSPI_flash_read_reg_quad(uint8_t cmd);
static void QSPI_flash_clr_errors_quad(void);
static void QSPI_DMA_config_read(__IO uint8_t* rxbufptr, uint32_t tfrsize);
static void QSPI_DMA_config_write(uint8_t* txbufptr, uint32_t tfrsize);
#if QSPI_USE_NONBLK_WRITES
static void QSPI_handle_error(void);
#endif

#if !QSPI_USE_NONBLK_WRITES
static __IO uint8_t subsectorBuf[N25Q_SUBSECTOR_SIZE];	/* subsector buffer */
#else
__IO uint8_t qspiState = QSPI_STATE_IDLE;				/* state of the QSPI flash interface */
static __IO QSPIWriteState_t qspiWriteState;			/* state of the ongoing write operation */
#endif

/**
  * @brief  Initialize the QSPI peripheral for accessing N25Q128A Flash.
  * @param  None
  * @retval None
  */
void QSPI_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_12, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);

	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_VERY_HIGH);

	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_10);
	LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_2, LL_GPIO_AF_9);
	LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_11, LL_GPIO_AF_9);
	LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_12, LL_GPIO_AF_9);
	LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_13, LL_GPIO_AF_9);
	LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_2, LL_GPIO_AF_9);

	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

	LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_QSPI);

	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_PRESCALER | QUADSPI_CR_SSHIFT,
			0x01 << QUADSPI_CR_PRESCALER_Pos | 0x01 << QUADSPI_CR_SSHIFT_Pos);			/* QUADSPI clock = AHBclk/2 = 100MHz, shifted sampling */
	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_FTHRES, 0x03 << QUADSPI_CR_FTHRES_Pos);			/* FIFO threshold = 4 bytes */
	MODIFY_REG(QUADSPI->DCR, QUADSPI_DCR_FSIZE, 23 << QUADSPI_DCR_FSIZE_Pos);			/* Flash size = 2^(23 + 1) bytes */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADSIZE, 0x02 << QUADSPI_CCR_ADSIZE_Pos);		/* 24-bit addressing */

	/* DMA initialization */
	QSPI_DMA_CLK_ENABLE();
	QSPI_STREAM->CR  &= ~0x01;															/* disable stream and confirm */
	while(QSPI_STREAM->CR & 0x01);
	QSPI_DMA->LIFCR |= 0x0F400000;														/* clear stream7 flags/errors */
	QSPI_STREAM->CR = QSPI_DMA_CHANNEL | 												/* ch3, byte-size transfer, low priority,... */
								0x00000400;												/* mem inc, periph-to-mem, normal mode */

	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_SMIE, 0x01 << QUADSPI_CR_SMIE_Pos);				/* status match interrupt enable */

	NVIC_SetPriority(QUADSPI_IRQn, 5);
}

/**
  * @brief  Initialize N25Q128A Flash memory.
  * @param  None
  * @retval None
  */
void QSPI_flash_init(void)
{
	static uint8_t flashInitDone = 0;

	/* if initialization already done */
	if(flashInitDone){
		return;
	}

	/* force the Flash to extended SPI mode. This is needed if the STM32 operates in extended SPI mode
	   following a button/debugger reset, but the flash continues to operate in quad SPI mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_ENABLE << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction and start the command */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	LL_mDelay(10);
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_EVCONFREG << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->DR = 0xFF;																/* set data */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	LL_mDelay(10);

	/* enable writes to the flash */
	QSPI_flash_wren_single();

	/* set number of dummy cycles to 9 */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x01 << QUADSPI_CCR_IMODE_Pos);		/* single-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_VCONFREG << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x01 << QUADSPI_CCR_DMODE_Pos);		/* single-line data */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->DR = 0x9B;																/* set data */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	QSPI_flash_statuspoll_single(FLASH_CMD_RD_VCONFREG, 0xFF, 0x9B);				/* wait for it to take effect */

	/* enable writes to the flash */
	QSPI_flash_wren_single();

	/* enter quad mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x01 << QUADSPI_CCR_IMODE_Pos);		/* single-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_EVCONFREG << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x01 << QUADSPI_CCR_DMODE_Pos);		/* single-line data */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->DR = 0x7F;																/* set data; Seems that bit 5 of Enh vol conf reg needs */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);											/* to be '1' for it to work, unlike what the datasheet claims */
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	QSPI_flash_statuspoll_quad(FLASH_CMD_RD_EVCONFREG, 0xFF, 0x7F);					/* wait for it to take effect */

	flashInitDone = 1;
}

/**
  * @brief  Read data from flash using Quad IO.
  * @param  buf: read buffer
  * @param  addr: start address of read
  * @param  len: number of bytes to read (max 65535)
  * @retval success = 0, fail = 1
  */
uint8_t QSPI_flash_read(__IO uint8_t buf[], uint32_t addr, uint32_t len)
{
	if(len == 0){
		return 1;
	}

	QSPI_DMA_config_read(buf, len);													/* setup DMA for read */

	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x01 << QUADSPI_CCR_FMODE_Pos);		/* indirect read mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_FASTRD_QUADIO << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x03 << QUADSPI_CCR_ADMODE_Pos);	/* quad-line address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x09 << QUADSPI_CCR_DCYC_Pos);		/* 9 dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	QUADSPI->DLR = len - 1;															/* no. of data bytes */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->AR = addr;																/* address */
	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x01 << QUADSPI_CR_DMAEN_Pos);		/* use DMA */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));

	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x00 << QUADSPI_CR_DMAEN_Pos);		/* disable DMA */

	/* transfer error */
	if(QUADSPI->SR & QUADSPI_SR_TEF){
		QUADSPI->CR &= ~QUADSPI_CR_EN;
		return 1;
	}

	/* copy any remaining data in the FIFO */
	uint8_t cnt = (QUADSPI->SR & QUADSPI_SR_FLEVEL) >> QUADSPI_SR_FLEVEL_Pos;
	while(cnt--){
		buf[len - cnt - 1] = *(__IO uint8_t *)(&QUADSPI->DR);
	}

	QUADSPI->CR &= ~QUADSPI_CR_EN;

	return 0;
}

#if !QSPI_USE_NONBLK_WRITES
/**
  * @brief  Write data to the flash using Quad IO (blocking). The data must fall within a 4KiB subsector.
  * @param  buf: data bytes to be written
  * @param  addr: start address of write
  * @param  len: number of bytes to write
  * @retval success = 0, fail = 1
  */
uint8_t QSPI_flash_write(const uint8_t buf[], uint32_t addr, uint32_t len)
{
	uint8_t* pBuf;
	uint8_t temp;
	uint32_t i;

	/* data larger than a subsector or crosses subsector boundaries */
	if(len > N25Q_SUBSECTOR_SIZE || ((addr & 0xFFF000) != ((addr + len - 1) & 0xFFF000)) || len == 0){
		return 1;
	}

	temp = !(addr & 0x000FFF) && len == N25Q_SUBSECTOR_SIZE;							/* full subsector write? */

	if(!temp){
		if(QSPI_flash_read(subsectorBuf, addr & 0xFFF000, N25Q_SUBSECTOR_SIZE)){		/* read the entire subsector */
			return 1;
		}
	}

	if(QSPI_flash_erase_subsector(addr)){												/* erase the subsector */
		return 1;
	}

	if(!temp){
		/* modify the subsector buffer */
		for(i = 0; i < len; i++){
			subsectorBuf[(addr & 0xFFF) + i] = buf[i];
		}
	}

	pBuf = temp ? (uint8_t *)buf : (uint8_t *)subsectorBuf;

	QSPI_flash_clr_errors_quad();														/* clear flash errors */

	/* write page-by-page */
	for(i = 0; i < 16; i++){
		QSPI_flash_wren_quad();															/* enable writes to the flash */

		QSPI_DMA_config_write(pBuf + i*N25Q_PAGE_SIZE, N25Q_PAGE_SIZE);					/* setup DMA for write */

		QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_FASTPROG_QUADIEXT << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x03 << QUADSPI_CCR_ADMODE_Pos);	/* quad-line address */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
		MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
		QUADSPI->DLR = N25Q_PAGE_SIZE - 1;												/* no. of data bytes */
		QUADSPI->AR = (addr & 0xFFF000) + i*N25Q_PAGE_SIZE;								/* address */
		MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x01 << QUADSPI_CR_DMAEN_Pos);		/* use DMA */
		QUADSPI->CR |= QUADSPI_CR_EN;
		while(!(QUADSPI->SR & QUADSPI_SR_TCF));

		MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x00 << QUADSPI_CR_DMAEN_Pos);		/* disable DMA */

		/* transfer error */
		if(QUADSPI->SR & QUADSPI_SR_TEF){
			QUADSPI->CR &= ~QUADSPI_CR_EN;
			return 1;
		}

		QUADSPI->CR &= ~QUADSPI_CR_EN;

		QSPI_flash_statuspoll_quad(FLASH_CMD_RD_FLAGSTATUSREG, 0x80, 0x80);				/* wait for write to complete */

		temp = QSPI_flash_read_reg_quad(FLASH_CMD_RD_FLAGSTATUSREG);
		/* flash program error */
		if(temp & 0x10){
			QSPI_flash_clr_errors_quad();
			return 1;
		}
	}

	return 0;
}

#else
/**
  * @brief  Write one or more subsectors to the flash (non-blocking). addr should be subsector aligned.
  * @param  buf: data bytes to be written, length should be a multiple of sector size
  * @param  addr: start address of write
  * @param  subsecCnt: no. of subsectors to write
  * @retval success = 0, fail = 1
  */
uint8_t QSPI_flash_write_nb(const uint8_t buf[], uint32_t addr, uint32_t subsecCnt)
{
	uint8_t stat;

	switch(qspiState)
	{
		/* start a new multi-subsector write process */
		case QSPI_STATE_IDLE:
			/* address not subsector aligned */
			if((addr & 0xFFF) || subsecCnt == 0){
				return 1;
			}

			/* setup the non-blocking page-by-page write process */
			qspiWriteState.pageDataPtr = (uint8_t *)buf;
			qspiWriteState.pageAddr = addr;
			qspiWriteState.pagesLeft = (N25Q_SUBSECTOR_SIZE * subsecCnt) / N25Q_PAGE_SIZE;

			QSPI_flash_clr_errors_quad();							/* clear flash errors */

			/* start by erasing the first subsector */
			qspiState = QSPI_STATE_EIP;
			if(QSPI_flash_erase_subsector_nb(addr)){
				QSPI_handle_error();
			}
			break;

		/* an erase operation has finished */
		case QSPI_STATE_EIP:
			stat = QSPI_flash_read_reg_quad(FLASH_CMD_RD_FLAGSTATUSREG);
			/* flash erase error */
			if(stat & 0x20){
				QSPI_handle_error();
			}

			/* move on to program phase */
			qspiState = QSPI_STATE_PIP;
			if(QSPI_flash_write_page_nb(qspiWriteState.pageDataPtr, qspiWriteState.pageAddr)){
				QSPI_handle_error();
			}
			break;

		/* a page program operation has finished */
		case QSPI_STATE_PIP:
			stat = QSPI_flash_read_reg_quad(FLASH_CMD_RD_FLAGSTATUSREG);
			/* flash program error */
			if(stat & 0x10){
				QSPI_handle_error();
			}

			/* update write status */
			qspiWriteState.pageDataPtr += N25Q_PAGE_SIZE;
			qspiWriteState.pageAddr += N25Q_PAGE_SIZE;
			qspiWriteState.pagesLeft--;

			if(qspiWriteState.pagesLeft == 0){
				qspiState = QSPI_STATE_IDLE;	/* done */
			}
			else{
				/* start of a new subsector */
				if(!(qspiWriteState.pageAddr & 0xFFF)){
					/* perform subsector erase */
					qspiState = QSPI_STATE_EIP;
					if(QSPI_flash_erase_subsector_nb(qspiWriteState.pageAddr)){
						QSPI_handle_error();
					}
				}
				/* same subsector */
				else{
					/* program the next page */
					if(QSPI_flash_write_page_nb(qspiWriteState.pageDataPtr, qspiWriteState.pageAddr)){
						QSPI_handle_error();
					}
				}
			}
			break;
	}

	return 0;
}

/**
  * @brief  Write a page (256 bytes) of the flash using Quad IO (non-blocking).
  * @param  buf: data bytes to be sent
  * @param  addr: start address of the page
  * @retval success = 0, fail = 1
  */
static uint8_t QSPI_flash_write_page_nb(const uint8_t buf[], uint32_t addr)
{
	QSPI_flash_wren_quad();															/* enable writes to the flash */

	QSPI_DMA_config_write((uint8_t *)buf, N25Q_PAGE_SIZE);							/* setup DMA for write */

	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_FASTPROG_QUADIEXT << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x03 << QUADSPI_CCR_ADMODE_Pos);	/* quad-line address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	QUADSPI->DLR = N25Q_PAGE_SIZE - 1;												/* no. of data bytes */
	QUADSPI->AR = (addr & 0xFFFF00);												/* address */
	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x01 << QUADSPI_CR_DMAEN_Pos);		/* use DMA */
	QUADSPI->CR |= QUADSPI_CR_EN;
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));

	MODIFY_REG(QUADSPI->CR, QUADSPI_CR_DMAEN, 0x00 << QUADSPI_CR_DMAEN_Pos);		/* disable DMA */

	/* transfer error */
	if(QUADSPI->SR & QUADSPI_SR_TEF){
		QUADSPI->CR &= ~QUADSPI_CR_EN;
		return 1;
	}

	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_quad_nb(FLASH_CMD_RD_FLAGSTATUSREG, 0x80, 0x80);			/* interrupt once write is complete */

	return 0;
}
#endif

#if !QSPI_USE_NONBLK_WRITES
/**
  * @brief  Erase a 4KiB subsector of the flash using Quad IO (blocking).
  * @param  addr: any address within the subsector that is to be erased
  * @retval success = 0, fail = 1
  */
static uint8_t QSPI_flash_erase_subsector(uint32_t addr)
{
	uint8_t temp;

	QSPI_flash_clr_errors_quad();													/* clear flash errors */
	QSPI_flash_wren_quad();															/* enable writes to the flash */

	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_ERASE_SUBSECT << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x03 << QUADSPI_CCR_ADMODE_Pos);	/* quad-line address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->AR = addr;																/* address */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));

	/* transfer error */
	if(QUADSPI->SR & QUADSPI_SR_TEF){
		QUADSPI->CR &= ~QUADSPI_CR_EN;
		return 1;
	}

	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_quad(FLASH_CMD_RD_FLAGSTATUSREG, 0x80, 0x80);				/* wait for erase to complete */

	temp = QSPI_flash_read_reg_quad(FLASH_CMD_RD_FLAGSTATUSREG);
	/* flash erase error */
	if(temp & 0x20){
		return 1;
	}

	return 0;
}

#else
/**
  * @brief  Erase a 4KiB subsector of the flash using Quad IO (non-blocking).
  * @param  addr: any address within the subsector that is to be erased
  * @retval success = 0, fail = 1
  */
static uint8_t QSPI_flash_erase_subsector_nb(uint32_t addr)
{
	QSPI_flash_clr_errors_quad();													/* clear flash errors */
	QSPI_flash_wren_quad();															/* enable writes to the flash */

	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_ERASE_SUBSECT << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x03 << QUADSPI_CCR_ADMODE_Pos);	/* quad-line address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	QUADSPI->AR = addr;																/* address */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));

	/* transfer error */
	if(QUADSPI->SR & QUADSPI_SR_TEF){
		QUADSPI->CR &= ~QUADSPI_CR_EN;
		return 1;
	}

	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_quad_nb(FLASH_CMD_RD_FLAGSTATUSREG, 0x80, 0x80);			/* interrupt once erase is complete */

	return 0;
}
#endif

/**
  * @brief  Enable writes to the flash, using single line instn and data.
  * @param  None
  * @retval None
  */
static void QSPI_flash_wren_single(void)
{
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x01 << QUADSPI_CCR_IMODE_Pos);		/* single-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_ENABLE << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction and start the command */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_single(FLASH_CMD_RD_STATUSREG, 0x02, 0x02);				/* wait for the write enable latch bit to be set */
}

/**
  * @brief  Enable writes to the flash, using quad line instn and data.
  * @param  None
  * @retval None
  */
static void QSPI_flash_wren_quad(void)
{
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_WR_ENABLE << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction and start the command */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_quad(FLASH_CMD_RD_STATUSREG, 0x02, 0x02);					/* wait for the write enable latch bit to be set */
}

/**
  * @brief  Poll the status of a flash register using auto polling mode, using single line instn and data.
  * @param  cmd: flash command to read the register of interest
  * @param  mask: register bits of interest are set to 1 in mask
  * @param  match: waits until the masked register matches this value
  * @retval None
  */
static void QSPI_flash_statuspoll_single(uint8_t cmd, uint32_t mask, uint32_t match)
{
	QUADSPI->PSMKR = mask;
	QUADSPI->PSMAR = match;
	QUADSPI->PIR = 20;																/* 20 cycles between two status reads */
	QUADSPI->CR |= QUADSPI_CR_APMS;													/* stop polling on a match */

	QUADSPI->FCR = 0x08;															/* clear SM flag */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x02 << QUADSPI_CCR_FMODE_Pos);		/* auto poll mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x01 << QUADSPI_CCR_IMODE_Pos);		/* single-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x01 << QUADSPI_CCR_DMODE_Pos);		/* single-line data */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);											/* wait for a match */
	QUADSPI->CR &= ~QUADSPI_CR_EN;
}

/**
  * @brief  Poll the status of a flash register using auto polling mode, using quad line instn and data (blocking).
  * @param  cmd: flash command to read the register of interest
  * @param  mask: register bits of interest are set to 1 in mask
  * @param  match: waits until the masked register matches this value
  * @retval None
  */
static void QSPI_flash_statuspoll_quad(uint8_t cmd, uint32_t mask, uint32_t match)
{
	QUADSPI->PSMKR = mask;
	QUADSPI->PSMAR = match;
	QUADSPI->PIR = 20;																/* 20 cycles between two status reads */
	QUADSPI->CR |= QUADSPI_CR_APMS;													/* stop polling on a match */

	QUADSPI->FCR = 0x08;															/* clear SM flag */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x02 << QUADSPI_CCR_FMODE_Pos);		/* auto poll mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);											/* wait for a match */
	QUADSPI->CR &= ~QUADSPI_CR_EN;
}

#if QSPI_USE_NONBLK_WRITES
/**
  * @brief  Poll the status of a flash register using auto polling mode, using quad line instn and data (non-blocking).
  * @param  cmd: flash command to read the register of interest
  * @param  mask: register bits of interest are set to 1 in mask
  * @param  match: waits until the masked register matches this value
  * @retval None
  */
static void QSPI_flash_statuspoll_quad_nb(uint8_t cmd, uint32_t mask, uint32_t match)
{
	QUADSPI->PSMKR = mask;
	QUADSPI->PSMAR = match;
	QUADSPI->PIR = 20;																/* 20 cycles between two status reads */
	QUADSPI->CR |= QUADSPI_CR_APMS;													/* stop polling on a match */

	QUADSPI->FCR = 0x08;															/* clear SM flag */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x02 << QUADSPI_CCR_FMODE_Pos);		/* auto poll mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	NVIC_ClearPendingIRQ(QUADSPI_IRQn);
	NVIC_EnableIRQ(QUADSPI_IRQn);													/* enable interrupt on status match */
}
#endif

#ifdef DEBUG
/**
  * @brief  Read and print a flash register value, using single line instn and data.
  * @param  cmd: flash command to read the register of interest
  * @retval None
  */
static void QSPI_flash_prnt_reg_single(uint8_t cmd)
{
	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x01 << QUADSPI_CCR_FMODE_Pos);		/* indirect read mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x01 << QUADSPI_CCR_IMODE_Pos);		/* single-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x01 << QUADSPI_CCR_DMODE_Pos);		/* single-line data */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	printf("Cmd 0x%02X response: 0x%02X\n", (unsigned int)cmd, (unsigned int)(QUADSPI->DR));
}

/**
  * @brief  Read and print a flash register value, using quad line instn and data.
  * @param  cmd: flash command to read the register of interest
  * @retval None
  */
static void QSPI_flash_prnt_reg_quad(uint8_t cmd)
{
	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x01 << QUADSPI_CCR_FMODE_Pos);		/* indirect read mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	printf("Cmd 0x%02X response: 0x%02X\n", (unsigned int)cmd, (unsigned int)(QUADSPI->DR));
}
#endif

/**
  * @brief  Read a flash register, using quad line instn and data.
  * @param  cmd: flash command to read the register of interest
  * @retval Register value
  */
static uint8_t QSPI_flash_read_reg_quad(uint8_t cmd)
{
	QUADSPI->FCR = 0x03;															/* clear TC and TE flags */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x01 << QUADSPI_CCR_FMODE_Pos);		/* indirect read mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, cmd << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	QUADSPI->DLR = 0x00;															/* 1 data byte */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x03 << QUADSPI_CCR_DMODE_Pos);		/* quad-line data */
	while(!(QUADSPI->SR & QUADSPI_SR_TCF));
	QUADSPI->CR &= ~QUADSPI_CR_EN;
	return QUADSPI->DR;
}

/**
  * @brief  Clears error bits of the flag status register of the flash, using quad line instn and data.
  * @param  None
  * @retval None
  */
static void QSPI_flash_clr_errors_quad(void)
{
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, 0x00 << QUADSPI_CCR_FMODE_Pos);		/* indirect write mode */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_IMODE, 0x03 << QUADSPI_CCR_IMODE_Pos);		/* quad-line instruction */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_ADMODE, 0x00 << QUADSPI_CCR_ADMODE_Pos);	/* no address */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DCYC, 0x00 << QUADSPI_CCR_DCYC_Pos);		/* no dummy cycles */
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_DMODE, 0x00 << QUADSPI_CCR_DMODE_Pos);		/* no data */
	QUADSPI->CR |= QUADSPI_CR_EN;
	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_INSTRUCTION, FLASH_CMD_CLR_FLAGSTATUSREG << QUADSPI_CCR_INSTRUCTION_Pos);	/* set instruction and start the command */
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CR &= ~QUADSPI_CR_EN;

	QSPI_flash_statuspoll_quad(FLASH_CMD_RD_FLAGSTATUSREG, 0x30, 0x00);				/* wait for errors to get cleared */
}

/**
  * @brief  Configure DMA for flash read.
  * @param  rxbufptr: pointer to receive buffer
  * @param  tfrsize: transfer size in bytes (max 65535)
  * @retval None
  */
static void QSPI_DMA_config_read(__IO uint8_t* rxbufptr, uint32_t tfrsize)
{
	QSPI_STREAM->CR  &= ~0x01;							/* disable stream and confirm */
	while(QSPI_STREAM->CR & 0x01);
	QSPI_DMA->HIFCR |= 0x0F400000;						/* clear stream7 flags/errors */

	QSPI_STREAM->CR = QSPI_DMA_CHANNEL | 				/* ch3, byte-size transfer, low priority,... */
								0x00000400;				/* mem inc, periph-to-mem, normal mode */
	QSPI_STREAM->NDTR = tfrsize;						/* transfer size */
	QSPI_STREAM->PAR  = (uint32_t)&(QUADSPI->DR);		/* source addr */
	QSPI_STREAM->M0AR = (uint32_t)rxbufptr;				/* destination addr */
	QSPI_STREAM->CR |= 0x01;							/* enable stream */
}

/**
  * @brief  Configure DMA for flash write.
  * @param  txbufptr: pointer to transmit buffer
  * @param  tfrsize: transfer size in bytes (max 65535)
  * @retval None
  */
static void QSPI_DMA_config_write(uint8_t* txbufptr, uint32_t tfrsize)
{
	QSPI_STREAM->CR  &= ~0x01;							/* disable stream and confirm */
	while(QSPI_STREAM->CR & 0x01);
	QSPI_DMA->HIFCR |= 0x0F400000;						/* clear stream7 flags/errors */

	QSPI_STREAM->CR = QSPI_DMA_CHANNEL | 				/* ch3, byte-size transfer, low priority,... */
								0x00000440;				/* mem inc, mem-to-periph, normal mode */
	QSPI_STREAM->NDTR = tfrsize;						/* transfer size */
	QSPI_STREAM->PAR  = (uint32_t)&(QUADSPI->DR);		/* destination addr */
	QSPI_STREAM->M0AR = (uint32_t)txbufptr;				/* source addr */
	QSPI_STREAM->CR |= 0x01;							/* enable stream */
}

/**
  * @brief  Check QSPI flash interface state.
  * @param  None
  * @retval 0 = idle, 1 = busy
  */
uint8_t QSPI_is_busy(void)
{
	return (qspiState != QSPI_STATE_IDLE);
}

#if QSPI_USE_NONBLK_WRITES
/**
  * @brief  Handle QSPI errors.
  * @param  None
  * @retval None
  */
static void QSPI_handle_error(void)
{
	while(1);	/* hang firmware */
}
#endif
