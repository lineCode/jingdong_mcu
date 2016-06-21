/*
MPCP -- Mcu Private Communication Protocol 
*/
#include <stdio.h>
#include <string.h>
#include "analysis_fgcp.h"
#include "analysis_mpcp.h"

/*unsigned char gAndroidCommdBuf[30] = { 0x4E,0x53,0x4E,0x47,0x01,0xFF,0x02,0x00,
									   0x01,0x00,0x01,0x02,0xFF,0xFF,0xFF,0xFF,
									   0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									   0x00,0x00,0x00,0x00,0x00,0x00 };*/
unsigned char gAndroidCommdBuf[30] = { 0 };
//unsigned char gAndroidQuerybuf[12] = { 0xAA,0x0B,0xCA,0x00,0x00,0x01,0x00,0x00,0x03,0x09,0x66};
//unsigned char gAndroidQuerybuf[12] ={0x4E,0x53,0x4E,0x47,0x18,0xFF,0x01,0x00,0x01,0x00,0x00};
unsigned char gAndroidQuerybuf[10] = { 0 };

static unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};
/* Table of CRC values for low-order byte */
static unsigned char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};
/*计算CRC16值*/
static unsigned short CRC16(unsigned char * puchMsg, unsigned short  usDataLen)
{
	unsigned char uchCRCHi = 0xFF; /* high byte of CRC initialized */
	unsigned char uchCRCLo = 0xFF; /* low byte of CRC initialized */
	unsigned uIndex;
	while (usDataLen--)
	{
		uIndex = uchCRCLo ^ *puchMsg++;
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/*计算LenCheck字段*/
static unsigned char getLenCheckValue(unsigned char Type, unsigned char Len)
{
	unsigned char LenCheck = Type ^ Len;
	return LenCheck;
}

/*校验计算出的LenCheck是否与接受到的一致*/
 unsigned short lenCheckWhetherAlike(unsigned char *recvMessage)
{
	unsigned char recvLenCheck = recvMessage[7];
	unsigned char calculateLenCheck = getLenCheckValue(recvMessage[4], recvMessage[6]);
	if (recvLenCheck == calculateLenCheck)
		return CHECK_OK;
	else
		return CHECK_ERROR;
}

/*校验收到的是否是干扰报文*/
unsigned short checkFrameWhetherLegal(unsigned char *recvMessage)
{
	int i = 0;
	unsigned char magic[4];
	static unsigned char Magic[4] = { 0x4E, 0x53, 0x4E, 0x47 };
	memcpy(magic, &recvMessage[0], 4);
	for (i = 0; i < 4; i++)
	{
		if (magic[i] != Magic[i])
		{
			return CHECK_ERROR;
		}
	}
	return CHECK_OK;
}

/*计算Crc16值并提取*/
static void getCrc16(unsigned char * message, unsigned short messageLen, \
	unsigned char *Higbit, unsigned char *LowBit)
{
	unsigned short crc16 = CRC16(message, messageLen);
	getCrc16HigbitAndLowbit(crc16, Higbit, LowBit);
}

/*将得到的crc16值得高位和低位提取出来*/
static void getCrc16HigbitAndLowbit(unsigned short int crc16, unsigned char *Higbit, unsigned char *LowBit)
{
	unsigned short higbit = crc16 >> 8;
	unsigned short lowbit = crc16 - (higbit << 8);
	*Higbit = (unsigned char)higbit;
	*LowBit = (unsigned char)lowbit;
}
/*生成传输数据*/
unsigned short createMPCPMessageData(unsigned char *Message, unsigned char MessageType, \
	unsigned char Len, unsigned char Seq, unsigned char *Value)
{
	static unsigned char Magic[4] = { 0x4E, 0x53, 0x4E, 0x47 };
	unsigned short totalLen = 8 + Len;	//4 + 1 + 1 + 1 + 1 + Len;
	unsigned char crcHigbit;
	unsigned char crcLowbit;
	//Magic
	memcpy(&Message[0], Magic, sizeof(Magic));
	//Type
	Message[4] = MessageType;
	//Seq
	Message[5] = Seq;
	//Len
	Message[6] = Len;
	//LenCheck
	Message[7] = getLenCheckValue(MessageType, Len);
	//Value
	if (Len)
		memcpy(&Message[8], Value, Len);
	//Crc
	getCrc16(Message, totalLen, &crcHigbit, &crcLowbit);
	Message[8 + Len] = crcHigbit;
	Message[8 + Len + 1] = crcLowbit;
	return totalLen + 2;
}
static unsigned char FGCPGetMessageCheckout(unsigned char *messageBuf)
{
	unsigned char check;
	unsigned char i;
	unsigned char reseve;
	FGCP_DATA_TYPE* header = (FGCP_DATA_TYPE*)messageBuf;
	//unsigned char lenth = messageBuf[1];
	check = 0;
	for (i = 1; i < header->mLength; i++)
		check += messageBuf[i];

	reseve = 0xff;
	check = (check ^ reseve) + 1;

	return check;
}

unsigned short createFGCPMessageData(unsigned char *messageBuf, unsigned char messageType, unsigned char *data, unsigned char dataLength)
{
	//	int i;
	//	unsigned char *messageData;
	FGCP_DATA_TYPE* header = (FGCP_DATA_TYPE*)messageBuf;

	header->mSyncHeader = 0xAA;
	header->mLength = (FGCP_PACKAGE_INFO_LENTH - 1) + dataLength;
	header->mAppliances = 0xCA;
	header->mFrameSyncCheck = header->mLength ^ header->mAppliances;
	header->mReserve1 = 0;
	header->mReserve2 = 0;
	header->mMessageId = 0;
	header->mFrameworkVersion = 0;
	header->mFrameApplianceProtocolVersion = 0;
	header->mMessageType = messageType;

	if (dataLength)
		memcpy(messageBuf + FGCP_HEADER_LENTH, data, dataLength);

	messageBuf[header->mLength] = FGCPGetMessageCheckout(messageBuf);
	return header->mLength + 1;
}

unsigned short androidDataAnalysis(unsigned char *recvMessage, unsigned char *destData)
{
	//unsigned char sendToFridgeMessage[SEND_MESSAGE_MAX_SIZE] = {0};
	//unsigned char returnToAndroidMessage[SET_RESULT_LENTH] = { 0 };
	//	if ((checkFrameWhetherLegal(recvMessage) == CHECK_ERROR) || (lenCheckWhetherAlike(recvMessage) == CHECK_ERROR))
	//	{
	//		mcuResponseSetResult(recvMessage[5],(unsigned char)Failed);
	//		return;
	//	}
	unsigned char messageType = recvMessage[4];
	switch (messageType)
	{
	case SetFridgeSwitchTemperature:	 //设置冰箱状态0200
		return storageRecvMessageQueryFGstate(recvMessage, destData);
	case SetMcuUpdateMode:				 //设置MCU更新模式	　	9	0	N，1为打开；0为关闭

		return 0;
	case SetFridgeTestState:			 //设置冰箱测试状态
		return storageRecvMessageQueryFroststate(recvMessage, destData);
	case GetCurrentFridgeState:			 //获取冰箱状态0300
		return queryFGState(recvMessage, destData);
	case GetFridgeSN:					//
		return queryFGSN(recvMessage, destData);
	case GetFridgeWarningMessage:		//获取冰箱故障信息0302
		return queryFGWarning(recvMessage, destData);
	case GetFridgeReportData:			//获取冰箱状态上报数据0301
		return queryFGReportData(recvMessage, destData);
	case GetFridgeTestState:			//获取冰箱测试状态
		return queryFGFrezonState(recvMessage, destData);
	case SetInfraredSenseDistance:		 //设置红外感应距离		51	2	0≤N≤500，单位厘米
		return setMcuInfraredSenseDistance(recvMessage, destData);
	case SetInfraredWaitingTime:		 //设置红外等待时长		52	4	0≤N≤10000，单位毫秒
		return setMcuInfraredWaitingTime(recvMessage, destData);
	case SetDoorCameraCaptureAngle:		 //设置门区摄像头抓拍角度	53	1	0≤N≤180，单位度
		return setCamera0PhotoAngle(recvMessage, destData);
	case SetBoxCameraCaptureAngle:		 //设置箱体摄像头抓拍角度	54	1	0≤N≤180，单位度
		return setCamera1PhotoAngle(recvMessage, destData);
	case SetMcuTestStateSwitch:			 //设置MCU测试状态开关	　	55	1	N，1为打开；0为关闭
		return setMcuTestMode(recvMessage, destData);
	default:
			return 0;
	}
}

static unsigned short storageRecvMessageQueryFGstate(unsigned char *recvMessage, unsigned char *destData)
{
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	//setMcuFridgeState(MCU_FRIDGE_WAITFRIDGESTATE);
	return sendFGQueryState(destData);
}

static unsigned short sendFGQueryState(unsigned char *destData) //03 00
{
	unsigned char commandType = 0x00;
	//unsigned char sendMessage[12];

	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}
static unsigned short storageRecvMessageQueryFroststate(unsigned char *recvMessage, unsigned char *destData)
{
	//mcuQueryFridgeDefrost();
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	return mcuQueryFridgeDefrost(destData);
}
static unsigned short mcuQueryFridgeDefrost(unsigned char *destData)
{
	//unsigned char const sendMessage[12] = { 0xAA, 0x0B, 0xCA, 0xC1, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x09, 0x5D };
	//memcpy(destData, sendMessage,12);
	unsigned char commandType = 0x09;
	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}
static unsigned short queryFGState(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char commandType = 0x00;
	memcpy(gAndroidQuerybuf, recvMessage, sizeof(gAndroidQuerybuf));
	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}

static unsigned short queryFGSN(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char commandType = 0x00;
	memcpy(gAndroidQuerybuf, recvMessage, sizeof(gAndroidQuerybuf));
	return createFGCPMessageData(destData, 0x07, &commandType, 1);
}

static unsigned short queryFGWarning(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char commandType = 0x02;
	memcpy(gAndroidQuerybuf, recvMessage, sizeof(gAndroidQuerybuf));
	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}
static unsigned short queryFGReportData(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char commandType = 0x01;
	memcpy(gAndroidQuerybuf, recvMessage, sizeof(gAndroidQuerybuf));
	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}
static unsigned short queryFGFrezonState(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char commandType = 0x09;
	memcpy(gAndroidQuerybuf, recvMessage, sizeof(gAndroidQuerybuf));
	return createFGCPMessageData(destData, 0x03, &commandType, 1);
}

static unsigned short setMcuInfraredSenseDistance(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9] = { 0 };
	memcpy(data, &recvMessage[8],2);
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	return createFGCPMessageData(destData, 0x79, data, 9);
}

static unsigned short setMcuInfraredWaitingTime(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9] = { 0 };
	memcpy(&data[2], &recvMessage[8], 2);
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	return createFGCPMessageData(destData, 0x79, data, 9);
}

static unsigned short setMcuTestMode(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9] = { 0 };
	if (recvMessage[8])
	{
		data[8] = 0x01;
		memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	}
	else
	{
		data[8] = 0x00;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
	}
	return createFGCPMessageData(destData, 0x79, data, 9);
}

static unsigned short setCamera0PhotoAngle(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9] = { 0 };
	unsigned char value = recvMessage[8];
	data[4] = value;
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	return createFGCPMessageData(destData, 0x79, data, 9);
}

static unsigned short setCamera1PhotoAngle(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9] = { 0 };
	unsigned char value = recvMessage[8];
	data[5] = value;
	memcpy(gAndroidCommdBuf, recvMessage, sizeof(gAndroidCommdBuf));
	return createFGCPMessageData(destData, 0x79, data, 9);
}


unsigned char fgcpJDConverter(unsigned char *sourceData, unsigned char *destData)
{
	unsigned char dataType = sourceData[0];
	switch (dataType)
	{
	case 0xAA:
		return (unsigned char)fridgeDataAnalysis(sourceData, destData);
	case 0x4E:
		return (unsigned char)androidDataAnalysis(sourceData, destData);
	default:
		return 0;
	}
}
