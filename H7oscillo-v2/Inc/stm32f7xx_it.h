/**
  ******************************************************************************
  * @file    stm32f7xx_it.h
  * @author  anirudhr
  * @brief   Header file for stm32f7xx_it.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_IT_H
#define __STM32F7xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

 /* Includes */
#include "main.h"


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/* Peripheral interrupt handlers */
void QE_IRQHandler(void);
void User_Button_IRQHandler(void);
void CH1_ADC_DMA_IRQHandler(void);
void CH2_ADC_DMA_IRQHandler(void);
void QSPI_DMA_IRQHandler(void);
void LTDC_ER_IRQHandler(void);
void DMA2D_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM3_IRQHandler(void);
void QUADSPI_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_IT_H */
