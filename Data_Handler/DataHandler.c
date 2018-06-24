/**************************************************************************************
*File name: DataHandler.c
*Auther : 
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 13/6/2018
*Description:
*	This file contains:
*		- implementation of functions used to Handle the data came from GSM
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "DataHandler.h"

/**************************************************************************************/
/**********************			Define MACROS		***********************************/
/**************************************************************************************/
#define EXPIRE_TIME					15000U
#define LOC_FRAME_LENGTH			25U

#define T_EXP						(EXPIRE_TIME/DH_CYCLIC_TIME)

/*States MACROS*/
#define DH_IDLE_STATE				0U
#define DH_SEND_WARNING_STATE		1U
#define DH_SEND_SERVER_STATE		2U
#define DH_RECEIVE_SMS_STATE		3U
#define DH_ERROR_STATE				4U

#define DH_CALL_SERVICE				0U
#define DH_WAIT_FOR_SUCCESS			1U

/**************************************************************************************/
/**********************		Global Variables		***********************************/
/**************************************************************************************/

/*buffer to hold the data came from GSM manager*/
static uint8_t* ReceivedSMSBuffer;
/*pointer to hold the address of the structure that contains the location information*/
static Location* GPS_LocationBufferPtr;
/*array that holds the format of location required by mobile application*/
static uint8_t LocationFrame[LOC_FRAME_LENGTH];


/*start Send warning message operation flag*/
static uint8_t DH_StartSendSMSFlag;
/*start send Location to server operation flag*/
static uint8_t DH_StartSendServerFlag;

/*flag to indicate the state of the receive SMS*/
static uint8_t DH_StateReceiveFlag;
/*flag to indicate the state of the Operation of Manager for send server and send SMS*/
static uint8_t DH_StateSuccessFlag;

/*global variable to hold the state of ManageOnGoing operation*/
static uint8_t DH_ManageState;
/*global variable to hold state of helper function*/
static uint8_t DH_HelperState;

/*global variable to hold maximum available response time*/
static uint32_t MaxResponseTimeCounter;




/***********************************************************************************
**********                 DataHandler functions' ProtoTypes                ********
***********************************************************************************/

static DataHandlerCheckType SetWarningMessage(void);

static void ParseLocation(void);

static DataHandlerCheckType SetLocationToServer(void);

/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings       
 * Output:
 *		- an indication of the success of the function
*/

static DataHandlerCheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length);



/***********************************************************************************
**********                   DataHandler functions' bodies                  ********
***********************************************************************************/

/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void DH_Init(void)
{
	DH_StartSendSMSFlag = 0;
	DH_StartSendServerFlag = 0;

	DH_StateReceiveFlag = 0;
	DH_StateSuccessFlag = 0;	

	DH_ManageState = 0;
}

/*
 * This function is used to send a warning SMS 
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

DataHandlerCheckType DH_SendWarningSMS(void)
{
	DataHandlerCheckType RetVar = DATA_HANDLER_BSY;

	if (DH_StartSendSMSFlag == 0)
	{
		DH_StartSendSMSFlag = 1;
		RetVar = DATA_HANDLER_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}

/*
 * This function is used to send the location to the server 
 *Inputs:
 *		- GPS_Location : apointer to a structure carring the GPS Location
 * Output:
 *		- an indication of the success of the function
*/

