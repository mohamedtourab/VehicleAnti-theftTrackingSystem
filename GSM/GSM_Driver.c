/**************************************************************************************
*File name: GSM_Driver.c
*Author:
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 6/3/2018
*Description:
*	This file contains:
*		- implementation of functions used to control the M95 GSM module
*Microcontroller: STM32F407VG
***************************************************************************************/

#include "GSM_Driver.h"


/***********************************************************************************
**********							Macros									********
***********************************************************************************/
/*****Development mode Macro*****/
//#define DEVELOPMENT_MODE_ENABLE
/********************************/

/*AT Commands lengths Response lengths Macros*/
#define AT_CMD_LENGTH					3U
#define STOP_ECHO_CMD_LENGTH			5U	
#define RESET_DEFAULTS_CMD_LENGTH		5U
#define FIX_BR_CMD_LENGTH				12U
#define DELETE_SMS_CMD_LENGTH			11U
#define SMS_FORMAT_CMD_LENGTH			10U
#define CHAR_SET_CMD_LENGTH				14U
#define SEND_SMS_MOBNUM_CMD_LENGTH		24U
#define CHECK_RECEIVED_SMS_CMD_LENGTH	10U
#define READ_SMS_CMD_LENGTH				10U


/*AT Commands Response lengths Macros*/
#define AT_RRES_LENGHTH					7U
#define STOP_ECHO_RRES_LENGTH			9U
#define RESET_DEFAULTS_RRES_LENGTH		4U
#define FIX_BR_RRES_LENGTH				4U
#define DELETE_SMS_RRES_LENGTH			4U
#define SMS_FORMAT_RRES_LENGTH			4U
#define CHAR_SET_RRES_LENGTH			4U
#define SEND_SMS_MOBNUM_RRES_LENGTH		4U
#define SEND_SMS_WRITE_MSG_RRES_LENGTH	7U
#define CHECK_RECEIVED_SMS_RRES_LENGTH	6U
#define READ_SMS_RRES_LENGTH			65U

/*AT Commands expected Response lengths Response lengths Macros*/
#define AT_ERES_LENGHTH					7U
#define STOP_ECHO_ERES_LENGTH			9U
#define RESET_DEFAULTS_ERES_LENGTH		4U
#define FIX_BR_ERES_LENGTH				4U
#define DELETE_SMS_ERES_LENGTH			4U
#define SMS_FORMAT_ERES_LENGTH			4U
#define CHAR_SET_ERES_LENGTH			4U
#define SEND_SMS_MOBNUM_ERES_LENGTH		4U
#define SEND_SMS_WRITE_MSG_ERES_LENGTH	7U
#define CHECK_RECEIVED_SMS_ERES_LENGTH	6U
#define READ_SMS_ERES_LENGTH			6U


/******************************************************/
#define MAX_RESPONSE_LENGTH				100U
#define MAX_EXPECTED_RESPONSE_LENGTH	100U
/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings        
 * Output:
 *		- an indication of the success of the function
*/

static GSM_DriverCheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length);

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

static uint8_t ReceivedResponse[MAX_RESPONSE_LENGTH];//array to hold the received response
static uint8_t ReceivedResponseLength;//the length of the RecievedResponse

static uint8_t ExpectedResponse[MAX_EXPECTED_RESPONSE_LENGTH];//the expected response
static uint8_t ExpectedResponseLength;//the length of the expected Response

/***********************************************************************************
**********				GSM  Driver functions' bodies                      ********
***********************************************************************************/

/*
 * This function used to establish the communication with the GSM module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
GSM_DriverCheckType GSM_Driver_ATCMD_AT(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[AT_CMD_LENGTH] = {'A','T','\r'};//the command to be sent

	//assign the expected response
	ExpectedResponse[0] = 'A';
	ExpectedResponse[1] = 'T';
	ExpectedResponse[2] = '\r';
	ExpectedResponse[3] = '\r';
	ExpectedResponse[4] = '\n';
	ExpectedResponse[5] = 'O';
	ExpectedResponse[6] = 'K';

	ReceivedResponseLength = AT_RRES_LENGHTH;//assign the length of the Received Response
	ExpectedResponseLength = AT_ERES_LENGHTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, AT_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to stop echoing the commands from GSM module
 * Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
GSM_DriverCheckType GSM_Driver_ATCMD_StopEcho(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[STOP_ECHO_CMD_LENGTH] = {'A','T','E','0','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = 'A';
	ExpectedResponse[1] = 'T';
	ExpectedResponse[2] = 'E';
	ExpectedResponse[3]	= '0';
	ExpectedResponse[4] = '\r';
	ExpectedResponse[5]	= '\r';
	ExpectedResponse[6] = '\n';
	ExpectedResponse[7]	= 'O';
	ExpectedResponse[8] = 'K';

	ReceivedResponseLength = STOP_ECHO_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = STOP_ECHO_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, STOP_ECHO_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to reset all parameters to default
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_ResetDefaults(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[RESET_DEFAULTS_CMD_LENGTH] = {'A','T','&','F','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ReceivedResponseLength = RESET_DEFAULTS_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = RESET_DEFAULTS_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, RESET_DEFAULTS_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to set the baud rate of GSM module
 *Inputs:NONE
 * Output:
 * 	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_FixBR(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[FIX_BR_CMD_LENGTH] = {'A','T','+','I','P','R','=','9','6','0','0','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ReceivedResponseLength = FIX_BR_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = FIX_BR_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, FIX_BR_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to delete all sms messages on the GSM module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_DeleteSMS(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[DELETE_SMS_CMD_LENGTH] = {'A','T','+','Q','M','G','D','A','=','6','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ReceivedResponseLength = DELETE_SMS_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = DELETE_SMS_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, DELETE_SMS_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;

}

/*
 * This function used to set the sms format of GSM module
 *Inputs:
 * 	- Mode		: the sms format (PDU, TEXT)
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SMSFormat(uint8_t Mode)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[SMS_FORMAT_CMD_LENGTH] = {'A','T','+','C','M','G','F','=','0','\r'};//the command to be sent

	if(Mode == TEXT_MODE)
	{
		CommandToSend[8] = '1';
	}
	else{;/*the mode byte is initialized to '0' so no action needed here */}

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ReceivedResponseLength = SMS_FORMAT_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = SMS_FORMAT_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, SMS_FORMAT_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}	

