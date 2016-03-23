/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-07 Created by Fibo Lu

--*/

#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_userdefine.h"
#include "ring_buffer.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


void ringBufferInit(RingBufferContext* context)
{
	context->mIn  = 0;
	context->mOut = 0;
}


unsigned char ringBufferGetFree(RingBufferContext* context, unsigned char **buffer1, unsigned char *len1, \
	unsigned char **buffer2, unsigned char *len2)
{

	unsigned char l;
	unsigned char len;

	len = RING_BUFFER_SIZE - context->mIn + context->mOut;

	l = min(len, RING_BUFFER_SIZE - (context->mIn % RING_BUFFER_SIZE));
	*buffer1 = context->mBuffer + (context->mIn % RING_BUFFER_SIZE);

	*len1 = l;

	/* then put the rest (if any) at the beginning of the buffer */
	*buffer2 = context->mBuffer;
	*len2 = len - l;

	return len;
}


unsigned char ringBufferPut(RingBufferContext* context, unsigned char len)
{
	context->mIn += len;
	return len;
}


unsigned char ringBufferGetData(RingBufferContext* context, unsigned char **buffer1, unsigned char *len1, \
	unsigned char **buffer2, unsigned char *len2)
{
	unsigned char l;
	unsigned char len;

	len = context->mIn - context->mOut;

	l = min(len, RING_BUFFER_SIZE - (context->mOut % RING_BUFFER_SIZE));
	*buffer1 = context->mBuffer + (context->mOut % RING_BUFFER_SIZE);
	*len1 = l;

	*buffer2 = context->mBuffer;
	*len2 = len - l;

	return len;
}


unsigned int ringBufferGet(RingBufferContext* context, unsigned char len)
{
	context->mOut += len;
	return len;
}


boolean ringBufferIsEmpty(RingBufferContext* context)
{
	return (context->mOut == context->mIn);
}




