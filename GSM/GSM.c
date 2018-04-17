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

#define TIME_BETWEEN_COMMANDS		1000U
#define TIME_TILL_READY				15000U

//the time delays used in this file
#define T30		(30U/CYCLIC_TIME)
#define T800	(800U/CYCLIC_TIME)
#define T1100	(1100U/CYCLIC_TIME)
#define T3000	(3000U/CYCLIC_TIME)

#define T_CMD	(TIME_BETWEEN_COMMANDS/CYCLIC_TIME)
#define T_RDY	(TIME_TILL_READY/CYCLIC_TIME)



//the states of the manager function
#define MANGE_STATE_UNINIT 			0U
#define MANGE_STATE_IDLE 			1U
#define MANGE_STATE_START 			2U
#define MANGE_STATE_SWRESET 		3U
#define MANGE_STATE_HWRESET 		4U
#define MANGE_STATE_SEND_SMS 		5U
#define MANGE_STATE_RECEIVE_SMS 	6U
#define MANGE_STATE_ERROR 			7U

//the states of the power on function
#define POWER_ON_STATE_HIGH_1		0U
#define POWER_ON_STATE_LOW			1U
#define POWER_ON_STATE_HIGH_2		2U
#define POWER_ON_START_DELAY		3U

//the states of the hardware reset function
#define HARDWARE_RESET_STATE_LOW_1		0U
#define HARDWARE_RESET_STATE_HIGH_1		1U
#define HARDWARE_RESET_STATE_LOW_2		2U
#define HARDWARE_RESET_STATE_HIGH_2		3U
#define HARDWARE_RESET_START_DELAY		4U

//the states of the Start communication function
#define START_STATE_AT_CMD				0U
#define START_STATE_AT					1U
#define START_STATE_STOP_ECHO_CMD		2U
#define START_STATE_STOP_ECHO			3U
#define START_STATE_FIX_BR_CMD			4U
#define START_STATE_FIX_BR				5U
#define START_STATE_DELETE_SMS_CMD		6U
#define START_STATE_DELETE_SMS			7U

//the states of the send sms function
#define SEND_STATE_TEXT_MODE_CMD		0U
#define SEND_STATE_TEXT_MODE			1U
#define SEND_STATE_SMS_CHAR_SET_CMD		2U
#define SEND_STATE_SMS_CHAR_SET			3U
#define SEND_STATE_MOB_NUM_CMD			4U
#define SEND_STATE_MOB_NUM				5U
#define SEND_STATE_WRITE_MSG_CMD		6U
#define SEND_STATE_WRITE_MSG			7U
#define SEND_STATE_PDU_MODE_CMD			8U
#define SEND_STATE_PDU_MODE				9U

//the states of the receive sms function
#define RECEIVE_STATE_TEXT_MODE_CMD		0U
#define RECEIVE_STATE_TEXT_MODE			1U
#define RECEIVE_STATE_CHECK_CMD			2U
#define RECEIVE_STATE_CHECK				3U
#define RECEIVE_STATE_READ_CMD			4U
#define RECEIVE_STATE_READ				5U
#define RECEIVE_STATE_PDU_MODE_CMD		6U
#define RECEIVE_STATE_PDU_MODE			7U
#define RECEIVE_STATE_DELETE_SMS_CMD	8U
#define RECEIVE_STATE_DELETE_SMS		9U

//the states of the software reset function
#define SOFTWARE_RESET_STATE_CMD		0U
#define SOFTWARE_RESET_STATE 			1U

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

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

//variables to hold the data from the gsm driver callback function
static uint8_t* DriverReceivedResponse;
static uint8_t DriverReceivedResponseLength;

//flags to hold the state of the at command called by the functions in this file whether it succeed or not or it is still in progress
static uint8_t CMD_DoneFlag;
static uint8_t CMD_SuccessFlag;

//flags for the functions called by the client
static uint8_t SoftWareRstFlag;
static uint8_t HardWareRstFlag;
static uint8_t StartFlag;
static uint8_t SendMsgFlag;
static uint8_t RecieveMsgFlag;

