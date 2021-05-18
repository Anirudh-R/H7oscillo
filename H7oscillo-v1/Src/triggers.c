/**
  ******************************************************************************
  * @file    triggers.c
  * @author  anirudhr
  * @brief   Implements input waveform processing, including triggering.
  ******************************************************************************
  */

/* Includes */
#include "triggers.h"

/* FIR Low-pass filters. Filter firlpX has cutoff frequency fs/(2*X) */
static const uint32_t firlp2Len = 102;
static const float32_t firlp2[102] = {
  0.0003035604022,-0.0006326048169,-0.005484754685, -0.01257727481, -0.01366031636,
  -0.004364791792, 0.005786110647, 0.004537574016,-0.003685604315,-0.004158575088,
   0.003088340396, 0.003885122249,-0.003111463971, -0.00375519949, 0.003452106146,
   0.003718598979,-0.003998024389,-0.003726749681, 0.004701990634,  0.00373002002,
  -0.005556715652,-0.003692922881, 0.006563980132, 0.003580745775,-0.007733625825,
  -0.003358304268, 0.009082502685, 0.002991222311, -0.01062743086, -0.00243401574,
    0.01239356212, 0.001624591881, -0.01442694478,-0.0004857640888,  0.01679976843,
  -0.001084451564, -0.01962199621, 0.003238640726,  0.02307002433,-0.006238856353,
   -0.02746507153,  0.01057517156,  0.03347309679, -0.01721882448, -0.04257158563,
    0.02855459787,  0.05886974186, -0.05246412754, -0.09972984344,   0.1388520002,
     0.4606711864,   0.4606711864,   0.1388520002, -0.09972984344, -0.05246412754,
    0.05886974186,  0.02855459787, -0.04257158563, -0.01721882448,  0.03347309679,
    0.01057517156, -0.02746507153,-0.006238856353,  0.02307002433, 0.003238640726,
   -0.01962199621,-0.001084451564,  0.01679976843,-0.0004857640888, -0.01442694478,
   0.001624591881,  0.01239356212, -0.00243401574, -0.01062743086, 0.002991222311,
   0.009082502685,-0.003358304268,-0.007733625825, 0.003580745775, 0.006563980132,
  -0.003692922881,-0.005556715652,  0.00373002002, 0.004701990634,-0.003726749681,
  -0.003998024389, 0.003718598979, 0.003452106146, -0.00375519949,-0.003111463971,
   0.003885122249, 0.003088340396,-0.004158575088,-0.003685604315, 0.004537574016,
   0.005786110647,-0.004364791792, -0.01366031636, -0.01257727481,-0.005484754685,
  -0.0006326048169,0.0003035604022
};

static const uint32_t firlp4Len = 104;
static const float32_t firlp4[104] = {
	0, 0,
  -0.000140678676,-0.000830645673,-0.002154023619,-0.004206290934,-0.006503671408,
  -0.008199276403,-0.008316235617,-0.006302849855,-0.002497492358, 0.001782495878,
      0.004727697, 0.004947079346, 0.002333394019,-0.001677546767,-0.004744461272,
  -0.004919358529,-0.001890693908, 0.002640394028, 0.005828250665, 0.005409501959,
   0.001269049128,-0.004220771138,-0.007485557348,-0.006015394349,-0.0002119035489,
   0.006499882322, 0.009611153044, 0.006478833966,-0.001595472102,-0.009707104415,
   -0.01221927814,-0.006553091574, 0.004574327264,  0.01422891021,  0.01540814154,
   0.005880820565,-0.009485058486, -0.02088915743, -0.01950953901,-0.003777560312,
    0.01813935675,  0.03195870295,  0.02570845746,-0.001630547806, -0.03669090942,
   -0.05648824945, -0.03982437029,  0.02038088441,   0.1107223481,   0.2010830045,
     0.2574184239,   0.2574184239,   0.2010830045,   0.1107223481,  0.02038088441,
   -0.03982437029, -0.05648824945, -0.03669090942,-0.001630547806,  0.02570845746,
    0.03195870295,  0.01813935675,-0.003777560312, -0.01950953901, -0.02088915743,
  -0.009485058486, 0.005880820565,  0.01540814154,  0.01422891021, 0.004574327264,
  -0.006553091574, -0.01221927814,-0.009707104415,-0.001595472102, 0.006478833966,
   0.009611153044, 0.006499882322,-0.0002119035489,-0.006015394349,-0.007485557348,
  -0.004220771138, 0.001269049128, 0.005409501959, 0.005828250665, 0.002640394028,
  -0.001890693908,-0.004919358529,-0.004744461272,-0.001677546767, 0.002333394019,
   0.004947079346,    0.004727697, 0.001782495878,-0.002497492358,-0.006302849855,
  -0.008316235617,-0.008199276403,-0.006503671408,-0.004206290934,-0.002154023619,
  -0.000830645673,-0.000140678676
};

