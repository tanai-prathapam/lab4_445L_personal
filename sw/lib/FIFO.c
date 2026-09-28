
#include <stdint.h>
#include "FIFO.h"   
/*
----------------------------------------------FIFO1----------------------------------------
*/

// Two-index implementation of the transmit FIFO
// can hold 0 to TXFIFOSIZE-1 elements
uint32_t volatile TxPutI1; // where to put next
uint32_t volatile TxGetI1; // where to get next
char static TxFifo1[TXFIFOSIZE];

void TxFifo1_Init(void){
  TxPutI1 = TxGetI1 = 0; // empty
}
int TxFifo1_Put(char data){
uint32_t newPutI = (TxPutI1+1)&(TXFIFOSIZE-1);
  if(newPutI == TxGetI1) return 0; // fail if full
  TxFifo1[TxPutI1] = data;          // save in Fifo
  TxPutI1 = newPutI;               // next place to put
  return 1;
}
char TxFifo1_Get(void){char data;
  if(TxGetI1 == TxPutI1) return 0;      // fail if empty
  data = TxFifo1[TxGetI1];              // retrieve data
  TxGetI1 = (TxGetI1+1)&(TXFIFOSIZE-1); // next place to get
  return data;
}
uint32_t TxFifo1_Size(void){
  return (TxPutI1-TxGetI1)&(TXFIFOSIZE-1);
}

// Two-index implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements
uint32_t volatile RxPutI1; // where to put next
uint32_t volatile RxGetI1; // where to get next
char static RxFifo1[RXFIFOSIZE];

void RxFifo1_Init(void){
  RxPutI1 = RxGetI1 = 0;  // empty
}
int RxFifo1_Put(char data){
uint32_t newPutI = (RxPutI1+1)&(RXFIFOSIZE-1);
  if(newPutI == RxGetI1) return 0; // fail if full
  RxFifo1[RxPutI1] = data;          // save in Fifo
  RxPutI1 = newPutI;               // next place to put
  return 1;
}
char RxFifo1_Get(void){char data;
  if(RxGetI1 == RxPutI1) return 0;      // fail if empty
  data = RxFifo1[RxGetI1];              // retrieve data
  RxGetI1 = (RxGetI1+1)&(RXFIFOSIZE-1); // next place to get
  return data;
}
uint32_t RxFifo1_Size(void){
  return (RxPutI1-RxGetI1)&(RXFIFOSIZE-1);
}



