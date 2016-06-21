#ifndef _ANALYSIS_MPCP_H_
#define _ANALYSIS_MPCP_H_

#define CHECK_ERROR 0x00
#define CHECK_OK    0x01

#define FGCP_HEADER_LENTH	   10
#define FGCP_CHECKSUN_LENTH  1
#define FGCP_PACKAGE_INFO_LENTH (FGCP_HEADER_LENTH + FGCP_CHECKSUN_LENTH)
typedef struct tagFGCP_DATA_TYPE
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
} FGCP_DATA_TYPE;
typedef enum tagCommand_Type
{
	SetFridgeSwitchTemperature = 1,
	//SetFridgeIntelligentModeSwitch = 1,
	//SetColdStorageRoomSwitch,
	//SetChangedTemperatureRoomSwitch,
	//SetQuickFrozenSwitch,
	//SetQuickColdSwitch,
	//SetColdStorageRoomTemperature,
	//SetChangedTempeRoomTemperature,
	//SetColdRoomTemperature,
	SetFridgeTestState,
	SetMcuUpdateMode = 9,
	
	//SetForceDefrost,
	//SetForceCompressorSwitch,
	GetFridgeSN = 20,
	GetCurrentFridgeState = 21,
	GetFridgeTestState,
	GetFridgeWarningMessage,
	GetFridgeReportData,
	//GetFridgeDefrostState,

	SetInfraredSenseDistance = 51,
	SetInfraredWaitingTime,
	SetDoorCameraCaptureAngle,
	SetBoxCameraCaptureAngle,
	SetMcuTestStateSwitch,

	McuResponseSetResult = 100,
	McuReportFridgeRunParamter,
	McuReportFridgeFaultData,

	McuResponsFridgeSN = 120,
	McuResponsFridgeState = 121,
	McuResponsFridgeTestState,
	McuResponsFridgeWarningMessage,
	McuResponsFridgeData,
	//McuReportDefrostState,

	McuReportSocData = 151,
	
}Command_Type;

unsigned short CRC16(unsigned char * puchMsg, unsigned short  usDataLen);
unsigned char getLenCheckValue(unsigned char Type, unsigned char Len);
unsigned short lenCheckWhetherAlike(unsigned char *recvMessage);
unsigned short checkFrameWhetherLegal(unsigned char *recvMessage);
void getCrc16(unsigned char * message, unsigned short messageLen, \
	unsigned char *Higbit, unsigned char *LowBit);
void getCrc16HigbitAndLowbit(unsigned short int crc16, unsigned char *Higbit, unsigned char *LowBit);
unsigned short createMPCPMessageData(unsigned char *Message, unsigned char MessageType, \
	unsigned char Len, unsigned char Seq, unsigned char *Value);
unsigned short androidDataAnalysis(unsigned char *recvMessage, unsigned char *destData);
unsigned short storageRecvMessageQueryFGstate(unsigned char *recvMessage, unsigned char *destData);
unsigned short sendFGQueryState(unsigned char *destData);
unsigned short storageRecvMessageQueryFroststate(unsigned char *recvMessage, unsigned char *destData);
unsigned short mcuQueryFridgeDefrost(unsigned char *destData);


unsigned short createFGCPMessageData(unsigned char *messageBuf, unsigned char messageType, \
	unsigned char *data, unsigned char dataLength);
unsigned char FGCPGetMessageCheckout(unsigned char *messageBuf);
unsigned short queryFGSN(unsigned char *recvMessage, unsigned char *destData);
unsigned short queryFGWarning(unsigned char *recvMessage, unsigned char *destData);
unsigned short queryFGReportData(unsigned char *recvMessage, unsigned char *destData);
unsigned short queryFGFrezonState(unsigned char *recvMessage, unsigned char *destData);
unsigned short queryFGState(unsigned char *recvMessage, unsigned char *destData);
unsigned short setMcuInfraredSenseDistance(unsigned char *recvMessage, unsigned char *destData);
unsigned short setMcuInfraredWaitingTime(unsigned char *recvMessage, unsigned char *destData);
unsigned short setMcuTestMode(unsigned char *recvMessage, unsigned char *destData);
unsigned short setCamera0PhotoAngle(unsigned char *recvMessage, unsigned char *destData);
unsigned short setCamera1PhotoAngle(unsigned char *recvMessage, unsigned char *destData);

extern unsigned char gAndroidCommdBuf[30];
extern unsigned char gAndroidQuerybuf[10];


#endif