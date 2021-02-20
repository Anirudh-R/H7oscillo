/**
  ******************************************************************************
  * @file    globals.h
  * @author  anirudhr
  * @brief   Contains global variables declarations used by application.
  ******************************************************************************
  */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "stm32f7xx.h"
#include "params.h"
#include "ugui.h"


extern __IO uint32_t Tick_1ms;
extern uint32_t SystemCoreClock;

extern __IO uint8_t CH1_ADC_vals[ADC_BUF_SIZE];
extern __IO uint8_t CH2_ADC_vals[ADC_BUF_SIZE];

extern uint32_t tscale;
extern uint32_t vscale;

extern uint8_t CH1_acq_comp, CH2_acq_comp;

extern uint8_t LTDC_error_flag;
extern uint8_t DMA2D_error_flag;

#endif /* __GLOBALS_H */
