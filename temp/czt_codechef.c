#include <stdio.h>
#include <math.h>

#define ADC_BUF_SIZE    480
#define M_PI            3.141592653589793f

static void czt(unsigned char* x, float* oup, float f1, float f2, unsigned int N, unsigned int M);
void ComplexFFT(float vector[], unsigned int sample_rate, int sign);

unsigned char x[ADC_BUF_SIZE] = {115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 
149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 
82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 
115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 
149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96, 115, 135, 149, 154, 149, 135, 115, 96, 82, 77, 82, 96};

float cztout[545];

int main(void) 
{
	float f1, f2, maxmag;
	unsigned int maxidx,  i;
	
	maxidx = 43;
	
	f1 = (maxidx - 1)/512.0f;	/* zoom-in one bin to the left and right */
	if(maxidx == 255)
		f2 = maxidx/512.0f;
	else
		f2 = (maxidx + 1)/512.0f;

	czt(x, cztout, f1, f2, ADC_BUF_SIZE, 545);
    
    maxmag = 0;
    for(i = 0; i < 545; i++){
        if(cztout[i] > maxmag){
            maxmag = cztout[i];
            maxidx = i;
        }
    }
    
    printf("maxidx, freq := %u, %f\n\n", maxidx, 48000*(f1 + maxidx*(f2 - f1)/545.0));
    
    printf("oup9 = [");
    for(i = 0; i < 545; i++){
        if(i == 544)
            printf("%f", cztout[i]);
        else
            printf("%f, ", cztout[i]);
            
        if((i+1)%100 == 0)
            printf("...\n");
    }
    printf("];");
    
	return 0;
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
	float c1, s1, c2, s2, a, b;

	L = N + M - 1;
	theta0 = 2.0f*M_PI*f1;
	phi0 = -2.0f*M_PI*(f2 - f1)/(float)M;

	float yY[2*L], vV[2*L];

	/* yn = A^-n * W^(n^2/2) * xn */
	for(i = 0; i < N; i++){
		c1 = cos(-(float)i*theta0);
		s1 = sin(-(float)i*theta0);
		c2 = cos((float)(i*i)*phi0/2.0f);
		s2 = sin((float)(i*i)*phi0/2.0f);
		yY[2*i] = (c1*c2 - s1*s2) * (float)x[i];		/* real */
		yY[2*i+1] = (c1*s2 + c2*s1) * (float)x[i];		/* imaginary */
	}
	for(i = N; i < L; i++){
		yY[2*i] = 0;
		yY[2*i+1] = 0;
	}

	/* FFT(yY) */
    ComplexFFT(yY, L, 1);

	/* vn = W^(-n^2/2) for 0<=n<=M-1 */
	for(i = 0; i < M; i++){
		vV[2*i] = cos(-(float)(i*i)*phi0/2.0f);
		vV[2*i+1] = sin(-(float)(i*i)*phi0/2.0f);
	}
	/* vn = W^(-(L-n)^2/2) for M<=n<=L-1 */
	for(i = M; i < L; i++){
		vV[2*i] = vV[2*(L-i)];		/* exploit symmetry to reuse values from 0 to M-1 */
		vV[2*i+1] = vV[2*(L-i)+1];
	}

	/* FFT(vV) */
    ComplexFFT(vV, L, 1);

	/* FFT(yY)*FFT(vV) */
	for(i = 0; i < L; i++){
	    a = yY[2*i];
	    b = yY[2*i+1];
	    
	    yY[2*i] = a*vV[2*i] - b*vV[2*i+1];
	    yY[2*i+1] = a*vV[2*i+1] + b*vV[2*i];
	}

	/* gn = IFFT[FFT(yY)*FFT(vV)] */
    ComplexFFT(yY, L, -1);

	/* oup[n] = W^(n^2/2)*gn */
	for(i = 0; i < M; i++){
		c1 = cos((float)(i*i)*phi0/2.0f);
		s1 = sin((float)(i*i)*phi0/2.0f);
		vV[2*i] = c1*yY[2*i] - s1*yY[2*i+1];
		vV[2*i+1] = c1*yY[2*i+1] + s1*yY[2*i];
	}

    float* test = vV;
	for(i = 0; i < M; i++){
        oup[i] = sqrt(test[2*i]*test[2*i] + test[2*i+1]*test[2*i+1]);
	}

	return;
}


#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

// FFT 1D
void ComplexFFT(float vector[], unsigned int sample_rate, int sign)
{
	//variables for the fft
	unsigned long n,mmax,m,j,istep,i;
	float wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	//the complex array is real+complex so the array
    //as a size n = 2* number of complex samples
    //real part is the data[index] and
    //the complex part is the data[index+1]

	//binary inversion (note that the indexes
    //start from 0 witch means that the
    //real part of the complex is on the even-indexes
    //and the complex part is on the odd-indexes)
	n=sample_rate << 1;
	j=0;
	for (i=0;i<n/2;i+=2) {
		if (j > i) {
			SWAP(vector[j],vector[i]);
			SWAP(vector[j+1],vector[i+1]);
			if((j/2)<(n/4)){
				SWAP(vector[(n-(i+2))],vector[(n-(j+2))]);
				SWAP(vector[(n-(i+2))+1],vector[(n-(j+2))+1]);
			}
		}
		m=n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//end of the bit-reversed order algorithm

	//Danielson-Lanzcos routine
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=sign*(2.0f*M_PI/(float)mmax);
		wtemp=sin(0.5f*theta);
		wpr = -2.0f*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0f;
		wi=0.0f;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*vector[j-1]-wi*vector[j];
				tempi=wr*vector[j]+wi*vector[j-1];
				vector[j-1]=vector[i-1]-tempr;
				vector[j]=vector[i]-tempi;
				vector[i-1] += tempr;
				vector[i] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
	//end of the algorithm
}
