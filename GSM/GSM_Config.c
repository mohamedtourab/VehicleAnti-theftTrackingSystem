/**************************************************************************************
*File name: GSM_Config.c
*Auther :edited by the user
*Date: 6/3/2018
*Description:
*	This file contains:
*		- A structurs that containes the Groups to drive the GSM
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM.h"

void GSM_CallBack(GSM_CheckType GSM_Check, uint8_t* RecievedResponse, uint16_t ResponseLength);


const GSM_ConfigType GSM_ConfigParam=
{
        /*Uart Channel number*/
        0,

        &GSM_CallBack

};