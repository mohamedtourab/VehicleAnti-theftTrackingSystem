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

/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

static GSM_CheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint16_t Length);

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

volatile uint8_t RecievedResponse[100];//array to hold the recieved response
volatile uint16_t ResponseLength;//the length of the RecievedResponse

volatile static uint8_t ExpectedResponse[100];//the expected response
volatile static uint16_t ExpectedResponseLength;//the length of the expected Response


/***********************************************************************************
**********					GSM UART call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM UART Tx it is called when the transmissin of the command is done  
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Tx_CallBackFn (void)
{	
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the reciption begin
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the command

	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//start recieving the response of the command 
	UART_Check = UART_StartSilentReception(RecievedResponse, ResponseLength, ConfigPtr->UartChannelId);

	//if the reciption start didn't work 
	if (UART_Check == UART_NOK)
	{
		ResponseLength = 0;
		//call the manager call back function with start silant reciption error
		ConfigPtr->GSM_CallBackFnPtr(GSM_Check, RecievedResponse, ResponseLength); 
	}
	else{;/*MISRA*/}
}

/*
 * This function callback function for the GSM UART Rx it is called when the reciption of the command response is done
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Rx_CallBackFn (void)
{
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the reciption begin

	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//compare the recieved response with the expected response 
	GSM_Check = StrComp(RecievedResponse, ExpectedResponse, ExpectedResponseLength);

	//call the manager call back function with the state of the command success
	ConfigPtr->GSM_CallBackFnPtr(GSM_Check, RecievedResponse, ResponseLength); 
}

/***********************************************************************************
**********                      GSM functions' bodies                      ********
***********************************************************************************/

//-------------------------------- Hardware --------------------------------------

/*
 * This function used to power on the GSM module
 * Inputs:NONE
 * Output:
		- an indication of the success of the function
*/

GSM_CheckType GSM_PowerOn (void)
{
    //variable declaration
    GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the function
    GPIO_CheckType GPIO_Check = GPIO_NOK;// variable to indicate the success of the GPIO function
        
    const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//-----------------hardware power on ----------------------------
	//Power Key sequance
	GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);//PWRK HIGH
	if(GPIO_Check == GPIO_OK)
	{
		Delay_ms(30);
		GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);//PWRK LOW
		if(GPIO_Check == GPIO_OK)
		{
			Delay_ms(1100);
			GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);//PWRK HIGH
			if(GPIO_Check == GPIO_OK)
			{
				//RTS LOW for software handling
				GPIO_Check = GPIO_Write(GSM_ConfigPtr->RTSGroupId, GSM_ConfigPtr->RTSPinMask, LOW);
				if(GPIO_Check == GPIO_OK)
				{
					//set DTR HIGH for the sleep mode
					GPIO_Check = GPIO_Write(GSM_ConfigPtr->DTRGroupId, GSM_ConfigPtr->DTRPinMask, HIGH);
					if(GPIO_Check == GPIO_OK)
					{
						RetVar = GSM_OK;
					}
					else
					{
						RetVar = GSM_NOK;
					}
				}
				else
				{
					RetVar = GSM_NOK;
				}
			}
			else
			{
				RetVar = GSM_NOK;
			}
		}
		else
		{
			RetVar = GSM_NOK;
		}
	}
	else
	{
		RetVar = GSM_NOK;
	}	
      
	return RetVar;
}