static const uint32_t firlp5Len = 105;
static const float32_t firlp5[105] = {
	0, 0, 0,
  -0.0002192894754,-0.0004900338827,-0.0008244399214,-0.001026379876,-0.0008348952397,
  6.096513971e-06, 0.001618772978, 0.003867150284, 0.006302952301, 0.008234184235,
   0.008928229101, 0.007896493189, 0.005156408064, 0.001344137476,-0.002408566885,
  -0.004834255669,-0.005018508993,-0.002826549811,  0.00093121588, 0.004715034738,
   0.006821396295, 0.006102471147, 0.002541510155,-0.002595673082,-0.007164574228,
  -0.008998679928, -0.00689426763,-0.001293531037, 0.005689223297,  0.01100368798,
    0.01193379704, 0.007397812326,-0.001311189029, -0.01076315995, -0.01664204337,
   -0.01559210103,-0.006915339269, 0.006663545966,   0.0196470134,  0.02576087229,
     0.0206362959, 0.004135157913, -0.01870956458,  -0.0387220718,  -0.0454316251,
   -0.03084522486, 0.007204728667,  0.06344650686,   0.1260423213,   0.1798201799,
     0.2108555585,   0.2108555585,   0.1798201799,   0.1260423213,  0.06344650686,
   0.007204728667, -0.03084522486,  -0.0454316251,  -0.0387220718, -0.01870956458,
   0.004135157913,   0.0206362959,  0.02576087229,   0.0196470134, 0.006663545966,
  -0.006915339269, -0.01559210103, -0.01664204337, -0.01076315995,-0.001311189029,
   0.007397812326,  0.01193379704,  0.01100368798, 0.005689223297,-0.001293531037,
   -0.00689426763,-0.008998679928,-0.007164574228,-0.002595673082, 0.002541510155,
   0.006102471147, 0.006821396295, 0.004715034738,  0.00093121588,-0.002826549811,
  -0.005018508993,-0.004834255669,-0.002408566885, 0.001344137476, 0.005156408064,
   0.007896493189, 0.008928229101, 0.008234184235, 0.006302952301, 0.003867150284,
   0.001618772978,6.096513971e-06,-0.0008348952397,-0.001026379876,-0.0008244399214,
  -0.0004900338827,-0.0002192894754
};

static const uint32_t firlp6Len = 102;
static const float32_t firlp6[102] = {
  -8.117793914e-05,-0.0004384848871,-0.0009800364496,-0.001887621358,-0.003080827184,
   -0.00444824202,-0.005743627902,-0.006651055999, -0.00684549287,-0.006093672011,
  -0.004350417294,-0.001823442406, 0.001026207698, 0.003563924693, 0.005134160165,
   0.005251606926, 0.003776559141,  0.00101371971,-0.002312438563,-0.005219645798,
  -0.006738279015,-0.006219762843, -0.00358781917,0.0005587164778, 0.005047885701,
     0.0084387362, 0.009457306005, 0.007439511362, 0.002636473626,-0.003738297848,
  -0.009772107936, -0.01338436548, -0.01298988517,-0.008097392507,0.0003611686116,
    0.01011401694,  0.01807812974,  0.02122861706,  0.01758552156, 0.007029703818,
  -0.008320891298, -0.02434419841, -0.03576153889, -0.03742656857, -0.02573922835,
  0.0001964988624,   0.0380089432,  0.08228816837,   0.1256015301,   0.1600759625,
     0.1791663468,   0.1791663468,   0.1600759625,   0.1256015301,  0.08228816837,
     0.0380089432,0.0001964988624, -0.02573922835, -0.03742656857, -0.03576153889,
   -0.02434419841,-0.008320891298, 0.007029703818,  0.01758552156,  0.02122861706,
    0.01807812974,  0.01011401694,0.0003611686116,-0.008097392507, -0.01298988517,
   -0.01338436548,-0.009772107936,-0.003738297848, 0.002636473626, 0.007439511362,
   0.009457306005,   0.0084387362, 0.005047885701,0.0005587164778, -0.00358781917,
  -0.006219762843,-0.006738279015,-0.005219645798,-0.002312438563,  0.00101371971,
   0.003776559141, 0.005251606926, 0.005134160165, 0.003563924693, 0.001026207698,
  -0.001823442406,-0.004350417294,-0.006093672011, -0.00684549287,-0.006651055999,
  -0.005743627902, -0.00444824202,-0.003080827184,-0.001887621358,-0.0009800364496,
  -0.0004384848871,-8.117793914e-05
};

