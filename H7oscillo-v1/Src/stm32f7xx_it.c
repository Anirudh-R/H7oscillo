/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @author  anirudhr
  * @brief   Exception & Interrupt handlers.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_it.h"


/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	Tick_1ms++;
}

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles Quadrature encoder module push-button event.
  * @param  None
  * @retval None
  */
void QE_IRQHandler(void)
{
	static uint32_t prev_time = 0;
	uint32_t curr_time;

	LL_EXTI_ClearFlag_0_31(QE_PB_EXTI_LINE);

	curr_time = Get_tick();
	if(curr_time - prev_time > QE_PB_DEBOUNCE_TIMEOUT) {
		QE_PB_interrupt = 1;
		prev_time = curr_time;
	}
}

/**
  * @brief  This function handles User push-button event.
  * @param  None
  * @retval None
  */
void User_Button_IRQHandler(void)
{
	LL_EXTI_ClearFlag_0_31(USER_BUTTON_EXTI_LINE);
	LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

/**
  * @brief  This function handles ADC Ch1 DMA irq.
  * @param  None
  * @retval None
  */
void CH1_ADC_DMA_IRQHandler(void)
{
	CH1_ADC_DMA_STREAM->CR &= ~0x01;		/* disable stream and confirm */
	while(CH1_ADC_DMA_STREAM->CR & 0x01);

	CH1_acq_comp = (CH1_ADC_DMA->LISR & (1 << 5)) != 0;
	CH1_ADC_DMA->LIFCR      |= 0x0000003D;	/* clear flags/errors */
}

/**
  * @brief  This function handles ADC Ch2 DMA irq.
  * @param  None
  * @retval None
  */
void CH2_ADC_DMA_IRQHandler(void)
{
	CH2_ADC_DMA_STREAM->CR &= ~0x01;		/* disable stream and confirm */
	while(CH2_ADC_DMA_STREAM->CR & 0x01);

	CH2_acq_comp = (CH2_ADC_DMA->LISR & (1 << 11)) != 0;
	CH2_ADC_DMA->LIFCR      |= 0x00000F40;	/* clear flags/errors */
}

/**
  * @brief  This function handles LTDC underrun/transfer errors.
  * @param  None
  * @retval None
  */
void LTDC_ER_IRQHandler(void)
{
	LTDC->IER &= ~0x06;			/* disable transfer and FIFO underrun interrupts */
	LTDC->ICR |= 0x0F;			/* clear all flags */

	LTDC_error_flag = 1;
}

/**
  * @brief  This function handles DMA2D transfer error.
  * @param  None
  * @retval None
  */
void DMA2D_IRQHandler(void)
{
	DMA2D->CR &= ~0x0100;		/* disable transfer interrupts */
	DMA2D->IFCR |= 0x3F;		/* clear all flags */

	DMA2D_error_flag = 1;
}

/**
  * @brief  This function handles TIM4 interrupts.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
	TS_read_pending = 1;
	TIM_TS->SR &= ~0x01;
}

/**
  * @brief  This function handles TIM3 interrupts.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	measPending = 1;
	TIM_MEAS->SR &= ~0x01;
}
