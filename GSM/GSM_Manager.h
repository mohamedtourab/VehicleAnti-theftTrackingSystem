/**************************************************************************************
*        File name: GSM_manager.h
*        Auther : 
                        -Abdelrhman Hosny
                        -Amr Mohamed
*        Date: 18/3/2018
*        Description:
*                        this file contains:
                                -macros to manage the M95 GSM module
                                -prototype of the APIs of the M95 GSM module manager
*        Microcontroller: STM32F407VG
***************************************************************************************/ 
#ifndef GSM_MANAGER_H
#define GSM_MANAGER_H

#include "GSM.h"
#include "GPIO.h"
#include "GSM_ManagerConfig.h"

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//------------------------------------------------------------------------------------

//data type for the return values in the GSM manager
typedef enum {GSM_Manage_OK = 0, GSM_Manage_NOK, GSM_Manage_InProgress} GSM_Manage_CheckType;


//--------------------------------------------------------------------------------------
//a pointer to a call back function data type
typedef void (*GSM_ManagerCallBackFn)(void);


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

	/*GSM State Group ID*/
	uint8_t DTRGroupId;
	/*pin mask of the DTR in the selected GPIO group*/
	uint16_t DTRPinMask;

	/*GSM Ring Group ID*/
	uint8_t RingGroupId;
	/*pin mask of the Ring in the selected GPIO group*/
	uint16_t RingPinMask;

	/*a pointer to a buffer to hole the recived message for the client to read*/
	uint8_t* ReadMsgBuffer;

	/*a variable to indecate the expected length of the recived message*/
	uint8_t ReadMsgLength;

	/*a pointer to the send message callback function*/
	GSM_ManagerCallBackFn SendMsgCallBack;

	/*a pointer to the recieved message callback function*/
	GSM_ManagerCallBackFn RecieveMsgCallBack;

	/*a pointer to the error callback function*/
	GSM_ManagerCallBackFn ErrorCallBack;

}GSM_ManageConfigType;

//------------------------------------------------------------------------------

//extern constant GSM_ConfigType structure of the GSM manager 
extern const GSM_ManageConfigType GSM_ManageConfigParam;


/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- GSM driver back function --------------------------

/*
 * This function callback function for the GSM driver it is called when the AT command execution is done  
 * Inputs:
 		- GSM_Check 		: to indicate the success of the AT command execution
 		- RecievedResponse 	: a pointer to an array to hold the response from the module to the AT command
 		- ResponseLength 	: the length of the RecievedResponse
 * Output:NONE
*/

void GSM_CallBack(GSM_CheckType GSM_Check, uint8_t* RecievedResponse, uint16_t ResponseLength);


//--------------------------------- manager functions ---------------------------


/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Init(void);
/*
 * This function is used to configure the GSM module with the necessary settings
 *Inputs:NONE
 * Output:NONE
*/

void StartCommunication(void);

/*
 * This function is used to reset the defaults of the module
 *Inputs:NONE
 * Output:NONE
*/

void SoftWareReset(void);
/*
 * This function is used to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:NONE
*/

void SendSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum);

/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_Manage_CheckType GSM_ManageOngoingOperation(void);

/**************************************************************************************/

#endif