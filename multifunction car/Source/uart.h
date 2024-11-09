#ifndef __UART_H_
#define __UART_H_

#include "delay.h"

#define S2_S  0x01	
#define S2RI 0x01
#define S2TI 0x02


extern void Uart2_Init(void);
extern void UART2_Tx_Puts(void);


#endif
