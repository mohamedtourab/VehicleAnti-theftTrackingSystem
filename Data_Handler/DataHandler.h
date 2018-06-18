/**************************************************************************************
*File name: DataHandler.h
*Author:
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 13/6/2018
*Description:
*	This file contains:
*		-prototype of the callback function and data types 
*Microcontroller: STM32F407VG
***************************************************************************************/
#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

//#include "GSM.h"
#include "stdint.h"
#include "DataHandlerConfig.h"

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//--------------------------------------------------------------------------------

//data type for the return values in the DataHandler
typedef enum {DATA_HANDLER_OK = 0, DATA_HANDLER_NOK} DataHandlerCheckType;


//a pointer to a call back function data type
typedef void (*DataHandlerCallBackFnType)(void);

//--------------------------------------------------------------------------------

//structure to be filled by the user to configure the Data Handler
typedef struct
{
	/*UART Channel number*/
	uint8_t* DataHandlerCMD;

	/*a pointer to a call back function*/
	DataHandlerCallBackFnType DataHandlerCallBackFnPtr;

}DataHandlerConfigType;

//------------------------------------------------------------------------------

//extern constant array of DataHandlerConfigType 
extern const DataHandlerConfigType DataHandlerConfigParam;

/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- DataHandler back functions --------------------------

/*
 * This function callback function for the GSM Manager Receive state it is called when it receives SMS
 * Inputs: 
 *			- SMS_Buffer : a pointer to teh received SMS
 * Output:NONE
*/

void RecievedSMSCallBackFn(uint8_t* SMS_Buffer);


/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void DataHandlerInit(void);

/*
 * This function is a FSM to manage the on going operations of the DataHandler module
 *Inputs:NONE
 * Output:NONE
*/

void DataHandlerManageOngoingOperation(void);

#endif