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
#include "fgcp.h"
#include "signal_control.h"
#include "angle_as5600.h"

DOOR_STATE   gDoorState;
static unsigned short  gCloseDoorTick;

static FGCP_STATE_MACHINE  gFridgeFGCPStateMachine;
static unsigned  short gFridgeFGCPTestCount = 0; 


static void closeDoor(unsigned char correctStartAngleRaw)
{
	setDoorStatePin(0);
	
	if (correctStartAngleRaw)
		gDoorState = DOOR_STATE_IS_CLOSED;
	else
		gDoorState = DOOR_STATE_CLOSED;

	gCloseDoorTick = getTickCount();
}


static void openDoor()
{
	gDoorState = DOOR_STATE_OPEN;

	setDoorStatePin(1);
	sendWakeupSignal();
}


static void doorStateMachine()
{
	switch (gDoorState)
	{
	case DOOR_STATE_CLOSED:
		if ((gAS5600StartAngleRaw != INVALID_ANGLE_RAW) && gAS5600Degree > 300)
		{
			openDoor();
		}
		break;

	case DOOR_STATE_OPEN:
		if ((gAS5600StartAngleRaw != INVALID_ANGLE_RAW) && gAS5600Degree < 30)
		{
			closeDoor(0);
		}

		break;

	case DOOR_STATE_IS_CLOSED:
		if (overTickCount(gCloseDoorTick, 3000))
		{
			as5600CloseDoorSignal();
			gDoorState = DOOR_STATE_CLOSED;
			fgcpReportNeed(FGCP_REPORT_NEED_FLAG_0X78);
		}
		break;

	}
}


DOOR_STATE getDoorState()
{
	return gDoorState;
}

static unsigned short g_openCount;
static unsigned short g_closeCount;


static void fridgeFGCPMessageProcess()
{
	if (gFridgeFGCPStateMachine.mData.mHeader.mMessageType == 0x04 \
		&& gFridgeFGCPStateMachine.mData.mHeader.mData0 == 0x00)
	{
		if (gFridgeFGCPStateMachine.mData.mHeader.mData1 & 0x01)
		{
			g_openCount++;
			openDoor();
		}
		else
		{
			g_closeCount++;
			closeDoor(1);
		}
	}
}


void fridgeFGCPAnalysis(unsigned char *buf, unsigned char len)
{
	unsigned char i;

	gFridgeFGCPStateMachine.mWatchDogTickSecond = getTickCount();

	for (i = 0; i < len; i++)
		FGCPStateMachine(&gFridgeFGCPStateMachine, buf[i]);

	if (gFridgeFGCPStateMachine.mState == FGCP_STATE_RX_END)
	{
		if (FGCPMessageCheck(gFridgeFGCPStateMachine.mData.mBuffer) == MD_OK)
		{
			fridgeFGCPMessageProcess();
		}
		gFridgeFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}
}


void fgcpFridegInit()
{
	gFridgeFGCPStateMachine.mState = FGCP_STATE_IDLE;
	gFridgeFGCPStateMachine.mWatchDogTickSecond = 0;

	gDoorState = DOOR_STATE_CLOSED;
	gCloseDoorTick = 0;
	
	g_openCount = 0;
    g_closeCount = 0;
}


void fgcpFridegRun()
{
	if (overTickCount(gFridgeFGCPStateMachine.mWatchDogTickSecond, 50))
	{
		gFridgeFGCPStateMachine.mWatchDogTickSecond = getTickCount();
		gFridgeFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}

	doorStateMachine();
}



