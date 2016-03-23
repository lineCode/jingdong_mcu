/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/


#ifndef _SENSOR_ADC_H
#define _SENSOR_ADC_H

void sensorADCInit();
void sensorADCRun();

extern unsigned short gTemperatureSensorADCValue;
extern volatile unsigned short gHumiditySensorADCValue;
extern unsigned short gInfraredSensorADCValue;

extern unsigned short gInfraredHumanValve;
extern unsigned short gInfraredHumanDistanceTick;

#endif
