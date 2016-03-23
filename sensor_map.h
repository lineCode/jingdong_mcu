/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/


#ifndef _SENSOR_MAP_H
#define _SENSOR_MAP_H

char getTemperatureFromADCValue(unsigned short adcValue);
unsigned char getHumidityFromADCValue(char temperature, unsigned short adcValue);
unsigned short getVoltageIn10mVFromADCValue(unsigned short adcValue);

#endif
