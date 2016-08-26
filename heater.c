/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-08-26 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "utility.h"
#include "heater.h"


HEATER_DATA  gHeaterData;


static void setHeaterControlPin(unsigned char on)
{
	if (on)
		P1 |= _10_Pn4_OUTPUT_1;
	else
		P1 &= (unsigned char)(~(_10_Pn4_OUTPUT_1));
}


void timer0HeadterCallback()
{
	if (gHeaterData.mEnable == 0)
		return;

	gHeaterData.mCount++;
	if (gHeaterData.mCount >= gHeaterData.mPeriod)
	{ 
		if (gHeaterData.mDuty)
			setHeaterControlPin(1);

		gHeaterData.mCount = 0;
	}
	else if (gHeaterData.mCount >= gHeaterData.mDuty)
	{
		setHeaterControlPin(0);
	}
}


void heaterSetParamter(unsigned char freq, unsigned char duty)
{
//	DI();

	if (freq == 0)
	{
		gHeaterData.mEnable = 0;
		return;
	}
	else if (freq <= 200)
		gHeaterData.mPeriod = 2000 / freq;

	gHeaterData.mDuty = gHeaterData.mPeriod * duty / 100;

//	EI();
}


void heaterEnable(unsigned char enable)
{
	gHeaterData.mEnable = enable;
}


void heaterInit()
{
	gHeaterData.mCount = 0;
	heaterSetParamter(100, 50);

	gHeaterData.mTick = 0;
	heaterEnable(1);
}


void heaterRun()
{
	if (overTickCount(gHeaterData.mTick, 5000))
	{
		//table;
		gHeaterData.mTick = getTickCount();
	}
}


