/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include <string.h>
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "sensor_adc.h"
#include "r_cg_timer.h"
#include "fgcp.h"
#include "signal_control.h"
#include "angle_as5600.h"
#include "sensor_map.h"
#include "uart_manager.h"
#include "utility.h"
#include "heater.h"

#include "infrared_monitor.h"

typedef enum tagFGCP_REPORT_STATE
{
	FGCP_REPORT_STATE_INIT,
	FGCP_REPORT_STATE_IDLE,

} FGCP_REPORT_STATE;

static FGCP_TYPE          gFgcpType;
static FGCP_REPORT_STATE  gFGCPReportState;
static unsigned char	  gFGCPReportCount;
static unsigned short	  gFGCPReportTickSecond;
static unsigned short	  gFGCPReportTestTickSecond;

 
static unsigned char	  gFGCPeportNeedFlag;


static void fill_test_Message(unsigned char *messageBuf)
{
	unsigned char *dataBuf = messageBuf + 2;
	unsigned short value;
	char temperature;

	messageBuf[0] = 0x99;
	messageBuf[1] = 0x08;

	temperature = (char)getTemperatureFromADCValue(gTemperatureSensorADCValue);
	value = (unsigned short)temperature;
	dataBuf[0] = (unsigned char)value;
	dataBuf[1] = (unsigned char)(value >> 8);

	value = (unsigned short)getHumidityFromADCValue(temperature, gHumiditySensorADCValue);
	dataBuf[2] = (unsigned char)value;
	dataBuf[3] = (unsigned char)(value >> 8);

	value = gInfraredSensorADCValue;
	dataBuf[4] = (unsigned char)value;
	dataBuf[5] = (unsigned char)(value >> 8);

	value = gAS5600AngleRaw;
	dataBuf[6] = (unsigned char)value;
	dataBuf[7] = (unsigned char)(value >> 8);
}


static void fill_0x78_MessageData(unsigned char *messageBuf)
{
	unsigned char *dataBuf = messageBuf + FGCP_HEADER_BYTES;
	unsigned short value;
	unsigned long  temp = 0;
	char temperature;

	temperature = (char)getTemperatureFromADCValue(gTemperatureSensorADCValue);
	value = (unsigned char)(temperature * 2 + 100);
	dataBuf[0] = (unsigned char)value;

	value = (unsigned char)getHumidityFromADCValue(temperature, gHumiditySensorADCValue);
	dataBuf[1] = (unsigned char)value;

	value = gAS5600Degree;
	//value = last_count;
	//value = last_var_value;
	//value = last_avg_value;
	dataBuf[2] = (unsigned char)value;
	dataBuf[3] = (unsigned char)(value >> 8);

	value = gInfraredSensorADCValue;
	dataBuf[4] = (unsigned char)value;
	dataBuf[5] = (unsigned char)(value >> 8);

	value = gAS5600StartAngleRaw;
	//value = gInfraredMonitor.count;
	dataBuf[6] = (unsigned char)value;
	dataBuf[7] = (unsigned char)(value >> 8);

	value = gAS5600AngleRaw;
	//value = last_avg_value;
	//value = last_avg_value;
	dataBuf[8] = (unsigned char)value;
	dataBuf[9] = (unsigned char)(value >> 8);

	value = gInfraredHumanValve;
	dataBuf[10] = (unsigned char)value;
	dataBuf[11] = (unsigned char)(value >> 8);

	dataBuf[12] = (unsigned char)(FIRMWARE_VERSION);

	value = gInfraredHumanDistanceTick;
	dataBuf[13] = (unsigned char)value;
	dataBuf[14] = (unsigned char)(value >> 8);

	dataBuf[15] = (unsigned char)(gCamera0PhotoDegree / 10);
	dataBuf[16] = (unsigned char)(gCamera1PhotoDegree / 10);

	dataBuf[17] = (unsigned char)(gAS5600I2CErrorCount);
	
	dataBuf[18] = 0;
	dataBuf[19] = 0;
	if (gHeaterData.mPeriod > 0)
	{
		dataBuf[18] = 2000 / gHeaterData.mPeriod;
		temp = gHeaterData.mDuty;
		temp = temp * 100 / gHeaterData.mPeriod;
		dataBuf[19] = temp; 
		//((gHeaterData.mDuty * 100) / gHeaterData.mPeriod);
	}
	//dataBuf[18] = (unsigned char)gVar;
	//dataBuf[19] = (unsigned char)last_var_value;
}



