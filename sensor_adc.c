/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "utility.h"
#include "sensor_adc.h"
#include "signal_control.h"
#include "sensor_map.h"
#include "r_cg_adc.h"
#include "infrared_monitor.h"

typedef enum tagSENSOR_ADC_OP_STATE
{
	SENSOR_ADC_OP_STATE_IDLE,
	SENSOR_ADC_OP_STATE_TEMPERATURE, 
	SENSOR_ADC_OP_STATE_HUMIDITY,
	SENSOR_ADC_OP_STATE_HUMIDITY_ADCING,
	SENSOR_ADC_OP_STATE_INFRARED,

} SENSOR_ADC_OP_STATE;

#define TEMPERATURE_ADC_BUFFER_SIZE  5
static unsigned short gTemperatureADCBuffer[TEMPERATURE_ADC_BUFFER_SIZE];
static unsigned char  gTemperatureADCBufferIndex;
static unsigned char  gTemperatureADCBufferDataCount;

unsigned short gTemperatureSensorADCValue;
volatile unsigned short gHumiditySensorADCValue;
unsigned short gInfraredSensorADCValue;

static volatile unsigned char  gSensorADCOpFlag;

static SENSOR_ADC_OP_STATE   gSensorADCOpState;
static unsigned short gTemperatureADCTick;
static unsigned short gHumidityADCTick;
static unsigned short gInfraredADCTick;



typedef enum tagINFRARED_STATE
{
	INFRARED_STATE_IDLE,
	INFRARED_STATE_HUMAN,
	INFRARED_STATE_HUMAN_SIGNAL_END,
	INFRARED_STATE_DISABLE,

} INFRARED_STATE;

static INFRARED_STATE  gInfraredState;
unsigned short gInfraredHumanValve;
unsigned short gInfraredHumanDistanceTick;
static unsigned short gInfraredHumanTick;

static volatile unsigned short gADCResult;

void sensorADCCallback()
{
	gSensorADCOpFlag = 0;
}


typedef enum tagHUMIDITY_ADC_STATE
{
	HUMIDITY_ADC_STATE_INIT,
	HUMIDITY_ADC_STATE_IDLE,
	HUMIDITY_ADC_STATE_START,
	HUMIDITY_ADC_STATE_WORKING,
	HUMIDITY_ADC_STATE_END,

} HUMIDITY_ADC_STATE;

static volatile unsigned char  gTimer0HumidityCount;
static volatile unsigned char  gHumidityADCState;

//#define TEST_HUM_PIN
void timer0_callback()
{
	if (gTimer0HumidityCount & 0x01)
	{
		P1 |= _08_Pn3_OUTPUT_1;
		P12 &= (unsigned char)(~_01_Pn0_OUTPUT_1);
		

		if (gHumidityADCState == HUMIDITY_ADC_STATE_START)
		{
		//	gSensorADCOpFlag = 1;
			usleep(100);
			R_ADC_Start();

			gHumidityADCState = HUMIDITY_ADC_STATE_WORKING;
		}
	}
	else
	{
		if (gHumidityADCState == HUMIDITY_ADC_STATE_WORKING)
		{
			R_ADC_Get_Result(&gADCResult);
			R_ADC_Stop();
			gHumiditySensorADCValue = gADCResult;

			gHumidityADCState = HUMIDITY_ADC_STATE_END;
		}

		P1  &= (unsigned char)(~_08_Pn3_OUTPUT_1);
		P12 |= _01_Pn0_OUTPUT_1;
		

	}

	gTimer0HumidityCount++;
}


static void sensorInfraredProcess()
{
	switch (gInfraredState)
	{
	case INFRARED_STATE_HUMAN:
		if (gInfraredSensorADCValue < gInfraredHumanValve)
		{
			gInfraredState = INFRARED_STATE_IDLE;
			break;
		}

		if (overTickCount(gInfraredHumanTick, gInfraredHumanDistanceTick))
		{
			sendInfraredSignal();
			sendWakeupSignal();
			gInfraredState = INFRARED_STATE_HUMAN_SIGNAL_END;
		}
		break;

	case INFRARED_STATE_HUMAN_SIGNAL_END:
		if (gInfraredSensorADCValue < gInfraredHumanValve)
		{
			gInfraredState = INFRARED_STATE_IDLE;
		}
		break;
	case INFRARED_STATE_DISABLE:
		if(gInfraredMonitor.Enable)
		{
			gInfraredState = INFRARED_STATE_IDLE;
		}
		break;

	case INFRARED_STATE_IDLE:
	default:
		if (gInfraredSensorADCValue > gInfraredHumanValve)
		{
			gInfraredState = INFRARED_STATE_HUMAN;
			gInfraredHumanTick = getTickCount();
		}
		if(gInfraredMonitor.Enable == 0)
		{
			gInfraredState = INFRARED_STATE_DISABLE;
		}
		break;

	}
}


