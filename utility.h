/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/


#ifndef UTILITY_H
#define UTILITY_H

#include "R5F100BD-32PIN-BOOT\boot_utility.h"

#if 0
void usleep(unsigned short microsecond);


unsigned short getTickCount();
unsigned char  overTickCount(unsigned short originTick, unsigned short distanceTick);

unsigned long getTickCount32();
unsigned char overTickCount32(unsigned long originTick, unsigned long distanceTick);

void timeInit();
void timeRun();

typedef enum tagMCU_STATE
{
	MCU_STATE_BOOT,
	MCU_STATE_INIT,
	MCU_STATE_WORK,
	MCU_STATE_TEST,
	MCU_STATE_UPDATE,

} MCU_STATE;

extern MCU_STATE  gMcuState;

void mcuStateInit();
void mcuStateRun();

MCU_STATE  getMcuState();
void setMcuState(MCU_STATE state);
#endif


unsigned long getTickCount32();
unsigned char overTickCount32(unsigned long originTick, unsigned long distanceTick);

void memcpyEx(void *dest, void *src, unsigned short size);

#endif
