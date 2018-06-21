/**************************************************************************************
*File name: DataHandlerConfig.c
*Auther :edited by the user
*Date: 13/6/2018
*Description:
*       This file contains:
*               - A structurs that containes the DataHandler configurations 
*Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "DataHandler.h"

void Test(void);

const DataHandlerConfigType DataHandlerConfigParam =
{

        /*The Command*/
        "LOC",

        /*a pointer to a call back function*/
        &Test
};