static unsigned short dejitterTemperatureADC(unsigned short adcValue)
{
	unsigned short total = 0;
	unsigned char i;

	gTemperatureADCBuffer[gTemperatureADCBufferIndex] = adcValue;
	gTemperatureADCBufferIndex = (gTemperatureADCBufferIndex + 1) % TEMPERATURE_ADC_BUFFER_SIZE;
	if (gTemperatureADCBufferDataCount < TEMPERATURE_ADC_BUFFER_SIZE)
		gTemperatureADCBufferDataCount++;

	for (i = 0; i < gTemperatureADCBufferDataCount; i++)
		total += gTemperatureADCBuffer[i];

	return total / gTemperatureADCBufferDataCount;
}


static void sensorADCOpStateMachine()
{
	uint16_t adcResult;

	switch (gSensorADCOpState)
	{
	case SENSOR_ADC_OP_STATE_IDLE:
		if (((gMcuState != MCU_STATE_WORK) && overTickCount(gTemperatureADCTick, 50))
			|| ((gMcuState == MCU_STATE_WORK) && overTickCount(gTemperatureADCTick, 2000)))
		{
			R_ADC_Stop();
			ADS = _00_AD_INPUT_CHANNEL_0;

			gSensorADCOpFlag = 1;
			R_ADC_Start();
			
			gSensorADCOpState = SENSOR_ADC_OP_STATE_TEMPERATURE;
			gTemperatureADCTick = getTickCount();
		}
		else if (((gMcuState != MCU_STATE_WORK) && overTickCount(gHumidityADCTick, 50))
				|| ((gMcuState == MCU_STATE_WORK) && overTickCount(gHumidityADCTick, 2000)))
		{
			R_ADC_Stop();
			ADS = _01_AD_INPUT_CHANNEL_1;

			if (gHumidityADCState == HUMIDITY_ADC_STATE_INIT)
			{
				R_TAU0_Channel0_Start();
			}
			gHumidityADCState = HUMIDITY_ADC_STATE_START;
			
			gSensorADCOpState = SENSOR_ADC_OP_STATE_HUMIDITY;
			gHumidityADCTick = getTickCount();
		}
		else if (overTickCount(gInfraredADCTick, 100) && (gInfraredMonitor.Auto == 1))
		{
			R_ADC_Stop();
			ADS = _02_AD_INPUT_CHANNEL_2;

			gSensorADCOpFlag = 1;
			R_ADC_Start();
			
			gSensorADCOpState = SENSOR_ADC_OP_STATE_INFRARED;
			gInfraredADCTick = getTickCount();
		}
		break;

	case SENSOR_ADC_OP_STATE_TEMPERATURE:
		if (gSensorADCOpFlag == 0)
		{
			R_ADC_Get_Result(&adcResult);
			R_ADC_Stop();

			gTemperatureSensorADCValue = dejitterTemperatureADC(adcResult);
			gSensorADCOpState = SENSOR_ADC_OP_STATE_IDLE;
		}
		break;

	case SENSOR_ADC_OP_STATE_HUMIDITY:

		if (gHumidityADCState == HUMIDITY_ADC_STATE_END)
		{
		//	R_TAU0_Channel0_Stop();
			R_ADC_Stop();
			gHumidityADCState = HUMIDITY_ADC_STATE_IDLE;
			gSensorADCOpState = SENSOR_ADC_OP_STATE_IDLE;
		}
		break;

	case SENSOR_ADC_OP_STATE_INFRARED:
		if (gSensorADCOpFlag == 0)
		{
			R_ADC_Get_Result(&adcResult);
			R_ADC_Stop();

			gInfraredSensorADCValue = adcResult;
			sensorInfraredProcess();

			gSensorADCOpState = SENSOR_ADC_OP_STATE_IDLE;
		}
		break;
	}
}


void sensorADCInit()
{
	gSensorADCOpFlag = 0;
	gSensorADCOpState = SENSOR_ADC_OP_STATE_IDLE;

	gTemperatureADCTick = 0;
	gHumidityADCTick = 0;
	gInfraredADCTick = 0;

	gInfraredState = INFRARED_STATE_IDLE;

	gInfraredHumanValve = 12 * 1024 / 33;//1.2V--372 //248; //0.8 0.8/3.3 * 1024;

	gInfraredHumanDistanceTick = 0;
	gInfraredHumanTick = 0;

	gTemperatureSensorADCValue = 560; //1819 - 20
	gHumiditySensorADCValue    = 0;
	gInfraredSensorADCValue    = 0;

	gTemperatureADCBufferIndex = 0;
	gTemperatureADCBufferDataCount = 0;

	gTimer0HumidityCount = 0;
	gHumidityADCState = HUMIDITY_ADC_STATE_INIT;
}


void sensorADCRun()
{
	sensorADCOpStateMachine();
}


