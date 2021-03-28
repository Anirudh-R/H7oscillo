/**
  ******************************************************************************
  * @file    measure.c
  * @author  anirudhr
  * @brief   Computes various parameters of the input signal.
  ******************************************************************************
  */

/* Includes */
#include "measure.h"


const char measParamTexts[][5] = {"None", "Freq", "Duty", "Vrms", "Vmax", "Vmin", "Vpp ", "Vavg"};
Measure_TypeDef measure1, measure2, measure3, measure4;
static arm_rfft_fast_instance_f32 S_rfft_512;
static arm_cfft_instance_f32 S_cfft_1024;

static void czt(uint8_t x[], float32_t oup[], float32_t f1, float32_t f2, uint32_t N, uint32_t M);


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
	__IO uint8_t* x;

	if(channel == CHANNEL1)
		x = (__IO uint8_t *)CH1_ADC_vals + ADC_PRETRIGBUF_SIZE;
	else
		x = (__IO uint8_t *)CH2_ADC_vals + ADC_PRETRIGBUF_SIZE;

	if(param == MEAS_FREQ)
		return calcFreq(x);
	else if(param == MEAS_DUTY)
		return calcDuty(x);
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
* @brief  Calculate Frequency of signal.
* @param  x: input signal
* @retval calculated Frequency
*/
float32_t calcFreq(__IO uint8_t x[])
{
	float32_t inp[512], oup[545];
	uint8_t xcpy[ADC_TRIGBUF_SIZE];
	float32_t maxmag, f1, f2;
	uint32_t maxidx, i;

	/* create a zero-padded copy of x */
	for(i = 0; i < ADC_TRIGBUF_SIZE; i++)
		inp[i] = xcpy[i] = x[i];		/* backup x into xcpy, since x may get modified with new ADC values */
	for(i = ADC_TRIGBUF_SIZE; i < 512; i++)
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
			f1 = (float32_t)(maxidx - 1)/512.0;
			f2 = (float32_t)maxidx/512.0;
		}
		else if(maxidx == 0){
			f1 = (float32_t)maxidx/512.0;
			f2 = (float32_t)(maxidx + 1)/512.0;
		}
		else if(inp[maxidx-1] > inp[maxidx+1]){
			f1 = (float32_t)(maxidx - 1)/512.0;
			f2 = (float32_t)maxidx/512.0;
		}
		else{
			f1 = (float32_t)maxidx/512.0;
			f2 = (float32_t)(maxidx + 1)/512.0;
		}

		czt(xcpy, oup, f1, f2, ADC_TRIGBUF_SIZE, 545);
		arm_max_f32(oup, 545, &maxmag, &maxidx);

		return f1 + maxidx*(f2 - f1)/545.0f;
	}

	return 0;
}

/**
  * @brief  Calculate Vrms of signal.
  * @param  x: input signal
  * @retval calculated duty cycle percentage
  */
uint8_t calcDuty(__IO uint8_t x[])
{
	uint32_t N, cnt, i;
	float32_t f, temp;

	f = calcFreq(x);
	temp = ceil(1/f);						/* number of sample points in one cycle of the waveform */

	cnt = (float32_t)ADC_TRIGBUF_SIZE/temp;		/* number of integral cycles in the input frame */
	N = round(temp*cnt);					/* number of points for duty cycle calculation */
	if(N > ADC_TRIGBUF_SIZE)
		N = ADC_TRIGBUF_SIZE;

	/* calculate average value of one cycle */
	f = 0;
	for(i = 0; i < N; i++){
		f += x[i];
	}
	f /= N;

	/* count the number of points above the average value */
	cnt = 0;
	for(i = 0; i < N; i++){
		if(x[i] > f)
			cnt++;
	}

	return (float32_t)cnt*100.0f/(float32_t)N;;
}

/**
  * @brief  Calculate Vrms of signal.
  * @param  x: input signal
  * @retval calculated Vrms
  */
