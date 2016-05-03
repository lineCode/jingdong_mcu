/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "utility.h"
#include "angle_as5600.h"
#include "signal_control.h"

// AS5600 register & mask
#define AS5600_ZMCO					0x00
#define AS5600_ZPOS_H				0x01
#define AS5600_ZPOS_L				0x02
#define AS5600_MPOS_H				0x03				
#define AS5600_MPOS_L				0x04				
#define AS5600_MANG_H				0x05
#define AS5600_MANG_L				0x06
#define AS5600_CONF_H				0x07
#define AS5600_CONF_L				0x08
#define AS5600_RAW_ANGLE_H			0x0c
#define AS5600_RAW_ANGLE_L			0x0d
#define AS5600_ANGLE_H				0x0e
#define AS5600_ANGLE_L				0x0f
#define AS5600_STATUS				0x0b
#define AS5600_AGC					0x1a
#define AS5600_MAGNITUE_H			0x1b
#define AS5600_MAGNITUE_L			0x1c
#define AS5600_BURN					0xff

#define AS5600_ZMCO_MASK			0x03
#define AS5600_ZMCO_SHIFT			0x00


typedef enum tagAS5600_OP_STATE
{
	AS5600_OP_STATE_NULL = 0,
	AS5600_OP_STATE_IDEL,  
	AS5600_OP_STATE_WRITE_I2C_FOR_STATUS,
	AS5600_OP_STATE_READ_I2C_FOR_STATUS,
	AS5600_OP_STATE_READY_I2C_FOR_ANGLE,
	AS5600_OP_STATE_WRITE_I2C_FOR_ANGLE,
	AS5600_OP_STATE_READ_I2C_FOR_ANGLE,

} AS5600_OP_STATE;

#define  AS5600_I2C_ADDR  (0x36 << 1)
#define  I2C_WRITE_FLAG  0x00
#define  I2C_READ_FLAG   0x01

unsigned short gAS5600AngleRaw;
unsigned short gAS5600StartAngleRaw;
unsigned short gAS5600Degree;
unsigned short gCamera0PhotoDegree;
unsigned short gCamera1PhotoDegree;
unsigned char  gAS5600I2CErrorCount;

static volatile unsigned char gAS5600I2CFlag;

static AS5600_OP_STATE gAS5600OpState;
static unsigned short gAS5600OpTick;
static unsigned short gAS5600WatchDogTick;
static unsigned char  gAS5600I2CBuffer[2];

extern void IIC_Init(void);
extern void IIC_start();
extern unsigned char GetACK();
extern void IIC_WritByte(unsigned char byte);
extern unsigned char IIC_ReadByte();
extern void ACK(void);  //2014/9/12 17:12:56 spark ch
extern void NoACK(void);
extern void IIC_stop();
char as5600I2C_Write(unsigned char address)
{
	IIC_start();
	IIC_WritByte(AS5600_I2C_ADDR | I2C_WRITE_FLAG);
	
	if (GetACK())
	{
		return 1;
	}

	IIC_WritByte(address);
	if (GetACK())
	{
		return 1;
	}

	IIC_stop();
}


char  as5600I2C_Read(unsigned char *buf, unsigned char bytes)
{
	int i;
	IIC_start();
	IIC_WritByte(AS5600_I2C_ADDR | I2C_READ_FLAG);

	if (GetACK())
	{
		return 1;
	}

	for (i = 0; i < bytes; i++)
	{
		buf[i] = IIC_ReadByte();

		if (i == bytes - 1)
			NoACK();
		else
			ACK();
	}

	IIC_stop();
	return 0;
}


char  as5600I2C_ReadReg(unsigned char regAddress, unsigned char *buf, unsigned char bytes)
{
	if (as5600I2C_Write(regAddress))
	{
		IIC_stop();
		return 1;
	}

	if (as5600I2C_Read(buf, bytes))
	{
		IIC_stop();
		return 1;
	}

	return 0;
}

#define USB_AS5600_I2C

void as5600I2CEndCallback()
{
	gAS5600I2CFlag = 0;
}