/*
 * This function used to set character set of GSM module
 * Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_CharSet(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[CHAR_SET_CMD_LENGTH] = {'A','T','+','C','S','C','S','=','"','G','S','M','"','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ReceivedResponseLength = CHAR_SET_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = CHAR_SET_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CHAR_SET_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
 *	- PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SendSMS_MobNum(uint8_t* PhoneNum)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[SEND_SMS_MOBNUM_CMD_LENGTH] = {'A','T','+','C','M','G','S','=','"','+','2'};//the command to be sent

	//concatenate the phone number with the command
	CommandToSend[11] = PhoneNum[0];
	CommandToSend[12] = PhoneNum[1];
	CommandToSend[13] = PhoneNum[2];
	CommandToSend[14] = PhoneNum[3];
	CommandToSend[15] = PhoneNum[4];
	CommandToSend[16] = PhoneNum[5];
	CommandToSend[17] = PhoneNum[6];
	CommandToSend[18] = PhoneNum[7];
	CommandToSend[19] = PhoneNum[8];
	CommandToSend[20] = PhoneNum[9];
	CommandToSend[21] = PhoneNum[10];

	CommandToSend[22] = '"';
	CommandToSend[23] = '\r';

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '>';
	ExpectedResponse[3]	= ' ';


	ReceivedResponseLength = SEND_SMS_MOBNUM_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = SEND_SMS_MOBNUM_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, SEND_SMS_MOBNUM_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
 *	- Msg		: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *	- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_SendSMS_WriteMsg(uint8_t* Msg, uint8_t MsgLength)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';
	ExpectedResponse[6] = 'S';

	ReceivedResponseLength = SEND_SMS_WRITE_MSG_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = SEND_SMS_WRITE_MSG_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(Msg, MsgLength, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to check if there is SMS message in index 1 
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_CheckReceivedSMS(void)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[CHECK_RECEIVED_SMS_CMD_LENGTH] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ReceivedResponseLength = CHECK_RECEIVED_SMS_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = CHECK_RECEIVED_SMS_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CHAR_SET_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 * This function used to read the SMS message in index 1  
 *Inputs:
 *	- MsgLengrh		: the length of the expected SMS message 
 * Output:
 *	- an indication of the success of the function
*/

GSM_DriverCheckType GSM_Driver_ATCMD_ReadSMS(uint8_t MsgLength)
{
	//declarations
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the transmission beginning
	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[READ_SMS_CMD_LENGTH] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ReceivedResponseLength = READ_SMS_RRES_LENGTH + MsgLength;//assign the length of the Received Response
	ExpectedResponseLength = READ_SMS_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CHAR_SET_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = GSM_DRIVER_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;	
}

/***********************************************************************************
**********				GSM Driver UART call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM driver UART TX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_DriverTxCallBackFn(void)
{
	UART_ChkType UART_Check = UART_OK;// variable to indicate the success of the reception begin
	GSM_DriverCheckType GSM_DriverCheck = GSM_DRIVER_OK;// variable to indicate the success of the command

	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	//start receiving the response of the command 
	UART_Check = UART_StartSilentReception(ReceivedResponse, ReceivedResponseLength, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the reception start didn't work 
	if (UART_Check == UART_NOK)
	{
		ReceivedResponseLength = 0;
		//call the manager call back function with start silent reception error
		ConfigPtr->GSM_DriverCallBackFnPtr(GSM_DriverCheck, ReceivedResponse,  ReceivedResponseLength); 
	}
	else{/*UART check is initialized to OK so no action is needed here*/}

	#endif
}

/*
 * This function callback function for the GSM driver UART RX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_DriverRxCallBackFn(void)
{
	GSM_DriverCheckType GSM_DriverCheck = GSM_DRIVER_NOK;// variable to indicate the success of the reception begin

	const GSM_DriverConfigType* ConfigPtr = &GSM_DriverConfigParam;//declare a pointer to structure of the GSM_ConfigType

	//compare the received response with the expected response 
	GSM_DriverCheck = StrComp(ReceivedResponse, ExpectedResponse, ExpectedResponseLength);

	//call the manager call back function with the state of the command success
	ConfigPtr->GSM_DriverCallBackFnPtr(GSM_DriverCheck, ReceivedResponse, ReceivedResponseLength); 

}

/***********************************************************************************
**********							Helper functions						********
***********************************************************************************/

/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings       
 * Output:
 *		- an indication of the success of the function
*/
static GSM_DriverCheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length)
{
	//variable declaration
	uint8_t Index; //loop index
	GSM_DriverCheckType RetVar = GSM_DRIVER_OK;// variable to indicate the success of the function

	//String compare loop
	for(Index = 0; ((Index < Length) && (RetVar == GSM_DRIVER_OK)); Index++)
	{
		//compare the ith char of the two strings
		if(Str1[Index] != Str2[Index])
		{
			//the ith chars of the two strings don't match
			RetVar = GSM_DRIVER_NOK;
		}
		else{/*the loop shall continue till till the last character in the 2 strings or finding a miss match between them*/}
	}

	return RetVar;
}