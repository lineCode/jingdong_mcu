/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "sensor_adc.h"
#include "r_cg_timer.h"
#include "angle_as5600.h"
#include "uart_manager.h"
#include "fgcp.h"


static FGCP_STATE_MACHINE  gAndroidFGCPStateMachine;


static void androidSetMCUParameter(unsigned char *data)
{
	unsigned short value;

	value = data[0];
	value = value << 8;
	value += data[1];
	if (value > 0)
	{
		gInfraredHumanValve = value;
	}

	value = data[2];
	value = value << 8;
	value += data[3];
	if (value > 0 && value <= 10000)
	{
		gInfraredHumanDistanceTick = value;
	}

	value = data[4];
	if (value > 0 && value <= 180)
	{
		gCamera0PhotoDegree = value * 10;
	}

	value = data[5];
	if (value > 0 && value <= 180)
	{
		gCamera1PhotoDegree = value * 10;
	}


	value = data[6];
	value = value << 8;
	value += data[7];
	if (value >= 0 && value <= 4095)
	{
		gAS5600StartAngleRaw = value;
	}

	value = data[8];
	if (value == 0)
		setMcuState(MCU_STATE_WORK);
	else if (value == 1)
		setMcuState(MCU_STATE_TEST);
}



static void androidFGCPMessageProcess()
{
	unsigned char *dataBuf = gAndroidFGCPStateMachine.mData.mBuffer + FGCP_HEADER_BYTES;

	switch (gAndroidFGCPStateMachine.mData.mHeader.mMessageType)
	{
	case 0x79:
		androidSetMCUParameter(dataBuf);
		gAndroidFGCPStateMachine.mState = FGCP_STATE_TX_ACK_79;
		break;

	case 0x7A:
		break;
	}
}


static void androidFGCPACK79()
{
	unsigned char messageBuf[16];

	messageBuf[FGCP_HEADER_BYTES + 0] = 0;
	messageBuf[FGCP_HEADER_BYTES + 1] = 0;

	createFGCPMessage(messageBuf, 0x79, 0, 8);
	if (androidUartSend(messageBuf, FGCP_PACKAGE_INFO_BYTES + 2, 1) == MD_OK)
	{
		gAndroidFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}
}


void androidFGCPAnalysis(unsigned char *buf, unsigned char len)
{	
	unsigned char i;

	gAndroidFGCPStateMachine.mWatchDogTickSecond = getTickCount();

	for (i = 0; i < len; i++)
		FGCPStateMachine(&gAndroidFGCPStateMachine, buf[i]);

	if (gAndroidFGCPStateMachine.mState == FGCP_STATE_RX_END)
	{
		if (FGCPMessageCheck(gAndroidFGCPStateMachine.mData.mBuffer) == MD_OK)
		{
			androidFGCPMessageProcess();
		}
		gAndroidFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}

}


void androidFGCPInit()
{
	gAndroidFGCPStateMachine.mState = FGCP_STATE_IDLE;
	gAndroidFGCPStateMachine.mWatchDogTickSecond = 0;
}


void androidFGCPRun()
{
	if (overTickCount(gAndroidFGCPStateMachine.mWatchDogTickSecond, 50))
	{
		gAndroidFGCPStateMachine.mWatchDogTickSecond = getTickCount();
		gAndroidFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}

	if (gAndroidFGCPStateMachine.mState == FGCP_STATE_TX_ACK_79)
	{
		androidFGCPACK79();
	}
}