static const uint32_t firlp8Len = 104;
static const float32_t firlp8[104] = {
	0, 0,
  -2.220816714e-05,-0.000247949356,-0.0005245624343,-0.001007391373,-0.001684759744,
  -0.002548266901,-0.003543758066,  -0.0045713461,-0.005489705596,-0.006131541915,
  -0.006329186261,-0.005947266705,-0.004917300772,-0.003266985528,-0.001136958948,
   0.001221798011, 0.003473096993, 0.005242839921,  0.00618281262,  0.00604124926,
   0.004726913758, 0.002352166455,-0.0007568425499,-0.004090531264,-0.007027292158,
  -0.008937957697,  -0.0093073342,-0.007851367816,-0.004606175702,3.298773663e-05,
   0.005337453447,  0.01033907756,  0.01398413256,  0.01532559283,  0.01372189261,
   0.009006619453, 0.001593787922, -0.00750834588, -0.01678703353, -0.02441397496,
   -0.02851060405, -0.02745018527, -0.02014954947,-0.006299641449,  0.01350826584,
    0.03778779879,  0.06430278718,  0.09033516049,   0.1130402014,   0.1298346967,
     0.1387596726,   0.1387596726,   0.1298346967,   0.1130402014,  0.09033516049,
    0.06430278718,  0.03778779879,  0.01350826584,-0.006299641449, -0.02014954947,
   -0.02745018527, -0.02851060405, -0.02441397496, -0.01678703353, -0.00750834588,
   0.001593787922, 0.009006619453,  0.01372189261,  0.01532559283,  0.01398413256,
    0.01033907756, 0.005337453447,3.298773663e-05,-0.004606175702,-0.007851367816,
    -0.0093073342,-0.008937957697,-0.007027292158,-0.004090531264,-0.0007568425499,
   0.002352166455, 0.004726913758,  0.00604124926,  0.00618281262, 0.005242839921,
   0.003473096993, 0.001221798011,-0.001136958948,-0.003266985528,-0.004917300772,
  -0.005947266705,-0.006329186261,-0.006131541915,-0.005489705596,  -0.0045713461,
  -0.003543758066,-0.002548266901,-0.001684759744,-0.001007391373,-0.0005245624343,
  -0.000247949356,-2.220816714e-05
};

static const uint32_t firlp10Len = 110;
static const float32_t firlp10[110] = {
	0, 0, 0, 0, 0, 0, 0, 0,
  0.0001654278894,0.0003190242278,0.0005896441289, 0.000967394386, 0.001452219673,
   0.002027462237,  0.00265640649, 0.003281273646, 0.003825789783, 0.004200952593,
   0.004314531572, 0.004083777778, 0.003449436743, 0.002389595145,0.0009314040071,
  -0.0008420853992,-0.002791818231,-0.004730394576, -0.00643818872,-0.007687148172,
  -0.008269888349,-0.008030935191, -0.00689613726,-0.004895655438,-0.002176293638,
   0.001000169548, 0.004274534062, 0.007223426364,  0.00940602459,  0.01041992474,
    0.00995981507, 0.007871546783, 0.004194161855,-0.0008169600042,-0.006691038609,
   -0.01276939642, -0.01825886965, -0.02230712213, -0.02409272268,  -0.0229205396,
   -0.01831152476, -0.01007588487, 0.001640125643,  0.01634085365,  0.03320492804,
    0.05114527792,  0.06890047342,  0.08514855057,   0.0986315906,   0.1082775369,
     0.1133059412,   0.1133059412,   0.1082775369,   0.0986315906,  0.08514855057,
    0.06890047342,  0.05114527792,  0.03320492804,  0.01634085365, 0.001640125643,
   -0.01007588487, -0.01831152476,  -0.0229205396, -0.02409272268, -0.02230712213,
   -0.01825886965, -0.01276939642,-0.006691038609,-0.0008169600042, 0.004194161855,
   0.007871546783,  0.00995981507,  0.01041992474,  0.00940602459, 0.007223426364,
   0.004274534062, 0.001000169548,-0.002176293638,-0.004895655438, -0.00689613726,
  -0.008030935191,-0.008269888349,-0.007687148172, -0.00643818872,-0.004730394576,
  -0.002791818231,-0.0008420853992,0.0009314040071, 0.002389595145, 0.003449436743,
   0.004083777778, 0.004314531572, 0.004200952593, 0.003825789783, 0.003281273646,
    0.00265640649, 0.002027462237, 0.001452219673, 0.000967394386,0.0005896441289,
  0.0003190242278,0.0001654278894
};