static void as5600OpStateMachine()
{
#ifdef USB_AS5600_I2C

	if (!overTickCount(gAS5600OpTick, 5))
		return;

	if (as5600I2C_ReadReg(AS5600_STATUS, gAS5600I2CBuffer, 1))
		goto exit;

	if ((gAS5600I2CBuffer[0] & 0x20) == 0)
		goto exit;

	if (as5600I2C_ReadReg(AS5600_ANGLE_H, gAS5600I2CBuffer, 2))
	{
		gAS5600I2CErrorCount++;
		goto exit;
	}

	gAS5600AngleRaw = (((unsigned short)gAS5600I2CBuffer[0]) << 8) | (gAS5600I2CBuffer[1]);
	gAS5600WatchDogTick = getTickCount();

	doorCalibratorMachine(gAS5600AngleRaw);

exit:
	gAS5600OpTick = getTickCount();

#else
	MD_STATUS  status;

	if (gAS5600I2CFlag)
		return;

	switch (gAS5600OpState)
	{
	case AS5600_OP_STATE_IDEL:
		if (!overTickCount(gAS5600OpTick, 5))
			break;

		gAS5600I2CBuffer[0] = AS5600_ANGLE_H; // AS5600_STATUS;
		gAS5600I2CFlag = 1;
		status = R_IICA0_Master_Send(AS5600_I2C_ADDR | I2C_WRITE_FLAG, gAS5600I2CBuffer, 1, 2);
		if (status == MD_OK)
			gAS5600OpState = AS5600_OP_STATE_WRITE_I2C_FOR_ANGLE; //AS5600_OP_STATE_WRITE_I2C_FOR_STATUS;
		else
		{
			gAS5600OpTick  = getTickCount();
			gAS5600I2CFlag = 0;
		}
		break;
/*
	case AS5600_OP_STATE_WRITE_I2C_FOR_STATUS:
		gAS5600I2CFlag = 1;
		status = R_IICA0_Master_Receive(AS5600_I2C_ADDR | I2C_READ_FLAG, gAS5600I2CBuffer, 1, 10);
		if (status == MD_OK)
			gAS5600OpState = AS5600_OP_STATE_READ_I2C_FOR_STATUS;
		else
		{
			gAS5600OpTick  = getTickCount();
			gAS5600I2CFlag = 0;
		}
		break;

	case AS5600_OP_STATE_READ_I2C_FOR_STATUS:
		if ((gAS5600I2CBuffer[0] & 0x20) == 0)
		{
			gAS5600OpState = AS5600_OP_STATE_IDEL;
			gAS5600OpTick = getTickCount();
		}
		else
		{
			gAS5600OpState = AS5600_OP_STATE_READY_I2C_FOR_ANGLE;
			gAS5600OpTick = getTickCount();
		}
		break;

	case AS5600_OP_STATE_READY_I2C_FOR_ANGLE:
		gAS5600I2CBuffer[0] = AS5600_ANGLE_H;
		gAS5600I2CFlag = 1;
		status = R_IICA0_Master_Send(AS5600_I2C_ADDR | I2C_WRITE_FLAG, gAS5600I2CBuffer, 1, 2);
		if (status == MD_OK)
			gAS5600OpState = AS5600_OP_STATE_WRITE_I2C_FOR_ANGLE;
		else
		{
			gAS5600I2CFlag = 0;
		}
		break;
*/
	case AS5600_OP_STATE_WRITE_I2C_FOR_ANGLE:
		if (!overTickCount(gAS5600OpTick, 1))
			break;
			
		gAS5600I2CFlag = 1;
	//	usleep(100);
		status = R_IICA0_Master_Receive(AS5600_I2C_ADDR | I2C_READ_FLAG, gAS5600I2CBuffer, 2, 10);
		if (status == MD_OK)
			gAS5600OpState = AS5600_OP_STATE_READ_I2C_FOR_ANGLE;
		else
		{
			gAS5600I2CFlag = 0;
			gAS5600OpState = AS5600_OP_STATE_IDEL;
			gAS5600OpTick = getTickCount();
		}
		break;

	case AS5600_OP_STATE_READ_I2C_FOR_ANGLE:
		gAS5600AngleRaw = (((unsigned short)gAS5600I2CBuffer[0]) << 8) | (gAS5600I2CBuffer[1]);
		gAS5600OpState = AS5600_OP_STATE_IDEL;
		gAS5600OpTick = getTickCount();
		gAS5600WatchDogTick = getTickCount();
		break;
	}
#endif
}



static void as5600DegreeProcess()
{
	unsigned short degree;
	unsigned short raw;

//	if (gAS5600AngleRaw > gAS5600StartAngleRaw)
//		raw = gAS5600AngleRaw - gAS5600StartAngleRaw;
//	else
//		raw = gAS5600StartAngleRaw - gAS5600AngleRaw;

	raw = (gAS5600AngleRaw + 4096 - gAS5600StartAngleRaw) % 4096;
	if (raw > 4096 / 2)
		raw = 4096 - raw;

	degree = (unsigned short)(((unsigned long)raw) * 3600 / 4096);

	if (gAS5600Degree >= gCamera0PhotoDegree && degree <= gCamera0PhotoDegree)
	{
		sendCamera0Signal();
	}


	if (gAS5600Degree >= gCamera1PhotoDegree && degree <= gCamera1PhotoDegree)
	{
		sendCamera1Signal();
	}


	gAS5600Degree = degree;
}


void as5600CloseDoorSignal()
{
	gAS5600StartAngleRaw = gAS5600AngleRaw;
}



void as5600Init()
{
	gAS5600AngleRaw = 0;
	gAS5600I2CFlag = 0;

	gCamera0PhotoDegree = 450;
	gCamera1PhotoDegree = 450;

	gAS5600OpState = AS5600_OP_STATE_IDEL;
	gAS5600OpTick = 0;
	gAS5600StartAngleRaw = INVALID_ANGLE_RAW;
	gAS5600WatchDogTick = 0;
	gAS5600I2CErrorCount = 0;


	IIC_Init();
	doorCalibratorInit();
}


void as5600Run()
{
	as5600OpStateMachine();
	if (overTickCount(gAS5600WatchDogTick, 1500))
	{
#ifndef USB_AS5600_I2C
		R_IICA0_Stop();
		R_IICA0_Create();
#else
		IIC_Init();
#endif
		gAS5600OpState = AS5600_OP_STATE_IDEL;
		gAS5600I2CFlag = 0;

		gAS5600OpTick = getTickCount();
		gAS5600WatchDogTick = gAS5600OpTick;
	}
	if (gAS5600StartAngleRaw == INVALID_ANGLE_RAW)
		return;

	as5600DegreeProcess();
}





