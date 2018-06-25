/**************************************************************************************
*File name: GSM.c
*Auther : 
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 18/3/2018
*Description:
*	This file contains:
*		- implementation of functions used to manage the M95 GSM module
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM.h"

#define TIME_BETWEEN_COMMANDS		50U
#define TIME_TILL_READY				15000U
#define EXPIRE_TIME					300000U

/*****Development mode Macro*****/
#define DEVELOPMENT_MODE_ENABLE
/********************************/

//the time delays used in this file
#define T30		(30U/GSM_CYCLIC_TIME)
#define T800	(800U/GSM_CYCLIC_TIME)
#define T1100	(1100U/GSM_CYCLIC_TIME)
#define T3000	(3000U/GSM_CYCLIC_TIME)
#define C120 	(80U/GSM_CYCLIC_TIME)

#define T_CMD	(TIME_BETWEEN_COMMANDS/GSM_CYCLIC_TIME)
#define T_RDY	(TIME_TILL_READY/GSM_CYCLIC_TIME)
#define T_EXP 	(EXPIRE_TIME/GSM_CYCLIC_TIME)



//the states of the manager function
#define MANGE_STATE_UNINIT 			0U
#define MANGE_STATE_IDLE 			1U
#define MANGE_STATE_START 			2U
#define MANGE_STATE_SWRESET 		3U
#define MANGE_STATE_HWRESET 		4U
#define MANGE_STATE_SEND_SMS 		5U
#define MANGE_STATE_RECEIVE_SMS 	6U
#define MANGE_STATE_SEND_SERVER_MSG	7U
#define MANGE_STATE_ERROR 			8U

//the states of the power on function
#define POWER_ON_STATE_HIGH_1		1U
#define POWER_ON_STATE_LOW			2U
#define POWER_ON_STATE_HIGH_2		3U
#define POWER_ON_START_DELAY		4U

//the states of the hardware reset function
#define HARDWARE_RESET_STATE_LOW_1		1U
#define HARDWARE_RESET_STATE_HIGH_1		2U
#define HARDWARE_RESET_STATE_LOW_2		3U
#define HARDWARE_RESET_STATE_HIGH_2		4U
#define HARDWARE_RESET_START_DELAY		5U

//the states of the Start communication function
#define START_BUSY_STATE				0U
#define START_STATE_AT_CMD				1U
#define START_STATE_STOP_ECHO_CMD		2U
#define START_STATE_FIX_BR_CMD			3U
#define START_STATE_DELETE_SMS_CMD		4U
#define START_STATE_GPRS_APN_CMD		5U
#define START_STATE_GPRS_ACTIVATE_CMD	6U
#define START_STATE_GPRS_ATTACH_CMD		7U
#define START_STATE_TCP_CONTEXT_CMD		8U
#define START_STATE_TCP_SP_INFO_CMD		9U
#define START_STATE_TCP_MUX_CMD			10U
#define START_STATE_TCP_MODE_CMD		11U
#define START_STATE_TCP_DNSIP_CMD		12U

#define START_NUMBER_OF_CMDS			12U


//the states of the send sms function
#define SEND_BUSY_STATE					0U
#define SEND_STATE_TEXT_MODE_CMD		1U
#define SEND_STATE_SMS_CHAR_SET_CMD		2U
#define SEND_STATE_MOB_NUM_CMD			3U
#define SEND_STATE_WRITE_MSG_CMD		4U
#define SEND_STATE_PDU_MODE_CMD			5U

#define SEND_NUMBER_OF_CMDS				5U

//the states of the receive sms function
#define RECEIVE_BUSY_STATE				0U
#define RECEIVE_STATE_TEXT_MODE_CMD		1U
#define RECEIVE_STATE_CHECK_CMD			2U
#define RECEIVE_STATE_READ_CMD			3U
#define RECEIVE_STATE_PDU_MODE_CMD		4U
#define RECEIVE_STATE_DELETE_SMS_CMD	5U

#define RECEIVE_NUMBER_OF_CMDS			5U

//the states of the send server message function
#define SERVER_SEND_BUSY_STATE				0U
#define SERVER_SEND_STATE_CONNECT_CMD		1U
#define SERVER_SEND_STATE_INIT_SEND_CMD		2U
#define SERVER_SEND_STATE_SEND_CMD 			3U
#define SERVER_SEND_STATE_CLOSE_CMD			4U
#define SERVER_SEND_STATE_DEACTIVATE_CMD	5U

