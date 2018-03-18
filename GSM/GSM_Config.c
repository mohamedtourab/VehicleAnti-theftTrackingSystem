/**************************************************************************************
*	File name: GSM_Config.c
*Auther :edited by the user
*	Date: 6/3/2018
*	Description:
*			This file contains:
*			- An array of structurs that containes the croups to initialize the GPIO
*	Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM.h"
#include "GSM_Config.h"


const GSM_ConfigType GSM_ConfigParam 
{
        /*Uart Channel number*/
        0,

        /*GSM power key Group ID*/
        1,
        /*pin mask of the power key in the selected GPIO group*/
        PIN_MASK_2,

        /*GSM ready to send Group ID*/
        2,
        /*pin mask of the RTS in the selected GPIO group*/
        PIN_MASK_13,

        /*GSM State Group ID*/
        2,
        /*pin mask of the DTR in the selected GPIO group*/
        PIN_MASK_14,
        
        /*GSM Ring Group ID*/
        3,
        /*pin mask of the Ring in the selected GPIO group*/
        PIN_MASK_0

};