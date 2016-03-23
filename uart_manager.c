/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include <string.h>
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "uart_manager.h"

//fridge --   UART0
//android °å -- UART1

static volatile unsigned char  gAndroidUartTxLen;
static volatile unsigned char  gFridgeUartTxLen;

static unsigned short  gAndroidUartTxIdleTick;
static unsigned short  gFridegUartTxIdleTick;

static unsigned char gAndroidUartTxBuffer[UART_TX_BUFFER_MAX_SIZE];
static unsigned char gFridegUartTxBuffer[UART_TX_BUFFER_MAX_SIZE];


void androidUartCallbackSendend()
{
	gAndroidUartTxLen = 0x00;
}


void FridgeUartCallbackSendend()
{
	gFridgeUartTxLen = 0x00;
}


unsigned char androidUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle)
{
	if (length > UART_TX_BUFFER_MAX_SIZE)
		return MD_ERROR;

	if (gAndroidUartTxLen)
	{
		gAndroidUartTxIdleTick = getTickCount();
		return MD_ERROR;
	}

	if (inIdle)
	{
		if (!overTickCount(gAndroidUartTxIdleTick, 100))
			return MD_ERROR;
	}

	memcpy(gAndroidUartTxBuffer, buffer, length);

	gAndroidUartTxLen = 1;
	R_UART1_Send(gAndroidUartTxBuffer, length);

	gAndroidUartTxIdleTick = getTickCount();
	return MD_OK;
}


unsigned char fridegUartSend(unsigned char *buffer, unsigned char length, unsigned char inIdle)
{
	if (length > UART_TX_BUFFER_MAX_SIZE)
		return MD_ERROR;

	if (gFridgeUartTxLen)
	{
		gFridegUartTxIdleTick = getTickCount();
		return MD_ERROR;
	}

	if (inIdle)
	{
		if (!overTickCount(gFridegUartTxIdleTick, 100))
			return MD_ERROR;
	}

	memcpy(gFridegUartTxBuffer, buffer, length);

	gFridgeUartTxLen = 1;
	R_UART0_Send(gFridegUartTxBuffer, length);

	gFridegUartTxIdleTick = getTickCount();

	return MD_OK;
}


void uartManagerInit()
{
	gAndroidUartTxLen = 0;
	gFridgeUartTxLen = 0;

	gAndroidUartTxIdleTick = 0;
	gFridegUartTxIdleTick  = 0;

}

void uartManagerRun()
{

}



