/**
  ******************************************************************************
  * @file    inits.c
  * @author  anirudhr
  * @brief   Contains initialization functions for peripherals.
  ******************************************************************************
  */

#include "inits.h"


/**
  * @brief  Configures all timers needed for application.
  * @param  None
  * @retval None
  */
void Timers_init(void)
{
	TimQE_init();

	TimADC_init(tscale);

	TimTS_init();

	TimMeas_init();
}

/**
  * @brief  Configures timer for quadrature encoder operation.
  * @param  None
  * @retval None
  */
void TimQE_init(void)
{
	TIM_QE_GPIO_CLK_ENABLE();

	LL_GPIO_SetPinMode(TIM_QE_INP_PORT, TIM_QE_INP1_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(TIM_QE_INP_PORT, TIM_QE_INP2_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(TIM_QE_INP_PORT, TIM_QE_INP1_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinSpeed(TIM_QE_INP_PORT, TIM_QE_INP2_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(TIM_QE_INP_PORT, TIM_QE_INP1_PIN | TIM_QE_INP2_PIN, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(TIM_QE_INP_PORT, TIM_QE_INP1_PIN, LL_GPIO_PULL_UP);
	LL_GPIO_SetPinPull(TIM_QE_INP_PORT, TIM_QE_INP2_PIN, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_0_7(TIM_QE_INP_PORT, TIM_QE_INP1_PIN, LL_GPIO_AF_3);
	LL_GPIO_SetAFPin_0_7(TIM_QE_INP_PORT, TIM_QE_INP2_PIN, LL_GPIO_AF_3);

	/* Configure TIM_QE for encoder mode operation */
	TIM_QE_CLK_ENABLE();
	TIM_QE->CCER &= (uint32_t)~(TIM_CCER_CC1E | TIM_CCER_CC2E);		/* disable CC1 & CC2 */
	TIM_QE->CCMR1 = 0x00000101;										/* no filter, no prescalar, direct mapping of inputs */
	TIM_QE->CCER  = 0x00000002;										/* INP1 & INP2 are inverted */
	TIM_QE->SMCR  = 0x00000001;										/* count up/down on CC1 edge depending on CC2 level */
	TIM_QE->CCER |= (uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E);		/* enable CC1 & CC2 */
	TIM_QE->CR1  |= 0x01;											/* start timer */
}

/**
  * @brief  Configures timer for ADC conversion triggering operation.
  * @param  tbase: parameter to set sampling rate
  * 				0. 21.6us/div  - 2.222MHz - 0x2D
  * 				1. 25us/div    - 1.923MHz - 0x34
  * 				2. 50us/div    - 0.962MHz - 0x68
  * 				3. 100us/div   - 0.481MHz - 0xD0
  * 				4. 200us/div   - 0.24MHz  - 0x1A1
  * 				5. 500us/div   - 0.096MHz - 0x412
  * 				6. 1ms/div     - 0.048MHz - 0x823
  * @retval None
  */
void TimADC_init(int8_t tbase)
{
	uint32_t div;		/* timer divider value */

	switch(tbase)
	{
		case 0: div = 0x2D;  break;
		case 1: div = 0x34;  break;
		case 2: div = 0x68;  break;
		case 3: div = 0xD0;  break;
		case 4: div = 0x1A1; break;
		case 5: div = 0x412; break;
		case 6: div = 0x823; break;
		default: div = 0x2D; break;
	}

	TIM_ADC_CLK_ENABLE();
	TIM_ADC->CR1 &= ~(uint32_t)0x01;	/* disable timer */
	TIM_ADC->CR2  = 0x00200000;			/* TRGO2 generated on updates */
	TIM_ADC->PSC  = 0x00;				/* counter_clk = timer_inp_clk/1 = 100MHz */
	TIM_ADC->ARR  = div - 1;		    /* default sampling freq = counter_clk/0x2D = 2.222MHz */
	TIM_ADC->CR1 |= 0x01;				/* start timer */
}

/**
  * @brief  Configures timer for Touch screen polling.
  * @param  None
  * @retval None
  */
void TimTS_init(void)
{
	/* Configure TIM_TS for 20Hz frequency */
	TIM_TS_CLK_ENABLE();
	TIM_TS->CR1 &= ~(uint32_t)0x01;		/* disable timer */
	TIM_TS->PSC  = 10000 - 1;			/* counter_clk = timer_inp_clk/10000 = 100MHz/10000 = 10KHz */
	TIM_TS->ARR  = 500 - 1;		    	/* Touch screen polling frequency = counter_clk/500 = 20Hz */
	TIM_TS->DIER |= 0x01;				/* Enable interrupt on counter overflow */
	TIM_TS->CR1 |= 0x01;				/* start timer */

	NVIC_SetPriority(TIM4_IRQn, 4);
	NVIC_EnableIRQ(TIM4_IRQn);
}

/**
  * @brief  Configures timer for calculating measurements.
  * @param  None
  * @retval None
  */
void TimMeas_init(void)
{
	/* Configure TIM_MEAS for 20Hz frequency */
	TIM_MEAS_CLK_ENABLE();
	TIM_MEAS->CR1 &= ~(uint32_t)0x01;	/* disable timer */
	TIM_MEAS->PSC  = 10000 - 1;			/* counter_clk = timer_inp_clk/10000 = 100MHz/10000 = 10KHz */
	TIM_MEAS->ARR  = 2500 - 1;		    /* Touch screen polling frequency = counter_clk/2500 = 4Hz */
	TIM_MEAS->DIER |= 0x01;				/* Enable interrupt on counter overflow */
	TIM_MEAS->CR1 |= 0x01;				/* start timer */

	NVIC_SetPriority(TIM3_IRQn, 4);
	NVIC_EnableIRQ(TIM3_IRQn);
}

/**
  * @brief  Configures ADCs and associated DMAs.
  * @param  None
  * @retval None
  */
void ADC_init(void)
{
	/* Configure Ch1 */
	ADC_Ch1_init();

	/* Configure Ch2 */
	ADC_Ch2_init();
}

/**
  * @brief  Configures ADC & DMA for Ch1 of oscilloscope.
  * @param  None
  * @retval None
  */
void ADC_Ch1_init(void)
{
	/* Configure DMA for servicing ADC */
	CH1_ADC_DMA_CLK_ENABLE();
	CH1_ADC_DMA_STREAM->CR  &= ~(uint32_t)0x01;				/* disable stream and confirm */
	while(CH1_ADC_DMA_STREAM->CR & 0x01);
	CH1_ADC_DMA->LIFCR      |= 0x0000003D;					/* clear stream0 flags/errors */

	CH1_ADC_DMA_STREAM->CR = CH1_ADC_DMA_CHANNEL | 			/* ch0, byte-size transfer, high priority,... */
								0x00020510;					/* mem inc, periph-to-mem, TCIE=1, 0x00020500(circular), 0x00020410(normal) */
	CH1_ADC_DMA_STREAM->NDTR = (uint32_t)ADC_BUF_SIZE;		/* transfer size */
	CH1_ADC_DMA_STREAM->PAR  = (uint32_t)&(CH1_ADC->DR);	/* source addr */
	CH1_ADC_DMA_STREAM->M0AR = (uint32_t)CH1_ADC_vals;		/* destination addr */
	CH1_ADC_DMA_STREAM->CR  |= 0x01;						/* enable stream */

	NVIC_SetPriority(CH1_ADC_DMA_IRQn, 2);
	NVIC_EnableIRQ(CH1_ADC_DMA_IRQn);

	/*
	 * Configure ADC channel for TIM1 triggered conversion at 8-bits resolution.
	 * ADCCLK = APB2/4 = 25MHz. Each channel takes 8+3 = 11 cycles to be
	 * converted.
	 */
	CH1_ADC_INP_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(CH1_ADC_INP_GPIO_PORT, CH1_ADC_INP_PIN, LL_GPIO_MODE_ANALOG);

	CH1_ADC_CLK_ENABLE();
	CH1_ADC->CR2 &= ~(uint32_t)0x01;	/* turn off ADC */

	CH1_ADC->CR1  = 0x02000000;		    /* 8-bits resolution */
	CH1_ADC->CR2  = 0x1A000300;			/* triggered by TIM1 TRGO2, rising edge, DMA enabled, DDS=1, EOCS=0 */
	CH1_ADC->SQR3 = CH1_ADC_CHANNEL;    /* set channel */
	CH1_ADC->SMPR2= LL_ADC_SAMPLINGTIME_3CYCLES << 0U;
	ADC->CCR      = 0x00010000;			/* ADCCLK = APB2CLK/4 */

	CH1_ADC->CR2 |= 0x01;				/* turn on ADC */
}

/**
  * @brief  Reinitialize Ch1 ADC & DMA after disabling.
  * @param  None
  * @retval None
  */
void ADC_Ch1_reinit(void)
{
	CH1_ADC->CR2 &= ~0x01;				/* turn off ADC */

	/* Reinitialize DMA for servicing ADC */
	CH1_ADC_DMA_STREAM->CR = CH1_ADC_DMA_CHANNEL | 0x00020510;
	CH1_ADC_DMA_STREAM->NDTR = (uint32_t)ADC_BUF_SIZE;		/* transfer size */
	CH1_ADC_DMA_STREAM->CR |= 0x01;							/* enable stream */

	/* Reinitialize ADC */
	CH1_ADC->SR &= ~(1 << 5);			/* clear overflow bit */
	CH1_ADC->CR2 |= 0x01;				/* turn on ADC */
}

/**
  * @brief  Configures ADC & DMA for Ch2 of oscilloscope.
  * @param  None
  * @retval None
  */
void ADC_Ch2_init(void)
{
	/* Configure DMA for servicing ADC */
	CH2_ADC_DMA_CLK_ENABLE();
	CH2_ADC_DMA_STREAM->CR  &= ~(uint32_t)0x01;				/* disable stream and confirm */
	while(CH2_ADC_DMA_STREAM->CR & 0x01);
	CH2_ADC_DMA->LIFCR      |= 0x00000F40;					/* clear stream1 flags/errors */

	CH2_ADC_DMA_STREAM->CR = CH2_ADC_DMA_CHANNEL | 			/* ch2, byte-size transfer, high priority,... */
								0x00020510;					/* mem inc, periph-to-mem, TCIE=1, 0x00020500(circular), 0x00020410(normal) */
	CH2_ADC_DMA_STREAM->NDTR = (uint32_t)ADC_BUF_SIZE;		/* transfer size */
	CH2_ADC_DMA_STREAM->PAR  = (uint32_t)&(CH2_ADC->DR);	/* source addr */
	CH2_ADC_DMA_STREAM->M0AR = (uint32_t)CH2_ADC_vals;		/* destination addr */
	CH2_ADC_DMA_STREAM->CR  |= 0x01;						/* enable stream */

	NVIC_SetPriority(CH2_ADC_DMA_IRQn, 2);
	NVIC_EnableIRQ(CH2_ADC_DMA_IRQn);

	/*
	 * Configure ADC channel for TIM1 triggered conversion at 8-bits resolution.
	 * ADCCLK = APB2/4 = 25MHz. Each channel takes 8+3 = 11 cycles to be
	 * converted.
	 */
	CH2_ADC_INP_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(CH2_ADC_INP_GPIO_PORT, CH2_ADC_INP_PIN, LL_GPIO_MODE_ANALOG);

	CH2_ADC_CLK_ENABLE();
	CH2_ADC->CR2 &= ~(uint32_t)0x01;	/* turn off ADC */

	CH2_ADC->CR1  = 0x02000000;		    /* 8-bits resolution */
	CH2_ADC->CR2  = 0x1A000300;			/* triggered by TIM1 TRGO2, rising edge, DMA enabled, DDS=1, EOCS=0 */
	CH2_ADC->SQR3 = CH2_ADC_CHANNEL;    /* set channel */
	CH2_ADC->SMPR2= LL_ADC_SAMPLINGTIME_3CYCLES << 24U;
	ADC->CCR      = 0x00010000;			/* ADCCLK = APB2CLK/4 */

	CH2_ADC->CR2 |= 0x01;				/* turn on ADC */
}

/**
  * @brief  Reinitialize Ch2 ADC & DMA after disabling.
  * @param  None
  * @retval None
  */
void ADC_Ch2_reinit(void)
{
	CH2_ADC->CR2 &= ~0x01;				/* turn off ADC */

	/* Reinitialize DMA for servicing ADC */
	CH2_ADC_DMA_STREAM->CR = CH2_ADC_DMA_CHANNEL | 0x00020510;
	CH2_ADC_DMA_STREAM->NDTR = (uint32_t)ADC_BUF_SIZE;		/* transfer size */
	CH2_ADC_DMA_STREAM->CR |= 0x01;							/* enable stream */

	/* Reinitialize ADC */
	CH2_ADC->SR &= ~(1 << 5);			/* clear overflow bit */
	CH2_ADC->CR2 |= 0x01;				/* turn on ADC */
}

/**
  * @brief  Configures all LEDs and Buttons needed for application.
  * @param  None
  * @retval None
  */
void LEDs_Buttons_init(void)
{
	/* Configure user LED */
	LED1_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_ResetOutputPin(LED1_GPIO_PORT, LED1_PIN);

	/* Configure user button interrupt */
	USER_BUTTON_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinSpeed(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	USER_BUTTON_SYSCFG_SET_EXTI();
	USER_BUTTON_EXTI_LINE_ENABLE();
	USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();
	LL_EXTI_ClearFlag_0_31(USER_BUTTON_EXTI_LINE);
	NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 14);
	NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);

	/* Configure the switch input from QE as interrupt */
	QE_PB_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(QE_PB_GPIO_PORT, QE_PB_PIN, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinSpeed(QE_PB_GPIO_PORT, QE_PB_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinPull(QE_PB_GPIO_PORT, QE_PB_PIN, LL_GPIO_PULL_UP);
	QE_PB_SYSCFG_SET_EXTI();
	QE_PB_EXTI_LINE_ENABLE();
	QE_PB_EXTI_RISING_TRIG_ENABLE();
	LL_EXTI_ClearFlag_0_31(QE_PB_EXTI_LINE);
	NVIC_SetPriority(QE_IRQn, 14);
	NVIC_EnableIRQ(QE_IRQn);
}

/**
  * @brief  Configures all clocks and GPIOs for LTDC and initializes it.
  * @param  None
  * @retval None
  */
void LTDC_init(void)
{
	/* Generate LCD_CLK = 9.6MHz */
	LL_RCC_PLLSAI_Disable();

	MODIFY_REG(RCC->PLLSAICFGR, RCC_PLLSAICFGR_PLLSAIN | RCC_PLLSAICFGR_PLLSAIR, (192U << RCC_PLLSAICFGR_PLLSAIN_Pos) | LL_RCC_PLLSAIR_DIV_5);
	MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVR, LL_RCC_PLLSAIDIVR_DIV_4);

	LL_RCC_PLLSAI_Enable();
	while(!LL_RCC_PLLSAI_IsReady()){
	}

	/* LTDC GPIOs configuration */
	 /* GPIOE 4 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	GPIOE->MODER &= ~(0x3<<8); 	GPIOE->MODER |= (0x2<<8);
	GPIOE->OSPEEDR &= ~(0x3<<8); GPIOE->OSPEEDR |= (0x2<<8);
	GPIOE->AFR[0] &= ~(0xF<<16); GPIOE->AFR[0] |= (0xE<<16);

	 /* GPIOG 12 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
	GPIOG->MODER &= ~(0x3<<24); GPIOG->MODER |= (0x2<<24);
	GPIOG->OSPEEDR &= ~(0x3<<24); GPIOG->OSPEEDR |= (0x2<<24);
	GPIOG->AFR[1] &= ~(0xF<<16); GPIOG->AFR[1] |= (0xE<<16);

	 /* GPIOI 9, 10, 13-15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI);
	GPIOI->MODER &= ~(0x3<<18 | 0x3<<20 | 0x3<<26 | 0x3<<28 | 0x3<<30);
	GPIOI->MODER |= (0x2<<18 | 0x2<<20 | 0x2<<26 | 0x2<<28 | 0x2<<30);
	GPIOI->OSPEEDR &= ~(0x3<<18 | 0x3<<20 | 0x3<<26 | 0x3<<28 | 0x3<<30);
	GPIOI->OSPEEDR |= (0x2<<18 | 0x2<<20 | 0x2<<26 | 0x2<<28 | 0x2<<30);
	GPIOI->AFR[1] &= ~(0xF<<4 | 0xF<<8 | 0xF<<20 | 0xF<<24 | 0xF<<28);
	GPIOI->AFR[1] |= (0xE<<4 | 0xE<<8 | 0xE<<20 | 0xE<<24 | 0xE<<28);

	 /* GPIOJ 0-11, 13-15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOJ);
	GPIOJ->MODER &= ~(0xFCFFFFFF); GPIOJ->MODER |= (0xA8AAAAAA);
	GPIOJ->OSPEEDR &= ~(0xFCFFFFFF); GPIOJ->OSPEEDR |= (0xA8AAAAAA);
	GPIOJ->AFR[0] &= ~(0xFFFFFFFF); GPIOJ->AFR[0] |= (0xEEEEEEEE);
	GPIOJ->AFR[1] &= ~(0xFFF0FFFF); GPIOJ->AFR[1] |= (0xEEE0EEEE);

	/* GPIOK 0-2, 4-7 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOK);
	GPIOK->MODER &= ~(0x3<<0 | 0x3<<2 | 0x3<<4 | 0x3<<8 | 0x3<<10 | 0x3<<12 | 0x3<<14);
	GPIOK->MODER |= (0x2<<0 | 0x2<<2 | 0x2<<4 | 0x2<<8 | 0x2<<10 | 0x2<<12 | 0x2<<14);
	GPIOK->OSPEEDR &= ~(0x3<<0 | 0x3<<2 | 0x3<<4 | 0x3<<8 | 0x3<<10 | 0x3<<12 | 0x3<<14);
	GPIOK->OSPEEDR |= (0x2<<0 | 0x2<<2 | 0x2<<4 | 0x2<<8 | 0x2<<10 | 0x2<<12 | 0x2<<14);
	GPIOK->AFR[0] &= ~(0xF<<0 | 0xF<<4 | 0xF<<8 | 0xF<<16 | 0xF<<20 | 0xF<<24 | 0xF<<28);
	GPIOK->AFR[0] |= (0xE<<0 | 0xE<<4 | 0xE<<8 | 0xE<<16 | 0xE<<20 | 0xE<<24 | 0xE<<28);

	/* GPIOI 12 (disp en) */
	GPIOI->MODER &= ~(0x3<<24); GPIOI->MODER |= (0x1<<24);

	/* GPIOK 3 (bl ctrl) */
	GPIOK->MODER &= ~(0x3<<6); GPIOK->MODER |= (0x1<<6);

	/* Enable display and backlight */
	LL_GPIO_SetOutputPin(GPIOI, LL_GPIO_PIN_12);
	LL_GPIO_SetOutputPin(GPIOK, LL_GPIO_PIN_3);

	/* LTDC initialization */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_LTDC);

	LTDC->GCR = 0x00002220;																			/* all signals active LOW, dithering disabled */
	LTDC->SSCR = (RK043FN48H_HSYNC-1)<<16 | (RK043FN48H_VSYNC-1);									/* sync pulse widths */
	LTDC->BPCR = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1)<<16 | 										/* accumulated backporch */
					(RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
	LTDC->AWCR = (RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_WIDTH - 1)<<16 | 					/* accumulated active width */
					(RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_HEIGHT - 1);
	LTDC->TWCR = (RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_WIDTH + RK043FN48H_HFP - 1)<<16 | 	/* total width */
					(RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_HEIGHT + RK043FN48H_VFP - 1);
	LTDC->BCCR = (255U << 16) | (0U << 8) | (0U);		/* background color */
	LTDC->ICR |= 0x0F;									/* clear all flags */
	LTDC->IER |= 0x06;									/* enable transfer and FIFO underrun interrupts */

	/* Layer1 initialization */
	LTDC_Layer1->WHPCR = (RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_WIDTH - 1)<<16 |
								(RK043FN48H_HSYNC + RK043FN48H_HBP);
	LTDC_Layer1->WVPCR = (RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_HEIGHT - 1)<<16 |
								(RK043FN48H_VSYNC + RK043FN48H_VBP);
	LTDC_Layer1->PFCR = 0x02;							/* RGB565 */
	LTDC_Layer1->CACR = 0xFF;							/* constant layer alpha value */
	LTDC_Layer1->BFCR = (0x5)<<0 | (0x4)<<8;			/* constant_alpha, 1-constant_alpha */
	LTDC_Layer1->CFBAR = LCD_FRAME_BUFFER;				/* frame buffer address */
	LTDC_Layer1->CFBLR = (2*RK043FN48H_WIDTH)<<16 | (2*RK043FN48H_WIDTH + 3)<<0;
	LTDC_Layer1->CFBLNR = RK043FN48H_HEIGHT;
	LTDC_Layer1->CR |= 0x01;							/* enable layer1 */
	LTDC->SRCR |= 0x01;									/* immediate reload */
	LTDC->GCR  |= 0x01;									/* enable LTDC */

	NVIC_SetPriority(LTDC_ER_IRQn, 2);
	NVIC_EnableIRQ(LTDC_ER_IRQn);
}

/**
  * @brief  SDRAM config & initialization for 100MHz operation.
  * @param  None
  * @retval None
  */
void SDRAM_init(void)
{
	LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_FMC);

	/* FMC GPIOs configuration */
	/* GPIOC 3 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	GPIOC->MODER &= ~(0x3<<6); GPIOC->MODER |= (0x2<<6);
	GPIOC->OSPEEDR &= ~(0x3<<6); GPIOC->OSPEEDR |= (0x2<<6);
	GPIOC->AFR[0] &= ~(0xF<<12); GPIOC->AFR[0] |= (0xC<<12);

	/* GPIOD 0, 1, 8-10, 14, 15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	GPIOD->MODER &= ~(0x3<<0 | 0x3<<2 | 0x3<<16 | 0x3<<18 | 0x3<<20 | 0x3<<28 | 0x3<<30);
	GPIOD->MODER |= (0x2<<0 | 0x2<<2 | 0x2<<16 | 0x2<<18 | 0x2<<20 | 0x2<<28 | 0x2<<30);
	GPIOD->OSPEEDR &= ~(0x3<<0 | 0x3<<2 | 0x3<<16 | 0x3<<18 | 0x3<<20 | 0x3<<28 | 0x3<<30);
	GPIOD->OSPEEDR |= (0x2<<0 | 0x2<<2 | 0x2<<16 | 0x2<<18 | 0x2<<20 | 0x2<<28 | 0x2<<30);
	GPIOD->AFR[0] &= ~(0xF<<0 | 0xF<<4);
	GPIOD->AFR[0] |= (0xC<<0 | 0xC<<4);
	GPIOD->AFR[1] &= ~(0xF<<0 | 0xF<<4 | 0xF<<8 | 0xF<<24 | 0xF<<28);
	GPIOD->AFR[1] |= (0xC<<0 | 0xC<<4 | 0xC<<8 | 0xC<<24 | 0xC<<28);

	/* GPIOE 0, 1, 7-15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	GPIOE->MODER &= ~(0xFFFFC00F); GPIOE->MODER |= (0xAAAA800A);
	GPIOE->OSPEEDR &= ~(0xFFFFC00F); GPIOE->OSPEEDR |= (0xAAAA800A);
	GPIOE->AFR[0] &= ~(0xF00000FF); GPIOE->AFR[0] |= (0xC00000CC);
	GPIOE->AFR[1] &= ~(0xFFFFFFFF); GPIOE->AFR[1] |= (0xCCCCCCCC);

	/* GPIOF 0-5, 11-15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
	GPIOF->MODER &= ~(0xFFC00FFF); GPIOF->MODER |= (0xAA800AAA);
	GPIOF->OSPEEDR &= ~(0xFFC00FFF); GPIOF->OSPEEDR |= (0xAA800AAA);
	GPIOF->AFR[0] &= ~(0x00FFFFFF); GPIOF->AFR[0] |= (0x00CCCCCC);
	GPIOF->AFR[1] &= ~(0xFFFFF000); GPIOF->AFR[1] |= (0xCCCCC000);

	/* GPIOG 0, 1, 4, 5, 8, 15 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
	GPIOG->MODER &= ~(0x3<<0 | 0x3<<2 | 0x3<<8 | 0x3<<10 | 0x3<<16 | 0x3<<30);
	GPIOG->MODER |= (0x2<<0 | 0x2<<2 | 0x2<<8 | 0x2<<10 | 0x2<<16 | 0x2<<30);
	GPIOG->OSPEEDR &= ~(0x3<<0 | 0x3<<2 | 0x3<<8 | 0x3<<10 | 0x3<<16 | 0x3<<30);
	GPIOG->OSPEEDR |= (0x2<<0 | 0x2<<2 | 0x2<<8 | 0x2<<10 | 0x2<<16 | 0x2<<30);
	GPIOG->AFR[0] &= ~(0xF<<0 | 0xF<<4 | 0xF<<16 | 0xF<<20);
	GPIOG->AFR[0] |= (0xC<<0 | 0xC<<4 | 0xC<<16 | 0xC<<20);
	GPIOG->AFR[1] &= ~(0xF<<0 | 0xF<<28);
	GPIOG->AFR[1] |= (0xC<<0 | 0xC<<28);

	/* GPIOH 3, 5 */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
	GPIOH->MODER &= ~(0x3<<6 | 0x03<<10); GPIOH->MODER |= (0x2<<6 | 0x02<<10);
	GPIOH->OSPEEDR &= ~(0x3<<6 | 0x03<<10); GPIOH->OSPEEDR |= (0x2<<6 | 0x02<<10);
	GPIOH->AFR[0] &= ~(0xF<<12 | 0xF<<20); GPIOH->AFR[0] |= (0xC<<12 | 0xC<<20);

	/* Bank 1 configuration */
	FMC_Bank5_6->SDCR[0] = (0x00)<<13 	|	/* RPIPE = 0 */
						   (0x01)<<12 	|	/* Burst enable */
						   (0x02)<<10	|	/* SDCLK = 2*HCLK */
						   (0x00)<<9	|	/* write prot disable */
						   (0x02)<<7	|	/* CAS latency = 2 cycles */
						   (0x01)<<6	|	/* device has 4 internal banks */
						   (0x01)<<4	|	/* 16-bit data bus */
						   (0x01)<<2	|	/* 12 row addr bits */
						   (0x00)<<0;		/* 8 col addr bits */

	FMC_Bank5_6->SDTR[0] = (0x01)<<24	|	/* Row-to-col delay = 2 */
						   (0x01)<<20	|	/* Row prech delay = 2 */
						   (0x01)<<16	|	/* Recovery delay = 2 */
						   (0x06)<<12	|	/* Row cycle delay = 7 */
						   (0x03)<<8	|	/* Self-refresh time = 4 */
						   (0x06)<<4	|	/* Exit self-refresh delay = 7 */
						   (0x01)<<0;		/* Load-to-active delay = 2 */

	/* SDRAM device initialization */
	/* Clock Configuration Enable */
	FMC_Bank5_6->SDCMR =   (0x00)<<9	|
						   (0x00)<<5	|
						   (0x01)<<4	|	/* cmd target Bank1 */
						   (0x00)<<3	|
						   (0x01)<<0;		/* cmd mode */
	while(FMC_Bank5_6->SDSR & 0x20);		/* wait for cmd to complete */
	LL_mDelay(1);							/* wait atleast 100us */

	/* Precharge all banks */
	FMC_Bank5_6->SDCMR =   (0x00)<<9	|
						   (0x00)<<5	|
						   (0x01)<<4	|	/* cmd target Bank1 */
						   (0x00)<<3	|
						   (0x02)<<0;		/* cmd mode */
	while(FMC_Bank5_6->SDSR & 0x20);		/* wait for cmd to complete */

	/* Set no. of Auto-refresh cycles */
	FMC_Bank5_6->SDCMR =   (0x00)<<9	|
						   (0x07)<<5	|	/* no. of auto-refresh (8 cycles) */
						   (0x01)<<4	|	/* cmd target Bank1 */
						   (0x00)<<3	|
						   (0x03)<<0;		/* cmd mode */
	while(FMC_Bank5_6->SDSR & 0x20);		/* wait for cmd to complete */

	/* Load mode register */
	FMC_Bank5_6->SDCMR =   (0x220)<<9	|	/* mode reg definition */
						   (0x00)<<5	|
						   (0x01)<<4	|	/* cmd target Bank1 */
						   (0x00)<<3	|
						   (0x04)<<0;		/* cmd mode */
	while(FMC_Bank5_6->SDSR & 0x20);		/* wait for cmd to complete */

	/* Set refresh rate */
	FMC_Bank5_6->SDRTR =   (0x0603)<<1;
}

/**
  * @brief  DMA2D initialization.
  * @param  None
  * @retval None
  */
void DMA2D_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2D);

	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OCOLR = C_BLACK;					/* fill uGUI draw buffer with black */

	DMA2D->OMAR	= LCD_DRAW_BUFFER_UGUI;		/* output addr */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->NLR = (RK043FN48H_WIDTH)<<16 |	/* frame dimensions */
					(RK043FN48H_HEIGHT)<<0;

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */

	/* wait for the transfer to complete */
	while(DMA2D->CR & 0x01);

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OCOLR = C_BLACK;					/* fill Wave draw buffer with black */

	DMA2D->OMAR	= LCD_DRAW_BUFFER_WAVE;		/* output addr */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->NLR = (RK043FN48H_WIDTH)<<16 |	/* frame dimensions */
					(RK043FN48H_HEIGHT)<<0;

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */

	NVIC_SetPriority(DMA2D_IRQn, 2);
	NVIC_EnableIRQ(DMA2D_IRQn);
}

/**
  * @brief  Touch screen I2C initialization.
  * @param  None
  * @retval None
  */
void I2C_TS_init(void)
{
	/* Configure SCL/SDA pins */
	TS_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();
	LL_GPIO_SetPinMode(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SCL_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SDA_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SCL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinSpeed(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SDA_PIN, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SCL_PIN | TS_I2Cx_SDA_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetAFPin_0_7(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SCL_PIN, TS_I2Cx_SCL_SDA_AF);
	LL_GPIO_SetAFPin_8_15(TS_I2Cx_SCL_SDA_GPIO_PORT, TS_I2Cx_SDA_PIN, TS_I2Cx_SCL_SDA_AF);

	TS_I2Cx_CLK_ENABLE();
	TS_I2Cx_FORCE_RESET();
	TS_I2Cx_RELEASE_RESET();

	TS_I2Cx->TIMINGR = I2Cx_TIMING;
	TS_I2Cx->CR2 	|= LL_I2C_MODE_AUTOEND;
	TS_I2Cx->CR1 	|= I2C_CR1_PE;			 /* Enable I2C */
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Enable HSE */
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1){
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 400, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1){
  }

  /* Enable PWR clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* Activation OverDrive Mode */
  LL_PWR_EnableOverDriveMode();
  while(LL_PWR_IsActiveFlag_OD() != 1){
  }

  /* Activation OverDrive Switching */
  LL_PWR_EnableOverDriveSwitching();
  while(LL_PWR_IsActiveFlag_ODSW() != 1){
  }

  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_6);

  /* Check if the new latency value has taken effect */
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_6){
  }

  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL){
  }

  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

  /* Set systick to 1ms */
  SysTick_Config(200000000 / 1000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = 200000000;
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}
