/**
  ******************************************************************************
  * @file    globals.h
  * @author  anirudhr
  * @brief   Contains global variables declarations used by application.
  ******************************************************************************
  */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "main.h"


/* possible voltage scale values */
#define VSCALE_10mV			0.01609f
#define VSCALE_50mV			0.08049f
#define VSCALE_100mV		0.16099f
#define VSCALE_200mV		0.32197f
#define VSCALE_500mV		0.80492f
#define VSCALE_1V			1.60984f
#define VSCALE_2V			3.21968f
#define VSCALE_4V			6.43936f
#define VSCALE_MAXVALS		8

/* possible time scale values */
#define TSCALE_21_6us		0
#define TSCALE_25us			1
#define TSCALE_50us			2
#define TSCALE_100us		3
#define TSCALE_200us		4
#define TSCALE_500us		5
#define TSCALE_1ms			6
#define TSCALE_MAXVALS		7

/* possible trigger source values */
#define TRIGSRC_CH1			0
#define TRIGSRC_CH2			1
#define TRIGSRC_MAXVALS		2

/* possible trigger type values */
#define TRIGTYPE_RIS		0
#define TRIGTYPE_FAL		1
#define TRIGTYPE_EDGE		2
#define TRIGTYPE_MAXVALS	3

/* possible trigger mode values */
#define TRIGMODE_AUTO		0
#define TRIGMODE_SNGL		1
#define TRIGMODE_NORM		2
#define TRIGMODE_MAXVALS	3

/* possible Run/Stop values */
#define RUNSTOP_RUN			0
#define RUNSTOP_STOP		1
#define RUNSTOP_MAXVALS		2

extern __IO uint32_t Tick_1ms;
extern uint32_t SystemCoreClock;

extern __IO uint8_t CH1_ADC_vals[ADC_BUF_SIZE];
extern __IO uint8_t CH2_ADC_vals[ADC_BUF_SIZE];

extern uint8_t vscale1;				/* Vertical scale of channel 1 */
extern uint8_t vscale2;				/* Vertical scale of channel 2 */
extern uint8_t tscale;				/* Horizontal scale */
extern uint8_t trigsrc;				/* Trigger source */
extern uint8_t trigtype;			/* Trigger type */
extern uint8_t triglvl;				/* Trigger level */
extern uint8_t trigmode;			/* Trigger mode */
extern uint8_t runstop;				/* Run/Stop */
extern int16_t voff1;				/* Vertical offset of channel 1 */
extern int16_t voff2;				/* Vertical offset of channel 2 */
extern uint16_t toff;				/* Horizontal offset */

extern const float32_t vscaleVals[];
extern const char vscaleDispVals[][6];
extern const int8_t tscaleVals[];
extern const char tscaleDispVals[][7];
extern const uint8_t trigsrcVals[];
extern const uint8_t trigtypeVals[];
extern const char trigtypeDispVals[][4];
extern const uint8_t trigmodeVals[];
extern const char trigmodeDispVals[][5];
extern const uint8_t runstopVals[];
extern const char runstopDispVals[][5];

extern const uint32_t samprateVals[];

extern __IO uint8_t QE_PB_interrupt;
extern __IO uint8_t Meas_pending;

extern __IO uint8_t CH1_acq_comp, CH2_acq_comp;

extern __IO uint8_t LTDC_error_flag;
extern __IO uint8_t DMA2D_error_flag;

#endif /* __GLOBALS_H */
