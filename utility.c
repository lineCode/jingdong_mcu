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
#include "r_cg_timer.h"



#if 0
volatile unsigned long  gTickMillisecondCount;

MCU_STATE  gMcuState;
unsigned short gMcuStateTick;



unsigned short getTickCount()
{
	unsigned short count;

	TMMK01 = 1U;    /* disable INTTM01 interrupt */
	count = (unsigned short)gTickMillisecondCount;
	TMMK01 = 0U;    /* enable INTTM01 interrupt */

	return count;
}


unsigned char overTickCount(unsigned short originTick, unsigned short distanceTick)
{
	unsigned short currentTick = getTickCount();
	unsigned short diff;

	diff = currentTick - originTick;
	if (diff >= distanceTick)
		return 1;
	else
		return 0;
}



void timeInit()
{
	gTickMillisecondCount = 0;
	R_TAU0_Channel1_Start();
}


void timeRun()
{
		
}



void mcuStateInit()
{
	gMcuState = MCU_STATE_INIT;
	gMcuStateTick = 0;
}


void mcuStateRun()
{
	if (gMcuState == MCU_STATE_INIT)
	{
		if (overTickCount(gMcuStateTick, 3000))
		{
			gMcuState = MCU_STATE_WORK;//MCU_STATE_TEST; //MCU_STATE_WORK;
		}
	}
}


MCU_STATE  getMcuState()
{
	return gMcuState;
}


void setMcuState(MCU_STATE state)
{
	gMcuState = state;
}

#endif


extern volatile unsigned long  gTickMillisecondCount;


unsigned long getTickCount32()
{
	unsigned long count;

	TMMK01 = 1U;    /* disable INTTM01 interrupt */
	count = gTickMillisecondCount;
	TMMK01 = 0U;    /* enable INTTM01 interrupt */

	return count;
}


unsigned char overTickCount32(unsigned long originTick, unsigned long distanceTick)
{
	unsigned long currentTick = getTickCount32();
	unsigned long diff;

	diff = currentTick - originTick;
	if (diff >= distanceTick)
		return 1;
	else
		return 0;
}


void memcpyEx(void *dest, void *src, unsigned short size)
{
	unsigned char *uDest = (unsigned char *)dest;
	unsigned char *uSrc = (unsigned char *)src;

	while (size > 0)
	{
		*uDest = *uSrc;
		uDest++;
		uSrc++;
		size--;
	}
}