#define SERVER_SEND_NUMBER_OF_CMDS			5U

//the states of the software reset function
#define SOFTWARE_RESET_STATE_CMD		1U
#define SOFTWARE_RESET_STATE 			2U

/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

/*
 * This function is a FSM to power on the GSM module
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType PowerOn(void);

/*
 * This function is a FSM to preform a hardware reset on the GSM module
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType HardWareReset(void);

/*
 * This function is a FSM to configure the GSM module with the necessary settings
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Start(void);

/*
 * This function is a FSM to preform a software reset on the GSM module 
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType SoftWareReset(void);

/*
 * This function is a FSM to read the SMS message in index 1  
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Read_Msg(void);

/*
 * This function is a FSM to send an SMS from GSM module
  * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Send_Msg(void);

/*
 * This function is a FSM to send aserver message from GSM module
  * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static GSM_CheckType Send_ServerMsg(void);

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/
//variables to hold the data from the gsm driver callback function
static uint8_t* DriverReceivedResponse;
static uint8_t DriverReceivedResponseLength;

//a variable to hold the SMS response length
static uint8_t SMSReceivedResponseLength;

//flags to hold the state of the at command called by the functions in this file whether it succeed or not or it is still in progress
static uint8_t CMD_DoneFlag;
static uint8_t CMD_SuccessFlag;

//flags for the functions called by the client
static uint8_t SoftWareRstFlag;
static uint8_t HardWareRstFlag;
static uint8_t StartFlag;
static uint8_t SendMsgFlag;
static uint8_t RecieveMsgFlag;
static uint8_t SendServerMsgFlag;

//variables to hold the data needed to send an SMS
static uint8_t* MsgToSend;
static uint8_t  MsgToSendLength;
static uint8_t* MsgToSendPhoneNum;

//variables to hold the data needed to send a server message
static uint8_t* ServerMsgToSend;
static uint8_t  ServerMsgToSendLength;
static uint8_t* ServerIP_Address;
static uint8_t  ServerIP_AddressLength;
static uint8_t* ServerPortNumber;
static uint8_t  ServerPortNumberLength;

//FSMs State variables
static uint8_t ManageState;
static uint8_t FSM_State;
static uint8_t TempFSM_State;



//counters
static uint16_t DelayCounter;
static uint32_t MaxResponseTimeCounter;
static uint8_t  Ring120msCounter;

static uint8_t ReadMsgBuffer[READ_MSG_LENGTH];

/***********************************************************************************
**********                      GSM functions' bodies                      ********
***********************************************************************************/

/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Init(void)
{
	CMD_DoneFlag = 0;
	CMD_SuccessFlag = 0;

	SoftWareRstFlag = 0;
	HardWareRstFlag = 0;
	StartFlag = 0;
	SendMsgFlag = 0;
	RecieveMsgFlag = 0;
	SendServerMsgFlag = 0;

	ManageState = MANGE_STATE_UNINIT;
	FSM_State = 1;
	TempFSM_State = 0;

	DelayCounter = 0;
	MaxResponseTimeCounter = 0;
	Ring120msCounter = 0;
}

/*
 * This function is used to reset the defaults of the module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_SoftWareReset(void)
{
	uint8_t RetVar = GSM_BSY;

	if (SoftWareRstFlag == 0)
	{
		SoftWareRstFlag = 1;
		RetVar = GSM_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}

/*
 * This function is used to restart the module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_HardWareReset(void)
{
	uint8_t RetVar = GSM_BSY;

	if (HardWareRstFlag == 0)
	{
		HardWareRstFlag = 1;
		RetVar = GSM_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}

/*
 * This function is used to send an SMS from GSM module
 *Inputs:
 *		- Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *		- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 *		- PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
 *		- an indication of the success of the function
*/

