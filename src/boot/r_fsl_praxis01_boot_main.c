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
* File Name     : r_fsl_praxis01_boot_main.c
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
/* (#pragma) interrupt function                                              */
/*---------------------------------------------------------------------------*/
/*  */
#pragma interrupt INTWDTI prIntComWdt               /*  */
#pragma interrupt INTP0   prIntComSwIntp0           /*  */


/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
/*    */
#include "r_fsl_praxis01_BranchTable.h"

/*    */
#include "r_fsl_praxis01_BootSection.h"

/*    */
#include "r_fsl_praxis01_com.h"

/*    */
#include <string.h>


/*---------------------------------------------------------------------------*/
/* Parameter                                                                 */
/*---------------------------------------------------------------------------*/
/*  */
UB prDubBootMode = PR_BOOT_MODE_UNKNOWN;        /*   */


/*---------------------------------------------------------------------------*/
/* Prototypes                                                                */
/*---------------------------------------------------------------------------*/
/*  */
void prBootTargetStart( void );                 /*   */
void prBootTargetEnd  ( UB   );                 /*   */


/*############################################################################*/
/******************************************************************************
* Outline           : hdwinit
* Include           : none
* Declaration       : void hdwinit(void)
* Function Name     : main
* Description       : Main function and set some SFR registers.
* Argument          : none
* Return Value      : none
* Calling Functions : start-up routine( boot project )
******************************************************************************/
void hdwinit( void )
{
    /************************/
    /* LVIM, LVIS           */
    /************************/
    LVIM = 0x00;                /*  */
    LVIS = 0x00;                /*  */
    
    
/*--------------------------------------------------------------------
;   
;-------------------------------------------------------------------*/

    /**********************/
    /* CMC                */
    /**********************/
    CMC     = 0b00000000;   
/*              |||||||+--- AMPH
                |||||||      []
                |||||||       0: 1MHzÅÖfXÅÉ10MHz
                |||||||       1:10MHzÅÉfXÅÖ20MHz
                |||||++---- AMPHS1-AMPHS0
                |||||        []
                |||||         00:
                |||||         01:
                |||||         10:
                |||||         11:
                ||||+------ 0
                ||++------- EXCLKS/OSCSELS
                ||           []
                ||            00:
                ||            01:
                ||            10:
                ||            11:
                ++--------- EXCLK/OSCSEL
                             []
                              00:
                              01:
                              10:
                              11:
*/
    /******************************/
    /* CSC                        */
    /******************************/
    CSC     = 0b11000000;   
/*              |||||||+--- HIOSTOP
                |||||||      []
                |||||||       0:
                |||||||       1:
                ||+++++---- 0
                |+--------- XTSTOP
                |            []
                |             0:
                |             1:
                +---------- MSTOP
                             []
                              0:
                              1:
*/
    /************************/
    /* OSMC                 */
    /************************/
    OSMC    = 0b10000000;   
/*              |++|++++--- 0
                |  +------- WUTMMCK0
                |            [
                |             ]
                |              0:
                |              1:
                +---------- RTCLPC
                             []
                              
                               0:
                               1:
                                 
                                 
*/
    /****************/
    /* CKC          */
    /****************/
    CKC     = 0b00000000;   
/*              ||||++++--- 0
                |||+------- MCM0
                |||          [fMAIN]
                |||            0:(fIH)
                |||              
                |||            1:(fMX)
                |||              
                ||+-------- MCS <Read Only>
                ||           [fMAIN]
                ||             0:(fIH)
                ||             1:(fMX)
                |+--------- CSS
                |            []
                |              0:(fMAIN)
                |              1:(fSUB)
                +---------- CLS <Read Only>
                             []
                               0:(fMAIN)
                               1:(fSUB)
*/

/*--------------------------------------------------------------------
;   
;-------------------------------------------------------------------*/

/*   */

    /****************************************/
    /* PER0                                 */
    /****************************************/
    PER0 =  0b00000000;
/*            |+||||+|-- 0
              | |||| |
              | |||| +--TAU0EN: 
              | ||||        0   
              | ||||        1   
              | ||||
              | |||+----SAU0EN: 
              | |||         0   
              | |||         1   
              | |||
              | ||+-----SAU1EN: 
              | ||          0   
              | ||          1   
              | ||
              | |+------IICAEN: 
              | |           0   
              | |           1   
              | |
              | +-------ADCEN:
              |             0   
              |             1   
              |
              +---------RTCEN: 
                            0   
                            1   
*/

/*------------------------------------------------------------------
    
------------------------------------------------------------------*/

    /***********************************************/
    /* ADPC                                        */
    /***********************************************/
    ADPC = 0b00000001;  
/*           ||||++++------- ADPC3-ADPC0
             ||||             []
             ||||                +++++++---------- ANI14-ANI8/P156-P150
             ||||                |||||||++++++++-- ANI7-ANI0/P27-P20
             ||||                |||||||||||||||
             ||||           0000:AAAAAAAAAAAAAAA
             ||||           0001:DDDDDDDDDDDDDDD
             ||||           0010:DDDDDDDDDDDDDDA
             ||||           0011:DDDDDDDDDDDDDAA
             ||||           0100:DDDDDDDDDDDDAAA
             ||||           0101:DDDDDDDDDDDAAAA
             ||||           0110:DDDDDDDDDDAAAAA
             ||||           0111:DDDDDDDDDAAAAAA
             ||||           1000:DDDDDDDDAAAAAAA
             ||||           1001:DDDDDDDAAAAAAAA
             ||||           1010:DDDDDDAAAAAAAAA
             ||||           1011:DDDDDAAAAAAAAAA
             ||||           1100:DDDDAAAAAAAAAAA
             ||||           1101:DDDAAAAAAAAAAAA
             ||||           1110:DDAAAAAAAAAAAAA
             ||||           1111:DAAAAAAAAAAAAAA
             ++++-----------0    */


/*------------------------------------------------------------------
    
------------------------------------------------------------------*/

    /******************************/
    /* UART                       */
    /******************************/
    PR_UART_PORT_INIT();
    
    /*****************/
    /* LED           */
    /*****************/
    PR_LED_INIT();
    
    /****************/
    /* SW           */
    /****************/
    PR_SW_INIT();
    
    /*****************/
    /* A/D           */
    /*****************/
    PR_AD_INIT();
    
    /************************************/
    /* WDT                              */
    /************************************/
    PR_WD_INT_ON();
}


