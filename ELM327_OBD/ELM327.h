/**************************************************************************************
*File name: ELM327.h
*Auther : 
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 30/4/2018
*Description:
*	This file contains:
		-macros to control the ELM327 OBD module
*		-prototype of the APIs of the ELM327 OBD module
*Microcontroller: STM32F407VG
***************************************************************************************/ 


#ifndef _ELM327_H_
#define _ELM327_H_

#include "UART.h"
#include "ELM327_Config.h"


/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//------------------------------------------------------------------------------------

/*data type for the return values in the ELM327*/
typedef enum 
{
	ELM327_OK=0,
	ELM327_BUSY,
	ELM327_NOK
}ELM327_CheckType;

//------------------------------------------------------------------------------------

/*structure to be filled by the user to configure the ELM327*/
typedef struct 
{
	/*UART channel Id*/
	uint8_t UartChannelId;

}ELM327_ConfigType;

//extern constant ELM327_ConfigType structure of the ELM327
extern const ELM327_ConfigType ELM327_ConfigParam;

/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/
void GSM_Init(void);


/*
 * This function is used to Reset to defaults
 *Inputs:NONE
 * Output:
 			-indication of success of the function
*/
ELM327_CheckType ELM327_ResetDefaults(void);


/*
 * This function is used to Read the speed
 *Inputs:NONE
 * Output:
 			-indication of success of the function
*/
ELM327_CheckType ELM327_ReadSpeed(int16_t* VehicleSpeed);


/*
 * This function is used to Read the RPM 
 *Inputs:NONE
 * Output:
 			-indication of success of the function
*/
ELM327_CheckType ELM327_Read_RPM(int16_t* VehicleRPM);


/*
 * This function is used to Read the state of the battery
 *Inputs:NONE
 * Output:
 			-indication of success of the function
*/
ELM327_CheckType ELM327_ReadBatteryState(uint8_t* BatteryState);


/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:NONE
*/
void GSM_ManageOngoingOperation(void);


#endif