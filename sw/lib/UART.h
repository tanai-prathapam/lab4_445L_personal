#ifndef UART_H
#define UART_H

char UART1_InCharNonBlock(void);
void UART1_OutString(char *pt);
void UART1_OutUDec(uint32_t n);
void UART1_IRQHandler(void);
// void UART1_OutChar(char data);
// char UART1_InChar(void);

#endif