uint8_t calcVrms(__IO uint8_t x[])
{
	uint32_t i;
	float32_t temp, out;

	temp = 0;
	for(i = 0; i < ADC_TRIGBUF_SIZE; i++)
		temp += (float32_t)x[i]*(float32_t)x[i];

	temp /= (float32_t)ADC_TRIGBUF_SIZE;

	arm_sqrt_f32(temp, &out);

	return out;
}

/**
  * @brief  Calculate Vmax of signal.
  * @param  x: input signal
  * @retval calculated Vmax
  */
uint8_t calcVmax(__IO uint8_t x[])
{
	uint32_t i;
	uint8_t vmax;

	vmax = 0;
	for(i = 0; i < ADC_TRIGBUF_SIZE; i++)
		if(x[i] > vmax)
			vmax = x[i];

	return vmax;
}

/**
  * @brief  Calculate Vmin of signal.
  * @param  x: input signal
  * @retval calculated Vmin
  */
uint8_t calcVmin(__IO uint8_t x[])
{
	uint32_t i;
	uint8_t vmin;

	vmin = 255;
	for(i = 0; i < ADC_TRIGBUF_SIZE; i++)
		if(x[i] < vmin)
			vmin = x[i];

	return vmin;
}

/**
  * @brief  Calculate Vpp of signal.
  * @param  x: input signal
  * @retval calculated Vpp
  */
uint8_t calcVpp(__IO uint8_t x[])
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
uint8_t calcVavg(__IO uint8_t x[])
{
	uint32_t i, vavg;

	vavg = 0;
	for(i = 0; i < ADC_TRIGBUF_SIZE; i++)
		vavg += x[i];

	vavg /= ADC_TRIGBUF_SIZE;

	return vavg;
}