static const uint32_t firlp23Len = 253;
static const float32_t firlp23[253] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  9.369998588e-05,8.120395796e-05,0.0001145840506,0.0001554510964,0.0002043798158,
  0.0002619018778,0.0003283704573,0.0004039391351,0.0004884337541,0.0005816709599,
  0.0006829940248,0.0007913885638, 0.000905746303, 0.001024355181,  0.00114539545,
   0.001266587875, 0.001385444077, 0.001499111648, 0.001604557154,  0.00169859617,
    0.00177788327, 0.001839085715, 0.001878902083, 0.001894132583, 0.001881851582,
   0.001839430188,  0.00176471856,   0.0016560402, 0.001512388466, 0.001333418768,
   0.001119567547,0.0008721207269, 0.000593229197,0.0002859489759,-4.576577703e-05,
  -0.0003970865218,-0.0007623524871,-0.001135160215,-0.001508419053,-0.001874532434,
  -0.002225488191, -0.00255305809,-0.002848969772,-0.003105109092,-0.003313732566,
  -0.003467678325,-0.003560594283,-0.003587132785,-0.003543152241,-0.003425914329,
   -0.00323421997,-0.002968569519,-0.002631242154,-0.002226370387,-0.001759942039,
  -0.001239783247,-0.0006754820351,-7.825625653e-05,0.0005392036983, 0.001162962173,
   0.001778100268, 0.002369012451, 0.002919752151, 0.003414389677,  0.00383741362,
   0.004174116068,   0.0044110124, 0.004536232911, 0.004539903719, 0.004414511379,
   0.004155220464, 0.003760153661, 0.003230616217, 0.002571256133, 0.001790162642,
  0.0008988755872,-8.765856182e-05,-0.001151238568,-0.002270616125,-0.003421800677,
  -0.004578427412,-0.005712203681, -0.00679341238,-0.007791468874,-0.008675527759,
  -0.009415114298,-0.009980774485, -0.01034473907, -0.01048156153, -0.01036875229,
  -0.009987351485, -0.00932246726,-0.008363736793,-0.007105711382,-0.005548156332,
  -0.003696243512,-0.001560652629,0.0008424476837,  0.00349152484, 0.006359921303,
   0.009416203946,  0.01262461208,  0.01594561338,   0.0193365328,  0.02275226451,
    0.02614604682,  0.02947028354,  0.03267738596,  0.03572063521,  0.03855504096,
    0.04113817215,  0.04343095422,  0.04539838806,  0.04701023549,  0.04824157432,
    0.04907328635,  0.04949241132,  0.04949241132,  0.04907328635,  0.04824157432,
    0.04701023549,  0.04539838806,  0.04343095422,  0.04113817215,  0.03855504096,
    0.03572063521,  0.03267738596,  0.02947028354,  0.02614604682,  0.02275226451,
     0.0193365328,  0.01594561338,  0.01262461208, 0.009416203946, 0.006359921303,
    0.00349152484,0.0008424476837,-0.001560652629,-0.003696243512,-0.005548156332,
  -0.007105711382,-0.008363736793, -0.00932246726,-0.009987351485, -0.01036875229,
   -0.01048156153, -0.01034473907,-0.009980774485,-0.009415114298,-0.008675527759,
  -0.007791468874, -0.00679341238,-0.005712203681,-0.004578427412,-0.003421800677,
  -0.002270616125,-0.001151238568,-8.765856182e-05,0.0008988755872, 0.001790162642,
   0.002571256133, 0.003230616217, 0.003760153661, 0.004155220464, 0.004414511379,
   0.004539903719, 0.004536232911,   0.0044110124, 0.004174116068,  0.00383741362,
   0.003414389677, 0.002919752151, 0.002369012451, 0.001778100268, 0.001162962173,
  0.0005392036983,-7.825625653e-05,-0.0006754820351,-0.001239783247,-0.001759942039,
  -0.002226370387,-0.002631242154,-0.002968569519, -0.00323421997,-0.003425914329,
  -0.003543152241,-0.003587132785,-0.003560594283,-0.003467678325,-0.003313732566,
  -0.003105109092,-0.002848969772, -0.00255305809,-0.002225488191,-0.001874532434,
  -0.001508419053,-0.001135160215,-0.0007623524871,-0.0003970865218,-4.576577703e-05,
  0.0002859489759, 0.000593229197,0.0008721207269, 0.001119567547, 0.001333418768,
   0.001512388466,   0.0016560402,  0.00176471856, 0.001839430188, 0.001881851582,
   0.001894132583, 0.001878902083, 0.001839085715,  0.00177788327,  0.00169859617,
   0.001604557154, 0.001499111648, 0.001385444077, 0.001266587875,  0.00114539545,
   0.001024355181, 0.000905746303,0.0007913885638,0.0006829940248,0.0005816709599,
  0.0004884337541,0.0004039391351,0.0003283704573,0.0002619018778,0.0002043798158,
  0.0001554510964,0.0001145840506,8.120395796e-05,9.369998588e-05
};


