#include "boot_r_cg_macrodriver.h"
#include "boot_r_cg_cgc.h"
#include "boot_r_cg_port.h"
#include "boot_r_cg_wdt.h"
#include "boot_r_cg_userdefine.h"
#include "boot_r_cg_wdt.h"
#include "boot_r_cg_serial.h"
#include <string.h>

#include "..\incrl78\fsl.h"                /*  */
#include "..\incrl78\fsl_types.h"          /*  */
#include "boot_fgcp.h"

#include "boot_uart_manager.h"
#include "boot_utility.h"
#include "boot_update.h"
#include "boot_fgcp.h"

#define PR_MAX_BLOCK_NUM            64      /*  */
#define PR_BLOCK_SIZE               0x400   /*  */
#define PR_WORD_SIZE                4       /*  */

#define UPDATE_PACKET_DATA_SIZE   128

typedef struct tagUPDATE_MACHINE
{
	unsigned long mEndWriteAddress;
	unsigned long mWriteAdressIndex;
	unsigned long mWriteSize;

	volatile unsigned char mMessageFlag;
	unsigned char mMessageBuffer[FGCP_MAX_DATA_LENGTH];

	fsl_write_t   mFSL_WriteT;

} UPDATE_MACHINE;


//static __far const fsl_descriptor_t fsl_descriptor_pstr = { 0x00, 0x20, 0x01 };
static __far const fsl_descriptor_t fsl_descriptor_pstr = { 0x00, 0x08, 0x01 };
//static UB prDubWriteBuffer[UPDATE_PACKET_DATA_SIZE];  
static UPDATE_MACHINE gUpdateMachine;
static unsigned char  gUpdateWaitCommandFlag;
static unsigned short gBootVersionTick;


static fsl_u08 prFslStart(void)
{
	fsl_u08 dubRetCode;

	/*  */
	dubRetCode = FSL_Init(&fsl_descriptor_pstr);

	/*  */
	if (dubRetCode == FSL_OK)
	{
		/*  */
		FSL_Open();

		/*  */
		FSL_PrepareFunctions();
		FSL_PrepareExtFunctions();
	}

	return dubRetCode;
}


static void prFslEnd(void)
{
	/*  */
	FSL_Close();
}


unsigned char prFslErrorCheck(fsl_u08 dubSelfResult)
{
	unsigned char  dubMsgErrCode;          /*  */

								/*  */
	switch (dubSelfResult)
	{
	case FSL_OK:
		dubMsgErrCode = PR_MSG_RET_NORM_END;    /*  */
		break;

	case FSL_ERR_PROTECTION:
		dubMsgErrCode = PR_MSG_RET_PROTECT_ERR; /*  */
		break;

	case FSL_ERR_ERASE:
		dubMsgErrCode = PR_MSG_RET_ERASE_ERR;   /*  */
		break;

	case FSL_ERR_WRITE:
		dubMsgErrCode = PR_MSG_RET_WRITE_ERR;   /*  */
		break;

	case FSL_ERR_IVERIFY:
		dubMsgErrCode = PR_MSG_RET_IVERIFY_ERR; /*  */
		break;

	default:
		dubMsgErrCode = PR_MSG_RET_ERR_END;     /*  */
		break;
	}

	return dubMsgErrCode;
}


void updateUartMessageProc(unsigned char * buffer)
{
	if (gUpdateMachine.mMessageFlag)
		return;

	gUpdateMachine.mMessageFlag = 1;

	memcpy_f(gUpdateMachine.mMessageBuffer, buffer, buffer[1] + 1);

	return;
}


static void updateSendAckEx(unsigned char* recvMessage, unsigned char *ackData,unsigned char ackLength)
{
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)recvMessage;

	if (ackData)
		memcpy_f(&(header->mData0), ackData, ackLength);

	header->mLength = FGCP_PACKAGE_INFO_BYTES - 1 + ackLength;
	header->mFrameSyncCheck = header->mLength ^ header->mAppliances;

	recvMessage[header->mLength] = bootFGCPGetMessageCheck(recvMessage);
	bootUartSend(recvMessage, header->mLength + 1, 0);
}


static void updateSendAck(unsigned char* recvMessage, unsigned char ack)
{
	updateSendAckEx(recvMessage, &ack, 1);
}



static void updateMachineWriteInit(UPDATE_MACHINE* machine)
{
	unsigned char  dub_i;
	unsigned char  dubStartEraseBlock;
	unsigned long  duwStartWriteAddress;
	unsigned char  dubBlockLength;
	unsigned long  duwWriteSize = 0;

	unsigned char dubMsgResult;
	unsigned char dubSelfResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);


	//-- Store received data (Block to program, address, size) from buffer. --
	dubStartEraseBlock = header->mData0;
	dubBlockLength = header->mData1;
