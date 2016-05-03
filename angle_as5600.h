/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/
#ifndef ANGLE_AS5600_H
#define ANGLE_AS5600_H

#define INVALID_ANGLE_RAW  0xffff

void as5600CloseDoorSignal();
void as5600Init();
void as5600Run();

void doorCalibratorInit();
void doorCalibratorSetCloseSignal();
void doorCalibratorMachine(unsigned short rawAngle);

extern unsigned short gAS5600AngleRaw;
extern unsigned short gAS5600StartAngleRaw;
extern unsigned short gAS5600Degree;
extern unsigned short gCamera0PhotoDegree;
extern unsigned short gCamera1PhotoDegree;
extern unsigned char  gAS5600I2CErrorCount;

#endif
