/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

Fridge  General Configuration Protocol

--*/
#ifndef BOOT_FRIDGE_GCP_H
#define BOOT_FRIDGE_GCP_H

//0	同步头	1 BYTE	0XAA
//1	消息长度	1 BYTE	length
//2	家电类型	1 BYTE	冰箱--0xCA
//冷柜--0xC9
//冰吧--0xC8
//展示柜--0xC7
//3	帧同步校验	1 BYTE	帧同步校验 = 消息长度^家电类型（异或）
//4	保留	1 BYTE	保留填充0
//5	保留	1 BYTE	保留填充0
//6	消息标识	1 BYTE	原数返回
//7	框架协议版本	1 BYTE	无定义填0
//8	家电协议版本	1 BYTE	无定义填0
//9	消息类型标识	1 BYTE	原数返回消息类型
//消息体	N BYTE
//消息校验码	1 BYTE	（消息长度 + 家电类型 + ...消息体) ^ 0xff + 1


typedef enum tagFGCP_STATE
{
	FGCP_STATE_IDLE,
	FGCP_STATE_SYNC_HEADER,
	FGCP_STATE_LENGTH,
	FGCP_STATE_OTHER_DATA,
	FGCP_STATE_RX_END,
	FGCP_STATE_TX_ACK_79,
	FGCP_STATE_TX_ACK_1,
	FGCP_STATE_ERROR,

} FGCP_STATE;

#define SYNC_HEADER_ID  0x99

typedef enum tagMESSAGE_TYPE
{
	MESSAGE_ID_NULL,
	MESSAGE_ID_WRITE,
	MESSAGE_ID_DATA0,
	MESSAGE_ID_DATA1,
	MESSAGE_ID_DATA_END,
	MESSAGE_ID_IVERIFY,
	MESSAGE_ID_BOOTSWAP,
	MESSAGE_ID_RESET,
	MESSAGE_ID_ERASE,
	MESSAGE_ID_ACK,
	MESSAGE_ID_TEST,
};

typedef struct tagFGCP_DATA_HEADR
{
	unsigned char mSyncHeader;
	unsigned char mLength;
	unsigned char mAppliances;
	unsigned char mFrameSyncCheck;
	unsigned char mReserve1;
	unsigned char mReserve2;
	unsigned char mMessageId;
	unsigned char mFrameworkVersion;
	unsigned char mFrameApplianceProtocolVersion;
	unsigned char mMessageType;
	unsigned char mData0;
	unsigned char mData1;
	unsigned char mData2;
	unsigned char mData3;
	unsigned char mData4;
	unsigned char mData5;
	unsigned char mData6;
	unsigned char mData7;

} FGCP_DATA_HEADR;

#define FGCP_HEADER_BYTES	   10
#define FGCP_TAIL_CHECK_BYTES  1
#define FGCP_PACKAGE_INFO_BYTES (FGCP_HEADER_BYTES + FGCP_TAIL_CHECK_BYTES)

#define FGCP_MAX_DATA_LENGTH   (128+16)

typedef struct tagFGCP_STATE_MACHINE
{
	FGCP_STATE  mState;
	unsigned short mPos;
	unsigned short mWatchDogTickSecond;
	union
	{
		unsigned char mBuffer[FGCP_MAX_DATA_LENGTH];
		FGCP_DATA_HEADR mHeader;
	} mData;

} FGCP_STATE_MACHINE;

unsigned char bootFGCPGetMessageCheck(unsigned char *messageBuf);
unsigned char bootFGCPMessageCheck(unsigned char *messageBuf);
void bootFGCPStateMachine(FGCP_STATE_MACHINE *stateMachine, unsigned char rxData);

void bootFGCPAnalysis(unsigned char *buf, unsigned char len);
void bootFGCPInit();
void bootFGCPRun();

#endif