static void fill_0xED_MessageData(unsigned char *messageBuf)
{
	unsigned char *dataBuf = messageBuf + FGCP_HEADER_BYTES;
	unsigned char value;
	unsigned char i;
	char temperature;

	temperature = (char)getTemperatureFromADCValue(gTemperatureSensorADCValue);
	value = (unsigned char) (temperature * 2 + 100);
	dataBuf[0] = (unsigned char)value;

	value = (unsigned char)getHumidityFromADCValue(temperature, gHumiditySensorADCValue);
	dataBuf[1] = (unsigned char)value;

	memset(dataBuf + 2, 0, 8);
}


static void fgcpReport0xED()
{
	unsigned char messageBuf[24];

	fill_0xED_MessageData(messageBuf);
	createFGCPMessage(messageBuf, 0xED, 0, 10);
	fridegUartSend(messageBuf, FGCP_PACKAGE_INFO_BYTES + 10, 1);
}


static void fgcpReport0x78()
{
	unsigned char messageBuf[32];

	fill_0x78_MessageData(messageBuf);
	createFGCPMessage(messageBuf, 0x78, 0, 20);
	
	if (androidUartSend(messageBuf, FGCP_PACKAGE_INFO_BYTES + 20, 1) == MD_OK)
	{
		gFGCPeportNeedFlag &= (unsigned char)(~FGCP_REPORT_NEED_FLAG_0X78);
	}
}


static void fgcpReportTest()
{
	unsigned char messageBuf[10];

	fill_test_Message(messageBuf);
	androidUartSend(messageBuf, 10, 1);
}


static void fgcpReportStateMachineInitProcess()
{
	if (overTickCount(gFGCPReportTickSecond, 1000))
	{
		fgcpReport0xED();
		gFGCPReportCount++;

		if (gFGCPReportCount > 3)
		{
			gFGCPReportState = FGCP_REPORT_STATE_IDLE;
			return;
		}
	}
}




static void fgcpReportStateMachineIdleProcess()
{
	if (overTickCount(gFGCPReportTickSecond, 60000))
	//if (overTickCount(gFGCPReportTickSecond, 1000))
		{
		fgcpReport0xED();
	//	fgcpReport0x78();
		gFGCPReportTickSecond = getTickCount();
	}
	else if (((gMcuState == MCU_STATE_TEST) && overTickCount(gFGCPReportTestTickSecond, 200)) 
		|| (gFGCPeportNeedFlag & FGCP_REPORT_NEED_FLAG_0X78))
//	else if (overTickCount(gFGCPReportTestTickSecond, 500))
	{
		//fgcpReportTest();
		fgcpReport0x78();
		gFGCPReportTestTickSecond = getTickCount();
	}
}




static void fgcpReportStateMachine()
{
	switch (gFGCPReportState)
	{
	case FGCP_REPORT_STATE_INIT:
		fgcpReportStateMachineInitProcess();
		break;

	case FGCP_REPORT_STATE_IDLE:
		fgcpReportStateMachineIdleProcess();
		break;

	
	}
}


FGCP_TYPE getFgcpType()
{
	return gFgcpType;
}


void setFgcpType(FGCP_TYPE type)
{
	if (type == FGCP_TYPE_MIDEA || type == FGCP_TYPE_JD)
		gFgcpType = type;
}


void fgcpReportNeed(unsigned char needFlag)
{
	gFGCPeportNeedFlag |= needFlag;
}


void fgcpReportInit()
{
	gFgcpType = FGCP_TYPE_MIDEA;
	gFGCPReportState = FGCP_REPORT_STATE_INIT;
	gFGCPReportTickSecond = 0;
	gFGCPReportTestTickSecond = 0;
	gFGCPReportCount = 0;
}

void fgcpReportRun()
{
	fgcpReportStateMachine();
}