/*
 * This function used to WakeUp the GSM module from sleep mode
 * Inputs:NONOE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_WakeUp (void)
{
    //variable declaration
    GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the reset
    GPIO_CheckType GPIO_Check = GPIO_NOK;// variable to indicate the success of the GPIO function
    const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//set DTR LOW to wakeup the GSM module
	GPIO_Check = GPIO_Write(GSM_ConfigPtr->DTRGroupId, GSM_ConfigPtr->DTRPinMask, LOW);

	if(GPIO_Check == GPIO_OK)
	{
		RetVar = GSM_OK;
	}
	else
	{
		RetVar = GSM_NOK;
	}

	return RetVar;
}

/*
 * This function used to put the GSM module in sleep mode
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/




GSM_CheckType GSM_Sleep (void)
{
    //variable declaration
    GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the reset
    GPIO_CheckType GPIO_Check = GPIO_NOK;// variable to indicate the success of the GPIO function
    const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
            
	//set DTR HIGH to put the GSM module in sleep mode
	GPIO_Check = GPIO_Write(GSM_ConfigPtr->DTRGroupId, GSM_ConfigPtr->DTRPinMask, HIGH);
                
	if(GPIO_Check == GPIO_OK)
	{
		RetVar = GSM_OK;
	}
	else
	{
		RetVar = GSM_NOK;
	}
        
	//return RetVar;
}



//------------------------------------- Software -----------------------------------

/*
 * This function used to establish the communication with the GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_AT (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begining
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[3] = {'A','T','\r'};//the command to be sent
	
	//assign the expected response
	ExpectedResponse[0] = 'A';
	ExpectedResponse[1] = 'T';
	ExpectedResponse[2] = '\r';
	ExpectedResponse[3] = '\r';
	ExpectedResponse[4] = '\n';
	ExpectedResponse[5] = 'O';
	ExpectedResponse[6] = 'K';

	ResponseLength = 7;//assign the length of the Recieved Response
	ExpectedResponseLength = 7;//assign the length of the expectedResponse


	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 3, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
	
	return RetVar;
}


      /*
 * This function used to reset all parameters to default
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_ATCommand_RstDefault (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[5] = {'A','T','&','F','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//the length of the expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 5, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;  
}

/*
 * This function used to stop echoing the commands from GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_StopEcho (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[5] = {'A','T','E','0','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = 'A';
	ExpectedResponse[1] = 'T';
	ExpectedResponse[2] = 'E';
	ExpectedResponse[3]	= 'O';
	ExpectedResponse[4] = '\r';
	ExpectedResponse[5]	= '\r';
	ExpectedResponse[6] = '\n';
	ExpectedResponse[7]	= 'O';
	ExpectedResponse[8] = 'K';


	ResponseLength = 9;//assign the length of the Recieved Response
	ExpectedResponseLength = 9;//assign the length of the Expexted Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 5, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;  
}



/*
 * This function used to set the baud rate of GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_BRFix (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
	uint32_t UART_BR;//variable to hold the baud rate from the uart

	uint8_t CommandLength;//the length of the sent command
	uint8_t CommandToSend[14] = {'A','T','+','I','P','R','='};//the command to be sent

//------------------------------------------------------------------------------------------------
 	const UART_ConfigType* UART_ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType
	UART_ConfigPtr = UART_ConfigParam;

	//get the baud rate of the uart
	UART_BR = UART_ConfigPtr[ConfigPtr->UartChannelId].BaudRate;
//------------------------------------------------------------------------------------------------

	//concatinate the baudrate with the command and calculate the command length  
	if(UART_BR == BR9600)
	{
		CommandToSend[7] = '9';
		CommandToSend[8] = '6';
		CommandToSend[9] = '0';
		CommandToSend[10] = '0';
		CommandToSend[11] = '\r';

		CommandLength = 12;
	}
	else if(UART_BR == BR19200)
	{
		CommandToSend[7] = '1';
		CommandToSend[8] = '9';
		CommandToSend[9] = '2';
		CommandToSend[10] = '0';
		CommandToSend[11] = '0';
		CommandToSend[12] = '\r';

		CommandLength = 13;
	}
	else if(UART_BR == BR38400)
	{
		CommandToSend[7] = '3';
		CommandToSend[8] = '8';
		CommandToSend[9] = '4';
		CommandToSend[10] = '0';
		CommandToSend[11] = '0';
		CommandToSend[12] = '\r';

		CommandLength = 13;
	}
	else if(UART_BR == BR115200)
	{
		CommandToSend[7] = '1';
		CommandToSend[8] = '1';
		CommandToSend[9] = '5';
		CommandToSend[10] = '2';
		CommandToSend[11] = '0';
		CommandToSend[12] = '0';
		CommandToSend[13] = '\r';

		CommandLength = 14;
	}
	else{;/*MISRA*/}


	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, CommandLength, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;
}



/*
 * This function used to delete all sms messages on the GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_DeleteSMS (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[11] = {'A','T','+','Q','M','G','D','A','=','6','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 11, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;  
}

/*
 * This function used to set the sms format of GSM module
 * Inputs:
         - Mode		: the sms format (PDU, TEXT)
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SMSFormat (uint8_t Mode)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[10] = {'A','T','+','C','M','G','F','=','0','\r'};//the command to be sent

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

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 10, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;     
}

/*
 * This function used to set character set of GSM module
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_ATCommand_CharSet (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[14] = {'A','T','+','C','S','C','S','=','"','G','S','M','"','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = 'O';
	ExpectedResponse[3]	= 'K';

	ResponseLength = 4;//assign the length of the Recieved Response
	ExpectedResponseLength = 4;//assign the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 14, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;   
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SetSMSMobNum (uint8_t* PhoneNum)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[24] = {'A','T','+','C','M','G','S','=','"','+','2'};//the command to be sent

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

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 24, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
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

GSM_CheckType GSM_ATCommand_SetSMSWriteMsg (uint8_t* Msg, uint8_t MsgLength)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '\r';
	ExpectedResponse[3] = '\n';
	ExpectedResponse[4] = '+';
	ExpectedResponse[5] = 'C';
	ExpectedResponse[6] = 'M';
	ExpectedResponse[7] = 'G';
	ExpectedResponse[8] = 'S';

	ResponseLength = 9;//assign the length of the Recieved Response
	ExpectedResponseLength = 9;//the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(Msg, MsgLength, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;
}

/*
 * This function used to check if there is SMS message in index 1 
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_CheckRecievedSMS (void)
{
	//declarations
	GSM_CheckType RetVar = GSM_NOK;// variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_NOK;// variable to indicate the success of the transmission begin
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType

	uint8_t CommandToSend[10] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ResponseLength = 6 ;//assign the length of the Recieved Response
	ExpectedResponseLength = 6;//the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 10, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
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

	uint8_t CommandToSend[10] = {'A','T','+','C','M','G','R','=','1','\r'};//the command to be sent

	//assign the expected response 
	ExpectedResponse[0] = '\r';
	ExpectedResponse[1] = '\n';
	ExpectedResponse[2] = '+';
	ExpectedResponse[3] = 'C';
	ExpectedResponse[4] = 'M';
	ExpectedResponse[5] = 'G';

	ResponseLength = 65 + MsgLength;//assign the length of the Recieved Response
	ExpectedResponseLength = 6;//assign the length of the Expected Response

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, 10, ConfigPtr->UartChannelId);

	//if the the start of transmission was successfull
	if(UART_Check == UART_OK)
	{
		//transmission was successfull
		RetVar = GSM_OK;
	}
	else{;/*MISRA*/}
		
	return RetVar;
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
                else{;/*MISRA*/}
        }
        return RetVar;
}
