/**
  ******************************************************************************
  * @file    globals.c
  * @author  anirudhr
  * @brief   Contains global variables used by application.
  ******************************************************************************
  */

#include "globals.h"


/* Global counter incremented every 1ms */
__IO uint32_t Tick_1ms = 0;

/* Array to store ADC readings */
__IO uint8_t CH1_ADC_vals[ADC_BUF_SIZE];
__IO uint8_t CH2_ADC_vals[ADC_BUF_SIZE];

/* Field values in the top and bottom menubar. These index into the corresponding possible values array (except triglvl and offsets). */
uint8_t vscale1 = VSCALE1_INITVAL;			/* Vertical scale of channel 1 */
uint8_t vscale2 = VSCALE2_INITVAL;			/* Vertical scale of channel 2 */
uint8_t tscale = TSCALE_INITVAL;			/* Horizontal scale */
uint8_t trigsrc = TRIGSRC_INITVAL;			/* Trigger source */
uint8_t trigtype = TRIGTYPE_INITVAL;		/* Trigger type */
uint8_t triglvl = TRIGLVL_INITVAL;			/* Trigger level */
uint8_t trigmode = TRIGMODE_INITVAL;		/* Trigger mode */
uint8_t runstop = RUNSTOP_INITVAL;			/* Run/Stop */
int16_t voff1 = VOFF1_INITVAL;				/* Vertical offset of channel 1 */
int16_t voff2 = VOFF2_INITVAL;				/* Vertical offset of channel 2 */
uint16_t toff = TOFF_INITVAL;				/* Horizontal offset */

/* Possible values & display values for each field */
const float32_t vscaleVals[] = {VSCALE_10mV, VSCALE_50mV, VSCALE_100mV, VSCALE_200mV, VSCALE_500mV, VSCALE_1V, VSCALE_2V, VSCALE_4V};
const char vscaleDispVals[][6] = {"10mV", "50mV", "100mV", "200mV", "500mV", "1V", "2V", "4V"};
const int8_t tscaleVals[] = {TSCALE_21_6us, TSCALE_25us, TSCALE_50us, TSCALE_100us, TSCALE_200us, TSCALE_500us, TSCALE_1ms};
const char tscaleDispVals[][7] = {"21.6us", "25us", "50us", "100us", "200us", "500us", "1ms"};
const uint8_t trigsrcVals[] = {TRIGSRC_CH1, TRIGSRC_CH2};		/* source channel is denoted using color */
const uint8_t trigtypeVals[] = {TRIGTYPE_RIS, TRIGTYPE_FAL, TRIGTYPE_EDGE};
const char trigtypeDispVals[][4] = {"R", "F", "R/F"};
const uint8_t trigmodeVals[] = {TRIGMODE_AUTO, TRIGMODE_SNGL, TRIGMODE_NORM};
const char trigmodeDispVals[][5] = {"AUTO", "SNGL", "NORM"};
const uint8_t runstopVals[] = {RUNSTOP_RUN, RUNSTOP_STOP};
const char runstopDispVals[][5] = {"RUN", "STOP"};

const uint32_t samprateVals[] = {2222222, 1923076, 961538, 480769, 239808, 95969, 48007};	/* sample rate corresponding to each time scale value */

__IO uint8_t QE_PB_interrupt = 0;	/* Flag for Quadrature encoder push button */
__IO uint8_t measPending = 0;		/* Pending measurement calculations */

__IO uint8_t CH1_acq_comp = 0;		/* ADC Acquisition complete flags */
__IO uint8_t CH2_acq_comp = 0;

/* Error flags */
__IO uint8_t LTDC_error_flag = 0;
__IO uint8_t DMA2D_error_flag = 0;

/* Static mode related */
uint8_t staticMode = 0;
int32_t toffStm = TOFF_INITVAL;
uint8_t vscale1Changed = 0, vscale2Changed = 0, voff1Changed = 0, voff2Changed = 0, toffChanged = 0;

uint8_t chDispMode = CHDISPMODE_SPLIT;

uint8_t fftSrcChannel = CHANNELNONE;

uint8_t mathOp = MATH_OP_NONE;
uint8_t mathVscale = MATHVSCALE_INITVAL;
int16_t mathVoff = MATHVOFF_INITVAL;

uint8_t scrnshtViewMode = 0;

/* FatFs */
FATFS fatFs;       					/* file system object */
FIL fp;            					/* file object */
const MKFS_PARM mkfsParam = {		/* file system parameters */
	.fmt = FM_FAT,
	.au_size = 32*SECTOR_SIZE,
	.align = 0,
	.n_fat = 1,
	.n_root = 128
};