GSM_CheckType GSM_Send_SMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	uint8_t RetVar = GSM_BSY;

	if (SendMsgFlag == 0)
	{
		SendMsgFlag = 1;

		MsgToSend = Msg;
		MsgToSendLength = MsgLength;
		MsgToSendPhoneNum = PhoneNum;

		RetVar = GSM_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}

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
GSM_CheckType GSM_SendServerMsg(uint8_t* ServerIP, uint8_t ServerIPLength, uint8_t* PortNum, uint8_t PortNumLength, uint8_t* Msg, uint8_t MsgLength)
{
	uint8_t RetVar = GSM_BSY;

	if (SendServerMsgFlag == 0)
	{
		SendServerMsgFlag = 1;


		ServerMsgToSend = Msg;
		ServerMsgToSendLength = MsgLength;
		ServerIP_Address = ServerIP;
		ServerIP_AddressLength = ServerIPLength;
		ServerPortNumber = PortNum;
		ServerPortNumberLength = PortNumLength;

		RetVar = GSM_OK;	
	}
	else{/*the RetVar is initialized to BSY so no action is needed here*/}
	
	return RetVar;
}



/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

void GSM_ManageOngoingOperation(void)
{ 
	GSM_CheckType GSM_Check = GSM_BSY;// variable to indicate indicate the state of the called function
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType
	uint16_t RingData;//variable to read the Ring pin
	uint8_t Index; //aloop index

	//read the Ring pin into RingData
	GPIO_Read(ConfigPtr->RingGroupId, &RingData);

	//check if the Ring pin is low and the module is initialized
	if(((RingData & (ConfigPtr->RingPinMask)) == 0) && (ManageState != MANGE_STATE_UNINIT)&&(RecieveMsgFlag == 0))
	{ 
		if (Ring120msCounter < C120)
		{
			Ring120msCounter++;	
		}
		else
		{
			//set the received message flag
			RecieveMsgFlag = 1;
			Ring120msCounter = 0;
		}	
	}
	else
	{
		Ring120msCounter = 0;
	}

	//Manage on going operation FSM
	switch(ManageState)
	{
		//uninitialized state
		case MANGE_STATE_UNINIT : 
		{
			//start the power on routine function
			GSM_Check = PowerOn();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
				ManageState = MANGE_STATE_IDLE;
				//set the start flag to configure the module right after power on 
                StartFlag = 1;
                //reset the state
                FSM_State = 1;
			}

			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the state
                FSM_State = 1;
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_IDLE : 
		{
			//if start flag was set
			if(StartFlag == 1)
			{
				//change the state to start communication state
				ManageState = MANGE_STATE_START;
			}

			//if the hardware reset flag was set
			else if(HardWareRstFlag == 1)
			{
				//change the state to the hardware reset state
				ManageState = MANGE_STATE_HWRESET;
			}

			//if the software reset flag was set
			else if(SoftWareRstFlag == 1)
			{
				//change the state to the software reset state
				ManageState = MANGE_STATE_SWRESET;
			}
			
			//if send message flag was set
			else if(SendMsgFlag == 1)
			{
				//change the state to the send SMS state
				ManageState = MANGE_STATE_SEND_SMS;
			}

			//if the hardware reset flag was set
			else if(SendServerMsgFlag == 1)
			{
				//change the state to the hardware reset state
				ManageState = MANGE_STATE_SEND_SERVER_MSG;
			}

			//if receive message flag was set
			else if(RecieveMsgFlag == 1)
			{
				//change the state to the read SMS state
				ManageState = MANGE_STATE_RECEIVE_SMS;
			}
			else{/*there is no other flag to be checked*/}

			break;
		}

		case MANGE_STATE_START :
		{
			//call start communication function
			GSM_Check = Start();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                StartFlag = 0;
                //reset the state
                FSM_State = 1;
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                StartFlag = 0; 
                //reset the state
                FSM_State = 1;   
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_SWRESET :
		{
			//call software reset function
			GSM_Check = SoftWareReset();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                SoftWareRstFlag = 0;
                //set the start communication flag
                StartFlag = 1;
                //reset the state
               	FSM_State = 1;
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                SoftWareRstFlag = 0; 
                //reset the state
                FSM_State = 1;
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_HWRESET :
		{
			//call software reset function
			GSM_Check = HardWareReset();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                HardWareRstFlag = 0;
                //set the start communication flag
                StartFlag = 1;
                //reset the state
           		FSM_State = 1;
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                HardWareRstFlag = 0;
                //reset the state
                FSM_State = 1;  
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_SEND_SMS :
		{
			//call send message function
			GSM_Check = Send_Msg();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                SendMsgFlag = 0;
                //reset the state
                FSM_State = 1;

                //call he callback function
                ConfigPtr->SendMsgCallBackFn();
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                SendMsgFlag = 0;
                //reset the state
                FSM_State = 1; 
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_RECEIVE_SMS :
		{
			//call read message function
			GSM_Check = Read_Msg();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                RecieveMsgFlag = 0;
                //reset the state
                FSM_State = 1;

                //exectract the msg from the response
				for(Index = 0; Index < READ_MSG_LENGTH; Index++)
				{
					ReadMsgBuffer[Index] = DriverReceivedResponse[SMSReceivedResponseLength - READ_MSG_LENGTH + Index];
				}

                //call he callback function
                ConfigPtr->RecieveMsgCallBackFn(ReadMsgBuffer);   
			
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                RecieveMsgFlag = 0;
                //reset the state
                FSM_State = 1;   
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_SEND_SERVER_MSG :
		{
			//call send message function
			GSM_Check = Send_ServerMsg();

			//if the function was executed successfully
			if(GSM_Check == GSM_OK)
			{
				//change the state to the idle state
                ManageState = MANGE_STATE_IDLE;
				//reset the flag 
                SendServerMsgFlag = 0;
                //reset the state
                FSM_State = 1;

                //call he callback function
                ConfigPtr->SendServerMsgCallBackFn();
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                SendServerMsgFlag= 0;
                //reset the state
                FSM_State = 1;    
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_ERROR :
		{
			//call the callback function
			ConfigPtr->ErrorCallBackFn(GSM_ERROR_ID);

			//change the state to the idle state
            ManageState = MANGE_STATE_IDLE;
			
		}

		default : {/*there is no thing to do*/}
	}
}


/***********************************************************************************
**********					GSM driver call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM driver it is called when the AT command execution is done  
 * Inputs:
 *		- GSM_DriverCheck	: to indicate the success of the AT command execution
 *		- RecievedResponse 	: a pointer to an array to hold the response from the module to the AT command
 *		- ResponseLength 	: the length of the RecievedResponse
 * Output:NONE
*/

void GSM_DriverCallBackFn(GSM_DriverCheckType GSM_DriverCheck, uint8_t* ReceivedResponse, uint8_t ReceivedResponseLength)
{
	#ifdef DEVELOPMENT_MODE_ENABLE
	UART_StartSilentTransmission (ReceivedResponse,ReceivedResponseLength,0);
	#endif
	//if the AT command was executed successfully
	if(GSM_DriverCheck == GSM_DRIVER_OK)
	{
		//set the command flags to done successfully
		CMD_DoneFlag = 1;
		CMD_SuccessFlag = 1;

		//assign the response and the response length to the global variables
		DriverReceivedResponse = ReceivedResponse;
		DriverReceivedResponseLength = ReceivedResponseLength;
	}
	//if the AT command wasn't executed successfully
	else
	{
		//set the command flags to done not successfully
		CMD_DoneFlag = 1;
		CMD_SuccessFlag = 0;
	}
}

/***********************************************************************************
**********							Helper functions						********
***********************************************************************************/

/*
 * This function used to power on the GSM module
 Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType PowerOn(void)
{
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 
	const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	switch(FSM_State)
	{
		case POWER_ON_STATE_HIGH_1 :
		{ 
    		//RTS LOW for software handling
    		GPIO_Write(GSM_ConfigPtr->RTSGroupId, GSM_ConfigPtr->RTSPinMask, LOW);
    		//PWRK HIGH
			GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);

			//change the state to the next state
			FSM_State = POWER_ON_STATE_LOW;

			break;
		}

		case POWER_ON_STATE_LOW :
		{
			//preform a delay
			if (DelayCounter < T30)
			{
				DelayCounter++;
			}
			//if the delay is done
			else
			{
				//PWRK LOW
				GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);
				//change the state to the next state
				FSM_State = POWER_ON_STATE_HIGH_2;
				//reset the counter
				DelayCounter = 0;	
			}

			break;	
		}

		case POWER_ON_STATE_HIGH_2 :
		{
			//preform a delay
			if (DelayCounter < T1100)
			{
				DelayCounter++;
			}
			//if the delay is done
			else
			{
				//PWRK LOW
				GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);
				//change the state to the next state
				FSM_State = POWER_ON_START_DELAY;
				//reset the counter
				DelayCounter = 0;	
			}

			break;	
		}

		case POWER_ON_START_DELAY :
		{
			//preform a delay
			if (DelayCounter < T_RDY)
			{
				DelayCounter++;
			}
			//if the delay is done
			else 
			{
				//reset the counter
				DelayCounter = 0;

				//set the return value to OK

				RetVar = GSM_OK;	
			}
			
			break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}

	}

	return RetVar;
}

/*
 * This function is a FSM to preform a hardware reset on the GSM module
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType HardWareReset(void)
{
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 
	const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	switch(FSM_State)
	{
		case HARDWARE_RESET_STATE_LOW_1 :
		{ 
    		//PWRK HIGH
			GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);

			//change the state to the next state
			FSM_State = HARDWARE_RESET_STATE_HIGH_1;

			break;
		}

		case HARDWARE_RESET_STATE_HIGH_1 :
		{
			//preform a delay
			if (DelayCounter < T800)
			{
				DelayCounter++;
			}
			//if the delay is done
			else
			{
				//PWRK LOW
				GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);
				//change the state to the next state
				FSM_State = HARDWARE_RESET_STATE_LOW_2;
				//reset the counter
				DelayCounter = 0;	
			}

			break;	
		}

		case HARDWARE_RESET_STATE_LOW_2 :
		{
			//preform a delay
			if (DelayCounter < T3000)
			{
				DelayCounter++;
			}
			//if the delay is done
			else
			{
				//PWRK LOW
				GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);
				//change the state to the next state
				FSM_State = HARDWARE_RESET_STATE_HIGH_2;
				//reset the counter
				DelayCounter = 0;	
			}

			break;	
		}

		case HARDWARE_RESET_STATE_HIGH_2 :
		{
			//preform a delay
			if (DelayCounter < T1100)
			{
				DelayCounter++;
			}
			//if the delay is done
			else
			{
				//PWRK LOW
				GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);
				//change the state to the next state
				FSM_State = HARDWARE_RESET_START_DELAY;
				//reset the counter
				DelayCounter = 0;	
			}

			break;	
		}


		case HARDWARE_RESET_START_DELAY :
		{
			//preform a delay
			if (DelayCounter < T_RDY)
			{
				DelayCounter++;
			}
			//if the delay is done
			else 
			{
				//reset the counter
				DelayCounter = 0;
				//set the return value to OK
				RetVar = GSM_OK;	
			}

			break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}

	}

	return RetVar;
}

/*
 * This function is a FSM to preform a software reset on the GSM module 
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType SoftWareReset(void)
{
	//variables declarations 
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 

	switch(FSM_State)
	{
		case SOFTWARE_RESET_STATE_CMD :
		{
			//start the command transmission
			GSM_Driver_ATCMD_ResetDefaults();

			FSM_State = SOFTWARE_RESET_STATE;

			MaxResponseTimeCounter = 0;

			break;
		}

		case SOFTWARE_RESET_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				CMD_DoneFlag = 0;

				if(CMD_SuccessFlag == 1)
				{
					//set the return value to ok 
					RetVar = GSM_OK;
					
					CMD_SuccessFlag = 0;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
				}
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
					GSM_Driver_ATCMD_Stop();
					RetVar = GSM_NOK; 
				}
			}
			break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}
	}
	
	return RetVar;
}

/*
 * This function is a FSM to configure the GSM module with the necessary settings
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Start(void)
{
	//variables declarations 
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	switch(FSM_State)
	{
		case START_BUSY_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				CMD_DoneFlag = 0;

				if(CMD_SuccessFlag == 1)
				{
					CMD_SuccessFlag = 0;

					if (TempFSM_State == START_NUMBER_OF_CMDS)
					{
						RetVar = GSM_OK; 
					}
					else
					{
						FSM_State = TempFSM_State + 1;
					}
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;		
				}
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
					GSM_Driver_ATCMD_Stop();
					RetVar = GSM_NOK; 
				}
			}
		
			break;
		}

		case START_STATE_AT_CMD :
		{
			GSM_Driver_ATCMD_AT();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			break;
		}

		case  START_STATE_STOP_ECHO_CMD :
		{
			
			GSM_Driver_ATCMD_StopEcho();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;
   
    		break;
		}

		case START_STATE_FIX_BR_CMD :
		{
			
			GSM_Driver_ATCMD_FixBR();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;
	
    		break;
		}

		case START_STATE_DELETE_SMS_CMD :
		{
			
			GSM_Driver_ATCMD_DeleteSMS();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		case START_STATE_GPRS_APN_CMD :
		{
			
    		GSM_Driver_ATCMD_GPRS_APN(ConfigPtr->ServiceProviderAPN, ConfigPtr->ServiceProviderAPNLength);

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;
    		
			
			break;
		}

		case START_STATE_GPRS_ACTIVATE_CMD :
		{
			
			GSM_Driver_ATCMD_GPRS_Activate();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		case START_STATE_GPRS_ATTACH_CMD :
		{
			
			GSM_Driver_ATCMD_GPRS_Attach();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}
			
		case START_STATE_TCP_CONTEXT_CMD :
		{
			
			GSM_Driver_ATCMD_TCP_SetContext();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		case START_STATE_TCP_SP_INFO_CMD :
		{
    		GSM_Driver_ATCMD_TCP_SP_Info(ConfigPtr->ServiceProviderAPN, ConfigPtr->ServiceProviderAPNLength, ConfigPtr->ServiceProviderUserName, ConfigPtr->ServiceProviderUserNameLength, ConfigPtr->ServiceProviderPassWord, ConfigPtr->ServiceProviderPassWordLength);

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}
		
		case START_STATE_TCP_MUX_CMD :
		{
			
			GSM_Driver_ATCMD_TCP_DisableMUX();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		case START_STATE_TCP_MODE_CMD :
		{
			
			GSM_Driver_ATCMD_TCP_SetMode();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		case START_STATE_TCP_DNSIP_CMD :
		{
			
			GSM_Driver_ATCMD_TCP_SetDNSIP();

			TempFSM_State = FSM_State;
			FSM_State = START_BUSY_STATE;

			MaxResponseTimeCounter = 0;

    		break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}
	}

	return RetVar;
}

/*
 * This function is a FSM to send an SMS from GSM module
  * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Send_Msg(void)
{
	//variables declarations 
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 

	switch(FSM_State)
	{
		case SEND_BUSY_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				CMD_DoneFlag = 0;

				if(CMD_SuccessFlag == 1)
				{
					CMD_SuccessFlag = 0;

					if (TempFSM_State == SEND_NUMBER_OF_CMDS)
					{
						RetVar = GSM_OK; 
					}
					else
					{
						FSM_State = TempFSM_State + 1;
					}
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;		
				}
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
					GSM_Driver_ATCMD_Stop();
					RetVar = GSM_NOK; 
				}
			}
		
			break;
		}

		case SEND_STATE_TEXT_MODE_CMD :
		{
			GSM_Driver_ATCMD_SMSFormat(TEXT_MODE);

			TempFSM_State = FSM_State;
			FSM_State = SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}

		case SEND_STATE_SMS_CHAR_SET_CMD :
		{
			GSM_Driver_ATCMD_CharSet();

			TempFSM_State = FSM_State;
			FSM_State = SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;

			break;
		}

		case SEND_STATE_MOB_NUM_CMD :
		{
			GSM_Driver_ATCMD_SendSMS_MobNum(MsgToSendPhoneNum);

			TempFSM_State = FSM_State;
			FSM_State = SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;

			break;
		}

		case SEND_STATE_WRITE_MSG_CMD :
		{
			GSM_Driver_ATCMD_SendSMS_WriteMsg(MsgToSend, MsgToSendLength);

			TempFSM_State = FSM_State;
			FSM_State = SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;

			break;
		}

		case SEND_STATE_PDU_MODE_CMD :
		{
			GSM_Driver_ATCMD_SMSFormat(PDU_MODE);

			TempFSM_State = FSM_State;
			FSM_State = SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;

			break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}
	}

	return RetVar;
}

/*
 * This function is a FSM to read the SMS message in index 1  
 *Inputs:NONE 
 * Output:
 *		- an indication of the success of the function
*/

static GSM_CheckType Read_Msg(void)
{
	//variables declarations 
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 
	const GSM_ConfigType* ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType

	switch(FSM_State)
	{
		case RECEIVE_BUSY_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				CMD_DoneFlag = 0;

				if(CMD_SuccessFlag == 1)
				{
					CMD_SuccessFlag = 0;

					if(TempFSM_State == RECEIVE_STATE_READ_CMD)
					{
						SMSReceivedResponseLength = DriverReceivedResponseLength;
					}
					else
					{/*if the state wasn't the read msg state then there is no need to store the recieved response length*/}

					if (TempFSM_State == RECEIVE_NUMBER_OF_CMDS)
					{
						RetVar = GSM_OK; 
					}
					else
					{
						FSM_State = TempFSM_State + 1;
					}
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;		
				}
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
					GSM_Driver_ATCMD_Stop();
					RetVar = GSM_NOK; 
				}
			}
		
			break;
		}

		case RECEIVE_STATE_TEXT_MODE_CMD :
		{
			GSM_Driver_ATCMD_SMSFormat(TEXT_MODE);

			TempFSM_State = FSM_State;
			FSM_State = RECEIVE_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}

		case RECEIVE_STATE_CHECK_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_CheckReceivedSMS();

				TempFSM_State = FSM_State;
				FSM_State = RECEIVE_BUSY_STATE;

				DelayCounter = 0;

				MaxResponseTimeCounter = 0;
    		}
			
			break;
		}

		case RECEIVE_STATE_READ_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_ReadSMS(READ_MSG_LENGTH);

				TempFSM_State = FSM_State;
				FSM_State = RECEIVE_BUSY_STATE;

				DelayCounter = 0;

				MaxResponseTimeCounter = 0;
    		}
			
			break;
		}

		case RECEIVE_STATE_PDU_MODE_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_SMSFormat(PDU_MODE);

				TempFSM_State = FSM_State;
				FSM_State = RECEIVE_BUSY_STATE;

				DelayCounter = 0;

				MaxResponseTimeCounter = 0;
    		}

			break;
		}

		case RECEIVE_STATE_DELETE_SMS_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_DeleteSMS();

				TempFSM_State = FSM_State;
				FSM_State = RECEIVE_BUSY_STATE;

				DelayCounter = 0;

				MaxResponseTimeCounter = 0;
    		}

			break;
		}

		default :
		{
			RetVar = GSM_NOK;
		}
	}

	return RetVar;
}


