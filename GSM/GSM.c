/**************************************************************************************
*File name: GSM.c
*Auther : 
		-Abdelrhman Hosny
		-Amr Mohamed
*Date: 6/3/2018
*Description:
*	This file contains:
*		- implementation of functions used to control the M95 GSM module
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM.h"


#define INDEVELOPMODE 1		//COMMENT this line in run mode , it exist only to check if UART_StartSilentTransmission is called correctly 

//lengths of the received msg buffer and expected received msg buffer
#define MAXIMUM_RECEIVED_MSG_LENGTH		100
#define MAXIMUM_EXPECTED_MSG_LENGTH 	100


#define AT_COMMAND_LENGTH				3
#define RSTDEFAULT_COMMAND_LENGTH		5
#define STOPECHO_COMMAND_LENGTH			5
#define BRFIX_COMMAND_LENGTH			12
#define DELETESMS_COMMAND_LENGTH		11
#define SMSFORMAT_COMMAND_LENGTH		10
#define CHARSET_COMMAND_LENGTH			14
#define SETSMSMOBNUM_COMMAND_LENGTH		24
#define CHECKRECIEVEDSMS_COMMAND_LENGTH	10
#define READSMS_COMMAND_LENGTH			10

/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

/*
 * This function used to compare two strings
 * Inputs:
         - Str1		: A pointer to the first string
         - Str2		: A pointer to the second string
         - Length	: the length of the two strings
         
 * Output:
         - an indication of the success of the function
*/

static GSM_CheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint16_t Length);

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

static uint8_t ReceivedResponse[MAXIMUM_RECEIVED_MSG_LENGTH];//array to hold the recieved response
static uint16_t ResponseLength;//the length of the ReceivedResponse

static uint8_t ExpectedResponse[MAXIMUM_EXPECTED_MSG_LENGTH];//the expected response
static uint16_t ExpectedResponseLength;//the length of the expected Response


/***********************************************************************************
**********                      GSM functions' bodies                      ********
***********************************************************************************/

/*
 * This function used to establish the communication with the GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_AT(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission beginning
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[AT_COMMAND_LENGTH] = {'A','T','\r'};//the command to be sent
	
	//assign the expected response
	ExpectedResponse[0] = 'A';
	ExpectedResponse[1] = 'T';
	ExpectedResponse[2] = '\r';
	ExpectedResponse[3] = '\r';
	ExpectedResponse[4] = '\n';
	ExpectedResponse[5] = 'O';
	ExpectedResponse[6] = 'K';

	ResponseLength = 7;//assign the length of the Received Response
	ExpectedResponseLength = 7;//assign the length of the expectedResponse


	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, AT_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste excution time on assginning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, AT_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif

	
	return RetVar;
}

/*
 * This function used to reset all parameters to default
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_ATCommand_RstDefault(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[RSTDEFAULT_COMMAND_LENGTH] = {'A','T','&','F','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//the length of the expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, RSTDEFAULT_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, RSTDEFAULT_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;  
}

/*
 * This function used to stop echoing the commands from GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_StopEcho(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[STOPECHO_COMMAND_LENGTH] = {'A','T','E','0','\r'};//the command to be sent

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


	ResponseLength = 9;//assign the length of the Recieved Response
	ExpectedResponseLength = 9;//assign the length of the Expexted Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, STOPECHO_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, STOPECHO_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;  
}

/*
 * This function used to set the baud rate of GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_BRFix(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[BRFIX_COMMAND_LENGTH] = {'A','T','+','I','P','R','='};//the command to be sent

	//now we fix baudrate to 9600 , to change it we change the command length
	CommandToSend[7] = '9';
	CommandToSend[8] = '6';
	CommandToSend[9] = '0';
	CommandToSend[10] = '0';
	CommandToSend[11] = '\r';


	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, BRFIX_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, BRFIX_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;
}

/*
 * This function used to delete all sms messages on the GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_DeleteSMS(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[DELETESMS_COMMAND_LENGTH] = {'A','T','+','Q','M','G','D','A','=','6','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, DELETESMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, DELETESMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;  
}

/*
 * This function used to set the sms format of GSM module
 * Inputs:
         - Mode		: the sms format (PDU, TEXT)
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SMSFormat(uint8_t Mode)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[SMSFORMAT_COMMAND_LENGTH] = {'A','T','+','C','M','G','F','=','0','\r'};//the command to be sent

	//set the sms format
	if(Mode == TEXT)
	{
		CommandToSend[8] = '1';
	}
	else{;/*MISRA*/}

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, SMSFORMAT_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, SMSFORMAT_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;     
}