//variables to hold the data needed to send an SMS
static uint8_t* MsgToSend;
static uint8_t MsgToSendLength;
static uint8_t* MsgToSendPhoneNum;

//FSMs State variables
static uint8_t ManageState;
static uint8_t PowerOnState;
static uint8_t HardWareResetState;
static uint8_t SoftWareResetState;
static uint8_t StartState;
static uint8_t SendState;
static uint8_t ReceiveState;

//counter to preform delays
static uint16_t DelayCounter;

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

	ManageState = MANGE_STATE_UNINIT;
	PowerOnState = POWER_ON_STATE_HIGH_1;
	HardWareResetState = HARDWARE_RESET_STATE_LOW_1;
	SoftWareResetState = SOFTWARE_RESET_STATE_CMD;
	StartState = START_STATE_AT_CMD;
	SendState = SEND_STATE_TEXT_MODE_CMD;
	ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;

	DelayCounter = 0;
}

/*
 * This function is used to reset the defaults of the module
 *Inputs:NONE
 * Output:NONE
*/

void GSM_SoftWareReset(void)
{
	SoftWareRstFlag = 1;
}

/*
 * This function is used to restart the module
 *Inputs:NONE
 * Output:NONE
*/

void GSM_HardWareReset(void)
{
	HardWareRstFlag = 1;
}

/*
 * This function is used to send an SMS from GSM module
 *Inputs:
 *		- Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
 *		- MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 *		- PhoneNum		: a pointer to the phone number to send the SMS
 * Output:NONE
*/

