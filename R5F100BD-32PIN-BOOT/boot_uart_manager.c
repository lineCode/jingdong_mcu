/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/
#include "..\inc\R5F100BD_BranchTable.h"
#include "boot_r_cg_macrodriver.h"
#include <string.h>
#include "boot_r_cg_port.h"
#include "boot_r_cg_userdefine.h"
#include "boot_r_cg_serial.h"
#include "boot_utility.h"
#include "boot_uart_manager.h"

//fridge --   UART0
//android °å -- UART1
void uartStationPutAndroidRxData(unsigned char data);
void androidUartCallbackSendend();
void bootFGCPRecv(unsigned char rxData);


static volatile unsigned char  gBootUartTxLen;
static unsigned short  gBootUartTxIdleTick;
static unsigned char gBootUartTxBuffer[32];

void bootUartCallbackSoftwareoverrun(unsigned char rxData)
{
	if (gMcuState == MCU_STATE_BOOT)
	{
		bootFGCPRecv(rxData);
	}
	else
	{
		uartStationPutAndroidRxData(rxData);
	}
}


void bootUartCallbackSendend()
{
	if (gMcuState == MCU_STATE_BOOT)
	{
		gBootUartTxLen = 0x00;
	}
	else
	{
		androidUartCallbackSendend();
	}

}


unsigned char bootUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle)
{
/*
	if (length > UART_TX_BUFFER_MAX_SIZE)
		return MD_ERROR;

	if (gBootUartTxLen)
	{
		gBootUartTxIdleTick = getTickCount();
		return MD_ERROR;
	}

	if (inIdle)
	{
		if (!overTickCount(gBootUartTxIdleTick, 100))
			return MD_ERROR;
	}
*/
	int i;
	for (i = 0; i < length; i++)
		gBootUartTxBuffer[i] = buffer[i];
	//memcpy(gBootUartTxBuffer, buffer, length);

	gBootUartTxLen = 1;
	R_UART1_Send(gBootUartTxBuffer, length);

	gBootUartTxIdleTick = getTickCount();
	return 0;
	//return MD_OK;
}


void bootUartManagerInit()
{
	gBootUartTxLen = 0;
	gBootUartTxIdleTick = 0;

}


void bootUartManagerRun()
{

}



