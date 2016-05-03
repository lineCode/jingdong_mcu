/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-04-29 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "angle_as5600.h"
#include "signal_control.h"
#include "fgcp.h"

typedef enum tagDOOR_CALIBRATOR_STATE
{
	DOOR_CALIBRATOR_STATE_INIT,
	DOOR_CALIBRATOR_STATE_CLOSING,

} DOOR_CALIBRATOR_STATE;


static DOOR_CALIBRATOR_STATE gDoorCalibratorState;
static unsigned short gDoorCalibratorTick;
static unsigned short gDoorCalibratorAngleCount;
static unsigned short gDoorCalibratorAngle[5];

void doorCalibratorInit()
{
	gDoorCalibratorState = DOOR_CALIBRATOR_STATE_INIT;
	gDoorCalibratorTick = 0;
	gDoorCalibratorAngleCount = 0;
}


void doorCalibratorSetCloseSignal()
{
	gDoorCalibratorState = DOOR_CALIBRATOR_STATE_CLOSING;
	gDoorCalibratorTick = 0;
	gDoorCalibratorAngleCount = 0;

}

#define abs(a,b) ((a) > (b) ? (a) - (b) : (b) - (a))

void doorCalibratorMachine(unsigned short rawAngle)
{
	unsigned char i;
	if (gDoorCalibratorState != DOOR_CALIBRATOR_STATE_CLOSING)
		return;

	if (getDoorState() != DOOR_STATE_CLOSED)
	{
		gDoorCalibratorState = DOOR_CALIBRATOR_STATE_INIT;
		return;
	}

	if (!overTickCount(gDoorCalibratorTick, 2000))
		return;

	gDoorCalibratorTick = getTickCount();

	gDoorCalibratorAngle[gDoorCalibratorAngleCount % 5] = rawAngle;
	gDoorCalibratorAngleCount++;

	if (gDoorCalibratorAngleCount < 5)
		return;

	for (i = 1; i < 5; i++)
	{
		if (abs(gDoorCalibratorAngle[0], gDoorCalibratorAngle[i]) > 5)
		{
			break;
		}
	}

	if (i < 5)
		return;

	as5600CloseDoorSignal();
	gDoorCalibratorState = DOOR_CALIBRATOR_STATE_INIT;
	//
	fgcpReportNeed(FGCP_REPORT_NEED_FLAG_0X78);
}




