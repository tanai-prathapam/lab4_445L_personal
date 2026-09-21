// ----------------------------------------------------------------------------
// 
// File name:     DSP.c
//
// ----------------------------------------------------------------------------
//
// Description:   This code is used for DSP processing of audio output data
//                
// Author:        Mark McDermott, Jon Valvano
// Orig gen date: Aug 1, 2020
// 
//
//
// ----------------------------------------------------------------------------
#include <stdint.h>
int32_t t0,t1,t2; // last 3 inputs/32
int32_t Rxx2;     // autocorrelation factor 
#define K 128     // how fast it responds
#define M 128
// compare audio signal with itself two samples ago
// Rxx(2) = (1/N) sum{x(n)x(n-2)} in limit as N-> infinity
// Approximate infinite sum with IIR filter
// Rxx2 = (127*Rxx2 +x(n)x(n-2) )/128
// K is the attack ratio (how fast it responds)
// M is the scale
// t0 = x(n)/32
// t1 = x(n-1)/32
// t2 = x(n-2)/32
void NoiseReject_Init(void){
  t0 = t1 = t2 = 2048/32;
}
// Rxx2 is 0 if uncorrelated (noise)
// Rxx2 is M if correlated (signal)
// Rxx2 is -M if correlated (signal); this does not occur with sound
// returns input signal x scaled by Rxx2/M
int32_t NoiseReject(int32_t x){
  t2 = t1; 
  t1 = t0; 
  t0 = x/32; 
  Rxx2 = ((K-1)*Rxx2 + t0*t2)/K;
  if(Rxx2 < -M) Rxx2 = -M; 
  if(Rxx2 >  M) Rxx2 =  M;
  return (Rxx2*x)/M; 
}

const int32_t ReW[32] ={
  1024, 946, 724, 392, 0, -392, -724, -946, -1024, -946, -724, -392, 0, 392, 724, 946,
  1024, 946, 724, 392, 0, -392, -724, -946, -1024, -946, -724, -392, 0, 392, 724, 946};
const int32_t ImW[32] ={
  0, -392, -724, -946, -1024, -946, -724, -392, 0, 392, 724, 946, 1024, 946, 724, 392,
  0, -392, -724, -946, -1024, -946, -724, -392, 0, 392, 724, 946, 1024, 946, 724, 392};
int32_t x[16];
int32_t rsum1,isum1;
int32_t rsum2,isum2;
void DFT_Init(void){
  rsum1=isum1=0;
  rsum2=isum2=0;
}
  // one data point
void DFT(uint32_t i, int32_t x){
	i = i&0x0F; // 0 to 15
  rsum1 += x*ReW[i];  
  isum1 += x*ImW[i];
  rsum2 += x*ReW[2*i];  
  isum2 += x*ImW[2*i];
}
int32_t Mag1(void){int32_t mag;
//  rsum1 /= 1024;
//  isum1 /= 1024;
  rsum1 /= 8192;
  isum1 /= 8192;
  mag = rsum1*rsum1+isum1*isum1;
  rsum1=isum1=0;
  return mag;
}
int32_t Mag2(void){int32_t mag;
//  rsum2 /= 1024;
//  isum2 /= 1024;
  rsum2 /= 8192;
  isum2 /= 8192;
  mag = rsum2*rsum2+isum2*isum2;
  rsum2=isum2=0;
  return mag;
}
int32_t aMag1(void){ // equiv freq = fs/16
  rsum1=isum1=0;
  for(int i=0;i<16;i++){
    rsum1 += x[i]*ReW[i];  
    isum1 += x[i]*ImW[i];
  }
  rsum1 /= 1024;
  isum1 /= 1024;
  return rsum1*rsum1+isum1*isum1;
}
int32_t aMag2(void){// equiv freq = 2*fs/16
  rsum2=isum2=0;
  for(int i=0;i<16;i++){
    rsum2 += x[i]*ReW[2*i];  
    isum2 += x[i]*ImW[2*i];
  }
  rsum2 /= 1024;
  isum2 /= 1024;
  return rsum2*rsum2+isum2*isum2;
}
