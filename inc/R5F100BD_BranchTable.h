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
* File Name     : r_fsl_praxis01_FlashSection.h
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
/******************************************************************************
Include <System Includes>, "Project Includes"
******************************************************************************/

/*---------------------------------------------------------------------------*/
/* (#pragma) branch table functions (ext_func)                               */
/*---------------------------------------------------------------------------*/
#pragma ext_func uartStationPutAndroidRxData 1
#pragma ext_func androidUartCallbackSendend  2
