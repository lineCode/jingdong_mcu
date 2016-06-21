/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-13 Created by Fibo Lu

--*/


#ifndef UART_MANAGER_H
#define UART_MANAGER_H

#define UART_TX_BUFFER_MAX_SIZE  128

unsigned char androidUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle);
unsigned char fridegUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle);

void uartManagerInit();
void uartManagerRun();


#endif