DataHandlerCheckType DH_SendLocationToServer(Location* GPS_Location)
{
	DataHandlerCheckType RetVar = DATA_HANDLER_BSY;

	if (DH_StartSendServerFlag == 0)
	{
		GPS_LocationBufferPtr = GPS_Location;

		DH_StartSendServerFlag = 1;
		RetVar = DATA_HANDLER_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}

/*
 * This function is a FSM to manage the on going operations of the DataHandler module
 *Inputs:NONE
 * Output:NONE
*/

void DH_ManageOngoingOperation(void)
{
	DataHandlerCheckType StringCheck = DATA_HANDLER_OK;
	DataHandlerCheckType Check = DATA_HANDLER_BSY;// variable to indicate the success of the function

	const DataHandlerConfigType* ConfigPtr = &DataHandlerConfigParam;

	switch(DH_ManageState)
	{
		case DH_IDLE_STATE:
		{
			if(DH_StartSendSMSFlag == 1)
			{
				DH_ManageState = DH_SEND_WARNING_STATE;
			}
			else if(DH_StartSendServerFlag == 1)
			{
				DH_ManageState = DH_SEND_SERVER_STATE;
			}
			else if(DH_StateReceiveFlag == 1)
			{
				DH_ManageState = DH_RECEIVE_SMS_STATE;
			}
			else
			{
				/*there is no other flag to be checked*/
			}	
			break;
		}

		case DH_SEND_WARNING_STATE:
		{
			Check = SetWarningMessage();
			if(Check == DATA_HANDLER_OK)
			{
				//change the state to the idle state
                DH_ManageState = DH_IDLE_STATE;
				//reset the flag 
                DH_StartSendSMSFlag = 0;
                //reset the state
                DH_HelperState = DH_CALL_SERVICE;

                ConfigPtr -> DH_SendWarningCallBackFnPtr();
			}
			//if the function wasn't executed successfully
			else if(Check == DATA_HANDLER_NOK)
			{
				//change the state to the error state
				DH_ManageState = DH_ERROR_STATE;
				//reset the flag 
                DH_StartSendSMSFlag = 0;
                //reset the state
                DH_HelperState = DH_CALL_SERVICE; 
			}
			else{/*the Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case DH_SEND_SERVER_STATE:
		{
			ParseLocation();

			Check = SetLocationToServer();

			if(Check == DATA_HANDLER_OK)
			{
				//change the state to the idle state
                DH_ManageState = DH_IDLE_STATE;
				//reset the flag 
                DH_StartSendServerFlag = 0;
                //reset the state
                DH_HelperState = DH_CALL_SERVICE;

                ConfigPtr -> DH_SendLocationToServerCallBackFnPtr();
			}
			//if the function wasn't executed successfully
			else if(Check == DATA_HANDLER_NOK)
			{
				//change the state to the error state
				DH_ManageState = DH_ERROR_STATE;
				//reset the flag 
                DH_StartSendServerFlag = 0;
                //reset the state
                DH_HelperState = DH_CALL_SERVICE; 
			}
			else{/*the Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case DH_RECEIVE_SMS_STATE:
		{
			DH_StateReceiveFlag = 0;
			
			DH_ManageState = DH_IDLE_STATE;

			StringCheck = StrComp(ReceivedSMSBuffer, ConfigPtr -> ExpectedMsgPtr, ConfigPtr -> ExpectedMsgLength);

			if(StringCheck == DATA_HANDLER_OK)
			{
				ConfigPtr -> DH_ReceiveCMDCallBackFnPtr();
			}
			else{/*no action is needed if the sms didn't match the CMD*/}

			break;
		}

		case DH_ERROR_STATE:
		{
			//call the callback function
			ConfigPtr->DH_ErrorCallBackFn(DH_ERROR_ID);

			//change the state to the idle state
            DH_ManageState = DH_IDLE_STATE;

			break;
		}

		default:
		{
			break;
		}
	}
}





/***********************************************************************************
**********					DataHandler call back functions					********
***********************************************************************************/

/*
 * This function callback function from the GSM Manager Receive state it is called when it receives SMS
 * Inputs: 
 *			- SMS_Buffer : a pointer to the received SMS
 * Output:NONE
*/

void RecievedSMSCallBackFn(uint8_t* SMS_Buffer)
{
	DH_StateReceiveFlag = 1;
	ReceivedSMSBuffer = SMS_Buffer;
}

/*
 * This function callback function from the GSM Manager Send SMS state it is called when it finishes sending SMS
 * Inputs:NONE
 * Output:NONE
*/

void SendWarningCallBackFn(void)
{
	DH_StateSuccessFlag = 1;
}

/*
 * This function callback function from the GSM Manager Send Server MSG state it is called when it finishes sending Server MSG
 * Inputs:NONE
 * Output:NONE
*/

void SendServerCallBackFn(void)
{
	DH_StateSuccessFlag = 1;
}


/***********************************************************************************
**********							Helper functions						********
***********************************************************************************/

/*
 * This function is used to request sending a warning SMS 
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static DataHandlerCheckType SetWarningMessage(void)
{
	DataHandlerCheckType RetVar = DATA_HANDLER_BSY;
	const DataHandlerConfigType* ConfigPtr = &DataHandlerConfigParam;

	switch(DH_HelperState)
	{
		case DH_CALL_SERVICE:
		{
			GSM_Send_SMS(ConfigPtr->WarningMsgPtr, ConfigPtr->WarningMsgLength, ConfigPtr->PhoneNumPtr);

			DH_HelperState = DH_WAIT_FOR_SUCCESS;
			MaxResponseTimeCounter = 0;

			break;
		}

		case DH_WAIT_FOR_SUCCESS:
		{
			//check the state of the command success
			if (DH_StateSuccessFlag == 1)
			{
				DH_StateSuccessFlag = 0;

				RetVar = DATA_HANDLER_OK;
			}
			else
			{
				if(MaxResponseTimeCounter < T_EXP)
				{
					MaxResponseTimeCounter++;
				}
				else
				{
					MaxResponseTimeCounter = 0;
					RetVar = DATA_HANDLER_NOK; 
				}
			}

			break;
		}

		default:
		{
			RetVar = DATA_HANDLER_NOK; 
			break;
		}
	}

	return RetVar;
}

/*
 * This function is used to request sending the location to the server
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static DataHandlerCheckType SetLocationToServer(void)
{
	DataHandlerCheckType RetVar = DATA_HANDLER_BSY;
	const DataHandlerConfigType* ConfigPtr = &DataHandlerConfigParam;

	switch(DH_HelperState)
	{
		case DH_CALL_SERVICE:
		{
			GSM_SendServerMsg(ConfigPtr->ServerIP, ConfigPtr->ServerIPLength, ConfigPtr->PortNum, ConfigPtr->PortNumLength, LocationFrame, LOC_FRAME_LENGTH);

			DH_HelperState = DH_WAIT_FOR_SUCCESS;
			MaxResponseTimeCounter = 0;

			break;
		}

		case DH_WAIT_FOR_SUCCESS:
		{
			//check the state of the command success
			if (DH_StateSuccessFlag == 1)
			{
				DH_StateSuccessFlag = 0;

				RetVar = DATA_HANDLER_OK;
			}
			else
			{
				if(MaxResponseTimeCounter < T_EXP)
				{
					MaxResponseTimeCounter++;
				}
				else
				{
					MaxResponseTimeCounter = 0;
					RetVar = DATA_HANDLER_NOK; 
				}
			}

			break;
		}

		default:
		{
			RetVar = DATA_HANDLER_NOK; 
			break;
		}
	}

	return RetVar;
}

/*
 * This function is form the location frame before sending it to the server
 *Inputs:NONE
 * Output:NONE
*/
static void ParseLocation(void)
{
	uint8_t LoopIndex;

	for(LoopIndex=0; LoopIndex<2; LoopIndex++)
	{
		LocationFrame[LoopIndex] = GPS_LocationBufferPtr->Longitude[LoopIndex];
	}

	LocationFrame[2] = '$';

	for(LoopIndex=3; LoopIndex<10; LoopIndex++)
	{
		LocationFrame[LoopIndex] = GPS_LocationBufferPtr->Longitude[LoopIndex-1];
	}

	LocationFrame[10] = '$';

	LocationFrame[11] = GPS_LocationBufferPtr->LongitudeDir;

	LocationFrame[12] = '@';

	for(LoopIndex=0; LoopIndex<2; LoopIndex++)
	{
		LocationFrame[LoopIndex+13] = GPS_LocationBufferPtr->Latitude[LoopIndex];
	}

	LocationFrame[15] = '$';

	for(LoopIndex=3; LoopIndex<10; LoopIndex++)
	{
		LocationFrame[LoopIndex+13] = GPS_LocationBufferPtr->Latitude[LoopIndex-1];
	}

	LocationFrame[23] = '$';

	LocationFrame[24] = GPS_LocationBufferPtr->LatitudeDir;
}

/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings       
 * Output:
 *		- an indication of the success of the function
*/
static DataHandlerCheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length)
{
	//variable declaration
	uint8_t Index; //loop index
	 DataHandlerCheckType RetVar = DATA_HANDLER_OK;// variable to indicate the success of the function

	//String compare loop
	for(Index = 0; ((Index < Length) && (RetVar == DATA_HANDLER_OK)); Index++)
	{
		//compare the ith char of the two strings
		if(Str1[Index] != Str2[Index])
		{
			//the ith chars of the two strings don't match
			RetVar = DATA_HANDLER_NOK;
		}
		else{/*the loop shall continue till till the last character in the 2 strings or finding a miss match between them*/}
	}

	return RetVar;
}

