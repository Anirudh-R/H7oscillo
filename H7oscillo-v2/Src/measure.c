/**
  ******************************************************************************
  * @file    measure.c
  * @author  anirudhr
  * @brief   Computes various parameters of the input signal.
  ******************************************************************************
  */

/* Includes */
#include "measure.h"


Measure_TypeDef measure1, measure2, measure3, measure4;
const char measParamTexts[][5] = {"None", "Freq", "Duty", "Vrms", "Vmax", "Vmin", "Vpp ", "Vavg"};


/**
  * @brief  Calculate the given parameter of a particular channel signal.
  * @param  channel: source signal
  * @param  param: parameter to calculate
  * @retval calculated value
  */
uint8_t calcMeasure(uint8_t channel, uint8_t param)
{
	uint8_t* x;

	if(channel == CHANNEL1)
		x = (uint8_t *)CH1_ADC_vals;
	else
		x = (uint8_t *)CH2_ADC_vals;

	if(param == MEAS_FREQ)
		return 0;
	else if(param == MEAS_DUTY)
		return 0;
	else if(param == MEAS_VRMS)
		return calcVrms(x);
	else if(param == MEAS_VMAX)
		return calcVmax(x);
	else if(param == MEAS_VMIN)
		return calcVmin(x);
	else if(param == MEAS_VPP)
		return calcVpp(x);
	else if(param == MEAS_VAVG)
		return calcVavg(x);
	else
		return 0;

	return 0;
}

/**
  * @brief  Calculate Vrms of signal.
  * @param  x: input signal
  * @retval calculated Vrms
  */
uint8_t calcVrms(uint8_t* x)
{
	uint32_t i;
	float temp;

	temp = 0;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		temp += (float)x[i]*x[i];

	temp /= ADC_BUF_SIZE;

	return sqrt(temp);
}

/**
  * @brief  Calculate Vmax of signal.
  * @param  x: input signal
  * @retval calculated Vmax
  */
uint8_t calcVmax(uint8_t* x)
{
	uint32_t i;
	uint8_t vmax;

	vmax = 0;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		if(x[i] > vmax)
			vmax = x[i];

	return vmax;
}

/**
  * @brief  Calculate Vmin of signal.
  * @param  x: input signal
  * @retval calculated Vmin
  */
uint8_t calcVmin(uint8_t* x)
{
	uint32_t i;
	uint8_t vmin;

	vmin = 255;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		if(x[i] < vmin)
			vmin = x[i];

	return vmin;
}

/**
  * @brief  Calculate Vpp of signal.
  * @param  x: input signal
  * @retval calculated Vpp
  */
uint8_t calcVpp(uint8_t* x)
{
	uint8_t vmax, vmin;

	vmax = calcVmax(x);
	vmin = calcVmin(x);

	return (vmax - vmin);
}

/**
  * @brief  Calculate Vavg of signal.
  * @param  x: input signal
  * @retval calculated Vavg
  */
uint8_t calcVavg(uint8_t* x)
{
	uint32_t i, vavg;

	vavg = 0;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		vavg += x[i];

	vavg /= ADC_BUF_SIZE;

	return vavg;
}