//	duwStartWriteAddress = ((unsigned long)(header->mData1)) << 16;
//	duwStartWriteAddress |= ((unsigned long)(header->mData2)) << 8;
//	duwStartWriteAddress |= ((unsigned long)(header->mData3));
//	duwWriteSize = ((unsigned long)(header->mData4)) << 8;
//	duwWriteSize |= ((unsigned long)(header->mData5));
//	machine->mEndWriteAddress = duwStartWriteAddress + duwWriteSize - 1;
//	dubBlockLength = (unsigned char)((duwWriteSize - 1) / PR_BLOCK_SIZE) + 1;
    duwWriteSize = (unsigned long)dubBlockLength * PR_BLOCK_SIZE;
	duwStartWriteAddress = (unsigned long)dubStartEraseBlock * PR_BLOCK_SIZE;
	machine->mEndWriteAddress = duwStartWriteAddress + duwWriteSize;

	// Parameter check (blocks 0-3 protection, inhibit 0 size writing, etc.) 
	if ((dubStartEraseBlock >= 4) &&   
		(dubStartEraseBlock < PR_MAX_BLOCK_NUM) &&   
		(duwWriteSize != 0))    
	{
		// Check state of blocks subject to programming and erase processing 
		for (dub_i = 0; dub_i < dubBlockLength; dub_i++)
		{
#ifdef PR_USE_OCD_MODE      //Do nothing on monitor area in OCD mode. 
			if ((dubStartEraseBlock + dub_i) != PR_OCD_MONITOR_BLOCK)
			{
#endif
				DI();
				dubSelfResult = FSL_BlankCheck(dubStartEraseBlock + dub_i);

				// If the target block is nonblank. 
				if (dubSelfResult == FSL_ERR_BLANKCHECK)
				{
					dubSelfResult = FSL_Erase(dubStartEraseBlock + dub_i);
				}

				EI();

#ifdef PR_USE_OCD_MODE      /* Do nothing on monitor area in OCD mode. */
			}
			else 
			{
				dubSelfResult = FSL_OK;
			}
#endif
		}

		// Set address to write. 
		machine->mWriteAdressIndex = duwStartWriteAddress;

		/* Convert flash self programming result to a transmit parameter. */
		dubMsgResult = prFslErrorCheck(dubSelfResult);
	}
	else 
	{
		dubMsgResult = PR_MSG_RET_PRM_ERR;
	}

	// Send result. 
	updateSendAck(machine->mMessageBuffer, dubMsgResult);
}



static void updateMachineWriteData(UPDATE_MACHINE* machine)
{
	unsigned char dubMsgResult;
	unsigned char dubSelfResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	// Check whether the target write address is smaller than the end address 
	if (machine->mWriteAdressIndex <= machine->mEndWriteAddress)
	{
#ifdef PR_USE_OCD_MODE  /* Do nothing on monitor area in OCD mode. */
		if (gUpdateMachine->mWriteAdressIndex < PR_OCD_MONITOR_ADDR)
		{
#endif
			// Copy write data into data buffer. 
			machine->mFSL_WriteT.fsl_data_buffer_p_u08 = &(header->mData0);
			machine->mFSL_WriteT.fsl_destination_address_u32 = machine->mWriteAdressIndex;
			machine->mFSL_WriteT.fsl_word_count_u08 = UPDATE_PACKET_DATA_SIZE / 4;

			DI();
			dubSelfResult = FSL_Write(&(machine->mFSL_WriteT));
			EI();
#ifdef PR_USE_OCD_MODE  /* Do nothing on monitor area in OCD mode. */
		}
		else
		{
			dubSelfResult = FSL_OK;
		}
#endif

		// Convert flash self programming result to a transmit parameter. 
		dubMsgResult = prFslErrorCheck(dubSelfResult);

		// Increment target write address by write size 
		machine->mWriteAdressIndex += UPDATE_PACKET_DATA_SIZE;
	}
	else
	{
		dubMsgResult = PR_MSG_RET_ERR_END;
	}

	// Send result. 
	updateSendAck(machine->mMessageBuffer, dubMsgResult);
}


static void updateMachineIVerify(UPDATE_MACHINE* machine)
{
	unsigned char dubMsgResult;
	unsigned char dubSelfResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	// Verify processing 
	DI();
	dubSelfResult = FSL_IVerify(header->mData0);
	EI();

	// Convert flash self programming result to a transmit parameter and send result. 
	dubMsgResult = prFslErrorCheck(dubSelfResult);

	updateSendAck(machine->mMessageBuffer, dubMsgResult);
}