/* Chirp signal coefficients {real, imag, ...}: W^(n*n/2) = exp(j*phi0*n*n/2); phi0 = -2*PI*(1/512)/545 */
static const float32_t chrp[1090] = {1.000000, 0.000000, 1.000000, -0.000011, 1.000000, -0.000045, 1.000000, -0.000101, 1.000000, -0.000180, 1.000000, -0.000281, 1.000000, -0.000405, 1.000000, -0.000552, 1.000000, -0.000721, 1.000000, -0.000912, 0.999999, -0.001126, 0.999999, -0.001362, 0.999999, -0.001621, 0.999998, -0.001903, 0.999998, -0.002207, 0.999997, -0.002533, 0.999996, -0.002882, 0.999995, -0.003254, 0.999993, -0.003648, 0.999992, -0.004064, 0.999990, -0.004503, 0.999988, -0.004965, 0.999985, -0.005449, 0.999982, -0.005956, 0.999979, -0.006485, 0.999975, -0.007037, 0.999971, -0.007611, 0.999966, -0.008207, 0.999961, -0.008827, 0.999955, -0.009468, 0.999949, -0.010133, 0.999942, -0.010819, 0.999934, -0.011529, 0.999925, -0.012260, 0.999915, -0.013015, 0.999905, -0.013791, 0.999894, -0.014591, 0.999881, -0.015412, 0.999868, -0.016257, 0.999854, -0.017123, 0.999838, -0.018013, 0.999821, -0.018925, 0.999803, -0.019859, 0.999784, -0.020816, 0.999763, -0.021795, 0.999740, -0.022797, 0.999717, -0.023821, 0.999691, -0.024868, 0.999664, -0.025937, 0.999635, -0.027029,
0.999604, -0.028143, 0.999572, -0.029279, 0.999537, -0.030438, 0.999500, -0.031620, 0.999462, -0.032824, 0.999420, -0.034051, 0.999377, -0.035300, 0.999331, -0.036571, 0.999283, -0.037865, 0.999233, -0.039181, 0.999179, -0.040520, 0.999123, -0.041881, 0.999064, -0.043264, 0.999002, -0.044670, 0.998937, -0.046099, 0.998869, -0.047550, 0.998798, -0.049023, 0.998724, -0.050518, 0.998646, -0.052036, 0.998564, -0.053576, 0.998479, -0.055139, 0.998391, -0.056724, 0.998298, -0.058331, 0.998201, -0.059961, 0.998101, -0.061613, 0.997996, -0.063287, 0.997887, -0.064984, 0.997774, -0.066703, 0.997656, -0.068444, 0.997533, -0.070207, 0.997406, -0.071993, 0.997274, -0.073800, 0.997137, -0.075630, 0.996995, -0.077483, 0.996847, -0.079357, 0.996694, -0.081254, 0.996536, -0.083172, 0.996372, -0.085113, 0.996203, -0.087076, 0.996027, -0.089061, 0.995846, -0.091068, 0.995658, -0.093097, 0.995464, -0.095149, 0.995264, -0.097222, 0.995057, -0.099317, 0.994843, -0.101434, 0.994623, -0.103573, 0.994396, -0.105734, 0.994161, -0.107917, 0.993919, -0.110122,
0.993670, -0.112348, 0.993413, -0.114597, 0.993149, -0.116867, 0.992877, -0.119159, 0.992596, -0.121472, 0.992308, -0.123807, 0.992011, -0.126164, 0.991705, -0.128543, 0.991391, -0.130943, 0.991069, -0.133365, 0.990737, -0.135808, 0.990396, -0.138273, 0.990046, -0.140759, 0.989686, -0.143266, 0.989316, -0.145795, 0.988937, -0.148345, 0.988548, -0.150917, 0.988149, -0.153509, 0.987739, -0.156123, 0.987319, -0.158758, 0.986889, -0.161414, 0.986447, -0.164092, 0.985994, -0.166790, 0.985531, -0.169509, 0.985055, -0.172249, 0.984569, -0.175010, 0.984070, -0.177791, 0.983560, -0.180594, 0.983037, -0.183417, 0.982503, -0.186260, 0.981955, -0.189124, 0.981395, -0.192009, 0.980823, -0.194914, 0.980237, -0.197840, 0.979638, -0.200785, 0.979025, -0.203751, 0.978399, -0.206737, 0.977759, -0.209744, 0.977105, -0.212770, 0.976437, -0.215816, 0.975754, -0.218882, 0.975057, -0.221968, 0.974344, -0.225074, 0.973617, -0.228199, 0.972875, -0.231344, 0.972117, -0.234508, 0.971344, -0.237691, 0.970554, -0.240894, 0.969749, -0.244117, 0.968927, -0.247358,
0.968089, -0.250618, 0.967234, -0.253897, 0.966362, -0.257195, 0.965474, -0.260512, 0.964567, -0.263848, 0.963644, -0.267202, 0.962702, -0.270574, 0.961743, -0.273965, 0.960765, -0.277374, 0.959769, -0.280801, 0.958755, -0.284247, 0.957721, -0.287710, 0.956669, -0.291191, 0.955597, -0.294689, 0.954505, -0.298205, 0.953394, -0.301739, 0.952263, -0.305289, 0.951112, -0.308857, 0.949940, -0.312443, 0.948748, -0.316044, 0.947535, -0.319663, 0.946301, -0.323299, 0.945045, -0.326951, 0.943768, -0.330619, 0.942470, -0.334303, 0.941149, -0.338004, 0.939806, -0.341721, 0.938440, -0.345453, 0.937052, -0.349201, 0.935641, -0.352964, 0.934207, -0.356743, 0.932749, -0.360537, 0.931268, -0.364346, 0.929763, -0.368170, 0.928234, -0.372009, 0.926680, -0.375862, 0.925102, -0.379729, 0.923500, -0.383611, 0.921872, -0.387506, 0.920219, -0.391416, 0.918540, -0.395339, 0.916836, -0.399275, 0.915106, -0.403225, 0.913350, -0.407188, 0.911567, -0.411163, 0.909757, -0.415152, 0.907921, -0.419152, 0.906058, -0.423166, 0.904167, -0.427191, 0.902249, -0.431228,
0.900302, -0.435277, 0.898328, -0.439337, 0.896326, -0.443408, 0.894295, -0.447491, 0.892235, -0.451584, 0.890146, -0.455687, 0.888028, -0.459802, 0.885880, -0.463926, 0.883703, -0.468060, 0.881496, -0.472203, 0.879259, -0.476357, 0.876991, -0.480519, 0.874693, -0.484690, 0.872363, -0.488870, 0.870003, -0.493058, 0.867612, -0.497254, 0.865189, -0.501458, 0.862734, -0.505670, 0.860247, -0.509889, 0.857728, -0.514116, 0.855177, -0.518349, 0.852592, -0.522588, 0.849976, -0.526834, 0.847325, -0.531086, 0.844642, -0.535343, 0.841925, -0.539606, 0.839174, -0.543874, 0.836390, -0.548147, 0.833571, -0.552425, 0.830718, -0.556706, 0.827830, -0.560992, 0.824907, -0.565281, 0.821949, -0.569573, 0.818956, -0.573868, 0.815928, -0.578166, 0.812863, -0.582467, 0.809763, -0.586769, 0.806627, -0.591073, 0.803455, -0.595378, 0.800246, -0.599684, 0.797000, -0.603991, 0.793718, -0.608298, 0.790398, -0.612606, 0.787041, -0.616912, 0.783647, -0.621218, 0.780215, -0.625523, 0.776745, -0.629827, 0.773238, -0.634128, 0.769692, -0.638428, 0.766108, -0.642725,
0.762485, -0.647019, 0.758823, -0.651309, 0.755123, -0.655596, 0.751383, -0.659879, 0.747604, -0.664157, 0.743786, -0.668430, 0.739928, -0.672698, 0.736031, -0.676961, 0.732093, -0.681217, 0.728116, -0.685467, 0.724098, -0.689710, 0.720040, -0.693945, 0.715941, -0.698173, 0.711802, -0.702392, 0.707622, -0.706603, 0.703402, -0.710805, 0.699140, -0.714998, 0.694836, -0.719180, 0.690492, -0.723353, 0.686106, -0.727514, 0.681679, -0.731664, 0.677210, -0.735803, 0.672699, -0.739929, 0.668146, -0.744043, 0.663552, -0.748143, 0.658915, -0.752230, 0.654236, -0.756304, 0.649514, -0.760362, 0.644751, -0.764406, 0.639944, -0.768434, 0.635096, -0.772446, 0.630204, -0.776442, 0.625270, -0.780421, 0.620293, -0.784383, 0.615273, -0.788327, 0.610211, -0.792252, 0.605105, -0.796159, 0.599956, -0.800046, 0.594765, -0.803913, 0.589530, -0.807760, 0.584252, -0.811586, 0.578931, -0.815390, 0.573566, -0.819172, 0.568159, -0.822932, 0.562708, -0.826669, 0.557213, -0.830383, 0.551676, -0.834072, 0.546095, -0.837736, 0.540471, -0.841376, 0.534804, -0.844990,
0.529093, -0.848577, 0.523339, -0.852138, 0.517542, -0.855671, 0.511702, -0.859176, 0.505819, -0.862653, 0.499892, -0.866101, 0.493922, -0.869520, 0.487910, -0.872908, 0.481854, -0.876265, 0.475756, -0.879591, 0.469614, -0.882886, 0.463430, -0.886147, 0.457203, -0.889376, 0.450934, -0.892571, 0.444622, -0.895732, 0.438268, -0.898859, 0.431871, -0.901949, 0.425432, -0.905004, 0.418951, -0.908023, 0.412429, -0.911004, 0.405864, -0.913948, 0.399258, -0.916853, 0.392610, -0.919719, 0.385921, -0.922546, 0.379191, -0.925333, 0.372420, -0.928079, 0.365607, -0.930784, 0.358755, -0.933446, 0.351861, -0.936067, 0.344928, -0.938644, 0.337954, -0.941177, 0.330941, -0.943666, 0.323888, -0.946110, 0.316795, -0.948509, 0.309664, -0.950861, 0.302493, -0.953166, 0.295284, -0.955425, 0.288036, -0.957635, 0.280750, -0.959796, 0.273427, -0.961908, 0.266066, -0.963970, 0.258667, -0.965982, 0.251231, -0.967942, 0.243759, -0.969851, 0.236250, -0.971708, 0.228706, -0.973511, 0.221125, -0.975261, 0.213510, -0.976957, 0.205859, -0.978597, 0.198173, -0.980183,
0.190453, -0.981712, 0.182700, -0.983185, 0.174912, -0.984600, 0.167092, -0.985957, 0.159239, -0.987256, 0.151353, -0.988496, 0.143435, -0.989676, 0.135486, -0.990795, 0.127506, -0.991854, 0.119496, -0.992851, 0.111455, -0.993786, 0.103384, -0.994658, 0.095284, -0.995467, 0.087156, -0.996211, 0.078999, -0.996892, 0.070815, -0.997506, 0.062603, -0.998056, 0.054364, -0.998538, 0.046100, -0.998954, 0.037809, -0.999302, 0.029494, -0.999582, 0.021154, -0.999794, 0.012790, -0.999936, 0.004402, -1.000008, -0.004008, -1.000010, -0.012441, -0.999940, -0.020895, -0.999800, -0.029370, -0.999587, -0.037866, -0.999301, -0.046381, -0.998942, -0.054915, -0.998509, -0.063468, -0.998002, -0.072039, -0.997420, -0.080627, -0.996763, -0.089231, -0.996030, -0.097851, -0.995220, -0.106486, -0.994333, -0.115135, -0.993369, -0.123799, -0.992327, -0.132474, -0.991206, -0.141163, -0.990006, -0.149862, -0.988726, -0.158572, -0.987367, -0.167292, -0.985927, -0.176021, -0.984406, -0.184759, -0.982804, -0.193504, -0.981120, -0.202256, -0.979353, -0.211013, -0.977504, -0.219776, -0.975571,
-0.228543, -0.973555, -0.237313, -0.971454, -0.246086, -0.969269, -0.254860, -0.966999, -0.263635, -0.964644, -0.272410, -0.962203, -0.281185, -0.959675, -0.289957, -0.957062, -0.298726, -0.954361, -0.307492, -0.951573, -0.316253, -0.948697, -0.325009, -0.945734, -0.333757, -0.942682, -0.342499, -0.939541, -0.351232, -0.936312, -0.359955, -0.932993, -0.368669, -0.929585, -0.377370, -0.926086, -0.386060, -0.922498, -0.394735, -0.918819, -0.403397, -0.915050, -0.412043, -0.911189, -0.420672, -0.907238, -0.429284, -0.903195, -0.437877, -0.899060, -0.446451, -0.894834, -0.455004, -0.890515, -0.463535, -0.886105, -0.472043, -0.881602, -0.480527, -0.877006, -0.488986, -0.872318, -0.497419, -0.867537, -0.505825, -0.862664, -0.514202, -0.857697, -0.522550, -0.852637, -0.530866, -0.847484, -0.539151, -0.842237, -0.547403, -0.836898, -0.555621, -0.831465, -0.563804, -0.825938, -0.571950, -0.820319, -0.580058, -0.814605, -0.588128, -0.808799, -0.596157, -0.802899, -0.604145, -0.796905, -0.612091, -0.790819, -0.619993, -0.784639, -0.627851, -0.778366, -0.635662, -0.772001, -0.643426, -0.765542,
-0.651141, -0.758990, -0.658807, -0.752346, -0.666421, -0.745610, -0.673984, -0.738781, -0.681493, -0.731861, -0.688947, -0.724848, -0.696345, -0.717744, -0.703686, -0.710548, -0.710968, -0.703262, -0.718191, -0.695884, -0.725353, -0.688417, -0.732452, -0.680858, -0.739487, -0.673211, -0.746458, -0.665473, -0.753363, -0.657646, -0.760200, -0.649731, -0.766969, -0.641727, -0.773667, -0.633636, -0.780294, -0.625457, -0.786849, -0.617191, -0.793330, -0.608838, -0.799736, -0.600399, -0.806065, -0.591875, -0.812316, -0.583266, -0.818489, -0.574572, -0.824581, -0.565794, -0.830592, -0.556933, -0.836519, -0.547990, -0.842363, -0.538964, -0.848121, -0.529857, -0.853793, -0.520669, -0.859376, -0.511402, -0.864870, -0.502054, -0.870274, -0.492629, -0.875585, -0.483125, -0.880804, -0.473544, -0.885928, -0.463887, -0.890957, -0.454155, -0.895888, -0.444347, -0.900722, -0.434466, -0.905456, -0.424512, -0.910089, -0.414486, -0.914621, -0.404389, -0.919049, -0.394221, -0.923373, -0.383985, -0.927591, -0.373680, -0.931703, -0.363307, -0.935706, -0.352869, -0.939601, -0.342365, -0.943384, -0.331797,
-0.947057, -0.321165, -0.950616, -0.310472, -0.954061, -0.299718, -0.957392, -0.288904, -0.960606, -0.278031, -0.963702, -0.267101, -0.966680, -0.256115, -0.969538, -0.245073, -0.972275, -0.233978, -0.974891, -0.222831, -0.977383, -0.211632, -0.979751, -0.200383, -0.981994, -0.189086, -0.984111, -0.177741, -0.986101, -0.166351, -0.987962, -0.154916, -0.989693, -0.143438, -0.991295, -0.131918, -0.992765, -0.120358, -0.994102, -0.108760, -0.995307, -0.097124, -0.996377, -0.085452, -0.997312, -0.073747, -0.998110, -0.062008, -0.998772, -0.050239, -0.999296, -0.038440, -0.999681, -0.026613, -0.999926, -0.014760, -1.000031, -0.002882, -0.999995, 0.009018, -0.999816, 0.020940, -0.999495, 0.032882, -0.999030, 0.044841, -0.998421, 0.056816, -0.997666, 0.068806, -0.996766, 0.080808, -0.995719, 0.092820, -0.994526, 0.104842, -0.993184, 0.116871, -0.991694, 0.128905, -0.990055, 0.140942, -0.988267, 0.152981, -0.986328, 0.165019, -0.984239, 0.177055, -0.981999, 0.189087};

