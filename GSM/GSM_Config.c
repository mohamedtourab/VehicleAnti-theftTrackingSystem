/**************************************************************************************
*File name: GSM_Config.c
*Auther :edited by the user
*Date: 6/3/2018
*Description:
*	This file contains:
*		- An array of structurs that containes the croups to initialize the GPIO
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM.h"
#include "GSM_Config.h"

void GSM_CallBack(GSM_CheckType GSM_Check, uint8_t* RecievedResponse, uint16_t ResponseLength);


const GSM_ConfigType GSM_ConfigParam=
{
        /*Uart Channel number*/
        0,

        /*GSM power key Group ID & it's pin mask*/
        2,PIN_MASK_2,

        /*GSM ready to send Group ID*/
        3,PIN_MASK_13,
        /*pin mask of the RTS in the selected GPIO group*/

        /*GSM State Group ID*/
        3,PIN_MASK_14,
        /*pin mask of the DTR in the selected GPIO group*/
        
        /*GSM Ring Group ID*/
        4,PIN_MASK_0,
        /*pin mask of the Ring in the selected GPIO group*/

        &GSM_CallBack

};