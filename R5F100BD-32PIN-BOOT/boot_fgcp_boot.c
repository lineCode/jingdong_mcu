/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

--*/
#include "boot_r_cg_macrodriver.h"
#include <string.h>
#include "boot_r_cg_port.h"
#include "boot_r_cg_userdefine.h"
#include "boot_r_cg_serial.h"
#include "boot_utility.h"
#include "boot_r_cg_timer.h"
#include "boot_uart_manager.h"
#include "boot_fgcp.h"
#include "boot_update.h"

static FGCP_STATE_MACHINE     gBootFGCPStateMachine;
static volatile unsigned char gBootFGCPRecvFlag;


void bootFGCPRecv(unsigned char rxData)
{
	bootFGCPStateMachine(&gBootFGCPStateMachine, rxData);
	gBootFGCPRecvFlag = 1;
}


void bootFGCPInit()
{
	gBootFGCPStateMachine.mState = FGCP_STATE_IDLE;
	gBootFGCPStateMachine.mWatchDogTickSecond = 0;
	gBootFGCPRecvFlag = 0;
}


void bootFGCPRun()
{
	if (gBootFGCPRecvFlag)
	{
		gBootFGCPRecvFlag = 0;
		gBootFGCPStateMachine.mWatchDogTickSecond = getTickCount();
	}
	else if (overTickCount(gBootFGCPStateMachine.mWatchDogTickSecond, 50))
	{
		gBootFGCPStateMachine.mWatchDogTickSecond = getTickCount();
		gBootFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}

	if (gBootFGCPStateMachine.mState == FGCP_STATE_RX_END)
	{
		if (bootFGCPMessageCheck(gBootFGCPStateMachine.mData.mBuffer) == MD_OK)
		{
			updateUartMessageProc(gBootFGCPStateMachine.mData.mBuffer);
		}
		gBootFGCPStateMachine.mState = FGCP_STATE_IDLE;
	}
}



