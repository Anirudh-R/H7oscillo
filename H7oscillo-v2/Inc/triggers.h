/**
  ******************************************************************************
  * @file    triggers.h
  * @author  anirudhr
  * @brief   Header file for triggers.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRIGGERS_H
#define __TRIGGERS_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define FILTERTYPE_DEC				0
#define FILTERTYPE_INT				1

#define MAX_FACTOR					10						/* Max interpolation/decimation factor supported */
#define MAX_RESAMPLEDSIG_LEN		(480 * MAX_FACTOR)		/* Max length of the resampled signal */
#define MAX_RESAMPLEDSIG_LEN1		4950					/* Max length of the resampled signal output of filter 1, including room for next stage */
#define MAX_RESAMPLEDSIG_LEN2		2494					/* Max length of the resampled signal output of filter 2, including room for next stage */
#define MAX_PREPEND_LEN2			50						/* Max length of prepended samples in filter 2 */
#define MAX_PREPEND_LEN3			10						/* Max length of prepended samples in filter 3 */

typedef struct {
	uint8_t type;
	uint8_t factor;		/* Decimation/Interpolation factor */
	float32_t* coeffs;
	uint32_t ntaps;
} Filter;

extern uint8_t CH1_ResampledVals[MAX_RESAMPLEDSIG_LEN];
extern uint8_t CH2_ResampledVals[MAX_RESAMPLEDSIG_LEN];

int32_t processTriggers(void);
int32_t chkTrigResampSig(int32_t len);
int32_t resampleChannels(uint32_t offset, uint32_t lenx, uint8_t origSR, uint8_t newSR);

#endif /* __TRIGGERS_H */
