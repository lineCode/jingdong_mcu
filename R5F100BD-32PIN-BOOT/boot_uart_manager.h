/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-13 Created by Fibo Lu

--*/


#ifndef BOOT_UART_MANAGER_H
#define BOOT_UART_MANAGER_H

#define UART_TX_BUFFER_MAX_SIZE  32

unsigned char bootUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle);

void bootUartManagerInit();
void bootUartManagerRun();


#endif
