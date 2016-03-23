/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/


#ifndef SIGNAL_CONTROL_H
#define SIGNAL_CONTROL_H

void sendWakeupSignal();
void sendCamera0Signal();
void sendCamera1Signal();
void sendInfraredSignal();
void sendResetSignal();

// 1 open   0:closed
void setDoorStatePin(unsigned char state);

void signalInit();
void signalRun();

#endif