/* Resampled channel signals */
uint8_t CH1_ResampledVals[MAX_RESAMPLEDSIG_LEN];
uint8_t CH2_ResampledVals[MAX_RESAMPLEDSIG_LEN];

static int32_t calcFilters(uint8_t origSR, uint8_t newSR, Filter* filt1ptr, Filter* filt2ptr, Filter* filt3ptr);
static int32_t findNextMultiple(int32_t n, int32_t q);
static uint32_t getFiltDelay(uint8_t fact);

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

/**
  * @brief  Process the resampled signal array for trigger.
  * @param  len: length of resampled signal
  * @retval Signal index where it triggered
  */
int32_t chkTrigResampSig(int32_t len)
{
	uint8_t* sig;
	int32_t i;

	if(trigsrcVals[trigsrc] == TRIGSRC_CH1)
		sig = CH1_ResampledVals;
	else
		sig = CH2_ResampledVals;

	/* Rising edge trigger */
	if(trigtypeVals[trigtype] == TRIGTYPE_RIS){
		for(i = 1; i < len; i++)
			if(sig[i-1] <= triglvl && triglvl < sig[i])
				return i-1;
	}
	/* Falling edge trigger */
	else if(trigtypeVals[trigtype] == TRIGTYPE_FAL){
		for(i = 1; i < len; i++)
			if(sig[i-1] > triglvl && triglvl >= sig[i])
				return i-1;
	}
	/* Rising/Falling edge trigger */
	else if(trigtypeVals[trigtype] == TRIGTYPE_EDGE){
		for(i = 1; i < len; i++)
			if((sig[i-1] > triglvl && triglvl >= sig[i]) ||
			   (sig[i-1] <= triglvl && triglvl < sig[i]))
				return i-1;
	}

	return len/2;		/* trigger condition not met, return midpoint as default */
}

/**
  * @brief  Resample both the channel signals of length lenx of samplerate index origSR to new samplerate newSR.
  * @param  offset: Signal's sample offset in ADC buffer
  * @param  lenx: length of x
  * @param  origSR: Index of the sample rate of x
  * @param  newSR: Index of the sample rate of output
  * @retval Length of the resampled signal
  */
