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
#include "sensor_adc.h"
#include "sensor_map.h"
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
		setHeaterControlPin(0);
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


void heaterAutoUpdate(unsigned char update)
{
	gHeaterData.mAuto = update;
}


//on£º15  off : 45	on£º25  off : 35	on£º35  off : 25	on£º45  off : 45	on£º50  off : 10	on£º55  off : 5
//on£º10  off : 50	on£º20  off : 45	on£º30  off : 40	on£º35  off : 30	on£º45  off : 20	on£º50  off : 10
//on£º10  off : 45	on£º15  off : 45	on£º25  off : 35	on£º30  off : 30	on£º40  off : 45	on£º45  off : 45
//on£º5  off : 55	on£º10  off : 50	on£º15  off : 45	on£º20  off : 40	on£º30  off : 30	on£º40  off : 20
//on£º0  off : 60	on£º5  off : 55	on£º10  off : 50	on£º15  off : 45	on£º20  off : 40	on£º30  off : 30
static const unsigned char conHeaterDutyTable[][6] =
{
	{15, 25, 35, 45, 60, 55},
	{10, 20, 30, 35, 45, 50},
	{10, 15, 25, 30, 40, 45},
	{5,  10, 15, 20, 30, 40},
	{0,  5,  10, 15, 20, 30},
};

static const char conHeaterDutyTableTemperature[] =
{ 10, 14, 22, 28, 35};

static const char conHeaterDutyTableHumidity[] =
{55, 65, 75, 85};


char indexForValue(char value, const char limits[], char size)
{
	char i;
	for (i = 0; i < size; i++)
	{
		if (value <= limits[i])
			break;
	}
	return i;
}


void heaterLoadParameter()
{
	char tIndex;
	char hIndex;
	char temperature;
	char humidity;
	unsigned char duty;

	if (!gHeaterData.mAuto)
		return;

	temperature = (char)getTemperatureFromADCValue(gTemperatureSensorADCValue);
	humidity = (unsigned short)getHumidityFromADCValue(temperature, gHumiditySensorADCValue);


	tIndex = indexForValue(temperature, conHeaterDutyTableTemperature, sizeof(conHeaterDutyTableTemperature) / sizeof(conHeaterDutyTableTemperature[0]));
	hIndex = indexForValue(humidity, conHeaterDutyTableHumidity, sizeof(conHeaterDutyTableHumidity) / sizeof(conHeaterDutyTableHumidity[0]));

	duty = conHeaterDutyTable[hIndex][tIndex];
	heaterSetParamter(33, duty * 5 / 3);
}


void heaterInit()
{
	gHeaterData.mCount = 0;
	heaterSetParamter(100, 50);

	gHeaterData.mTick = 0;
	heaterEnable(1);
	heaterAutoUpdate(1);
}


void heaterRun()
{
	if (overTickCount(gHeaterData.mTick, 5000))
	{
		//table;
		heaterLoadParameter();
		gHeaterData.mTick = getTickCount();
	}
}


