/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* 
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_fsl_praxis01_flash_main.c
* Version       : 1.00
* Device(s)     : RL78/G13( R5F100LEA )
* Tool-Chain    : CubeSuite+ ( V1.01 )
* OS            : none
* H/W Platform  : QB-R5F100LE-TB
* Description   : 
* Operation     : 
* Limitations   : FSL AN-sample.
******************************************************************************/
/******************************************************************************
* History       : Version Description
*               : 09.29.2011 1.00 First Release
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* (#pragma) Expanded functions                                              */
/*---------------------------------------------------------------------------*/
#pragma sfr                             /*  */
#pragma DI                              /*  */
#pragma EI                              /*  */
#pragma NOP                             /*  */


/*---------------------------------------------------------------------------*/
/* Include common files                                                      */
/*---------------------------------------------------------------------------*/
/*  */
#include "D:\Solutions\r01an0718_praxis01\inc\r_fsl_praxis01_BranchTable.h"

/*  */
#include "D:\Solutions\r01an0718_praxis01\inc\r_fsl_praxis01_FlashSection.h"

/*  */
#include "D:\Solutions\r01an0718_praxis01\inc\r_fsl_praxis01_com.h"

/*---------------------------------------------------------------------------*/
/* Parameters                                                                */
/*---------------------------------------------------------------------------*/
#define PR_LED_DEFAULT_WAIT     0x0000FFFFl
#define PR_LED_WAIT_MAG         3l

/*---------------------------------------------------------------------------*/
/* constant definitions                                                      */
/*---------------------------------------------------------------------------*/
/*  */
static __far const UB prFcubSendMsgData[] = 
#ifdef PR_USE_OCD_MODE
{ 
"Test=Ver.1.00\n\rOCD test mode\n\r"
};
#else
{ 
/******************************************************************************/
/* For r_fsl_praxis01_flash.hex                                               */	
"\n\rTest=Ver.1.00\n\rApplication forced reset\n\rSW1 number = "
/******************************************************************************/
/* For r_fsl_praxis01_write_test.hex                                          */
/*
"\n\rTest=Ver.2.00\n\rApplication forced reset\n\rSW1 number = "     
*/
/******************************************************************************/
};
#endif

/*---------------------------------------------------------------------------*/
/* Parameters                                                                */
/*---------------------------------------------------------------------------*/
UW  prDuwLedTime = PR_LED_DEFAULT_WAIT;         /*                */
UH  prDuhSwNum   = 0;                           /*                */

/*---------------------------------------------------------------------------*/
/* prototypes                                                                */
/*---------------------------------------------------------------------------*/
void prFlashTargetStart( void );                /*   */
void prFlashTargetEnd  ( void );                /*   */

/*---------------------------------------------------------------------------*/
/* main functions                                                            */
/*---------------------------------------------------------------------------*/
/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prBootTargetStart(void)
* Function Name     : main
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : start-up routine(flash project)
******************************************************************************/
void main( void )
{
    UB dubLed2flag;
    UW duw_i;
    
    prFlashTargetStart();
    dubLed2flag  = true;
    
    /* Main loop */
    while(1)
    {
        /* LED turn-on processing */
        if( dubLed2flag == false )
        {
            dubLed2flag = true;
            PR_LED2_ON();
        }
        else {
            dubLed2flag = false;
            PR_LED2_OFF();
        }
        
        /*  */
        for( duw_i = 0 ; duw_i < prDuwLedTime ; duw_i++ )
        {
            prWait10clcok();
        }
    }
    
    prFlashTargetEnd();
}


/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prFlashTargetStart(void)
* Function Name     : prBootTargetStart
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : main
******************************************************************************/
void prFlashTargetStart( void )
{
    /*****************/
    /* LED           */
    /*****************/
    PR_LED1_ON();
    
    /****************/
    /* UART         */
    /****************/
    prUartInit();         /*  */
    
    /**************/
    /*            */
    /**************/
    prDubBootMode = PR_BOOT_MODE_USER;
    
    /************************/
    /*                      */
    /************************/
    EI();
}


/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prFlashTargetEnd(void)
* Function Name     : prFlashTargetEnd
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : main
******************************************************************************/
void prFlashTargetEnd( void )
{
    while(1)
    {
        NOP();
        NOP();
    }
}


/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prFlashTargetEnd(void)
* Function Name     : prFlashTargetEnd
* Description       : 
*                     
* Argument          : none
* Return Value      : none
* Calling Functions : prIntComSwIntp0
******************************************************************************/
void prSendMsgAsciiData( void )
{
    UH  duh_i;
    UB  dubSendData[5];
    
    /*  */
    for( duh_i = 0 ; duh_i < sizeof( prFcubSendMsgData ) ; duh_i++ )
    {
        dubSendData[0] = prFcubSendMsgData[ duh_i ];
        prUartSendData( &dubSendData[0] );
    }
    
    /*  */
    prDuhSwNum++;
    if( prDuhSwNum > 999 )
    {
        prDuhSwNum = 0;
    }
    
    /*  */
    dubSendData[0] = (UB)( prDuhSwNum / 100 ) | 0x30;
    dubSendData[1] = (UB)( ( prDuhSwNum % 100 ) / 10 ) | 0x30;
    dubSendData[2] = (UB)( prDuhSwNum % 10  ) | 0x30;
    dubSendData[3] = '\n';
    dubSendData[4] = '\r';
    
    /*  */
    for( duh_i = 0 ; duh_i < 5 ; duh_i++ )
    {
        prUartSendData( &dubSendData[duh_i] );
    }
    
#ifdef PR_USE_OCD_MODE
    
#else
    /*  */
    PR_WD_INT_OFF();
    
    /*  */
    if( prDuwLedTime <= PR_LED_DEFAULT_WAIT )
    {
        prDuwLedTime = PR_LED_DEFAULT_WAIT * PR_LED_WAIT_MAG;
    }
#endif
}

