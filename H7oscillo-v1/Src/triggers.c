/**
  ******************************************************************************
  * @file    triggers.c
  * @author  anirudhr
  * @brief   Implements input waveform processing, including triggering.
  ******************************************************************************
  */

/* Includes */
#include "triggers.h"


/**
  * @brief  Processes input waveform for trigger.
  * @param  None
  * @retval Signal index where it triggered, -1 if no trigger
  */
int32_t processTriggers(void)
{
	__IO uint8_t* sig;
	int32_t i;

	if(trigsrcVals[trigsrc] == TRIGSRC_CH1)
		sig = (__IO uint8_t *)CH1_ADC_vals;
	else
		sig = (__IO uint8_t *)CH2_ADC_vals;

	/* Rising edge trigger */
	if(trigtypeVals[trigtype] == TRIGTYPE_RIS){
		for(i = 1; i < ADC_BUF_SIZE; i++)
			if(sig[i-1] <= triglvl && triglvl < sig[i])
				return i-1;
	}
	/* Falling edge trigger */
	else if(trigtypeVals[trigtype] == TRIGTYPE_FAL){
		for(i = 1; i < ADC_BUF_SIZE; i++)
			if(sig[i-1] > triglvl && triglvl >= sig[i])
				return i-1;
	}
	/* Rising/Falling edge trigger */
	else if(trigtypeVals[trigtype] == TRIGTYPE_EDGE){
		for(i = 1; i < ADC_BUF_SIZE; i++)
			if((sig[i-1] > triglvl && triglvl >= sig[i]) ||
			   (sig[i-1] <= triglvl && triglvl < sig[i]))
				return i-1;
	}

	return -1;		/* trigger condition not met */
}