int32_t resampleChannels(uint32_t offset, uint32_t lenx, uint8_t origSR, uint8_t newSR)
{
	Filter filt1, filt2, filt3;
	int32_t nFilters;
	uint8_t *x, *CHx_ResampledVals;
	uint32_t ch, i;

	/* no change in sample rate, return input signal */
	if(newSR == origSR){
		for(i = 0; i < lenx; i++){
			CH1_ResampledVals[i] = CH1_ADC_vals[offset + i];
			CH2_ResampledVals[i] = CH2_ADC_vals[offset + i];
		}

		return lenx;
	}

	nFilters = calcFilters(origSR, newSR, &filt1, &filt2, &filt3);

	/* given sample rate conversion not supported */
	if(nFilters == -1)
		return -1;

	arm_fir_interpolate_instance_f32 hInterpolator;
	arm_fir_decimate_instance_f32 hDecimator;
	float32_t tempbuf1[MAX_RESAMPLEDSIG_LEN1];
	uint32_t prevStageOupLen, prevStageOupOffset;

	/* iterate for each channel */
	for(ch = 0; ch < 2; ch++){
		if(ch == 0){
			x = (uint8_t *)CH1_ADC_vals + offset;
			CHx_ResampledVals = CH1_ResampledVals;
		}
		else{
			x = (uint8_t *)CH2_ADC_vals + offset;
			CHx_ResampledVals = CH2_ResampledVals;
		}

		/* Filter 1 */
		if(filt1.type == FILTERTYPE_INT){
			uint32_t filtdelay = getFiltDelay(filt1.factor);
			uint32_t leninpbuf = lenx + floor((float32_t)(filtdelay + filt1.factor - 1)/filt1.factor);
			float32_t inpbuf[leninpbuf];
			float32_t statebuf[leninpbuf + filt1.ntaps/filt1.factor - 1];

			/* copy x into a float array for filtering */
			for(i = 0; i < lenx; i++)
				inpbuf[i] = (float32_t)x[i] * filt1.factor;		/* compensate for the interpolation loss */
			for(i = lenx; i < leninpbuf; i++)
				inpbuf[i] = 0;		/* append zeros to signal to obtain the complete signal output from the filter */

			arm_fir_interpolate_init_f32(&hInterpolator, filt1.factor, filt1.ntaps, filt1.coeffs, statebuf, leninpbuf);
			arm_fir_interpolate_f32(&hInterpolator, inpbuf, tempbuf1, leninpbuf);

			prevStageOupOffset = filtdelay;			/* ignore first filtdelay elements of the output */
			prevStageOupLen = lenx * filt1.factor;
		}
		else{
			uint32_t filtdelay = getFiltDelay(filt1.factor);
			uint32_t leninpbuf = findNextMultiple(lenx + filtdelay, filt1.factor);
			float32_t inpbuf[leninpbuf];
			float32_t statebuf[filt1.ntaps + leninpbuf - 1];

			/* append zeros to obtain the complete signal output and also to make input length a multiple of dec factor */
			for(i = 0; i < lenx; i++)
				inpbuf[i] = x[i];
			for(i = lenx; i < leninpbuf; i++)
				inpbuf[i] = 0;

			arm_fir_decimate_init_f32(&hDecimator, filt1.ntaps, filt1.factor, filt1.coeffs, statebuf, leninpbuf);
			arm_fir_decimate_f32(&hDecimator, inpbuf, tempbuf1, leninpbuf);

			prevStageOupOffset = floor((float32_t)(filtdelay + filt1.factor - 1)/filt1.factor);
			prevStageOupLen = leninpbuf/filt1.factor - prevStageOupOffset;
		}

		/* processing over, put results in output array */
		if(nFilters == 1){
			for(i = 0; i < prevStageOupLen; i++)
				CHx_ResampledVals[i] = tempbuf1[prevStageOupOffset + i];
		}
		else{
			float32_t tempbuf2[MAX_RESAMPLEDSIG_LEN2];

			/* Filter 2 */
			if(filt2.type == FILTERTYPE_INT){
				uint32_t filtdelay = getFiltDelay(filt2.factor);
				uint32_t leninpbuf = prevStageOupLen + floor((float32_t)(filtdelay + filt2.factor - 1)/filt2.factor);
				float32_t statebuf[leninpbuf + filt2.ntaps/filt2.factor - 1];

				for(i = 0; i < prevStageOupLen; i++)
					tempbuf1[prevStageOupOffset + i] = tempbuf1[prevStageOupOffset + i] * filt2.factor;
				for(i = prevStageOupLen; i < leninpbuf; i++)
					tempbuf1[prevStageOupOffset + i] = 0;

				arm_fir_interpolate_init_f32(&hInterpolator, filt2.factor, filt2.ntaps, filt2.coeffs, statebuf, leninpbuf);
				arm_fir_interpolate_f32(&hInterpolator, (float32_t *)tempbuf1 + prevStageOupOffset, tempbuf2, leninpbuf);

				prevStageOupOffset = filtdelay;
				prevStageOupLen = prevStageOupLen * filt2.factor;
			}
			else{
				uint32_t filtdelay = getFiltDelay(filt2.factor);
				uint32_t leninpbuf = findNextMultiple(prevStageOupLen + filtdelay, filt2.factor);
				float32_t statebuf[filt2.ntaps + leninpbuf - 1];

				for(i = prevStageOupLen; i < leninpbuf; i++)
					tempbuf1[prevStageOupOffset + i] = 0;

				arm_fir_decimate_init_f32(&hDecimator, filt2.ntaps, filt2.factor, filt2.coeffs, statebuf, leninpbuf);
				arm_fir_decimate_f32(&hDecimator, (float32_t *)tempbuf1 + prevStageOupOffset, tempbuf2, leninpbuf);

				prevStageOupOffset = floor((float32_t)(filtdelay + filt2.factor - 1)/filt2.factor);
				prevStageOupLen = leninpbuf/filt2.factor - prevStageOupOffset;
			}

			/* processing over, put results in output array */
			if(nFilters == 2){
				for(i = 0; i < prevStageOupLen; i++)
					CHx_ResampledVals[i] = tempbuf2[prevStageOupOffset + i];
			}
			else{
				/* Filter 3 */
				if(filt3.type == FILTERTYPE_INT){
					uint32_t filtdelay = getFiltDelay(filt3.factor);
					uint32_t leninpbuf = prevStageOupLen + floor((float32_t)(filtdelay + filt3.factor - 1)/filt3.factor);
					float32_t statebuf[leninpbuf + filt3.ntaps/filt3.factor - 1];

					for(i = 0; i < prevStageOupLen; i++)
						tempbuf2[prevStageOupOffset + i] = tempbuf2[prevStageOupOffset + i] * filt3.factor;
					for(i = prevStageOupLen; i < leninpbuf; i++)
						tempbuf2[prevStageOupOffset + i] = 0;

					arm_fir_interpolate_init_f32(&hInterpolator, filt3.factor, filt3.ntaps, filt3.coeffs, statebuf, leninpbuf);
					arm_fir_interpolate_f32(&hInterpolator, (float32_t *)tempbuf2 + prevStageOupOffset, tempbuf1, leninpbuf);

					prevStageOupOffset = filtdelay;
					prevStageOupLen = prevStageOupLen * filt3.factor;
				}
				else{
					uint32_t filtdelay = getFiltDelay(filt3.factor);
					uint32_t leninpbuf = findNextMultiple(prevStageOupLen + filtdelay, filt3.factor);
					float32_t statebuf[filt3.ntaps + leninpbuf - 1];

					for(i = prevStageOupLen; i < leninpbuf; i++)
						tempbuf2[prevStageOupOffset + i] = 0;

					arm_fir_decimate_init_f32(&hDecimator, filt3.ntaps, filt3.factor, filt3.coeffs, statebuf, leninpbuf);
					arm_fir_decimate_f32(&hDecimator, (float32_t *)tempbuf2 + prevStageOupOffset, tempbuf1, leninpbuf);

					prevStageOupOffset = floor((float32_t)(filtdelay + filt3.factor - 1)/filt3.factor);
					prevStageOupLen = leninpbuf/filt3.factor - prevStageOupOffset;
				}

				for(i = 0; i < prevStageOupLen; i++)
					CHx_ResampledVals[i] = tempbuf1[prevStageOupOffset + i];
			}
		}
	}

	return prevStageOupLen;
}

