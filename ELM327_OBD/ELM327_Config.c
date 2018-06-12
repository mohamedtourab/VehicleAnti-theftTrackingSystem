
/**************************************************************************************
*File name: ELM327_Config.c
*Auther :edited by the user
*Date: 30/4/2018
*Description:
*       This file contains:
*               - A structurs that containes the Groups to manage the ELM327
*Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "ELM327.h"

void Data(uint16_t VData);
void Err(uint16_t id);

const ELM327_ConfigType ELM327_ConfigParam = 
{
	/*UART channel Id*/
	0,
	&Data,&Data,&Data,&Err
};

