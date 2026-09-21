
/* Median.c
 * Jonathan Valvano
 * Various implementations of the median filter
 * July 13, 2026
 */
 #include <stdint.h>

uint32_t median5(uint32_t u[5]){ int i; int mask[5]; uint32_t max,ix;
  max = 0;
  ix = 0;
  for(i=0;i<5; i++){
    mask[i] = 1;
    if(u[i] > max){
      max = u[i];
      ix = i;
    }
  }
  mask[ix] = 0; // exclude largest
  max = 0;
  ix = 0;
  for(i=0;i<5; i++){
    if(mask[i]&&(u[i] > max)){
      max = u[i];
      ix = i;
    }
  }
  mask[ix] = 0; // exclude second largest
  max = 0;
  for(i=0;i<5; i++){
    mask[i] = 1;
    if(mask[i]&&(u[i] > max)){
      max = u[i];
    }
  }
  return max;
}


int8_t x0,x1,x2; // last 3 inputs
// 1 usec running at 80 MHz
int8_t Median(int8_t x){ 
int8_t result;
  x2 = x1;
  x1 = x0;
  x0 = x;
  if(x0>x1){
    if(x1>x2){
      result = x1;   // x0>x1,x1>x2       x0>x1>x2
    }else{
      if(x0>x2){
        result = x2;   // x0>x1,x2>x1,x0>x2 x0>x2>x1
      }else{
        result = x0;   // x0>x1,x2>x1,x2>x0 x2>x0>x1
      }
    }
  }else{ 
    if(x2>x1){
      result = x1;   // x1>x0,x2>x1       x2>x1>x0
    }else{
      if(x0>x2){ 
        result = x0;   // x1>x0,x1>x2,x0>x2 x1>x0>x2
      }else{
        result = x2;   // x1>x0,x1>x2,x2>x0 x1>x2>x0
      }
    }
  }
  return(result);
}


int8_t Median3(int8_t u1,int8_t u2,int8_t u3){ 
int8_t result;
  if(u1>u2){
    if(u2>u3){
      result = u2;   // u1>u2,u2>u3       u1>u2>u3
    }else{
      if(u1>u3){
         result = u3;   // u1>u2,u3>u2,u1>u3 u1>u3>u2
      }else{
          result = u1;   // u1>u2,u3>u2,u3>u1 u3>u1>u2
      }
    }
  }else{ 
    if(u3>u2){
      result = u2;   // u2>u1,u3>u2       u3>u2>u1
    }else{
      if(u1>u3){
        result = u1;   // u2>u1,u2>u3,u1>u3 u2>u1>u3
      }else{
        result = u3;   // u2>u1,u2>u3,u3>u1 u2>u3>u1
      }
    }
  }
  return(result);
}

int8_t x7[7]; // last 7 inputs
int8_t f7[7]; // found flag
// 11 usec running at 80 MHz
int8_t Median5(int8_t x){ int i,j; int8_t max;
  for(i=3; i>=0; i--){
    x7[i+1] = x7[i]; // push into MACQ
    f7[i]=1; // 1 means look at it
  }
  f7[5] = 1;
  x7[0] = x;
  max = x7[0]; j=0;
  for(i=1; i<7; i++){
    if(x7[i] > max){
      max = x7[i];
      j = i;
    }
  }
  f7[j] = 0; // 0 means ignore the max
  max = -128;
  for(i=0; i<7; i++){
    if((x7[i] > max)&&f7[i]){
      max = x7[i];
      j = i;
    }
  }
  f7[j] = 0; // 0 means ignore the second highest
  max = -128;
  for(i=0; i<7; i++){
    if((x7[i] > max)&&f7[i]){
      max = x7[i];
    }
  }
  return max; // median is third highest
}
// 16 usec running at 80 MHz
int8_t Median7(int8_t x){ int i,j; int8_t max;
  for(i=5; i>=0; i--){
    x7[i+1] = x7[i]; // push into MACQ
    f7[i]=1; // 1 means look at it
  }
  f7[6] = 1;
  x7[0] = x;
  max = x7[0]; j=0;
  for(i=1; i<7; i++){
    if(x7[i] > max){
      max = x7[i];
      j = i;
    }
  }
  f7[j] = 0; // 0 means ignore the max
  max = -128;
  for(i=0; i<7; i++){
    if((x7[i] > max)&&f7[i]){
      max = x7[i];
      j = i;
    }
  }
  f7[j] = 0; // 0 means ignore the second highest
  max = -128;
  for(i=0; i<7; i++){
    if((x7[i] > max)&&f7[i]){
      max = x7[i];
      j = i;
    }
  }
  f7[j] = 0;// 0 means ignore the third highest
  max = -128;
  for(i=0; i<7; i++){
    if((x7[i] > max)&&f7[i]){
      max = x7[i];
    }
  }
  return max; // median is fourth highest
}  
