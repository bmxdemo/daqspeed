
#define FFT_t int16_t
#define FFTSIZE (4096)
#define NCHAN 8
#define NTEST 100000

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
  
  FFT_t* data = (FFT_t*) malloc(sizeof(FFT_t)*NCHAN*FFTSIZE*2*NTEST);
  float* output = (float*) malloc(sizeof(float)*FFTSIZE*NCHAN*NCHAN);

  #pragma omp parallel for
  for (size_t count=0;count<NTEST;count++) {
    size_t dataofs = count*FFTSIZE*NCHAN*2;
    for (size_t chan1=0;chan1<NCHAN;chan1++) {
      FFT_t* cdata1 = &(data[dataofs+chan1*FFTSIZE*NCHAN*2]);
      for (size_t chan2=chan1;chan2<NCHAN; chan2++) {
	FFT_t* cdata2 = &(data[dataofs+chan2*FFTSIZE*NCHAN*2]);
	// REAL PART 
	float* outptr = &(output[(chan1*NCHAN+chan2)*FFTSIZE]);
	for (size_t i=0;i<FFTSIZE;i++) {
	  size_t j=2*i;
	  outptr[i] = (cdata1[j]*cdata2[j]+cdata1[j+1]*cdata2[j+1]);
	}
	// IMAG PART
	if (chan1!=chan2) {
	  float* outptr = &(output[(chan2*NCHAN+chan1)*FFTSIZE]);
	  for (size_t i=0;i<FFTSIZE;i++) {
	    size_t j=2*i;
	    outptr[i] = (cdata1[j]*cdata2[j+1]-cdata1[j+1]*cdata2[j]);
	  }
	}
      }
    }
  }

  // need to show we actually use outputr
  float x=0;
  for (size_t i=0;i<FFTSIZE*NCHAN*NCHAN;i++) {
    x+=output[i];
  }
  printf("X=%f\n",x);
  
  return 0;
}

  
  