/*----------------------------------------------------------------------------*/
/* functions                                                                  */
/*----------------------------------------------------------------------------*/
/******************************************************************************
* Outline           : boot_main
* Include           : none
* Declaration       : void boot_main(void)
* Function Name     : boot_main
* Description       : none
* Argument          : none
* Return Value      : none
* Calling Functions : start-up routine( boot project )
******************************************************************************/
void boot_main(void)
{
 //   if( PR_BOOT_SW != PR_SW_ON )
    {
        UB dubRetCode;
        
        prBootTargetStart();                /* Target start processing */
        
        dubRetCode = prBootWriteProgram();  /* Run the write program (flash self programming). */
        
        prBootTargetEnd( dubRetCode );      /* Target termination processing */
    }
}

/******************************************************************************
* Outline           : prBootTargetStart
* Include           : none
* Declaration       : void prBootTargetStart(void)
* Function Name     : prBootTargetStart
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : boot_main
******************************************************************************/
void prBootTargetStart( void )
{
    /*****************/
    /* LED           */
    /*****************/
    PR_LED2_ON();
    
    /**************/
    /*            */
    /**************/
    prDubBootMode = PR_BOOT_MODE_WRITE;
    
    /************************/
    /*                      */
    /************************/
    EI();
}


/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prBootTargetEnd(void)
* Function Name     : prBootTargetEnd
* Description       : 
*                     
* Argument          : none
* Return Value      : none
* Calling Functions : boot_main
******************************************************************************/
void prBootTargetEnd( UB dubEndMode )
{
    UH  duh_i;
    UB  dubLed1flag = false;
    
    /*  */
    if( dubEndMode == false )
    {
        while( 1 )
        {
            if( dubLed1flag == false )
            {
                dubLed1flag = true;
                PR_LED1_ON();
            }
            else {
                dubLed1flag = false;
                PR_LED1_OFF();
            }
            
            for( duh_i = 0 ; duh_i < 65000 ; duh_i++ )
            {
                prWait10clcok();
            }
        }
    }
    /*  */
    else {
        PR_LED1_ON();
        
        while( 1 )
        {
            NOP();
            NOP();
        }
    }
}


/******************************************************************************
* Outline           : 
* Include           : none
* Declaration       : void prWait10clcok(void)
* Function Name     : prWait10clcok
* Description       : none
* Argument          : none
* Return Value      : none
* Calling Functions : prBootTargetEnd, main( flash project )
******************************************************************************/
void prWait10clcok( void )
{
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
}

/*----------------------------------------------------------------------------*/
/* Common interrupt functions                                                 */
/*----------------------------------------------------------------------------*/
/******************************************************************************
* Outline           : WDT
* Include           : none
* Declaration       : void prIntComWdt(void)
* Function Name     : prIntComWdt
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : vector table
******************************************************************************/
void prIntComWdt( void )
{
    WDTE = 0xAC;                    /*  */
}


/******************************************************************************
* Outline           : SW1
* Include           : none
* Declaration       : void prIntComSwIntp0(void)
* Function Name     : prIntComSwIntp0
* Description       : 
* Argument          : none
* Return Value      : none
* Calling Functions : vector table
******************************************************************************/
void prIntComSwIntp0( void )
{
    if( prDubBootMode == PR_BOOT_MODE_USER )
    {
        prSendMsgAsciiData();
    }
}