/**
* @brief  Compute the chirp-z transform of the signal in given frequency range.
* @param  x: input signal (real)
* @param  oup: transformed output magnitude squared
* @param  f1: start frequency as a fraction of fs
* @param  f2: end frequency as a fraction of fs
* @param  N: no. of elements in x
* @param  M: no. of output transform points. Should be such that N+M-1 is a power of 2.
* @retval None
*/
static void czt(uint8_t x[], float32_t oup[], float32_t f1, float32_t f2, uint32_t N, uint32_t M)
{
	uint32_t L, i;
	float32_t theta0;			/* A = exp(j*theta0) */
	float32_t c1, s1, c2, s2;

	L = N + M - 1;
	theta0 = 2.0f*PI*f1;

	float32_t yY[2*L], vV[2*L];

	/* yn = A^-n * W^(n^2/2) * xn */
	for(i = 0; i < N; i++){
		c1 = arm_cos_f32(-(float32_t)i*theta0);
		s1 = arm_sin_f32(-(float32_t)i*theta0);
		c2 = chrp[2*i];
		s2 = chrp[2*i+1];
		yY[2*i] = (c1*c2 - s1*s2) * (float32_t)x[i];		/* real */
		yY[2*i+1] = (c1*s2 + c2*s1) * (float32_t)x[i];		/* imaginary */
	}
	for(i = N; i < L; i++){
		yY[2*i] = 0;
		yY[2*i+1] = 0;
	}

	/* FFT(yY) */
	arm_cfft_f32(&S_cfft_1024, yY, 0, 1);

	/* vn = W^(-n^2/2) for 0<=n<=M-1 */
	for(i = 0; i < M; i++){
		vV[2*i] = chrp[2*i];
		vV[2*i+1] = -chrp[2*i+1];
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
		c1 = chrp[2*i];
		s1 = chrp[2*i+1];
		vV[2*i] = c1*yY[2*i] - s1*yY[2*i+1];
		vV[2*i+1] = c1*yY[2*i+1] + s1*yY[2*i];
	}

	arm_cmplx_mag_squared_f32(vV, oup, M);

	return;
}

