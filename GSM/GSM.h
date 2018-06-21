/**************************************************************************************
*File name: GSM.h
*Author:
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 6/3/2018
*Description:
*	This file contains:
*		-macros to control the M95 GSM module
*		-prototype of the APIs of the M95 GSM module
*Microcontroller: STM32F407VG
***************************************************************************************/
#ifndef GSM_H
#define GSM_H

#include "GPIO.h"
#include "GSM_Driver.h"
#include "GSM_Config.h"

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//------------------------------------------------------------------------------------

//data type for the return values in the GSM manager
typedef enum {GSM_OK = 0, GSM_NOK, GSM_BSY} GSM_CheckType;


//--------------------------------------------------------------------------------------
//a pointer to a call back function data type
typedef void (*GSM_CallBackFnType)(void);
//a pointer to a call back function data type for SMS Receive
typedef void (*GSM_RecCallBackFnType)(uint8_t*);

//a pointer to the error call back function data type
typedef void (*GSM_ErrCallBackFnType)(uint8_t);


//--------------------------------------------------------------------------------------

//structure to be filled by the user to configure the GSM manager

typedef struct
{
	/*GSM power key Group ID*/
	uint8_t PWRKeyGroupId;
	/*pin mask of the power key in the selected GPIO group*/
	uint16_t PWRKeyPinMask;

	/*GSM ready to send Group ID*/
	uint8_t RTSGroupId;
	/*pin mask of the RTS in the selected GPIO group*/
	uint16_t RTSPinMask;

	/*GSM Ring Group ID*/
	uint8_t RingGroupId;
	/*pin mask of the Ring in the selected GPIO group*/
	uint16_t RingPinMask;
	
	/*a pointer to a String that holds the service provider APN*/
	uint8_t* ServiceProviderAPN;
	/*a variable to indicate the expected length of the service provider APN*/
	uint8_t ServiceProviderAPNLength;

	/*a pointer to a String that holds the service provider user name*/
	uint8_t* ServiceProviderUserName;
	/*a variable to indicate the expected length of the service provider user name*/
	uint8_t ServiceProviderUserNameLength;

	/*a pointer to a String that holds the service provider password*/
	uint8_t* ServiceProviderPassWord;
	/*a variable to indicate the expected length of the service provider password*/
	uint8_t ServiceProviderPassWordLength;

	/*a pointer to the send message callback function*/
	GSM_CallBackFnType SendMsgCallBackFn;

	/*a pointer to the received message callback function*/
	GSM_RecCallBackFnType RecieveMsgCallBackFn;

	/*a pointer to the server send message callback function*/
	GSM_CallBackFnType SendServerMsgCallBackFn;

	/*a pointer to the error callback function*/
	GSM_ErrCallBackFnType ErrorCallBackFn;

}GSM_ConfigType;

//------------------------------------------------------------------------------

//extern constant GSM_ConfigType structure of the GSM manager 
extern const GSM_ConfigType GSM_ConfigParam;


/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- GSM driver back function --------------------------

/*
 * This function callback function for the GSM driver it is called when the AT command execution is done  
 * Inputs:
 *		- GSM_Check 		: to indicate the success of the AT command execution
 *		- RecievedResponse 	: a pointer to an array to hold the response from the module to the AT command
 *		- ResponseLength 	: the length of the RecievedResponse
 * Output:NONE
*/

void GSM_DriverCallBackFn(GSM_DriverCheckType GSM_DriverCheck, uint8_t* ReceivedResponse, uint8_t ReceivedResponseLength);


//--------------------------------- manager functions ---------------------------


/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Init(void);

/*
 * This function is used to reset the defaults of the module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_SoftWareReset(void);

/*
 * This function is used to restart the module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_HardWareReset(void);

/*
 * This function is used to send an SMS from GSM module
 *Inputs:
 *		- Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *		- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 *		- PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_Send_SMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum);

/*
 * This function is used to send a server message from GSM module
 *Inputs:
 *		- ServerIP 		: Server IP address
 *		- ServerIPLength : the length of the serrver IP address
 *		- PortNum 		: the server TCP port number
 *		- PortNumLength : the length of the TCP server port number
 *		- Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *		- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_SendServerMsg(uint8_t* ServerIP, uint8_t ServerIPLength, uint8_t* PortNum, uint8_t PortNumLength, uint8_t* Msg, uint8_t MsgLength);

/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

void GSM_ManageOngoingOperation(void);

/**************************************************************************************/

#endif