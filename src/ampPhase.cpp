
#define FFT_t uint8_t
#define output_t float

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

const float pi = 3.14159265358979323846264338328;

float *computeSin(){
  float *sinTable = new float[256];
  for(int i=0; i<256; i++){
    float sinPlug = 2*pi*(i/256.0);
    sinTable[i] = sin(sinPlug);
  }
  return sinTable;
}

float *computeCos(){
  float *cosTable = new float[256];
  for(int i=0; i<256; i++){
    float cosPlug = 2*pi*(i/256.0);
    cosTable[i] = cos(cosPlug);
  }
  return cosTable;
}

float referenceSin(float phi, float *sinTable){
  float idxCalc = 256.0*(phi/2.0*pi); //Assuming phi is in radians, converting phi to the size of our reference array
  int idx = round(idxCalc);
  return sinTable[idx];
}

float referenceCos(float phi, float *cosTable){
  float idxCalc = 256.0*(phi/2.0*pi); //Assuming phi is in radians, converting phi to the closest index of our reference array
  int idx = round(idxCalc);
  return cosTable[idx];
}


int main(int argc, char **argv) {

  float *sinTable = computeSin();
  float *cosTable = computeCos();

  for(int i = 0; i<256; i++){ //Let's check our reference tables
    //printf("sinTable[%d] = %f\n", i, sinTable[i]);
    //printf("cosTable[%d] = %f\n", i, cosTable[i]);
  }

  int FFTSIZE, NCHAN, NTEST;

  FFTSIZE = atoi(argv[1]);
  NCHAN = atoi(argv[2]);
  NTEST = atoi(argv[3]);

  printf("FFTSIZE = %d, NCHAN = %d, NTEST = %d\n",FFTSIZE,NCHAN,NTEST);

  struct timeval preLoop, postLoop;

  int startTime = gettimeofday(&preLoop, NULL);
  printf("The start time of the main loop is  = %u.%06u s\n",preLoop.tv_sec,preLoop.tv_usec);

  FFT_t* data = (FFT_t*) malloc(sizeof(FFT_t)*NCHAN*FFTSIZE*2*NTEST);
  output_t* output = (output_t*) malloc(sizeof(output_t)*FFTSIZE*NCHAN*NCHAN);

  #pragma omp parallel for
  for (size_t count=0;count<NTEST;count++) {
    size_t dataofs = count*FFTSIZE*NCHAN*2;
    for (size_t chan1=0;chan1<NCHAN;chan1++) {
      FFT_t* cdata1 = &(data[dataofs+chan1*FFTSIZE*NCHAN*2]);
      for (size_t chan2=chan1;chan2<NCHAN; chan2++) {
	FFT_t* cdata2 = &(data[dataofs+chan2*FFTSIZE*NCHAN*2]);
	// REAL PART 
	output_t* outptr = &(output[(chan1*NCHAN+chan2)*FFTSIZE]);
	for (size_t i=0;i<FFTSIZE;i++) {
	  size_t j=2*i;
	  FFT_t PA = cdata1[j]*cdata2[j]; // Assuming data is stored as amplitude in j then phase in j+1
	  FFT_t Pphi = cdata1[j+1]-cdata2[j+1];
	  outptr[i] += PA*cosTable[Pphi];
	}
	// IMAG PART
	if (chan1!=chan2) {
	  output_t* outptr = &(output[(chan2*NCHAN+chan1)*FFTSIZE]);
	  for (size_t i=0;i<FFTSIZE;i++) {
	    size_t j=2*i;
	    FFT_t PA = cdata1[j]*cdata2[j]; // Assuming data is stored as amplitude in j then phase in j+1  
	    FFT_t Pphi = cdata1[j+1]-cdata2[j+1];
	    outptr[i] += PA*sinTable[Pphi];
	  }
	}
      }
    }
  }

  int endTime = gettimeofday(&postLoop,NULL);
  printf("The end time of the main loop is  = %u.%06u s\n",postLoop.tv_sec,postLoop.tv_usec);
  int loopTime = (1000000*(postLoop.tv_sec-preLoop.tv_sec)+(postLoop.tv_usec-preLoop.tv_usec));
  printf("The main loop took %ld us\n", loopTime);
  float avgTime = (float) loopTime / (float) NTEST;
  printf("On average, a packet is processed roughly every %f us\n\n", avgTime);

  startTime = gettimeofday(&preLoop, NULL);
  printf("The start time of the second loop is  = %u.%06u s\n",preLoop.tv_sec,preLoop.tv_usec);

  // need to show we actually use outputr
  float x=0;
  for (size_t i=0;i<FFTSIZE*NCHAN*NCHAN;i++) {
    x+=output[i];
  }
  printf("X=%f\n",x);
  
  endTime = gettimeofday(&postLoop,NULL);
  printf("The end time of the second loop is  = %u.%06u s\n",postLoop.tv_sec,postLoop.tv_usec);
  printf("The second loop took %u.%06u s\n\n",(postLoop.tv_sec-preLoop.tv_sec),(postLoop.tv_usec-preLoop.tv_sec));

  return 0;
}
