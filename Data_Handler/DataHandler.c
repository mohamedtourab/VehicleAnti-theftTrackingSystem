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
/**********************		Global Variables		***********************************/
/**************************************************************************************/

/*buffer to hold the data came from GSM manager*/
static uint8_t* DataHandlerBuffer;

/*flag to indicate the state of the received data*/
static uint8_t DataHandlerFlag;

/***********************************************************************************
**********                 DataHandler functions' ProtoTypes                ********
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

static DataHandlerCheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length);



/***********************************************************************************
**********                   DataHandler functions' bodies                  ********
***********************************************************************************/

/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void DataHandlerInit(void)
{
	DataHandlerFlag = 0;
}

/*
 * This function is a FSM to manage the on going operations of the DataHandler module
 *Inputs:NONE
 * Output:NONE
*/

void DataHandlerManageOngoingOperation(void)
{
	DataHandlerCheckType StringCheck = DATA_HANDLER_OK;// variable to indicate the success of the function

	const DataHandlerConfigType* ConfigPtr = &DataHandlerConfigParam;

	if(DataHandlerFlag == 1)
	{
		DataHandlerFlag = 0;
		StringCheck = StrComp(DataHandlerBuffer, ConfigPtr -> DataHandlerCMD, DATA_HANDLER_CMD_LENGTH);

		if(StringCheck == DATA_HANDLER_OK)
		{
			ConfigPtr -> DataHandlerCallBackFnPtr();
		}
		else{/*no action is needed if the sms didn't match the CMD*/}
	}
}





/***********************************************************************************
**********					DataHandler call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM Manager Receive state it is called when it receives SMS
 * Inputs: 
 *			- SMS_Buffer : a pointer to teh received SMS
 * Output:NONE
*/

void RecievedSMSCallBackFn(uint8_t* SMS_Buffer)
{
	DataHandlerFlag = 1;
	DataHandlerBuffer = SMS_Buffer;
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

