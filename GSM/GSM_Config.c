/**************************************************************************************
*File name: GSM_Config.c
*Auther :edited by the user
*Date: 6/3/2018
*Description:
*       This file contains:
*               - A structurs that containes the Groups to manage the GSM 
*Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "GSM.h"
#include "DataHandler.h"


void Error_func(uint8_t id);


const GSM_ConfigType GSM_ConfigParam =
{

        /*GSM power key Group ID & it's pin mask*/
        3,PIN_MASK_3,

        /*GSM ready to send Group ID*/
        4,PIN_MASK_14,
        /*pin mask of the RTS in the selected GPIO group*/
        
        /*GSM Ring Group ID*/
        5,PIN_MASK_12,
        /*pin mask of the Ring in the selected GPIO group*/

        "internet.vodafone.net",21,
        "internet",8,
				"internet",8,


        SendWarningCallBackFn,RecievedSMSCallBackFn,SendServerCallBackFn,&Error_func



};