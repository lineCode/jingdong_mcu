/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/


#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#define RING_BUFFER_SIZE  32

typedef struct tagRingBufferContext
{
	unsigned char mIn;
	unsigned char mOut;
	unsigned char mBuffer[RING_BUFFER_SIZE];

} RingBufferContext;

void ringBufferInit(RingBufferContext* context);
unsigned char ringBufferGetFree(RingBufferContext* context, unsigned char **buffer1, unsigned char *len1, \
	unsigned char **buffer2, unsigned char *len2);
unsigned char ringBufferPut(RingBufferContext* context, unsigned char len);

unsigned char ringBufferGetData(RingBufferContext* context, unsigned char **buffer1, unsigned char *len1, \
	unsigned char **buffer2, unsigned char *len2);

unsigned int ringBufferGet(RingBufferContext* context, unsigned char len);

boolean ringBufferIsEmpty(RingBufferContext* context);


#endif
