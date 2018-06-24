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

#include "GPS.h"
#include "GSM.h"
#include "DataHandlerConfig.h"

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//--------------------------------------------------------------------------------

//data type for the return values in the DataHandler
typedef enum {DATA_HANDLER_OK = 0, DATA_HANDLER_NOK , DATA_HANDLER_BSY} DataHandlerCheckType;


//a pointer to a call back function data type
typedef void (*DH_CallBackFnType)(void);

//a pointer to the error call back function data type
typedef void (*DH_ErrCallBackFnType)(uint8_t);

//--------------------------------------------------------------------------------

//structure to be filled by the user to configure the Data Handler
typedef struct
{
	/*Warning message to be sent*/
	uint8_t* WarningMsgPtr;

	/*length of warning message*/
	uint8_t WarningMsgLength;

	/*phone number*/
	uint8_t* PhoneNumPtr;

	/*the location server IP*/
	uint8_t* ServerIP;

	/*the location server IP Length*/
	uint8_t ServerIPLength;

	/*Server Port nubmer*/
	uint8_t* PortNum;

	/*Server Port nubmer Length*/
	uint8_t PortNumLength;

	/*the expected sms command message*/
	uint8_t* ExpectedMsgPtr;

	/*the expected sms command message Length*/
	uint8_t ExpectedMsgLength;

	/*a pointer to the warninig sent call back function*/
	DH_CallBackFnType DH_SendWarningCallBackFnPtr;

	/*a pointer to the location sent to the server call back function*/
	DH_CallBackFnType DH_SendLocationToServerCallBackFnPtr;

	/*a pointer to the SMS Command received call back function*/
	DH_CallBackFnType DH_ReceiveCMDCallBackFnPtr;


	/*a pointer to the error callback function*/
	DH_ErrCallBackFnType DH_ErrorCallBackFn;

}DataHandlerConfigType;

//------------------------------------------------------------------------------

//extern constant array of DataHandlerConfigType 
extern const DataHandlerConfigType DataHandlerConfigParam;

/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- DataHandler back functions --------------------------

/*
 * This function callback function from the GSM Manager Receive state it is called when it receives SMS
 * Inputs: 
 *			- SMS_Buffer : a pointer to the received SMS
 * Output:NONE
*/

void RecievedSMSCallBackFn(uint8_t* SMS_Buffer);

/*
 * This function callback function from the GSM Manager Send SMS state it is called when it finishes sending SMS
 * Inputs:NONE
 * Output:NONE
*/

void SendWarningCallBackFn(void);

/*
 * This function callback function from the GSM Manager Send Server MSG state it is called when it finishes sending Server MSG
 * Inputs:NONE
 * Output:NONE
*/

void SendServerCallBackFn(void);


/***************************************************************************************/


/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void DH_Init(void);

/*
 * This function is used to send a warning SMS 
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

DataHandlerCheckType DH_SendWarningSMS(void);

/*
 * This function is used to send the location to the server 
 *Inputs:
 *		- GPS_Location : apointer to a structure carring the GPS Location
 * Output:
 *		- an indication of the success of the function
*/

DataHandlerCheckType DH_SendLocationToServer(Location* GPS_Location);

/*
 * This function is a FSM to manage the on going operations of the DataHandler module
 *Inputs:NONE
 * Output:NONE
*/

void DH_ManageOngoingOperation(void);

#endif