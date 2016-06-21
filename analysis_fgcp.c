#include <stdio.h>
#include <string.h>
#include "analysis_fgcp.h"
#include "analysis_mpcp.h"
 
unsigned short setResultReturn(unsigned char * destData, unsigned char seq,unsigned char result)
{
	//1（Success）或2（Failed）
	if (gAndroidCommdBuf[0] != 0x00)
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
	if (gAndroidQuerybuf[0] != 0x00)
		memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
	return createMPCPMessageData(destData, (unsigned char)McuResponseSetResult, 1, seq, &result);
}

static unsigned short fridgeStateSetResultCheckout(unsigned char * recvMessage,unsigned char *destData)
{
	unsigned char setData[9] = { 0 };
	unsigned char setResultData[29] = { 0 };
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char tmp;
	memcpy(setData, &gAndroidCommdBuf[8],9);
	memcpy(setResultData, &recvMessage[10],29);
	if (setData[0] != 0xFF)	//智能模式设置
	{
		if (setData[0])
		{
			if (!(setResultData[1] & 0x04)){
				//tmp = 0x02;
				return setResultReturn(destData, seq, 0x02);
			}
		}
		else
		{
			if (setResultData[1] & 0x04){
				//tmp = 0x02;
				return setResultReturn(destData, seq, 0x02);
			}
		}
	}
	if (setData[1] != 0xFF)	//冷藏室开关
	{
		if (setData[1])
		{
			if (setResultData[6] & 0x01)
				return setResultReturn(destData, seq, 0x02);
		}
		else
		{
			if (!(setResultData[6] & 0x01))
				return setResultReturn(destData, seq, 0x02);
		}
	}
	if (setData[2] != 0xFF)	//变温室开关
	{
		if (setData[2])
		{
			if (setResultData[6] & 0x04)
				return setResultReturn(destData, seq, 0x02);
		}
		else
		{
			if (!(setResultData[6] & 0x04))
				return setResultReturn(destData, seq, 0x02);
		}
	}
	if (setData[3] != 0xFF)	//速冻开关
	{
		if (setData[3])
		{
			if (!(setResultData[1] & 0x02))
				return setResultReturn(destData, seq, 0x02);
		}
		else
		{
			if (setResultData[1] & 0x02)
				return setResultReturn(destData, seq, 0x02);
		}
	}
	if (setData[4] != 0xFF)	//速冷开关
	{
		if (setData[4])
		{
			if (!(setResultData[1] & 0x01))
				return setResultReturn(destData, seq, 0x02);
		}
		else
		{
			if (setResultData[1] & 0x01)
				return setResultReturn(destData, seq, 0x02);
		}
	}
	if (setData[5] != 0xFF)	//冷藏室温度
	{
		tmp = setResultData[2] & 0x0F;
		printf("tmp = %d\n",tmp);
		if (tmp < 2 || tmp > 8)
			return setResultReturn(destData, seq, 0x02);
	}
	if (setData[6] != 0xFF)	//变温室温度
	{
		tmp = setResultData[3];
		if (tmp < 0 || tmp > 34)
			return setResultReturn(destData, seq, 0x02);
	}
	if (setData[7] != 0xFF)	//冷冻室温度
	{
		tmp = setResultData[2] & 0xF0;
		tmp = tmp >> 4;
		if (tmp < 3 || tmp > 12)
			return setResultReturn(destData, seq, 0x02);
	}
	if (setData[8] != 0xFF)	//等离子除菌开关
	{
		if (setData[8])
		{
			if (!(setResultData[8] & 0x08))
				return setResultReturn(destData, seq, 0x02);
		}
		else
		{
			if (setResultData[8] & 0x08)
				return setResultReturn(destData, seq, 0x02);
		}
	}
	return setResultReturn(destData, seq, 0x01);
}

static unsigned short fridgeTestStateSetResultProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char setResultData[20] = { 0 };
	unsigned char setData[2] = { 0 };
	unsigned char seq = gAndroidCommdBuf[5];
	memcpy(setData, &gAndroidCommdBuf[8], 2);
	memcpy(setResultData, &recvMessage[10], 20);
	if (setData[0])
	{
		if (!(setResultData[9] & 0x02))
			return setResultReturn(destData, seq, 0x02);
	}
	else
	{
		if (setResultData[9] & 0x02)
			return setResultReturn(destData, seq, 0x02);
	}
	if (setData[1])
	{
		if (!(setResultData[9] & 0x04))
			return setResultReturn(destData, seq, 0x02);
	}
	else
	{
		if (setResultData[9] & 0x04)
			return setResultReturn(destData, seq, 0x02);
	}
	return setResultReturn(destData, seq, 0x01);
}
/*static unsigned short checkoutIntelligentMode(unsigned char *recvMessage, unsigned char *destData,\
	unsigned short position,unsigned char value,unsigned char flag)
{
	unsigned char data[29];
	unsigned char resultData;
	unsigned char setData = gAndroidCommdBuf[8];
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char result;
	printf("setData = %d\n", setData);
	memcpy(data, &recvMessage[10], 29);
	resultData = data[position] & value;
	printf("resultData = %d\n", resultData);
	if (flag)
	{
		resultData = data[position] >> 4;
	}
	if (resultData && setData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else if (!resultData && !setData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else
	{
		result = 0x02;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
}*/
/*static unsigned short checkoutSetResult(unsigned char *recvMessage, unsigned char *destData, unsigned char value)
{
	unsigned char data[29];
	unsigned char resultData;
	unsigned char setData = gAndroidCommdBuf[8];
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char result;
	memcpy(data, &recvMessage[10], 29);
	resultData = data[6] & value;
	printf("setData = %d\n", setData);
	printf("resultData = %d\n", resultData);
	if (setData == 0 && resultData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else if (setData && !resultData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else
	{
		result = 0x02;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
}*/

/*static unsigned short checkoutSetForceDefrost(unsigned char *recvMessage, unsigned char *destData, unsigned char value)
{
	unsigned char data[20];
	unsigned char resultData;
	unsigned char result;
	unsigned char setData = gAndroidCommdBuf[8];
	unsigned char seq = gAndroidCommdBuf[5];
	memcpy(data, &recvMessage[10], 20);
	resultData = data[9] & value;
	if (setData && resultData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else if (!setData && !resultData)
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else
	{
		result = 0x02;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
}*/

