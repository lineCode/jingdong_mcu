/*

infrared threshold adjust

Wisky.2017.10.10

*/

#include <math.h>
#include <string.h>
#include "fgcp.h"
#include "utility.h"
#include "sensor_adc.h"
#include "angle_as5600.h"
#include "infrared_monitor.h"


unsigned short gInfraredSampleTime;
unsigned short gInfraredAdjustTime;

unsigned short last_var_value;
unsigned short last_avg_value;

unsigned short last_temp;

unsigned short last_count;

//unsigned short long_lenth;

//unsigned short cycle_count;

INFRARED_MONITOR_DATA gInfraredMonitor;


//=================================================================
//  infrared threshold test value 20cm - 160cm  (JD provide)
//=================================================================
//static const unsigned short infrared_threshold_value[] =
//{ 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160 };

//static const unsigned short infrared_threshold_value[] =
//20cm, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160cm
//{ 845, 627, 481, 372, 327, 276, 230, 148};


void infrared_wakeup_enable(unsigned char enable)
{
	gInfraredMonitor.Enable = enable;
}

void infrared_auto_adjust(unsigned char Auto)
{
	gInfraredMonitor.Auto = Auto;
}
unsigned char get_infrared_auto_adjust()
{
	return gInfraredMonitor.Auto;
}

void set_infrared_threshold_value(unsigned short value)
{
	gInfraredHumanValve = value;
}

void infrared_data_init(void)
{
	gInfraredMonitor.count = 0;
	
	gInfraredMonitor.avg = 0;
	gInfraredMonitor.var = 0;

	gInfraredMonitor.temp = 0;
	//memset(gInfraredMonitor.value, 0, sizeof(gInfraredMonitor.value)/sizeof(short));
}

void infrared_data_sample_reset(void)
{
	infrared_data_init();
	gInfraredMonitor.infraredSampleTick = getTickCount();
	gInfraredMonitor.infraredAdjustTick = getTickCount();
}

void infrared_threshold_adjust(void)
{
	//int i = 0;
	unsigned long temp = 0;
	
	//if (!gInfraredMonitor.Auto)
	//	return;
	if(gInfraredMonitor.count < (SAMPLE_DATA_SIZE / 2))
	{
		//infrared_data_sample_reset();
		last_avg_value = 0xCCCC;
		return;
	}

	//for (i = 0; i < gInfraredMonitor.count; i++)
	//{
	//	temp += gInfraredMonitor.value[i];
	//}
	last_count = gInfraredMonitor.count;

	gInfraredMonitor.avg = gInfraredMonitor.avg / gInfraredMonitor.count;
	
	last_avg_value = gInfraredMonitor.avg;

	//if(gInfraredMonitor.avg == 0)
	//	return;

	//for (i = 0; i < gInfraredMonitor.count; i++)
	//{
	//	temp += ((gInfraredMonitor.value[i] - gInfraredMonitor.avg) * (gInfraredMonitor.value[i] - gInfraredMonitor.avg));
	//}

	
	//standard deviation
	gInfraredMonitor.var = sqrt((gInfraredMonitor.temp - gInfraredMonitor.count * gInfraredMonitor.avg * gInfraredMonitor.avg) / gInfraredMonitor.count);
	//temp = (gInfraredMonitor.temp - gInfraredMonitor.count * gInfraredMonitor.avg ) / gInfraredMonitor.count;
	
	//last_temp = temp;
	
	//gInfraredMonitor.var = sqrt(temp);
	//variance
	last_var_value = gInfraredMonitor.var;

	if (gInfraredMonitor.var > 50) // 10 for test
	{
		//invaild data
		//infrared_data_sample_reset();
		//last_var_value = 0xFFFF;
		return;
	}
	//adjust infrared threshold value according to avg 

	if (gInfraredMonitor.avg >= INFRARED_THRESHOLD_40CM)// distance <= 30 cm
	{ 
		//disable infrared wake up  
		set_infrared_threshold_value(INFRARED_THRESHOLD_30CM);
		infrared_wakeup_enable(0);
	}
	else if ((INFRARED_THRESHOLD_70CM < gInfraredMonitor.avg) && (gInfraredMonitor.avg < INFRARED_THRESHOLD_40CM))// 30 cm < avg < 55cm
	{ 
		set_infrared_threshold_value(INFRARED_THRESHOLD_30CM);
		infrared_wakeup_enable(1);
	}
	else if (gInfraredMonitor.avg <= INFRARED_THRESHOLD_70CM)	// > 55cm 
	{ 
		set_infrared_threshold_value(INFRARED_THRESHOLD_50CM);
		infrared_wakeup_enable(1);
	}

}



void infrared_threshold_adjust_init(void)
{
	//gInfraredThresholdMonitorTick = 0;
	gInfraredMonitor.infraredSampleTick = 0;
	gInfraredMonitor.infraredAdjustTick = 0;
	infrared_data_init();
	
	gInfraredSampleTime = 200;    //200ms
  	gInfraredAdjustTime = 60000; //60 sec

	//gInfraredThresholdMonitorTime = 180000;//3 Min

	infrared_wakeup_enable(1);
	infrared_auto_adjust(1);
	
	last_var_value = 0;
	last_avg_value = 0;
	
	//cycle_count = 0;	
	last_count = 0;
	last_temp = 0;
	
}

void infrared_data_sampling(void)
{
	unsigned long infrared_val = gInfraredSensorADCValue;
	
	if(infrared_val == 0 || gInfraredMonitor.count > SAMPLE_DATA_SIZE)
		return;
		
	gInfraredMonitor.avg += infrared_val;	
	gInfraredMonitor.temp += (infrared_val * infrared_val);	
	gInfraredMonitor.count++;	
}

void infrared_adjust_func(void)
{
	//cycle_count++;
	//if(cycle_count == SMAPLE_DATA_TIME)//3 min
	//{
	//	cycle_count = 0;
	infrared_threshold_adjust();
	infrared_data_init();
	//}
}


void infrared_threshold_adjust_run(void)
{
	if (!gInfraredMonitor.Auto)
		return;
	//if(!gInfraredMonitor.Enable)
	//	return;
	if(getDoorState() != DOOR_STATE_CLOSED)
		return;	
	if (overTickCount(gInfraredMonitor.infraredSampleTick, gInfraredSampleTime))
	{
		infrared_data_sampling();					
		gInfraredMonitor.infraredSampleTick = getTickCount();
	}
	if (overTickCount(gInfraredMonitor.infraredAdjustTick, gInfraredAdjustTime))
	{
		infrared_adjust_func();
		gInfraredMonitor.infraredAdjustTick = getTickCount();	
	}
}