/*
 * This function used to set character set of GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_ATCommand_CharSet(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[CHARSET_COMMAND_LENGTH] = {'A','T','+','C','S','C','S','=','"','G','S','M','"','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CHARSET_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, CHARSET_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;   
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SetSMSMobNum(uint8_t* PhoneNum)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[SETSMSMOBNUM_COMMAND_LENGTH] = {'A','T','+','C','M','G','S','=','"','+','2'};//the command to be sent

	//concatinate the phone number with the command
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

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, SETSMSMOBNUM_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, SETSMSMOBNUM_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;   
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SetSMSWriteMsg(uint8_t* Msg, uint8_t MsgLength)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';
	ExpectedResponse[6] = 'S';

	ResponseLength = 7;//assign the length of the Recieved Response
	ExpectedResponseLength = 7;//the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command with the message sent by calling the function
	UART_Check = UART_StartSilentTransmission(Msg, MsgLength, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type with the message sent by calling the function
	UART_StartSilentTransmission(Msg, MsgLength, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;
}

/*
 * This function used to check if there is SMS message in index 1 
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_CheckRecievedSMS(void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[CHECKRECIEVEDSMS_COMMAND_LENGTH] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ResponseLength = 6 ;//assign the length of the Recieved Response
	ExpectedResponseLength = 6;//the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CHECKRECIEVEDSMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, CHECKRECIEVEDSMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;
}

/*
 * This function used to read the SMS message in index 1  
 *Inputs:
 		- MsgLengrh	: the length of the expected SMS message 
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_ReadSMS(uint8_t MsgLength)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[READSMS_COMMAND_LENGTH] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ResponseLength = 65 + MsgLength;//assign the length of the Recieved Response
	ExpectedResponseLength = 6;//assign the length of the Expected Response

	#ifdef INDEVELOPMODE
	//we are in develop mode
	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, READSMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);

	//if the the start of transmission was successful
	if(UART_Check == UART_OK)
	{
		//transmission was successful
		RetVar = GSM_OK;
	}
	else
	{
		/*Dont take a procedure because RetVar already initialized by GSM_NOK
		so there's no need to waste execution time on assigning it again*/
	}


	#else
	//we are in run mode
	//start the transmission without checking of its return type
	UART_StartSilentTransmission(CommandToSend, READSMS_COMMAND_LENGTH, ConfigPtr->UartChannelId);
	//transmission done successfully
	RetVar = GSM_OK;

	#endif
		
	return RetVar;
}

/***********************************************************************************
**********					GSM UART call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM UART Tx it is called when the transmissin of the command is done  
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Tx_CallBackFn(void)
{	
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the reciption begin
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the command

	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//start recieving the response of the command 
	UART_Check = UART_StartSilentReception(ReceivedResponse, ResponseLength, ConfigPtr->UartChannelId);

	//if the reciption start didn't work 
	if (UART_Check == UART_NOK)
	{
		ResponseLength = 0;//################################################################################
		//call the manager call back function with start silant reciption error
		ConfigPtr->GSM_CallBackFnPtr(GSM_Check, ReceivedResponse, ResponseLength); 
	}
	else
	{
		/*UART_Check now = UART_OK , so we don't have to take any procedure , we just wait for 
		the reception to complete to enter the RX call back function  */
	}
}

/*
 * This function callback function for the GSM UART Rx it is called when the reciption of the command response is done
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Rx_CallBackFn(void)
{
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the reciption begin

	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//compare the recieved response with the expected response 
	GSM_Check = StrComp(ReceivedResponse, ExpectedResponse, ExpectedResponseLength);

	//call the manager call back function with the state of the command success
	ConfigPtr->GSM_CallBackFnPtr(GSM_Check, ReceivedResponse, ResponseLength); 
}

/***********************************************************************************
**********							Helper functions						********
***********************************************************************************/

/*
 * This function used to compare two strings
 * Inputs:
         - Str1		: A pointer to the first string
         - Str2		: A pointer to the second string
         - Length	: the length of the two strings
         
 * Output:
         - an indication of the success of the function
*/
static GSM_CheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint16_t Length)
{
	//variable declaration
	uint16_t i; //loop index
	GSM_CheckType RetVar = GSM_OK;// variable to indicate the success of the function

	//String compare loop
	for(i = 0; (i < Length) && (RetVar == GSM_OK); i++)
	{
		//compatre the ith char of the two strings
		if(Str1[i] != Str2[i])
		{
			//the ith chars of the two strings don't match
			RetVar = GSM_NOK;
		}
		else
		{
			/*when STR1[i]==STR2[i] then the two responses are equal so we continue to the next letter */
		}
	}

	return RetVar;
}

/************************************************************************************/