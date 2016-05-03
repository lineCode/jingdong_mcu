/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-13 Created by Fibo Lu

--*/


#ifndef UPDATE_H
#define UPDATE_H


#define PR_MSG_RET_NORM_END         0x00    /*  */
#define PR_MSG_RET_ERR_END          0x01    /*  */
#define PR_MSG_RET_PRM_ERR          0x05    /*  */
#define PR_MSG_RET_PROTECT_ERR      0x10    /*  */
#define PR_MSG_RET_ERASE_ERR        0x1A    /*  */
#define PR_MSG_RET_B_CHK_ERR        0x1B    /*  */
#define PR_MSG_RET_IVERIFY_ERR      0x1B    /*  */
#define PR_MSG_RET_WRITE_ERR        0x1C    /*  */
#define PR_MSG_RET_INT_ERR          0x1F    /*  */
#define PR_MSG_RET_READ_ERR         0x20    /*  */
#define PR_MSG_RET_CHKSUM_ERR       0xFF    /*  */

unsigned char needUpdate();
void updateInit();
void update();
void updateUartMessageProc(unsigned char * buffer);


#endif
