/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "signal_control.h"

#define SIGNAL_WAKEUP		0x01
#define SIGNAL_CAMERA0		0x02
#define SIGNAL_CAMERA1		0x04
#define SIGNAL_INFRARED		0x08
#define SIGNAL_RESET		0x10

static unsigned short gSignalWakupTick;
static unsigned short gSignalCameraTick;
static unsigned short gSignalInfraredTick;
static unsigned short gSignalResetTick;
static unsigned char  gSignalFlag;


//P16
static void setWakeupSignalPin(unsigned char on)
{
	if (on)
		P1 |= _40_Pn6_OUTPUT_1;
	else
		P1 &= (unsigned char)(~(_40_Pn6_OUTPUT_1));
}


void sendWakeupSignal()
{
	setWakeupSignalPin(1);
	gSignalFlag |= SIGNAL_WAKEUP;
	gSignalWakupTick = getTickCount();
}


//P10  PZ-GPIO1
static void setCamera0SignalPin(unsigned char on)
{
	if (on)
		P1 |= _01_Pn0_OUTPUT_1;
	else
		P1 &= (unsigned char)(~(_01_Pn0_OUTPUT_1));
}


void sendCamera0Signal()
{
	gSignalFlag |= SIGNAL_CAMERA0;
	gSignalCameraTick = getTickCount();
	setCamera0SignalPin(1);
}


//P51  PZ-GPIO2
static void setCamera1SignalPin(unsigned char on)
{
	if (on)
		P5 |= _02_Pn1_OUTPUT_1;
	else
		P5 &= (unsigned char)(~(_02_Pn1_OUTPUT_1));
}


void sendCamera1Signal()
{
	gSignalFlag |= SIGNAL_CAMERA1;
	gSignalCameraTick = getTickCount();
	setCamera1SignalPin(1);
}

//P50 WEIGOU
static void setInfraredSignalPin(unsigned char on)
{
	if (on)
		P5 |= _01_Pn0_OUTPUT_1;
	else
		P5 &= (unsigned char)(~(_01_Pn0_OUTPUT_1));
}


void sendInfraredSignal()
{
	gSignalFlag |= SIGNAL_INFRARED;
	gSignalInfraredTick = getTickCount();
	setInfraredSignalPin(1);
}


//P15
static void setResetSignalPin(unsigned char on)
{
	if (on)
		P1 |= _20_Pn5_OUTPUT_1;
	else
		P1 &= (unsigned char)(~(_20_Pn5_OUTPUT_1));
}


void sendResetSignal()
{
	setResetSignalPin(1);
	gSignalFlag |= SIGNAL_RESET;
	gSignalResetTick = getTickCount();
}


//YL-GPIO1  P30
void setDoorStatePin(unsigned char state)
{
	if (state)
		P3 |= _01_Pn0_OUTPUT_1;
	else
		P3 &= (unsigned char)(~(_01_Pn0_OUTPUT_1));
}


void signalInit()
{
	gSignalFlag = 0;

	gSignalWakupTick = 0;
	gSignalCameraTick = 0;
	gSignalInfraredTick = 0;
	gSignalResetTick = 0;
	setDoorStatePin(0);
}


void signalRun()
{
	if (gSignalFlag & SIGNAL_WAKEUP)
	{
		if (overTickCount(gSignalWakupTick, 500))
		{
			setWakeupSignalPin(0);
			gSignalFlag &= (unsigned char)(~SIGNAL_WAKEUP);
		}
	}

	if (gSignalFlag & (SIGNAL_CAMERA0 | SIGNAL_CAMERA1))
	{
		if (overTickCount(gSignalCameraTick, 500))
		{
			setCamera0SignalPin(0);
			setCamera1SignalPin(0);
			gSignalFlag &= (unsigned char)(~(SIGNAL_CAMERA0 | SIGNAL_CAMERA1));
		}
	}

	if (gSignalFlag & SIGNAL_INFRARED)
	{
		if (overTickCount(gSignalInfraredTick, 600))
		{
			setInfraredSignalPin(0);
			gSignalFlag &= (unsigned char)(~(SIGNAL_INFRARED));
		}
	}

	if (gSignalFlag & SIGNAL_RESET)
	{
		if (overTickCount(gSignalResetTick, 10))
		{
			setResetSignalPin(0);
			gSignalFlag &= (unsigned char)(~(SIGNAL_RESET));
		}
	}
}







