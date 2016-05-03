/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "ring_buffer.h"
#include "utility.h"
#include "sensor_adc.h"
#include "angle_as5600.h"
#include "fgcp.h"
#include "uart_manager.h"

//fridge --   UART0
//android °å -- UART1

static volatile RingBufferContext  gFridgeToAndroidContext;
static volatile RingBufferContext  gAndroidToFridgeContext;


void uartStationPutFridgeRxData(unsigned char data)
{
	unsigned char *buf1, *buf2;
	unsigned char len1, len2;

	ringBufferGetFree(&gFridgeToAndroidContext, &buf1, &len1, &buf2, &len2);

	if (len1 > 0)
	{
		*buf1 = data;
	}	
	else if (len2 > 0)
	{
		*buf2 = data;
	}

	ringBufferPut(&gFridgeToAndroidContext, 1);
}


void uartStationPutAndroidRxDataEx(unsigned char data)
{
	unsigned char *buf1, *buf2;
	unsigned char len1, len2;

	ringBufferGetFree(&gAndroidToFridgeContext, &buf1, &len1, &buf2, &len2);

	if (len1 > 0)
	{
		*buf1 = data;
	}
	else if (len2 > 0)
	{
		*buf2 = data;
	}

	ringBufferPut(&gAndroidToFridgeContext, 1);
}



static void fridegToAndroid()
{
	unsigned char *buf1, *buf2;
	unsigned char len1, len2;

	if (ringBufferIsEmpty(&gFridgeToAndroidContext))
		return;

	ringBufferGetData(&gFridgeToAndroidContext, &buf1, &len1, &buf2, &len2);
	if (len1)
	{
		if (androidUartSend(buf1, len1, 0) == MD_OK)
		{
			fridgeFGCPAnalysis(buf1, len1);
			ringBufferGet(&gFridgeToAndroidContext, len1);
		}
	}
	else if (len2)
	{
		if (androidUartSend(buf2, len2, 0) == MD_OK)
		{
			fridgeFGCPAnalysis(buf2, len2);
			ringBufferGet(&gFridgeToAndroidContext, len2);
		}
	}
}


static void androidToFrideg()
{
	unsigned char *buf1, *buf2;
	unsigned char len1, len2;

	if (ringBufferIsEmpty(&gAndroidToFridgeContext))
		return;

	ringBufferGetData(&gAndroidToFridgeContext, &buf1, &len1, &buf2, &len2);
	if (len1)
	{
		if (fridegUartSend(buf1, len1, 0) == MD_OK)
		{
			androidFGCPAnalysis(buf1, len1);
			ringBufferGet(&gAndroidToFridgeContext, len1);
		}
	}
	else if (len2)
	{
		if (fridegUartSend(buf2, len2, 0) == MD_OK)
		{
			androidFGCPAnalysis(buf2, len2);
			ringBufferGet(&gAndroidToFridgeContext, len2);
		}
	}
}




void uartStationInit()
{
	ringBufferInit(&gFridgeToAndroidContext);
	ringBufferInit(&gAndroidToFridgeContext);
}


void uartStationRun()
{
	fridegToAndroid();
	androidToFrideg();
}






