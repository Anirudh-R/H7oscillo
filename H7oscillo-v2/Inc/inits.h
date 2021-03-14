/**
  ******************************************************************************
  * @file    inits.h
  * @author  anirudhr
  * @brief	 Header file for inits.c.
  ******************************************************************************
  */

#ifndef __INITS_H
#define __INITS_H

#include "main.h"


extern void Timers_init(void);
extern void TimQE_init(void);
extern void TimADC_init(int8_t tbase);
extern void TimTS_init(void);
extern void ADC_init(void);
extern void ADC_Ch1_init(void);
extern void ADC_Ch2_init(void);
extern void LTDC_init(void);
extern void DMA2D_init(void);
extern void SDRAM_init(void);
extern void LEDs_Buttons_init(void);
extern void I2C_TS_init(void);
extern void SystemClock_Config(void);
extern void CPU_CACHE_Enable(void);

#endif /* __INITS_H */