/**
  * @brief  Determine the set of filters needed for a desired sample rate conversion.
  * @param  origSR: Index of the sample rate of input
  * @param  newSR: Index of the sample rate of output
  * @param  filt1ptr: Pointer to the first resultant filter
  * @param  filt1ptr: Pointer to the second resultant filter
  * @param  filt1ptr: Pointer to the third resultant filter
  * @retval Number of filters needed
  */
static int32_t calcFilters(uint8_t origSR, uint8_t newSR, Filter* filt1ptr, Filter* filt2ptr, Filter* filt3ptr)
{
	Filter filt1, filt2, filt3;		/* upto 3 interpolation/decimation filters are used to achieve the desired sample rate conversion ratio */
	uint8_t nFilters;

	/* Interpolation cases */
	if(origSR == 6 && newSR == 5){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 6 && newSR == 4){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;
	}
	else if(origSR == 6 && newSR == 3){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 10;
		filt1.coeffs = (float32_t *)firlp10;
		filt1.ntaps = firlp10Len;
	}
	else if(origSR == 5 && newSR == 4){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 5;
		filt2.coeffs = (float32_t *)firlp5;
		filt2.ntaps = firlp5Len;
	}
	else if(origSR == 5 && newSR == 3){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;
	}
	else if(origSR == 5 && newSR == 2){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 10;
		filt1.coeffs = (float32_t *)firlp10;
		filt1.ntaps = firlp10Len;
	}
	else if(origSR == 4 && newSR == 3){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 4 && newSR == 2){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 4;
		filt1.coeffs = (float32_t *)firlp4;
		filt1.ntaps = firlp4Len;
	}
	else if(origSR == 4 && newSR == 1){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 8;
		filt1.coeffs = (float32_t *)firlp8;
		filt1.ntaps = firlp8Len;
	}
	else if(origSR == 4 && newSR == 0){
		nFilters = 3;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 2;
		filt2.coeffs = (float32_t *)firlp2;
		filt2.ntaps = firlp2Len;

		filt3.type = FILTERTYPE_INT;
		filt3.factor = 23;
		filt3.coeffs = (float32_t *)firlp23;
		filt3.ntaps = firlp23Len;
	}
	else if(origSR == 3 && newSR == 2){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 3 && newSR == 1){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 4;
		filt1.coeffs = (float32_t *)firlp4;
		filt1.ntaps = firlp4Len;
	}
	else if(origSR == 3 && newSR == 0){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 23;
		filt2.coeffs = (float32_t *)firlp23;
		filt2.ntaps = firlp23Len;
	}
	else if(origSR == 2 && newSR == 1){
		nFilters = 1;
		filt1.type = FILTERTYPE_INT;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 2 && newSR == 0){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 10;
		filt1.coeffs = (float32_t *)firlp10;
		filt1.ntaps = firlp10Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 23;
		filt2.coeffs = (float32_t *)firlp23;
		filt2.ntaps = firlp23Len;
	}
	else if(origSR == 1 && newSR == 0){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 6;
		filt2.coeffs = (float32_t *)firlp6;
		filt2.ntaps = firlp6Len;
	}

	/* Decimation cases */
	else if(origSR == 5 && newSR == 6){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 4 && newSR == 6){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;
	}
	else if(origSR == 3 && newSR == 6){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 10;
		filt1.coeffs = (float32_t *)firlp10;
		filt1.ntaps = firlp10Len;
	}
	else if(origSR == 4 && newSR == 5){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 2;
		filt2.coeffs = (float32_t *)firlp2;
		filt2.ntaps = firlp2Len;
	}
	else if(origSR == 3 && newSR == 5){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 5;
		filt1.coeffs = (float32_t *)firlp5;
		filt1.ntaps = firlp5Len;
	}
	else if(origSR == 2 && newSR == 5){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 10;
		filt1.coeffs = (float32_t *)firlp10;
		filt1.ntaps = firlp10Len;
	}
	else if(origSR == 3 && newSR == 4){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 2 && newSR == 4){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 4;
		filt1.coeffs = (float32_t *)firlp4;
		filt1.ntaps = firlp4Len;
	}
	else if(origSR == 1 && newSR == 4){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 8;
		filt1.coeffs = (float32_t *)firlp8;
		filt1.ntaps = firlp8Len;
	}
	else if(origSR == 0 && newSR == 4){
		nFilters = 3;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 23;
		filt1.coeffs = (float32_t *)firlp23;
		filt1.ntaps = firlp23Len;

		filt2.type = FILTERTYPE_DEC;
		filt2.factor = 2;
		filt2.coeffs = (float32_t *)firlp2;
		filt2.ntaps = firlp2Len;

		filt3.type = FILTERTYPE_INT;
		filt3.factor = 5;
		filt3.coeffs = (float32_t *)firlp5;
		filt3.ntaps = firlp5Len;
	}
	else if(origSR == 2 && newSR == 3){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 1 && newSR == 3){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 4;
		filt1.coeffs = (float32_t *)firlp4;
		filt1.ntaps = firlp4Len;
	}
	else if(origSR == 0 && newSR == 3){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 23;
		filt1.coeffs = (float32_t *)firlp23;
		filt1.ntaps = firlp23Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 5;
		filt2.coeffs = (float32_t *)firlp5;
		filt2.ntaps = firlp5Len;
	}
	else if(origSR == 1 && newSR == 2){
		nFilters = 1;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 2;
		filt1.coeffs = (float32_t *)firlp2;
		filt1.ntaps = firlp2Len;
	}
	else if(origSR == 0 && newSR == 2){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 23;
		filt1.coeffs = (float32_t *)firlp23;
		filt1.ntaps = firlp23Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 10;
		filt2.coeffs = (float32_t *)firlp10;
		filt2.ntaps = firlp10Len;
	}
	else if(origSR == 0 && newSR == 1){
		nFilters = 2;
		filt1.type = FILTERTYPE_DEC;
		filt1.factor = 6;
		filt1.coeffs = (float32_t *)firlp6;
		filt1.ntaps = firlp6Len;

		filt2.type = FILTERTYPE_INT;
		filt2.factor = 5;
		filt2.coeffs = (float32_t *)firlp5;
		filt2.ntaps = firlp5Len;
	}

	/* given sample rate conversion not supported */
	else{
		return -1;
	}

	*filt1ptr = filt1;
	*filt2ptr = filt2;
	*filt3ptr = filt3;

	return nFilters;
}

/**
  * @brief  Find the next multiple of q greater than or equal to n.
  * @param  n: the multiple should be atleast n
  * @param  q: quotient
  * @retval Next mutiple of q >= n
  */
static int32_t findNextMultiple(int32_t n, int32_t q)
{
	if(!(n % q)){
		return n;
	}

	return n + (q - (n % q));
}

/**
  * @brief  Get the group delay of a filter.
  * @param  Decimation/Interpolation factor of the filter
  * @retval Integer group delay
  */
static uint32_t getFiltDelay(uint8_t fact)
{
	uint32_t origlen;	/* original length of filter without appended zeros */

	switch(fact)
	{
		case 2: origlen = 102; break;
		case 4: origlen = 102; break;
		case 5: origlen = 102; break;
		case 6: origlen = 102; break;
		case 8: origlen = 102; break;
		case 10: origlen = 102; break;
		case 23: origlen = 234; break;
		default: origlen = 102; break;
	}

	return (origlen - 1)/2;		/* grpdelay = filtorder/2 */
}
