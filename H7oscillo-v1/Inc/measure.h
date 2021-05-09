/**
  ******************************************************************************
  * @file    measure.h
  * @author  anirudhr
  * @brief   Header file for measure.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEASURE_H
#define __MEASURE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define MEASURE_NONE	0
#define MEASURE1		1
#define MEASURE2		2
#define MEASURE3		3
#define MEASURE4		4

#define MEAS_NONE		0		/* none */
#define MEAS_FREQ		1		/* frequency */
#define MEAS_DUTY		2		/* duty cycle */
#define MEAS_VRMS		3		/* RMS */
#define MEAS_VMAX		4		/* max */
#define MEAS_VMIN		5		/* min */
#define MEAS_VPP		6		/* peak-peak */
#define MEAS_VAVG		7		/* average */

typedef struct{
	uint8_t src;
	uint8_t param;
} Measure_TypeDef;

extern const char measParamTexts[][5];
extern Measure_TypeDef measure1, measure2, measure3, measure4;

void measure_init(void);
float32_t calcMeasure(uint8_t channel, uint8_t param);
float32_t calcFreq(__IO uint8_t x[]);
uint8_t calcDuty(__IO uint8_t x[]);
uint8_t calcVrms(__IO uint8_t x[]);
uint8_t calcVmax(__IO uint8_t x[]);
uint8_t calcVmin(__IO uint8_t x[]);
uint8_t calcVpp(__IO uint8_t x[]);
uint8_t calcVavg(__IO uint8_t x[]);

#endif /* __MEASURE_H */