/*static unsigned short setTemperatureCheck(unsigned char *recvMessage, unsigned char *destData, unsigned char type)
{
	unsigned char value;
	unsigned char result;
	unsigned char seq = gAndroidCommdBuf[5];
	if (type == SetColdStorageRoomTemperature)
	{
		value = recvMessage[12];
		value &= 0x0f;
		if (value < 2 || value > 8)
		{
			result = 0x02;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
		else
		{
			result = 0x01;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
	}
	else if (type == SetChangedTempeRoomTemperature)
	{
		value = recvMessage[13];
		if (value < 0 || value > 34)
		{
			result = 0x02;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
		else
		{
			result = 0x01;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
	}
	else if (type == SetColdRoomTemperature)
	{
		value = recvMessage[12];
		value &= 0xf0;
		value = value >> 4;
		if (value < 3 || value > 12)
		{
			result = 0x02;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
		else
		{
			result = 0x01;
			memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
			return createMPCPMessageData(destData, 0x98, 1, seq, &result);
		}
	}
	else
		return 0;
}*/
static unsigned short setFGResultProcess(unsigned char *recvMessage, unsigned char *destData)
{
	//if (gAndroidCommdBuf[0] != 0x4E)
	//	return 0;
	unsigned char messageType = recvMessage[10];
	unsigned char dataType = gAndroidCommdBuf[4];
	//printf("messageType = %x\n", messageType);
	//printf("dataType = %x\n", dataType);
	if (gAndroidCommdBuf[0] != 0x4E)
		return 0;
	if (messageType == FG_GENERAL_CONTROL)
	{
		//switch (dataType)
		//{
		//case SetFridgeIntelligentModeSwitch:
		//	return checkoutIntelligentMode(recvMessage, destData,1,0x04,0);
		//case SetColdStorageRoomSwitch:
		//	return checkoutSetResult(recvMessage, destData, 0x01);
		//case SetChangedTemperatureRoomSwitch:
		//	return checkoutSetResult(recvMessage, destData,0x04);
		//case SetQuickFrozenSwitch:
		//	return checkoutIntelligentMode(recvMessage, destData,1,0x02,0);
		//case SetQuickColdSwitch:
		//	return checkoutIntelligentMode(recvMessage, destData,1,0x01,0);
		//case SetColdStorageRoomTemperature:
		//	return setTemperatureCheck(recvMessage, destData, SetColdStorageRoomTemperature);
		//case SetChangedTempeRoomTemperature:
		//	return setTemperatureCheck(recvMessage, destData, SetChangedTempeRoomTemperature);
		//case SetColdRoomTemperature:
		//	return setTemperatureCheck(recvMessage, destData, SetColdRoomTemperature);
		//}
		fridgeStateSetResultCheckout(recvMessage, destData);
	}
	else if (messageType == FG_ADDTION_CONTROL)
	{
		fridgeTestStateSetResultProcess(recvMessage, destData);
	/*	switch (dataType)
		{
		case SetForceDefrost:
			return checkoutSetForceDefrost(recvMessage, destData,0x04);
		case SetForceCompressorSwitch:
			return checkoutSetForceDefrost(recvMessage, destData,0x02);
		}*/
	}
	else
	{
		return 0;
	}
}
static unsigned short setFridgeIntelligentMode(unsigned char *buf, unsigned char *destData)
{
	//unsigned char messageType = 0x00;
	unsigned char result = gAndroidCommdBuf[8];
	if (result)
		buf[1] |= 0x04;
	else
		buf[1] &= 0xFB;
	//printf("111111111111\n");
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setColdStorageRoomOpenOrClose(unsigned char *buf, unsigned char *destData)
{
	unsigned char result = gAndroidCommdBuf[8];
	if (result)
		buf[6] &= 0xFE;
	else
		buf[6] |= 0x01;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setChangedTemperatureRoomSwitchOpen(unsigned char *buf, unsigned char *destData)
{
	unsigned char result = gAndroidCommdBuf[8];
	//printf("1111111111111result = %d\n", result);
	if (result)
		buf[6] &= 0xFB;
	else
		buf[6] |= 0x04;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setQuickFrozenSwitchOpen(unsigned char *buf, unsigned char *destData)
{
	unsigned char result = gAndroidCommdBuf[8];
	//printf("result = %d\n", result);
	if (result)
		buf[1] |= 0x02;
	else
		buf[1] &= 0xFD;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setQuickColdSwitchOpen(unsigned char *buf, unsigned char *destData)
{
	unsigned char result = gAndroidCommdBuf[8];
	if (result)
		buf[1] |= 0x01;
	else
		buf[1] &= 0xFE;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setColdStorageRoomTemperatureChange(unsigned char *buf, unsigned char *destData)
{
	unsigned char data = gAndroidCommdBuf[8];
	unsigned char hbit = buf[2] >> 4;
	buf[2] = (hbit << 4) + data;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setChangedTempeRoomTemperatureChange(unsigned char *buf, unsigned char *destData)
{
	unsigned char data = gAndroidCommdBuf[8];
	buf[3] = data;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static unsigned short setColdRoomTemperatureChange(unsigned char *buf, unsigned char *destData)
{
	unsigned char data = gAndroidCommdBuf[8];
	unsigned char tmp = buf[2] << 4;
	tmp = tmp >> 4;
	//printf("2222222222\n");
	buf[2] = (data << 4) + tmp;
	return createFGCPMessageData(destData, 0x02, buf, 29);
}

static void fridgeStateDateProcess(unsigned char *buf, unsigned char *value)
{
	if (buf[1] & 0x04)
		value[0] = 1;
	if (!(buf[6] & 0x01))
		value[1] = 1;
	if (!(buf[6] & 0x04))
		value[2] = 1;
	if (buf[1] & 0x02)
		value[3] = 1;
	if (buf[1] & 0x01)
		value[4] = 1;
	value[5] = buf[17];
	value[6] = buf[19];
	value[7] = buf[18];
	if (buf[8] & 0x08)
		value[8] = 1;
}

static void fridgeWaringDataProcess(unsigned char *buf, unsigned char *value)
{
	if (buf[1] & 0x01)
		value[0] = 1;
	if (buf[2] & 0x01)
		value[1] = 1;
	if (buf[2] & 0x04)
		value[2] = 1;
	if (buf[2] & 0x08)
		value[3] = 1;
	if (buf[2] & 0x40)
		value[4] = 1;
	if (buf[2] & 0x80)
		value[5] = 1;
}

static unsigned short setFridgeSwitchTemperature(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[29] = { 0 };
	unsigned char setBuf[20] = { 0 };
	unsigned char tmp;
	//unsigned char seq = gAndroidCommdBuf[5];
	memcpy(data, &recvMessage[10], 29);
	memcpy(setBuf, &gAndroidCommdBuf[8],20);
	if (setBuf[0] != 0xFF)//智能模式开关
	{
		if (setBuf[0])
			data[1] |= 0x04;
		else
			data[1] &= 0xFB;
	}
	if (setBuf[1] != 0xFF)//冷藏室开关
	{
		if (setBuf[1])
			data[6] &= 0xFE;
		else
			data[6] |= 0x01;
	}
	if (setBuf[2] != 0xFF)//变温室开关
	{
		if (setBuf[2])
			data[6] &= 0xFB;
		else
			data[6] |= 0x04;
	}
	if (setBuf[3] != 0xFF)//速冻开关
	{
		if (setBuf[3])
			data[1] |= 0x02;
		else
			data[1] &= 0xFD;
	}
	if (setBuf[4] != 0xFF)//速冷开关
	{
		if (setBuf[4])
			data[1] |= 0x01;
		else
			data[1] &= 0xFE;
	}
	if (setBuf[5] != 0xFF)//冷藏室温度
	{
		tmp = data[2] >> 4;
		data[2] = (tmp << 4) + setBuf[5];
	}
	if (setBuf[6] != 0xFF)//变温室温度
	{
		data[3] = setBuf[6];
	}
	if (setBuf[7] != 0xFF)//冷冻室温度
	{
	    tmp = data[2] << 4;
		tmp = tmp >> 4;
		data[2] = (setBuf[7] << 4) + tmp;
	}
	if (setBuf[8] != 0xFF)//等离子除菌开关
	{
		if (setBuf[8])
			data[8] |= 0x08;
		else
			data[8] &= 0xF7;
	}
	return createFGCPMessageData(destData, 0x02, data, 29);
}
static unsigned short baseStateDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[20] = { 0 };
	unsigned char buf[29];
	unsigned char stateSeq = gAndroidQuerybuf[5];
	//unsigned char setType = gAndroidCommdBuf[4];
	memcpy(buf, &recvMessage[10],29);

	if (gAndroidCommdBuf[0] == 0x4E)
	{
		//switch (setType)
		//{
		//case SetFridgeIntelligentModeSwitch:
		//	return setFridgeIntelligentMode(buf, destData);
		//case SetColdStorageRoomSwitch:
		//	return setColdStorageRoomOpenOrClose(buf, destData);
		//case SetChangedTemperatureRoomSwitch:
		//	return setChangedTemperatureRoomSwitchOpen(buf, destData);
		//case SetQuickFrozenSwitch:
		//	return setQuickFrozenSwitchOpen(buf, destData);
		//case SetQuickColdSwitch:
		//	return setQuickColdSwitchOpen(buf, destData);
		//case SetColdStorageRoomTemperature:
		//	return setColdStorageRoomTemperatureChange(buf, destData);
		//case SetChangedTempeRoomTemperature:
		//	return setChangedTempeRoomTemperatureChange(buf, destData);
		//case SetColdRoomTemperature:
		//	return setColdRoomTemperatureChange(buf, destData);
		//}
		setFridgeSwitchTemperature(recvMessage, destData);
	}
	else
	{
		fridgeStateDateProcess(buf,data);
		memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
		return createMPCPMessageData(destData, 0x79, 20, stateSeq, data);
	}
}
static unsigned short reportDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[62] = {0};
	unsigned char seq = gAndroidQuerybuf[5];
	memcpy(data, &recvMessage[0],62);
	memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
	return createMPCPMessageData(destData, 0x7C, 62, seq, data);
}

static unsigned short warningDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[8] = { 0 };
	unsigned char value[20] = { 0 };
	unsigned char seq = gAndroidQuerybuf[5];
	memcpy(data, &recvMessage[10],8);
	fridgeWaringDataProcess(data, value);
	memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
	return createMPCPMessageData(destData, 0x7B, 20, seq, value);
}

static unsigned short defrozenStateDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[36] = { 0 };
	unsigned char defrostState = 0;
	unsigned char compressorState = 0;
	unsigned char setData[2] = { 0 };
	//unsigned char setType = gAndroidCommdBuf[4];
	//unsigned char setResult = gAndroidCommdBuf[8];
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char value[20] = { 0 };
	//unsigned char buf = 0;
	//printf("setType = %x\n", setType);
	memcpy(data, &recvMessage[10],36);
	if (data[29] & 0x04)
		defrostState = 1;
	if (data[29] & 0x02)
		compressorState = 1;
	if (gAndroidCommdBuf[0] == 0x4E)
	{
		memcpy(setData, &gAndroidCommdBuf[8],2);
		if (setData[0])
		{
			value[9] = 0x02;
			return createFGCPMessageData(destData, 0x02, value, 20);
		}
		else if (setData[1])
		{
			value[9] = 0x04;
			return createFGCPMessageData(destData, 0x02, value, 20);
		}
		else
			setResultReturn(destData, seq,0x02);
		/*if (setType == 0x0A)
		{
			if (setResult == 0 && defrostState == 1 ){
				data[29] = 0;
				return createFGCPMessageData(destData, 0x02, data, 36);
			}
			else if (setResult == 1 && defrostState == 0 && compressorState == 0)
			{
				data[29] |= 0x04;
				return createFGCPMessageData(destData, 0x02, data, 36);
			}
			else
			{
				setResult = 0x02;
				memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
				return createMPCPMessageData(destData, 0x98, 1, seq, &setResult);
			}

		}
		else if (setType == 0x0B)
		{
			if (setResult == 0 && compressorState == 1){
				data[29] = 0;
				return createFGCPMessageData(destData, 0x02, data, 36);
			}
			else if (setResult == 1 && compressorState == 0 && defrostState == 0)
			{
				data[29] = 0x02;
				return createFGCPMessageData(destData, 0x02, data, 36);
			}
			else
			{
				setResult = 0x02;
				memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
				return createMPCPMessageData(destData, 0x98, 1, seq, &setResult);
			}
		}*/
	}
	else
	{
		value[0] = compressorState;
		value[1] = defrostState;
		seq = gAndroidQuerybuf[5];
		memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
		return createMPCPMessageData(destData, 0x7A, 20, seq, value);
	}
}

static unsigned short stateDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char dataType = recvMessage[10];
	//printf("dataType = %x\n", dataType);
	switch (dataType)
	{
	case STATE_BASE:
		return baseStateDataProcess(recvMessage, destData);
	case STATE_SERVICE:
		return reportDataProcess(recvMessage, destData);
	case STATE_ERROR:
		return warningDataProcess(recvMessage, destData);
	case STATE_ADDTION:
		return defrozenStateDataProcess(recvMessage, destData);
	default:
		return 0;
	}
}

static unsigned short doorStateChangeReport(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[9];
	unsigned char value[20] = { 0 };
	memcpy(data, &recvMessage[10],9);
	if (data[1] & 0x01)
		value[0] = 1;
	if (data[3] & 0x01)
		value[1] = 1;
	if (data[3] & 0x02)
		value[2] = 1;
	if (data[3] & 0x08)
		value[3] = 1;
	return createMPCPMessageData(destData, (unsigned char)McuReportFridgeRunParamter, 20, 0, value);
}

static unsigned short activeWarningDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[19] = { 0 };
	//unsigned char value[6] = { 0 };
	memcpy(data, recvMessage,19);
	//fridgeWaringDataProcess(data,value);
	return createMPCPMessageData(destData,(unsigned char)McuReportFridgeFaultData, 19, 0, data);
}

static unsigned short fridgeSNDataProcess(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data[32];
	unsigned char seq = gAndroidQuerybuf[5];
	memcpy(data, &recvMessage[10],32);
	memset(gAndroidQuerybuf, 0, sizeof(gAndroidQuerybuf));
	return createMPCPMessageData(destData,(unsigned char)McuResponsFridgeSN, 0x20, seq, data);
}

static unsigned short setMcuParamterReturn(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char data = 0;
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char result;
	data = recvMessage[10];
	if (data == 0xFE)
	{
		result = 0x02;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
	else
	{
		result = 0x01;
		memset(gAndroidCommdBuf, 0, sizeof(gAndroidCommdBuf));
		return createMPCPMessageData(destData, 0x98, 1, seq, &result);
	}
}

static unsigned short mcuReportSocData(unsigned char *recvMessage, unsigned char *destData)
{
	//0x4E474E53 温度+湿度+红外距离+当前角度
	unsigned char data[17] = {0};
	unsigned char seq = gAndroidCommdBuf[5];
	unsigned char value[4] = {0};
	memcpy(data,&recvMessage[10],17);
	value[0] = data[0];
	value[1] = data[1];
	value[2] = (data[5] << 8) + data[4];
	value[3] = (data[3] << 8) + data[2];

	return createMPCPMessageData(destData, 0x97, 4, seq, value);

}

unsigned short fridgeDataAnalysis(unsigned char *recvMessage, unsigned char *destData)
{
	unsigned char dataType = recvMessage[9];
	switch (dataType)
	{
	case FG_SETRETURN_DATA:
		return setFGResultProcess(recvMessage, destData);
	case FG_STATE_DATA:
		return stateDataProcess(recvMessage, destData);
	case FG_DOORCHANGE_REPORT_DATA:
		return doorStateChangeReport(recvMessage, destData);
	case FG_WARNING_DATA:
		return activeWarningDataProcess(recvMessage, destData);
	case FG_SN_DATA:
		return fridgeSNDataProcess(recvMessage, destData);
	case FG_MCU_TEST_MODE:
		return mcuReportSocData(recvMessage, destData);
	case FG_SETMCUPARAMTER_RETURN:
		return setMcuParamterReturn(recvMessage, destData);
	default:
		return 0;
	}
}
