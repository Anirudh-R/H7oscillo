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
static arm_cfft_instance_f32 S_cfft_1024;

static void czt(unsigned char* x, float* oup, float f1, float f2, unsigned int N, unsigned int M);


/**
  * @brief  Initialize the FFT instances.
  * @param  None
  * @retval None
  */
void measure_init(void)
{
	arm_rfft_fast_init_f32(&S_rfft_512, 512);
	arm_cfft_init_f32(&S_cfft_1024, 1024);
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
		return calcFreq(x);
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


float32_t cztout[545];

/**
* @brief  Calculate Frequency of signal.
* @param  x: input signal
* @retval calculated Frequency
*/
float32_t calcFreq(uint8_t* x)
{
	float32_t inp[512], oup[545];
	uint8_t xcpy[ADC_BUF_SIZE];		//copy of x
	float32_t maxmag, f1, f2;
	uint32_t maxidx, i;

	/* create a zero-padded copy of x */
	for(i = 0; i < ADC_BUF_SIZE; i++)
		inp[i] = xcpy[i] = x[i];
	for(i = ADC_BUF_SIZE; i < 512; i++)
		inp[i] = 0;

	/* calculate FFT and its magnitude */
	arm_rfft_fast_f32(&S_rfft_512, inp, oup, 0);
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
		/* zoom-in one bin to the left or right */
		if(maxidx == 255){
			f1 = (float)(maxidx - 1)/512.0;
			f2 = (float)maxidx/512.0;
		}
		else if(maxidx == 0){
			f1 = (float)maxidx/512.0;
			f2 = (float)(maxidx + 1)/512.0;
		}
		else if(inp[maxidx-1] > inp[maxidx+1]){
			f1 = (float)(maxidx - 1)/512.0;
			f2 = (float)maxidx/512.0;
		}
		else{
			f1 = (float)maxidx/512.0;
			f2 = (float)(maxidx + 1)/512.0;
		}

		czt(xcpy, cztout, f1, f2, ADC_BUF_SIZE, 545);
		arm_max_f32(cztout, 545, &maxmag, &maxidx);

		return f1 + maxidx*(f2 - f1)/545.0;
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
	float32_t temp, out;

	temp = 0;
	for(i = 0; i < ADC_BUF_SIZE; i++)
		temp += (float32_t)x[i]*x[i];

	temp /= ADC_BUF_SIZE;

	arm_sqrt_f32(temp, &out);

	return out;
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
* @param  x: input signal (real)
* @param  oup: transformed output magnitude
* @param  f1: start frequency as a fraction of fs
* @param  f2: end frequency as a fraction of fs
* @param  N: no. of elements in x
* @param  M: no. of output transform points. Should be such that N+M-1 is a power of 2.
* @retval None
*/
static void czt(unsigned char* x, float* oup, float f1, float f2, unsigned int N, unsigned int M)
{
	unsigned int L, i;
	float theta0, phi0;		/* A = exp(j*theta0), W = exp(j*phi0) */
	float c1, s1, c2, s2;

	L = N + M - 1;
	theta0 = 2.0f*M_PI*f1;
	phi0 = -2.0f*M_PI*(f2 - f1)/(float)M;

	float yY[2*L], vV[2*L];

	/* yn = A^-n * W^(n^2/2) * xn */
	for(i = 0; i < N; i++){
		c1 = arm_cos_f32(-(float)i*theta0);
		s1 = arm_sin_f32(-(float)i*theta0);
		c2 = arm_cos_f32((float)(i*i)*phi0/2.0f);
		s2 = arm_sin_f32((float)(i*i)*phi0/2.0f);
		yY[2*i] = (c1*c2 - s1*s2) * (float)x[i];		/* real */
		yY[2*i+1] = (c1*s2 + c2*s1) * (float)x[i];		/* imaginary */
	}
	for(i = N; i < L; i++){
		yY[2*i] = 0;
		yY[2*i+1] = 0;
	}

	/* FFT(yY) */
	arm_cfft_f32(&S_cfft_1024, yY, 0, 1);

	/* vn = W^(-n^2/2) for 0<=n<=M-1 */
	for(i = 0; i < M; i++){
		vV[2*i] = arm_cos_f32(-(float)(i*i)*phi0/2.0f);
		vV[2*i+1] = arm_sin_f32(-(float)(i*i)*phi0/2.0f);
	}
	/* vn = W^(-(L-n)^2/2) for M<=n<=L-1 */
	for(i = M; i < L; i++){
		vV[2*i] = vV[2*(L-i)];		/* exploit symmetry to reuse values from 0 to M-1 */
		vV[2*i+1] = vV[2*(L-i)+1];
	}

	/* FFT(vV) */
	arm_cfft_f32(&S_cfft_1024, vV, 0, 1);

	/* FFT(yY)*FFT(vV) */
	arm_cmplx_mult_cmplx_f32(yY, vV, yY, L);

	/* gn = IFFT[FFT(yY)*FFT(vV)] */
	arm_cfft_f32(&S_cfft_1024, yY, 1, 1);

	/* oup[n] = W^(n^2/2)*gn */
	for(i = 0; i < M; i++){
		c1 = arm_cos_f32((float)(i*i)*phi0/2.0f);
		s1 = arm_sin_f32((float)(i*i)*phi0/2.0f);
		vV[2*i] = c1*yY[2*i] - s1*yY[2*i+1];
		vV[2*i+1] = c1*yY[2*i+1] + s1*yY[2*i];
	}

	arm_cmplx_mag_squared_f32(vV, oup, M);

	return;
}

