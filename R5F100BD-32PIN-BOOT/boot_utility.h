/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/


#ifndef BOOT_UTILITY_H
#define BOOT_UTILITY_H

void usleep(unsigned short microsecond);

unsigned short getTickCount();
unsigned char  overTickCount(unsigned short originTick, unsigned short distanceTick);


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

#define  FIRMWARE_VERSION  0x19

#endif
