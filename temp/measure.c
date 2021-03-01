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

static arm_rfft_fast_instance_f32 S_rfft_512;


/**
  * @brief  Initialize the FFT instances.
  * @param  None
  * @retval None
  */
void measure_init(void)
{
	arm_rfft_fast_init_f32(&S_rfft_512, 512);
}

/**
  * @brief  Calculate the given parameter of a particular channel signal.
  * @param  channel: source signal
  * @param  param: parameter to calculate
  * @retval calculated value
  */
float32_t calcMeasure(uint8_t channel, uint8_t param)
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
  * @brief  Calculate the dominant frequency of a signal, excluding DC offset.
  * @param  x: input signal
  * @retval calculated Freq
  */
float32_t calcFreq(uint8_t* x)
{
	float32_t inp[512], oup[512];
	float32_t maxmag;
	uint32_t maxidx, i;
	
	/* create a zero-padded copy of x */
	for(i = 0; i < ADC_BUF_SIZE; i++)
		inp[i] = x[i];
	for(i = ADC_BUF_SIZE; i < 512; i++)
		inp[i] = 0;
	
	/* calculate FFT and its magnitude */
	arm_rfft_fast_f32(&S_rfft_512, inp, oup, 1);
	arm_cmplx_mag_squared_f32(oup, inp, 256);	/* reuse inp array */
	
	/* find the dominant frequency (excluding DC) */
	inp[0] = 0;
	arm_max_f32(inp, 256, &maxmag, &maxidx);
	
	/* there is only DC component */
	if(maxmag == 0){
		return 0;
	}
	/* use chirp-z transform to zoom the FFT and find the exact frequency */
	else{
		
	}
	
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
	float32_t temp;

	temp = 0;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		temp += (float32_t)x[i]*x[i];

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

/**
  * @brief  Compute the chirp-z transform of the signal in given frequency range.
  * @param  x: input signal (say of length N)
  * @param  oup: transformed output magnitude of length m
  * @param  f1: start frequency as a fraction of fs
  * @param  f2: end frequency as a fraction of fs
  * @param  M: no. of output transform points. Should be such that N+M-1 is a power of 2.
  * @retval None
  */
void czt(uint8_t* x, float32_t* oup, float32_t f1, float32_t f2, uint32_t M)
{
	uint32_t N, L, i;
	float32_t theta0, phi0;		/* A = exp(j*theta0), W = exp(j*phi0) */
	
	N = ADC_BUF_SIZE;
	L = N + M - 1;
	theta0 = 2*pi * f1;
	phi0 = -2*pi * (f2 - f1)/m;
	
	float32_t yY[2*L], vV[2*L], gG[2*L];
	

	
	

	return;
}
