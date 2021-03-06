/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

--*/
#ifdef WIN32
#include <Windows.h>
#define MD_OK 0
#define MD_ERROR1 1
#else
#include "boot_r_cg_macrodriver.h"
#include <string.h>
#include "boot_r_cg_port.h"
#include "boot_r_cg_userdefine.h"
#include "boot_r_cg_serial.h"
#include "boot_utility.h"
#endif
#include "boot_fgcp.h"


unsigned char bootFGCPGetMessageCheck(unsigned char *messageBuf)
{
	unsigned char check;
	unsigned char i;
	unsigned char reseve; 
	FGCP_DATA_HEADR* header = (FGCP_DATA_HEADR*)messageBuf;

	//消息校验码	1 BYTE	（消息长度 + 家电类型 + ...消息体) ^ 0xff + 1
	check = 0;
	for (i = 1; i < header->mLength; i++)
		check += messageBuf[i];

	reseve = 0xff;
	check = (check ^ reseve) + 1;
	
	return check;
}


unsigned char bootFGCPMessageCheck(unsigned char *messageBuf)
{
	//3	帧同步校验	1 BYTE	帧同步校验 = 消息长度^家电类型（异或） Frame synchronization check
	unsigned char check;
	FGCP_DATA_HEADR* header = (FGCP_DATA_HEADR*)messageBuf;

	check = header->mLength ^ header->mAppliances;
	if (header->mFrameSyncCheck != check)
		return MD_ERROR1;

	//消息校验码	1 BYTE	（消息长度 + 家电类型 + ...消息体) ^ 0xff + 1
	check = bootFGCPGetMessageCheck(messageBuf);
	if (messageBuf[header->mLength] != check)
		return MD_ERROR1;

	return MD_OK;
}


void bootFGCPStateMachine(FGCP_STATE_MACHINE *stateMachine, unsigned char rxData)
{
	switch (stateMachine->mState)
	{
	case FGCP_STATE_IDLE:
		if (rxData == SYNC_HEADER_ID)
		{
			stateMachine->mState = FGCP_STATE_SYNC_HEADER;
			stateMachine->mPos = 0;

			stateMachine->mData.mBuffer[stateMachine->mPos] = rxData;
			stateMachine->mPos++;
		}
		else
			stateMachine->mState = FGCP_STATE_ERROR;
		break;

	case FGCP_STATE_SYNC_HEADER:
		if (rxData <= FGCP_MAX_DATA_LENGTH)
		{
			stateMachine->mState = FGCP_STATE_OTHER_DATA;
			stateMachine->mData.mBuffer[stateMachine->mPos] = rxData;
			stateMachine->mPos++;
		}
		else
		{
			stateMachine->mState = FGCP_STATE_ERROR;
		}
		break;

	case FGCP_STATE_OTHER_DATA:
		if (stateMachine->mPos <= stateMachine->mData.mHeader.mLength)
		{
			stateMachine->mData.mBuffer[stateMachine->mPos] = rxData;
			stateMachine->mPos++;

			if (stateMachine->mPos > stateMachine->mData.mHeader.mLength)
			{
				stateMachine->mState = FGCP_STATE_RX_END;
				stateMachine->mPos += 2;
			}
		}
		else
		{
			stateMachine->mState = FGCP_STATE_ERROR;
		}
		break;
	}
}