static void updateMachineBootSwap(UPDATE_MACHINE* machine)
{
	unsigned char dubMsgResult;
	unsigned char dubSelfResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	// Disabled in OCD mode. 
#ifdef PR_USE_OCD_MODE
	// Do nothing and end normally in OCD mode. 
	updateSendAck(gUpdateMachine->mMessageBuffer, PR_MSG_RET_NORM_END);
#else
	// Perform processing if not in OCD mode. 
	// Boot flag rewrite processing 
	DI();
	dubSelfResult = FSL_InvertBootFlag();
	EI();

	// Convert flash self programming result to a transmit parameter and send result. 
	dubMsgResult = prFslErrorCheck(dubSelfResult);
	updateSendAck(machine->mMessageBuffer, dubMsgResult);

	// Upon completion, perform forced reset processing. 
	if (dubMsgResult == PR_MSG_RET_NORM_END)
	{
		// UART communication termination processing 
		// prUartEnd();
		usleep(1000 * 20); //wait for send ack

		// Forced reset processing 
		FSL_ForceReset();
	}
#endif
}


static void updateMachineReset(UPDATE_MACHINE* machine)
{
	unsigned char dubMsgResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	// Disabled in OCD mode. 
#ifdef PR_USE_OCD_MODE
	// Do nothing and end normally in OCD mode. 
	return PR_MSG_RET_NORM_END;
#else
	// Perform processing if not in OCD mode. 
	// Send result of reception. 
	dubMsgResult = PR_MSG_RET_NORM_END;
	updateSendAck(machine->mMessageBuffer, dubMsgResult);

	// UART communication termination processing 
	//prUartEnd();

	// Formced reset processing
	FSL_ForceReset();
#endif
}


static void updateMachineErase(UPDATE_MACHINE* machine)
{
	unsigned char dubMsgResult;
	unsigned char dubSelfResult;
	FGCP_DATA_HEADR *header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	DI();
	dubSelfResult = FSL_Erase(header->mData0);
	EI();

	//Convert flash self programming result to a transmit parameter and send result. 
	dubMsgResult = prFslErrorCheck(dubSelfResult);

	updateSendAck(machine->mMessageBuffer, dubMsgResult);
}


static void updateMachine(UPDATE_MACHINE* machine)
{
	FGCP_DATA_HEADR *header;

	if (!machine->mMessageFlag)
		return;

	gUpdateWaitCommandFlag = 0;

	header = (FGCP_DATA_HEADR *)(machine->mMessageBuffer);

	switch (header->mMessageId)
	{
	case MESSAGE_ID_WRITE:
		updateMachineWriteInit(machine);
		break;

	case MESSAGE_ID_DATA0:
	case MESSAGE_ID_DATA1:
	case MESSAGE_ID_DATA_END:
		updateMachineWriteData(machine);
		break;

	case MESSAGE_ID_IVERIFY:
		updateMachineIVerify(machine);

		break;

	case MESSAGE_ID_BOOTSWAP:
		updateMachineBootSwap(machine);
		break;

	case MESSAGE_ID_RESET:
		updateMachineReset(machine);
		break;

	case MESSAGE_ID_ERASE:
		updateMachineErase(machine);
		break;

	case MESSAGE_ID_TEST:
		updateSendAck(machine->mMessageBuffer, 0);
		break;
	}
	
	machine->mMessageFlag = 0;
}


unsigned char needUpdate()
{
	//P7.0
	//
	//return 1;
	unsigned char *updateFlag;
	unsigned char flag = 0;

	gUpdateWaitCommandFlag = 2;

	updateFlag = (unsigned char *)(43 * 1024);
	if ((*updateFlag != 0xff) && (*(updateFlag + 1) != 0xff))
		return 1;

	flag = P7.0;
	if (flag > 0)
	{
		gUpdateWaitCommandFlag = 1;
		return 1;
	}

	gUpdateWaitCommandFlag = 99;
	return 0;
}


void updateInit()
{
	memset(&gUpdateMachine, 0, sizeof(gUpdateMachine));
}


void bootVersionInit()
{
	gBootVersionTick = getTickCount();
}


void bootVersionReport()
{
	unsigned char report[3];

	if (overTickCount(gBootVersionTick, 500))
	{
		report[0] = 0x99;
		report[1] = FIRMWARE_VERSION;
		report[2] = gUpdateWaitCommandFlag;
		bootUartSend(report, 3, 0);
		gBootVersionTick = getTickCount();
	}
}


void update()
{
	if (!needUpdate())
	{
		return;
	}

	setMcuState(MCU_STATE_BOOT);
	bootFGCPInit();
	updateInit();
	R_UART1_Init();
	R_UART1_Start();
	timeInit();
	bootUartManagerInit();

	if (prFslStart() != FSL_OK)
	{
		prFslEnd();
		return;
	}

	bootVersionInit();
		
	while (1)
	{
		Boot_R_WDT_Restart();
		bootFGCPRun();
		bootUartManagerRun();
		updateMachine(&gUpdateMachine);

		if (gUpdateWaitCommandFlag)
		{
			if (gUpdateWaitCommandFlag == 1)
			{
				if (getTickCount() > 1000 * 5)
				{
					break;
				}
			}
			else if (gUpdateWaitCommandFlag == 99)
			{
				if (getTickCount() > 1000 * 1)
				{
					break;
				}
			}
			bootVersionReport();
		}
	}

	//Terminate flash self programming. 
	prFslEnd();
}



