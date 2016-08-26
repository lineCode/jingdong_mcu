/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-08-26 Created by Fibo Lu

--*/


#ifndef _HEATER_H
#define _HEATER_H


typedef struct tagHEATER_DATA
{
	unsigned char  mEnable;
	unsigned short mTick;
	unsigned short mCount;
	unsigned short mPeriod;
	unsigned short mDuty;
} HEATER_DATA;

extern HEATER_DATA  gHeaterData;

void timer0HeadterCallback();

void heaterSetParamter(unsigned char freq, unsigned char duty);
void heaterEnable(unsigned char enable);
void heaterInit();
void heaterRun();

#endif
