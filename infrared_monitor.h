/*

infrared threshold monitor

Wisky.2017.10.10

*/


#ifndef _INFRARED_MONITOR_H
#define	_INFRARED_MONITOR_H

#define	 SAMPLE_DATA_SIZE		(60000 / 200)
//#define	 SMAPLE_DATA_TIME		3	// 3 * 60s  = 3 Min

#define	 INFRARED_THRESHOLD_30CM	612//556   //612
#define	 INFRARED_THRESHOLD_55CM	355//334   //355
#define  INFRARED_THRESHOLD_40CM	468
#define  INFRARED_THRESHOLD_70CM	285
#define  INFRARED_THRESHOLD_50CM	372

typedef struct infrared_threshold
{
	unsigned char Enable : 1;
	unsigned char Auto : 1;

	unsigned long avg;	//average 
	unsigned long count;	//data count
	unsigned long temp;	//temp value
	unsigned short var;	//variance
	unsigned short infraredSampleTick;
	unsigned short infraredAdjustTick;
	//unsigned short value[SAMPLE_DATA_SIZE];

}INFRARED_MONITOR_DATA;

/*
for test
*/
extern unsigned short last_var_value;
extern unsigned short last_avg_value;
extern unsigned short last_count;
extern unsigned short last_temp;
//extern unsigned short cycle_count;


//extern unsigned short long_lenth;



extern INFRARED_MONITOR_DATA gInfraredMonitor;


void infrared_wakeup_enable(unsigned char enable);
void infrared_auto_adjust(unsigned char Auto);
unsigned char get_infrared_auto_adjust();
void set_infrared_threshold_value(unsigned short value);
void infrared_data_init(void);
void infrared_threshold_adjust(void);
void infrared_threshold_adjust_init(void);
void infrared_data_sampling(void);
void infrared_adjust_func(void);
void infrared_threshold_adjust_run(void);
#endif
