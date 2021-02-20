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

uint32_t tscale = 0;			/* Time (horizontal) scale of oscilloscope */
uint32_t vscale = 1;			/* Vertical scale of oscilloscope */

uint8_t CH1_acq_comp = 0, CH2_acq_comp = 0;		/* ADC Acquisition complete flags */

uint8_t TS_read_pending = 0;	/* Pending touch screen read */
uint16_t TS_X, TS_Y;			/* XY coordinates of touch point */

/* Error flags */
uint8_t LTDC_error_flag = 0;
uint8_t DMA2D_error_flag = 0;

/* uGUI related globals */
UG_GUI gui;
/* window 1 */
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[UGUI_MAX_OBJECTS];
UG_BUTTON button1_0;
UG_BUTTON button1_1;
UG_BUTTON button1_2;
UG_BUTTON button1_3;
UG_BUTTON button1_4;
