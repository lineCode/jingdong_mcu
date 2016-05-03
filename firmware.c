/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-11 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include <string.h>
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "sensor_adc.h"
#include "r_cg_timer.h"
#include "fgcp.h"
#include "signal_control.h"
#include "angle_as5600.h"
#include "sensor_map.h"
#include "uart_manager.h"

const unsigned char firmwareGuid[16] =
{
	0x08, 0x2d, 0x5c, 0x03, 0xe3, 0x12, 0x48, 0xe5, 0xa5, 0x5c, 0xe8, 0x78, 0x24, 0xb3, 0xfb, 0x28,
};


unsigned char checkFirmwareGUID(unsigned char *guid)
{
	unsigned char i;

	for (i = 0; i < 16; i++)
	{
		if (guid[i] != firmwareGuid[i])
			return 1;
	}

	return 0;
}


#include "incrl78\fsl.h"                /*  */
#include "incrl78\fsl_types.h"          /*  */

static __far const fsl_descriptor_t fsl_descriptor_pstr = { 0x00, 0x08, 0x01 };



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



unsigned char setFirmwareFlagAndReset()
{
	setMcuState(MCU_STATE_UPDATE);
	FSL_ForceReset();
#if 0
	fsl_write_t   mFSL_WriteT;
	unsigned char dubSelfResult;

	unsigned char buf[4] = { 0x00, 0x00, 0x00, 0x00 };

	prFslStart();

	mFSL_WriteT.fsl_data_buffer_p_u08 = buf;
	mFSL_WriteT.fsl_destination_address_u32 = 43 * 1024;//machine->mWriteAdressIndex;
	mFSL_WriteT.fsl_word_count_u08 = 1; //UPDATE_PACKET_DATA_SIZE / 4;

	DI();
	dubSelfResult = FSL_Write(&mFSL_WriteT);
	EI();

	if (dubSelfResult == FSL_OK)
	{
		FSL_ForceReset();
	}

	prFslEnd();
#endif
	return 1;
}
