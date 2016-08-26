/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.03.02.01 [15 May 2015]
* Device(s)    : R5F100BD
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 2016/3/30
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
/* Start user code for include. Do not edit comment generated here */
#include "signal_control.h"
#include "uart_station.h"
#include "sensor_adc.h"
#include "utility.h"
#include "fgcp.h"
#include "monitor.h"
#include "heater.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
extern volatile uint16_t  g_uart0_tx_count;            /* uart0 send data number */
extern volatile uint16_t  g_uart1_tx_count;            /* uart1 send data number */
extern volatile uint16_t  g_iica0_tx_cnt;
extern volatile uint16_t  g_iica0_rx_cnt;
void test_uart2();
void test_i2c();
void uartStationInit();
void uartStationRun();
void as5600Init();
void as5600Run();
void test_led();
void test_time();
void flash_hdwinit(void);

/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);
void R_UART1_Start(void);
void R_UART1_Init(void);
MD_STATUS R_UART1_Send(uint8_t * const tx_buf, uint16_t tx_num);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
	/* Start user code. Do not edit comment generated here */
	flash_hdwinit();
	usleep(200);
	/* End user code. Do not edit comment generated here */

	R_MAIN_UserInit();
	/* Start user code. Do not edit comment generated here */
	mcuStateInit();
	monitorInit();
	FGCProtocolInit();

	uartStationInit();
	R_UART0_Init();
	R_UART0_Start();

	R_UART1_Init();
	R_UART1_Start();

	//R_IICA0_Create();
	as5600Init();
	heaterInit();
	signalInit();
	sensorADCInit();

	timeInit();
	R_ADC_Set_OperationOn();

	while (1U)
	{
		;
		if (gMcuState != MCU_STATE_UPDATE)
		{
			R_WDT_Restart();
		}
		monitorRun();
		uartStationRun();
		as5600Run();
		signalRun();
		sensorADCRun();
		timeRun();
		FGCProtocolRun();

		mcuStateRun();
		heaterRun();
		//	test_time();
		//	test_uart2();
	}
	/* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
#if 0
void test_led()
{
	unsigned short tick = getTickCount();
	if ((tick % 2000) < 1000)
		P1 |= _40_Pn6_OUTPUT_1;
	else
		P1 &= (unsigned char)~(_40_Pn6_OUTPUT_1);
}
#endif

static unsigned short gTestTick = 0;
static unsigned long  gTestTick32 = 0;
static unsigned long  gTestCount = 0;

extern void IIC_start();
extern unsigned char GetACK();
extern void IIC_stop();
extern void IIC_WritByte(unsigned char byte);
extern unsigned char IIC_ReadByte();
extern void ACK(void);  //

void test_time()
{
#if 0	
	unsigned long count = 0;

	for (count = 0; count < 100000; count++)
	{
		IIC_start();
		IIC_WritByte(0x23);
		GetACK();
		IIC_stop();

		if ((count % 600) == 599)
			R_WDT_Restart();
	}

	count = 0;  //3 x 1 nop 12.71 s  86.6khz; 3 x 2 nop 16.03  68.6KHZ
	count = 2;

#elif 1
	gTestCount++; //MAIN LOOP: 30S  368395  12.279KHZ
#elif 0
	//test usleep();
	unsigned short count;

	for (count = 0; count < 6000; count++)
	{
		usleep(10000);
		if ((count % 300) == 299)
			R_WDT_Restart();
	}

	count = 0;

	for (count = 0; count < 60000; count++)
	{
		usleep(1000);

		if ((count % 1000) == 999)
			R_WDT_Restart();
	}

	count = 0;
	count++;

#else
	unsigned char  t;
	unsigned short  tick;
	unsigned long   tick32;
	tick = getTickCount();
	if (tick < gTestTick)
	{
		tick = 0;

		t = overTickCount(gTestTick, 3);
		gTestTick = 0;
	}
	else
		gTestTick = tick;


	tick32 = getTickCount32();
	if (tick < gTestTick32)
	{
		tick32 = 0;
		gTestTick32 = 0;
	}
	else
		gTestTick32 = tick32;

	gTestCount++;
#endif

}

#if 1
// uint8_t buf0[12] = {0xAA, 0x0B, 0xCA, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x67};
// uint8_t buf0[12] = {'a', 'b', 'c', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
// uint8_t buf0[12] = {0xAA, 0x33, 0x2A, 0x3A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t buf0[12] = { 0xAA, 0x0B, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0xEF }; //fridge test command 

void test_uart2()
{
	// 	uint8_t buf0[12] = {0xAA, 0x0B, 0xCA, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x67};

	//   uint8_t buf1[2] = {0x55, 0x55}; //android �� -- 
	//	buf0[0] = gTestCount;
	//	gTestCount++;
	//	buf0[1] = gTestCount;
	//	gTestCount++;
	//	gTestCount++;
	//	if (gTestCount < 900)
	//		return;

	gTestCount++;

	if (gTestCount > 600) //g_uart1_tx_count == 0)
	{
		buf0[0] = 0xAA;
		buf0[1] = 0x00;
		buf0[2] = 0x01;
		R_UART1_Send(buf0, 3);
		gTestCount = 0;
		//	R_UART0_Send(buf0, 12);
	}

	//uartStationRun();

}
#endif

#if 0
#define  AS5600_I2C_ADDR  (0x36 << 1)
#define I2C_WRITE_FLAG  0x00
#define I2C_READ_FLAG   0x01

volatile uint8_t   gI2CTestFlag;
void i2c_end()
{
	gI2CTestFlag = 0;
}


//volatile uint16_t  gTestCount = 0;

void test_i2c()
{
	uint8_t buf[2] = { 0x0e, 0x25 };
	unsigned short raw;
	MD_STATUS  status;

	gTestCount++;
	if (gTestCount < 500)
		return;

	gTestCount = 0;

	do
	{
		gI2CTestFlag = 1;
		status = R_IICA0_Master_Send(AS5600_I2C_ADDR | I2C_WRITE_FLAG, buf, 1, 2);

	} while (status == MD_ERROR1);

	//	while (g_iica0_tx_cnt);
	while (gI2CTestFlag);

	do
	{
		gI2CTestFlag = 1;
		status = R_IICA0_Master_Receive(AS5600_I2C_ADDR | I2C_READ_FLAG, buf, 2, 10);
	} while (status == MD_ERROR1);


	//	while (g_iica0_rx_cnt < 2);

	while (gI2CTestFlag);

	//	if ((buf[0] & 0x20) == 0)
	//		return;

	//	usleep(10000);

	buf[0] = 0x0e;

	return;
	/*
	do
	{
	gI2CTestFlag = 1;
	status = R_IICA0_Master_Send(AS5600_I2C_ADDR | I2C_WRITE_FLAG, buf, 1, 2);
	} while (status == MD_ERROR1);

	while (gI2CTestFlag);


	do
	{
	gI2CTestFlag = 1;
	R_IICA0_Master_Receive(AS5600_I2C_ADDR | I2C_READ_FLAG, buf, 2, 10);

	} while (status == MD_ERROR1);

	//while (g_iica0_rx_cnt < 2);
	while (gI2CTestFlag);

	raw = (((unsigned short)buf[0]) << 8);

	/*
	buf[0] = 0x0f;

	do
	{
	status = R_IICA0_Master_Send(AS5600_I2C_ADDR | I2C_WRITE_FLAG, buf, 1, 2);
	} while (status == MD_ERROR1);

	while (g_iica0_tx_cnt);

	do
	{
	R_IICA0_Master_Receive(AS5600_I2C_ADDR | I2C_READ_FLAG, buf, 1, 10);
	} while (status == MD_ERROR1);

	while (g_iica0_rx_cnt < 1);


	R_IICA0_Stop();

	raw |= buf[0];
	raw = 0;
	*/

}
#endif
/* End user code. Do not edit comment generated here */
