/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

Fridge  General Configuration Protocol

--*/
#ifndef FRIDGE_GCP_H
#define FRIDGE_GCP_H



//0	ͬ��ͷ	1 BYTE	0XAA
//1	��Ϣ����	1 BYTE	length
//2	�ҵ�����	1 BYTE	����--0xCA
//���--0xC9
//����--0xC8
//չʾ��--0xC7
//3	֡ͬ��У��	1 BYTE	֡ͬ��У�� = ��Ϣ����^�ҵ����ͣ����
//4	����	1 BYTE	�������0
//5	����	1 BYTE	�������0
//6	��Ϣ��ʶ	1 BYTE	ԭ������
//7	���Э��汾	1 BYTE	�޶�����0
//8	�ҵ�Э��汾	1 BYTE	�޶�����0
//9	��Ϣ���ͱ�ʶ	1 BYTE	ԭ��������Ϣ����
//��Ϣ��	N BYTE
//��ϢУ����	1 BYTE	����Ϣ���� + �ҵ����� + ...��Ϣ��) ^ 0xff + 1


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

#define FGCP_MAX_DATA_LENGTH   128

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


unsigned char FGCPGetMessageCheck(unsigned char *messageBuf);
unsigned char FGCPMessageCheck(unsigned char *messageBuf);
void createFGCPACKMessage(unsigned char *messageBuf, unsigned char *data, unsigned char dataLength);
void createFGCPMessage(unsigned char *messageBuf, unsigned char messageType, unsigned char *data, unsigned char dataLength);
void FGCPStateMachine(FGCP_STATE_MACHINE *stateMachine, unsigned char rxData);

#define FGCP_REPORT_NEED_FLAG_0X78  0x01 

void fgcpReportNeed(unsigned char needFlag);
void fgcpReportInit();
void fgcpReportRun();


typedef enum tagDOOR_STATE
{
	DOOR_STATE_CLOSED,
	DOOR_STATE_OPEN,
	DOOR_STATE_IS_CLOSED,
} DOOR_STATE;

DOOR_STATE getDoorState();

void fridgeFGCPAnalysis(unsigned char *buf, unsigned char len);
void fgcpFridegInit();
void fgcpFridegRun();

void androidFGCPAnalysis(unsigned char *buf, unsigned char len);
void androidFGCPInit();
void androidFGCPRun();

void FGCProtocolInit();
void FGCProtocolRun();

#endif