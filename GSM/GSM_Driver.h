/**************************************************************************************
*File name: GSM_Driver.h
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
#ifndef GSM_DRIVER_H
#define GSM_DRIVER_H

#include "UART.h"

/*************************************************************************************
**********                     Macros to control GSM                            ******
*************************************************************************************/

/*************************************************************************************
**********              Macros to control GSM SMS format                        ******
*************************************************************************************/

#define PDU_MODE		0U
#define TEXT_MODE		1U

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//--------------------------------------------------------------------------------

//enumerate data type for the functions' return values
typedef enum {GSM_DRIVER_OK = 0, GSM_DRIVER_NOK} GSM_DriverCheckType;

//--------------------------------------------------------------------------------

//a pointer to a call back function data type
typedef void (*GSM_DriverCallBackFnType)(GSM_DriverCheckType, uint8_t*, uint8_t);

//--------------------------------------------------------------------------------

//structure to be filled by the user to configure the GSM
typedef struct
{
	/*UART Channel number*/
	uint8_t UartChannelId;

	/*a pointer to a call back function*/
	GSM_DriverCallBackFnType GSM_DriverCallBackFnPtr;

}GSM_DriverConfigType;

//------------------------------------------------------------------------------

//extern constant array of GSM_ConfigType structure of the GSM groups
extern const GSM_DriverConfigType GSM_DriverConfigParam;

/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- UART call back functions --------------------------

/*
 * This function callback function for the GSM UART TX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_DriverTxCallBackFn(void);

/*
 * This function callback function for the GSM UART RX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_DriverRxCallBackFn(void);


//---------------------------- AT commands functions ------------------------------

/*
 * This function used to establish the communication with the GSM module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_AT(void);

/*
 * This function used to stop echoing the commands from GSM module
 * Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_StopEcho(void);

/*
 * This function used to reset all parameters to default
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_ResetDefaults(void);

/*
 * This function used to set the baud rate of GSM module
 *Inputs:NONE
 * Output:
 * 	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_FixBR(void);

/*
 * This function used to delete all sms messages on the GSM module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

/************************************************************************************/

GSM_DriverCheckType GSM_Driver_ATCMD_DeleteSMS(void);

/*
 * This function used to set the sms format of GSM module
 *Inputs:
 * 	- Mode		: the sms format (PDU, TEXT)
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SMSFormat(uint8_t Mode);

/*
 * This function used to set character set of GSM module
 * Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_CharSet(void);

/*
 * This function used to send an SMS from GSM module
 *Inputs:
 *	- PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SendSMS_MobNum(uint8_t* PhoneNum);

/*
 * This function used to send an SMS from GSM module
 *Inputs:
 *	- Msg		: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *	- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SendSMS_WriteMsg(uint8_t* Msg, uint8_t MsgLength);

/*
 * This function used to check if there is SMS message in index 1 
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_CheckReceivedSMS(void);

/*
 * This function used to read the SMS message in index 1  
 *Inputs:
 *	- MsgLengrh		: the length of the expected SMS message 
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_ReadSMS(uint8_t MsgLength);

/********************************************************************************************/
/*
 * This function used to set the APN for the GPRS Servies provider
 *Inputs:
 *	- ServiceProviderAPN 	: a pointer to the APN array 
 *	- ServiceProviderAPNLength	: the length of the APN
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_GPRS_APN(uint8_t* ServiceProviderAPN, uint8_t ServiceProviderAPNLength);

/*
 * This function used to ativate the GPRS
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_GPRS_Activate(void);

/*
 * This function used to attach the GSM to the GPRS network
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_GPRS_Attach(void);

/************************************************************************************************************/

/*
 * This function used to set the information for the TCP Servies provider
 *Inputs:
 *	- ServiceProviderAPN 	: a pointer to the APN array 
 *	- ServiceProviderAPNLength	: the length of the APN
 *	- ServiceProviderUserName 	: a pointer to the user name array 
 *	- ServiceProviderUserNameLength	: the length of the user name
 *	- ServiceProviderPassWord 	: a pointer to the password array 
 *	- ServiceProviderPassWordLength	: the length of the password
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_SP_Info(uint8_t* ServiceProviderAPN, uint8_t ServiceProviderAPNLength, uint8_t* ServiceProviderUserName, uint8_t ServiceProviderUserNameLength, uint8_t* ServiceProviderPassWord, uint8_t ServiceProviderPassWordLength);

/*
 * This function used to set the IP address and port for the  server and connect to it
 *Inputs:
 *	- IP_Address 	: a pointer to the IP address array 
 *	- IP_AddressLength	: the length of the IP address
 *	- PortNum 	: a pointer to the port number array 
 *	- PortNumLength	: the length of the port number
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_Connect(uint8_t* IP_Address, uint8_t IP_AddressLength, uint8_t* PortNum, uint8_t PortNumLength);

/*
 * This function used to set the TCP context
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_SetContext(void);

/*
 * This function used to disable the TCP Multiplexer
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_DisableMUX(void);

/*
 * This function used to set the mode of TCP
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_SetMode(void);

/*
 * This function used to set the TCP DNSIP mode
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_SetDNSIP(void);

/*
 * This function used to close the TCP connection
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_Close(void);

/*
 * This function used to deactivate the TCP connection
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_TCP_Deactivate(void);

/*
 * This function used to start sending an message to a server from GSM module
 *Inputs:NONE
 * Output:
 *       - an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_InitSendServerMsg(void);

/*
 * This function used to send an message to a server from GSM module
 *Inputs:
 *	- Msg		: a pointer the message + (Ctrl+Z) or (ascii: 26)
 *	- MsgLengrh	: the length of the message + 1 (Ctrl+Z)
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SendServerMsg(uint8_t* Msg, uint8_t MsgLength);


/********************************************************************************************/
/*
 * This function used to Stop any inprogress at command
 *Inputs:NONE 
 * Output:NONE
*/

void GSM_Driver_ATCMD_Stop(void);



/**************************************************************************************/
#endif