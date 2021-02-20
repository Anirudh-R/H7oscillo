/**
  ******************************************************************************
  * @file    globals.c
  * @author  anirudhr
  * @brief   Contains global variables used by application.
  ******************************************************************************
  */

#include "globals.h"
#include "ugui.h"


/* Global counter incremented every 1ms */
__IO uint32_t Tick_1ms = 0;

/* Array to store ADC readings */
__IO uint8_t CH1_ADC_vals[ADC_BUF_SIZE];
__IO uint8_t CH2_ADC_vals[ADC_BUF_SIZE];

uint32_t tscale = 0;			/* Horizontal scale of oscilloscope */
uint32_t vscale = 1;			/* Vertical scale of oscilloscope */

uint8_t CH1_acq_comp = 0;		/* ADC Acquisition complete flags */
uint8_t	CH2_acq_comp = 0;

/* Error flags */
uint8_t LTDC_error_flag = 0;
uint8_t DMA2D_error_flag = 0;





