#ifndef _ANALYSIS_FGCP_H_
#define _ANALYSIS_FGCP_H_

#define FG_GENERAL_CONTROL				0x00
#define FG_ADDTION_CONTROL				0x09

#define FG_SETRETURN_DATA				0x02
#define FG_STATE_DATA					0x03
#define FG_DOORCHANGE_REPORT_DATA	    0X04
#define FG_WARNING_DATA					0x06
#define FG_SN_DATA					    0x07

#define FG_MCU_TEST_MODE				0x78
#define FG_SETMCUPARAMTER_RETURN		0x79


#define STATE_BASE						0x00
#define STATE_SERVICE					0x01
#define STATE_ERROR						0x02
#define	STATE_ADDTION					0x09

unsigned short fridgeStateSetResultCheckout(unsigned char * recvMessage, unsigned char *destData);
unsigned short setResultReturn(unsigned char * destData, unsigned char seq, unsigned char result);
//unsigned short checkoutIntelligentMode(unsigned char *recvMessage, unsigned char *destData, \
//	unsigned short position, unsigned char value, unsigned char flag);
unsigned short setFGResultProcess(unsigned char *recvMessage, unsigned char *destData);
//unsigned short checkoutSetResult(unsigned char *recvMessage, unsigned char *destData, unsigned char value);
unsigned short fridgeDataAnalysis(unsigned char *recvMessage, unsigned char *destData);
//unsigned short checkoutSetForceDefrost(unsigned char *recvMessage, unsigned char *destData, unsigned char value);
unsigned short stateDataProcess(unsigned char *recvMessage, unsigned char *destData);

unsigned short setFridgeIntelligentMode(unsigned char *buf, unsigned char *destData);
unsigned short setColdStorageRoomOpenOrClose(unsigned char *buf, unsigned char *destData);
unsigned short setQuickFrozenSwitchOpen(unsigned char *buf, unsigned char *destData);
unsigned short setQuickColdSwitchOpen(unsigned char *buf, unsigned char *destData);
unsigned short setColdStorageRoomTemperatureChange(unsigned char *buf, unsigned char *destData);
unsigned short setChangedTempeRoomTemperatureChange(unsigned char *buf, unsigned char *destData);
unsigned short setColdRoomTemperatureChange(unsigned char *buf, unsigned char *destData);

void fridgeStateDateProcess(unsigned char *buf, unsigned char *value);
void fridgeWaringDataProcess(unsigned char *buf, unsigned char *value);
unsigned short reportDataProcess(unsigned char *recvMessage, unsigned char *destData);
unsigned short warningDataProcess(unsigned char *recvMessage, unsigned char *destData);
unsigned short defrozenStateDataProcess(unsigned char *recvMessage, unsigned char *destData);
unsigned short doorStateChangeReport(unsigned char *recvMessage, unsigned char *destData);
unsigned short activeWarningDataProcess(unsigned char *recvMessage, unsigned char *destData);
unsigned short setMcuParamterReturn(unsigned char *recvMessage, unsigned char *destData);
//unsigned short setTemperatureCheck(unsigned char *recvMessage, unsigned char *destData, unsigned char type);
unsigned short mcuReportSocData(unsigned char *recvMessage, unsigned char *destData);
unsigned short setFridgeSwitchTemperature(unsigned char *recvMessage, unsigned char *destData);

#endif
