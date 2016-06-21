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
#include "monitor.h"

static unsigned short gAndroidMonitorTick;
static unsigned char  gAndroidBeatLevel;

unsigned char getAndroidBeatLevel()
{
	unsigned char flag;
	flag = P5.0;
	return flag;
}


unsigned char getAndroidStateLevel()
{
	unsigned char flag;
	flag = P7.0;
	return flag;
}


void monitorInit()
{
	gAndroidMonitorTick = 0;
	gAndroidBeatLevel = 0;
}


void monitorRun()
{
	if (getAndroidStateLevel())
	{
		gAndroidMonitorTick = getTickCount();
		return;
	}

	if (getAndroidBeatLevel() != gAndroidBeatLevel)
	{
		gAndroidBeatLevel   = getAndroidBeatLevel();
		gAndroidMonitorTick = getTickCount();
	}

	if (overTickCount(gAndroidMonitorTick, 50000))
	{
		sendResetSignal();
		gAndroidMonitorTick = getTickCount();
	}
}