/*
 * This function is a FSM to send aserver message from GSM module
  * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static GSM_CheckType Send_ServerMsg(void)
{
	//variables declarations 
	GSM_CheckType RetVar = GSM_BSY;// variable to hold the return value to indicate the state of the function 

	switch(FSM_State)
	{
		case SERVER_SEND_BUSY_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				CMD_DoneFlag = 0;

				if(CMD_SuccessFlag == 1)
				{
					CMD_SuccessFlag = 0;

					if (TempFSM_State == SERVER_SEND_NUMBER_OF_CMDS)
					{
						RetVar = GSM_OK; 
					}
					else
					{
						FSM_State = TempFSM_State + 1;
					}
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;		
				}
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
					GSM_Driver_ATCMD_Stop();
					RetVar = GSM_NOK; 
				}
			}
		
			break;
		}

		case SERVER_SEND_STATE_CONNECT_CMD :
		{
			GSM_Driver_ATCMD_TCP_Connect(ServerIP_Address, ServerIP_AddressLength, ServerPortNumber, ServerPortNumberLength);

			TempFSM_State = FSM_State;
			FSM_State = SERVER_SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}

		case SERVER_SEND_STATE_INIT_SEND_CMD :
		{
			GSM_Driver_ATCMD_InitSendServerMsg();

			TempFSM_State = FSM_State;
			FSM_State = SERVER_SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}

		case SERVER_SEND_STATE_SEND_CMD :
		{
			GSM_Driver_ATCMD_SendServerMsg(ServerMsgToSend, ServerMsgToSendLength);

			TempFSM_State = FSM_State;
			FSM_State = SERVER_SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}

		case SERVER_SEND_STATE_CLOSE_CMD :
		{
			GSM_Driver_ATCMD_TCP_Close();

			TempFSM_State = FSM_State;
			FSM_State = SERVER_SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}
		
		case SERVER_SEND_STATE_DEACTIVATE_CMD :
		{
			GSM_Driver_ATCMD_TCP_Deactivate();

			TempFSM_State = FSM_State;
			FSM_State = SERVER_SEND_BUSY_STATE;

			MaxResponseTimeCounter = 0;
			
			break;
		}
		
		default :
		{
			RetVar = GSM_NOK;
		}
	}

	return RetVar;
}



/*

  special callback function for error 

  start in a siparate state


*/