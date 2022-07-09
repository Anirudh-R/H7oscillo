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

/* Initial field values */
#define VSCALE1_INITVAL		5
#define VSCALE2_INITVAL		5
#define TSCALE_INITVAL		6
#define TRIGSRC_INITVAL		0
#define TRIGTYPE_INITVAL	0
#define TRIGLVL_INITVAL		127
#define TRIGMODE_INITVAL	0
#define RUNSTOP_INITVAL		0
#define VOFF1_INITVAL		0
#define VOFF2_INITVAL		0
#define TOFF_INITVAL		60

/* Channel display modes */
#define CHDISPMODE_SPLIT	0
#define CHDISPMODE_MERGE	1
#define CHDISPMODE_SNGL		2		/* ch1 only */
#define CHDISPMODE_FFT		3

#define CHDISPMODE_SPLIT_SIGMAX		119
#define CHDISPMODE_SPLIT_CH1BOT		134
#define CHDISPMODE_SPLIT_CH1TOP		(CHDISPMODE_SPLIT_CH1BOT - CHDISPMODE_SPLIT_SIGMAX)
#define CHDISPMODE_SPLIT_CH2BOT		254
#define CHDISPMODE_SPLIT_CH2TOP		(CHDISPMODE_SPLIT_CH2BOT - CHDISPMODE_SPLIT_SIGMAX)

#define CHDISPMODE_MERGE_SIGMAX		239
#define CHDISPMODE_MERGE_CHBOT		254
#define CHDISPMODE_MERGE_CHTOP		(CHDISPMODE_MERGE_CHBOT - CHDISPMODE_MERGE_SIGMAX)

#define MATH_OP_NONE		0
#define MATH_OP_1P2			1
#define MATH_OP_1M2			2
#define MATH_OP_2M1			3
#define MATH_OP_1X2			4

#define MATHVSCALE_INITVAL	5
#define MATHVOFF_INITVAL	210

#define CURSOR_MODE_OFF		0
#define CURSOR_MODE_DT		1
#define CURSOR_MODE_DVCH1	2
#define CURSOR_MODE_DVCH2	3

#define CURSORA_SPLITCH1_INITPOS	(CHDISPMODE_SPLIT_CH1BOT - 80)
#define CURSORB_SPLITCH1_INITPOS	(CHDISPMODE_SPLIT_CH1BOT - 50)
#define CURSORA_SPLITCH2_INITPOS	(CHDISPMODE_SPLIT_CH2BOT - 80)
#define CURSORB_SPLITCH2_INITPOS	(CHDISPMODE_SPLIT_CH2BOT - 50)
#define CURSORA_MERGE_INITPOS		(CHDISPMODE_MERGE_CHBOT - 80)
#define CURSORB_MERGE_INITPOS		(CHDISPMODE_MERGE_CHBOT - 50)
#define CURSORA_DT_INITPOS			200
#define CURSORB_DT_INITPOS			300

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
extern __IO uint8_t measPending;

extern __IO uint8_t CH1_acq_comp, CH2_acq_comp;

extern __IO uint8_t LTDC_error_flag;
extern __IO uint8_t DMA2D_error_flag;

extern uint8_t staticMode;
extern int32_t toffStm;
extern uint8_t vscale1Changed, vscale2Changed, voff1Changed, voff2Changed, toffChanged;

extern uint8_t chDispMode;

extern uint8_t fftSrcChannel;

extern uint8_t mathOp;
extern uint8_t mathVscale;
extern int16_t mathVoff;

extern uint8_t scrnshtViewMode;

extern FATFS fatFs;
extern FIL fp;
extern const MKFS_PARM mkfsParam;

#endif /* __GLOBALS_H */