void GSM_Send_SMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	SendMsgFlag = 1;

	MsgToSend = Msg;
	MsgToSendLength = MsgLength;
	MsgToSendPhoneNum = PhoneNum;
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

	//read the Ring pin into RingData
	GPIO_Read(ConfigPtr->RingGroupId, &RingData);

	//check if the Ring pin is low and the module is initialized
	if((RingData & (ConfigPtr->RingPinMask)) == 0 && (ManageState != MANGE_STATE_UNINIT))
	{
		//set the received message flag
		RecieveMsgFlag = 1;
	}
	else{/*No action needed if the Ring Pin is HIGH*/}

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
			}

			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
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
			else if(SoftWareRstFlag == 1)
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
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                StartFlag = 0;    
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
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                SoftWareRstFlag = 0;   
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
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                HardWareRstFlag = 0;   
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

                //call he callback function
                ConfigPtr->RecieveMsgCallBackFn();
			}
			//if the function wasn't executed successfully
			else if(GSM_Check == GSM_NOK)
			{
				//change the state to the error state
				ManageState = MANGE_STATE_ERROR;
				//reset the flag 
                RecieveMsgFlag = 0;    
			}
			else{/*the GSM_Check is initialized to BSY so no action is needed here*/}

			break;
		}

		case MANGE_STATE_ERROR :
		{
			//call the callback function
			ConfigPtr->ErrorCallBackFn();

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
	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled
	UART_StartSilentTransmission (ReceivedResponse,ReceivedResponseLength,1);
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

		#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled
		CMD_SuccessFlag = 1;
		#endif
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

	switch(PowerOnState)
	{
		case POWER_ON_STATE_HIGH_1 :
		{ 
    		//RTS LOW for software handling
    		GPIO_Write(GSM_ConfigPtr->RTSGroupId, GSM_ConfigPtr->RTSPinMask, LOW);
    		//PWRK HIGH
			GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);

			//change the state to the next state
			PowerOnState = POWER_ON_STATE_LOW;

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
				PowerOnState = POWER_ON_STATE_HIGH_2;
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
				PowerOnState = POWER_ON_START_DELAY;
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
				//change the state to the next state
				PowerOnState = POWER_ON_STATE_HIGH_1;
				//reset the counter
				DelayCounter = 0;

				//set the return value to OK

				RetVar = GSM_OK;	
			}
		}

		default :
		{/*there is no other thing to do*/}

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

	switch(HardWareResetState)
	{
		case HARDWARE_RESET_STATE_LOW_1 :
		{ 
    		//PWRK HIGH
			GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);

			//change the state to the next state
			HardWareResetState = HARDWARE_RESET_STATE_HIGH_1;

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
				HardWareResetState = HARDWARE_RESET_STATE_LOW_2;
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
				HardWareResetState = HARDWARE_RESET_STATE_HIGH_2;
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
				HardWareResetState = HARDWARE_RESET_START_DELAY;
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
				//change the state to the next state
				HardWareResetState = HARDWARE_RESET_STATE_LOW_1;
				//reset the counter
				DelayCounter = 0;
				//set the return value to OK
				RetVar = GSM_OK;	
			}
		}

		default :
		{/*there is no other thing to do*/}

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

	switch(SoftWareResetState)
	{
		case SOFTWARE_RESET_STATE_CMD :
		{
			//start the command transmission
			GSM_Driver_ATCMD_ResetDefaults();

			SoftWareResetState = SOFTWARE_RESET_STATE;

			break;
		}

		case SOFTWARE_RESET_STATE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//set the return value to ok 
					RetVar = GSM_OK;

					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					SoftWareResetState = SOFTWARE_RESET_STATE_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;

					SoftWareResetState = SOFTWARE_RESET_STATE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

			break;
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
	
	switch(StartState)
	{
		case START_STATE_AT_CMD :
		{
			GSM_Driver_ATCMD_AT();
			StartState = START_STATE_AT;
			break;
		}

		case START_STATE_AT :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					StartState = START_STATE_STOP_ECHO_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

			break;
		}

		case  START_STATE_STOP_ECHO_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_StopEcho();

    			StartState = START_STATE_STOP_ECHO;

    			//reset counter
				DelayCounter = 0;
    		}

    		break;
		}

		case START_STATE_STOP_ECHO :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					StartState = START_STATE_FIX_BR_CMD;
					
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					StartState = START_STATE_AT_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
		
			break;
		}

		case START_STATE_FIX_BR_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_FixBR();

    			StartState = START_STATE_FIX_BR;

    			//reset counter
				DelayCounter = 0;
    		}

    		break;
		}

		case START_STATE_FIX_BR :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					StartState = START_STATE_DELETE_SMS_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					StartState = START_STATE_AT_CMD;
					
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
		
			break;
		}

		case START_STATE_DELETE_SMS_CMD :
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

    			StartState = START_STATE_DELETE_SMS;

    			//reset counter
				DelayCounter = 0;
    		}

    		break;
		}

		case START_STATE_DELETE_SMS :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					StartState = START_STATE_AT_CMD;

					//set the return value to ok for 
					RetVar = GSM_OK;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					StartState = START_STATE_AT_CMD;
					
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
		
			break;
		}

		default :
		{/*nothing to be done*/}
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

	switch(SendState)
	{
		case SEND_STATE_TEXT_MODE_CMD :
		{
			GSM_Driver_ATCMD_SMSFormat(TEXT_MODE);

			SendState = SEND_STATE_TEXT_MODE;
			
			break;
		}

		case SEND_STATE_TEXT_MODE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					SendState = SEND_STATE_SMS_CHAR_SET_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

			break;
		}

		case SEND_STATE_SMS_CHAR_SET_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_CharSet();

    			SendState = SEND_STATE_SMS_CHAR_SET;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case SEND_STATE_SMS_CHAR_SET :
		{
		
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					SendState = SEND_STATE_MOB_NUM_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					SendState = SEND_STATE_TEXT_MODE_CMD;
				}
			}
			
			else{/*if the driver isn't done executing the command no action needed*/}

			break;
		}

		case SEND_STATE_MOB_NUM_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_SendSMS_MobNum(MsgToSendPhoneNum);

    			SendState = SEND_STATE_MOB_NUM;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case SEND_STATE_MOB_NUM :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					SendState = SEND_STATE_WRITE_MSG_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					SendState = SEND_STATE_TEXT_MODE_CMD;	
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

			break;
		}

		case SEND_STATE_WRITE_MSG_CMD :
		{
			//delay between commands 
    		if(DelayCounter < T_CMD)
    		{
    			DelayCounter++;
    		}

    		//if the delay is done
    		else
    		{
    			GSM_Driver_ATCMD_SendSMS_WriteMsg(MsgToSend, MsgToSendLength);


    			SendState = SEND_STATE_WRITE_MSG;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case SEND_STATE_WRITE_MSG :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					SendState = SEND_STATE_PDU_MODE_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					SendState = SEND_STATE_TEXT_MODE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
		
			break;
		}

		case SEND_STATE_PDU_MODE_CMD :
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


    			SendState = SEND_STATE_PDU_MODE;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case SEND_STATE_PDU_MODE :
		{

			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					SendState = SEND_STATE_TEXT_MODE_CMD;

					//set the return value to ok for 
					RetVar = GSM_OK;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					SendState = SEND_STATE_TEXT_MODE_CMD;
					
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
			
			break;
		}

		default :
		{/*nothing to be done*/}
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
	const GSM_ConfigType* GSM_ConfigPtr = &GSM_ConfigParam;//declare a pointer to structure of the GSM_ConfigType
	uint8_t Index;//loop index

	switch(ReceiveState)
	{
		case RECEIVE_STATE_TEXT_MODE_CMD :
		{
			GSM_Driver_ATCMD_SMSFormat(TEXT_MODE);

			ReceiveState = RECEIVE_STATE_TEXT_MODE;
			
			break;
		}

		case RECEIVE_STATE_TEXT_MODE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					ReceiveState = RECEIVE_STATE_CHECK_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

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

    			ReceiveState = RECEIVE_STATE_CHECK;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case RECEIVE_STATE_CHECK :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					ReceiveState = RECEIVE_STATE_READ_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

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
    			GSM_Driver_ATCMD_ReadSMS(GSM_ConfigPtr->ReadMsgLength);

    			ReceiveState = RECEIVE_STATE_READ;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case RECEIVE_STATE_READ :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//exec tract the msg from the response
					for(Index = 0; Index < (GSM_ConfigPtr->ReadMsgLength); Index++)
					{
						GSM_ConfigPtr->ReadMsgBuffer[Index] = DriverReceivedResponse[DriverReceivedResponseLength - (GSM_ConfigPtr->ReadMsgLength) + Index];
					}

					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					ReceiveState = RECEIVE_STATE_PDU_MODE_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}

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

    			ReceiveState = RECEIVE_STATE_PDU_MODE;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case RECEIVE_STATE_PDU_MODE :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					ReceiveState = RECEIVE_STATE_DELETE_SMS_CMD;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;
					CMD_DoneFlag = 0;
					//reset state
					ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
    		
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

    			ReceiveState = RECEIVE_STATE_DELETE_SMS;

    			//reset counter
				DelayCounter = 0;
			}

			break;
		}

		case RECEIVE_STATE_DELETE_SMS :
		{
			//check the state of the command success
			if (CMD_DoneFlag == 1)
			{
				if(CMD_SuccessFlag == 1)
				{
					//reset the command flags
					CMD_DoneFlag = 0;
					CMD_SuccessFlag = 0;

					//change the state to the next state
					ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;

					//set the return value to ok for 
					RetVar = GSM_OK;
				}
				else
				{
					//set the return value to not ok for the manager to handle the error
					RetVar = GSM_NOK;

					CMD_DoneFlag = 0;

					//reset state
					ReceiveState = RECEIVE_STATE_TEXT_MODE_CMD;
				}
			}
			else{/*if the driver isn't done executing the command no action needed*/}
			
			break;
		}

		default :
		{/*nothing to be done*/}
	}

	return RetVar;
}
