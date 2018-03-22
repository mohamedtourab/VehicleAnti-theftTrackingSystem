/**************************************************************************************
*File name: GSM_Manager.c
*Auther : 
		-Abdelrhman Hosny
		-Amr Mohamed
*Date: 18/3/2018
*Description:
*	This file contains:
*		- implementation of functions used to manage the M95 GSM module
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM_Manager.h"

//the time delays used in this file
#define T30		(30/CYCLIC_TIME)
#define T800	(800/CYCLIC_TIME)
#define T1000	(1000/CYCLIC_TIME)
#define T1100	(1100/CYCLIC_TIME)
#define T5000	(5000/CYCLIC_TIME)
#define T7000	(12000/CYCLIC_TIME)

//the states of the manager function
#define UNINIT 0U
#define IDLE 1U
#define START 2U
#define SWRESET 3U
#define SENDSMS 4U
#define RECIEVESMS 5U
#define ERROR 6U

/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

/*
 * This function is a FSM to power on the GSM module
 Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManagePowerOn(void);

/*
 * This function is a FSM to WakeUp the GSM module from sleep mode
 * Inputs:NONOE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageWakeUp(void);

/*
 * This function is a FSM to put the GSM module in sleep mode
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageSleep(void);


static GSM_Manage_CheckType GSM_ManageSWReset(void);

/*
 * This function is a FSM to configure the GSM module with the necessary settings
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


static GSM_Manage_CheckType GSM_ManageStart(void);

/*
 * This function is a FSM to read the SMS message in index 1  
 *Inputs:
 		- MsgBuffer : a pointer to an array tohold the read SMS
 		- MsgLengrh	: the length of the expected SMS message 
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageReadSMS(uint8_t* MsgBuffer, uint8_t MsgLength);

/*
 * This function is a FSM to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageSetSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum);

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

//variables to hold the data from the gsm driver callback function
uint8_t* RecievedResponsePtr;
uint16_t RecievedResponseLength;
//a flag to hold the state of the at command called by the functions in this file whether it succeded or not or it is still in progress
volatile GSM_Manage_CheckType GSM_Flag;

//flags for the functions called by the client
volatile static uint8_t SoftWareRstFlag;
volatile static uint8_t StartFlag;
volatile static uint8_t SendMsgFlag;
volatile static uint8_t RecieveMsgFlag;

//variables to hold the data needed to send an SMS
static uint8_t* MsgGlob;
static uint8_t MsgLengthGlob;
static uint8_t* PhoneNumGlob;


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
	GSM_Flag = GSM_Manage_InProgress;
	StartFlag = 0;
	SendMsgFlag = 0;
	SoftWareRstFlag = 0;
	RecieveMsgFlag = 0;
}

/*
 * This function is used to configure the GSM module with the necessary settings
 *Inputs:NONE
 * Output:NONE
*/

void StartCommunication(void)
{
	StartFlag = 1;
}

/*
 * This function is used to reset the defaults of the module
 *Inputs:NONE
 * Output:NONE
*/

void SoftWareReset(void)
{
	SoftWareRstFlag = 1;
}

/*
 * This function is used to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:NONE
*/

void SendSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	SendMsgFlag = 1;
	MsgGlob = Msg;
	MsgLengthGlob = MsgLength;
	PhoneNumGlob = PhoneNum;
}

/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_Manage_CheckType GSM_ManageOngoingOperation(void)
{
	GSM_ManageConfigType* ConfigPtr = &GSM_ManageConfigParam;
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_InProgress;// variable to indicate indecate the state of the called function
	static uint8_t State;//a variable to hold the state of the FSM
	uint16_t RingData;

	GPIO_Read(ConfigPtr->RingGroupId, &RingData);

	if((RingData & RingPinMask) == 0)
	{
		RecieveMsgFlag = 1;
	}
	else{;/*MISRA*/}


	switch(State)
	{
		//uninitialized state
		case UNINIT :
		{
			//start the poweron routine function
			GSM_ManageCheck = GSM_ManagePowerOn();

			//if the function was executed successfully
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				//change the state to the idle state
				State = IDLE;
				//set the start flag to configure the module right after poweron 
                StartFlag = 1;
                //set the return value to in progress
				RetVar = GSM_Manage_InProgress;
			}

			//if the function wasn't executed successfully
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				//change the state to the error state
				State = ERROR;

				//set the return value to not ok
				RetVar = GSM_Manage_NOK;
			}
			else{;/*MISRA*/}

			break;
		}

		//the idle state
		case IDLE :
		{
			//if the software reset flag was set
			if(SoftWareRstFlag == 1)
			{
				//change the state to the software reset state
				State = SWRESET;
			}
			//if start flag was set
			else if(StartFlag == 1)
			{
				//change the state to start communication state
				State = START;
			}
			//if send nessage flag was set
			else if(SendMsgFlag == 1)
			{
				//change the state to the send SMS state
				State = SENDSMS;
			}
			//if recieve message flag was set
			else if(RecieveMsgFlage == 1)
			{
				//change the state to the read SMS state
				State = RECIEVESMS;
			}
			else{;/*MISRA*/}

			break;
		}

		//start communication state
		case START :
		{
			//call start communication function
			GSM_ManageCheck = GSM_ManageStart();

			//if the function was executed successfully
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				//change the state to the idle state
                State = IDLE;
                 //reset the flag 
                StartFlag=0;
                //set the return value to in progress
				RetVar=GSM_Manage_OK;
			}
			//if the function wasn't executed successfully
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				//set the return value to not ok
				RetVar=GSM_Manage_NOK;
				 //reset the flag 
                StartFlag=0;
                //change the state to the error state
				State = ERROR;
			}
			else{;/*MISRA*/}

			break;
		}

		//software reset state
		case SWRESET :
		{
			//call software reset function
			GSM_ManageCheck = GSM_ManageSWReset();

			//if the function was executed successfully
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				//change the state to the idle state
                State = IDLE;
                //reset the flag 
                SoftWareRstFlag = 0;
                //set the start flag to configure the module right after software reset 
                StartFlag = 1;
				//set the return value to in progress
                RetVar=GSM_Manage_OK;
			}
			//if the function wasn't executed successfully
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				//set the return value to not ok
                RetVar=GSM_Manage_NOK;
                //reset the flag 
                SoftWareRstFlag = 0;
				//change the state to the error state
                State = ERROR;
			}
			else{;/*MISRA*/}

			break;
		}

		//send SMS state
		case SENDSMS :
		{
			//call send SMS function
			GSM_ManageCheck = GSM_ManageSetSMS(MsgGlob, MsgLengthGlob, PhoneNumGlob);

			//if the function was executed successfully
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				//change the state to the idle state
                State = IDLE;
                //reset the flag 
				SendMsgFlag = 0;
				//set the return value to in progress
                RetVar=GSM_Manage_OK;

				ConfigPtr->SendMsgCallBack(); 
			}
			//if the function wasn't executed successfully
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				//set the return value to not ok
                RetVar=GSM_Manage_NOK;
                 //reset the flag 
				SendMsgFlag = 0;
				//change the state to the error state
                State = ERROR;
			}
			else{;/*MISRA*/}

			
			break;
		}


		//read SMS state
		case RECIEVESMS :
		{
			//call read SMS function
			GSM_ManageCheck = GSM_ManageReadSMS(ConfigPtr->ReadMsgBuffer, ConfigPtr->ReadMsgLength);
			
			//if the function was executed successfully
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				//change the state to the idle state
                State = IDLE;
                //reset the flag 
                RecieveMsgFlage = 0;
                //set the return value to in progress
				RetVar=GSM_Manage_InProgress;
				ConfigPtr->RecieveMsgCallBack();
			}
			//if the function wasn't executed successfully
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				//set the return value to not ok
                RetVar=GSM_Manage_NOK;
                 //reset the flag 
				RecieveMsgFlage = 0;
				//change the state to the error state
                State = ERROR;
			}
			else{;/*MISRA*/}
			
			break;
		}

		case ERROR :
		{
			ConfigPtr->ErrorCallBack();
		}

		default : {;/*MISRA*/}
	}
}

/***********************************************************************************
**********					GSM dirver call back functions					********
***********************************************************************************/

/*
 * This function callback function for the GSM driver it is called when the AT command execution is done  
 * Inputs:
 		- GSM_Check 		: to indicate the success of the AT command execution
 		- RecievedResponse 	: a pointer to an array to hold the response from the module to the AT command
 		- ResponseLength 	: the length of the RecievedResponse
 * Output:NONE
*/

void GSM_CallBack(GSM_CheckType GSM_Check, uint8_t* RecievedResponse, uint16_t ResponseLength)
{
	if(GSM_Check == GSM_OK)
	{
		GSM_Flag = GSM_Manage_OK;
		RecievedResponsePtr = RecievedResponse;
		RecievedResponseLength = ResponseLength;
	}
	else
	{
		GSM_Flag = GSM_Manage_NOK;
	}

}

/***********************************************************************************
**********							Helper functions						********
***********************************************************************************/

/*
 * This function used to power on the GSM module
 Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManagePowerOn(void)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
    static GPIO_CheckType GPIO_Check;//variable to indecate the success of the GPIO operation
	const GSM_ManageConfigType* GSM_ManageConfigPtr = &GSM_ManageConfigParam;//declare a pointer to structur of the GSM_ConfigType   
	static uint16_t Counter = 0;//acounter to count how meny times this function was called to calculate time delays

    //if the function is called for the first time
    if (Counter == 0)
    {
    	//RTS LOW for software handling
    	GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->RTSGroupId, GSM_ManageConfigPtr->RTSPinMask, LOW);
		if(GPIO_Check == GPIO_OK)
		{
			//set DTR HIGH for the sleep mode
			GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->DTRGroupId, GSM_ManageConfigPtr->DTRPinMask, HIGH);
			if(GPIO_Check == GPIO_OK)
			{
				//PWRK HIGH
				GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->PWRKeyGroupId, GSM_ManageConfigPtr->PWRKeyPinMask, HIGH);
				if(GPIO_Check == GPIO_OK)
				{
					RetVar = GSM_Manage_InProgress;
				}
				else
				{
					RetVar = GSM_Manage_NOK;
				}
			}
			else
			{
				RetVar = GSM_Manage_NOK;
			}
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}    	
	}

	//after 30ms delay
	else if(Counter == T30 && GPIO_Check == GPIO_OK)
	{
		//PWRK LOW
		GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->PWRKeyGroupId, GSM_ManageConfigPtr->PWRKeyPinMask, LOW);//PWRK LOW

		if(GPIO_Check == GPIO_OK)
		{
			RetVar = GSM_Manage_InProgress;
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}

	}


	//after extra 1100ms delay
	else if( (Counter == T1100 + T30) && (GPIO_Check == GPIO_OK) )
	{
		GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->PWRKeyGroupId, GSM_ManageConfigPtr->PWRKeyPinMask, HIGH);//PWRK HIGH
	

		if(GPIO_Check == GPIO_OK)
		{
			RetVar = GSM_Manage_InProgress;
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}
	}
	else if(Counter == T1100 + T30 + T7000)
	{
		RetVar = GSM_Manage_OK;
	}
	else{;/*MISRA*/}

	Counter++;

	return RetVar;

} 

/*
 * This function used to WakeUp the GSM module from sleep mode
 * Inputs:NONOE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageWakeUp(void)
{
    //variable declaration
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
    GPIO_CheckType GPIO_Check = GPIO_NOK;// variable to indicate the success of the GPIO function
    const GSM_ManageConfigType* GSM_ManageConfigPtr = &GSM_ManageConfigParam;//declare a pointer to structur of the GSM_ConfigType

	//set DTR LOW to wakeup the GSM module
	GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->DTRGroupId, GSM_ManageConfigPtr->DTRPinMask, LOW);

	if(GPIO_Check == GPIO_OK)
	{
		RetVar = GSM_Manage_OK;
	}
	else
	{
		RetVar = GSM_Manage_NOK;
	}

	return RetVar;
}

/*
 * This function used to put the GSM module in sleep mode
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageSleep(void)
{
    //variable declaration
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
    GPIO_CheckType GPIO_Check = GPIO_NOK;// variable to indicate the success of the GPIO function
	const GSM_ManageConfigType* GSM_ManageConfigPtr = &GSM_ManageConfigParam;//declare a pointer to structur of the GSM_ConfigType
            
	//set DTR HIGH to put the GSM module in sleep mode
	GPIO_Check = GPIO_Write(GSM_ManageConfigPtr->DTRGroupId, GSM_ManageConfigPtr->DTRPinMask, HIGH);
                
	if(GPIO_Check == GPIO_OK)
	{
		RetVar = GSM_Manage_OK;
	}
	else
	{
		RetVar = GSM_Manage_NOK;
	}
        
	//return RetVar;
}


static GSM_Manage_CheckType GSM_ManageSWReset(void)
{
	//variables declarations 
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate indecate the state of the AT command
    static GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the AT command start transmitting
    static uint16_t Counter = 0;//acounter to count how meny times this function was called to calculate time delays

    //a condition to start the command transmision onley once	
	if (GSM_Check == GSM_NOK)
	{
		//wake up the module for communication 
		GSM_ManageCheck = GSM_ManageWakeUp();
		
		//if the wakeup was successfull
		if (GSM_ManageCheck == GSM_Manage_OK)
		{
			//start the command transmission
			GSM_Check = GSM_ATCommand_RstDefault();
		}
		else{;/*MISRA*/}
                
	}
	else {;/*MISRA*/}

	//if the command transmission was successfull
	if(GSM_Check == GSM_OK)
	{
		//check if the commad was executed successfully
		if(GSM_Flag == GSM_Manage_OK)
		{
			//delay between commands 
			if(Counter < T1000)
			{
				Counter++;
			}
			//if the delay is done
			else
			{
				//reset GSM_Check value to the idle value to be able to execute the next commands
				GSM_Check = GSM_NOK;
				//reset the counter to be able to execute other delays
				Counter = 0;
				//reset the GSM_Flag to the idle value to be able to execute the next commands
				GSM_Flag = GSM_Manage_InProgress;
				//set the return value to ok for the manager to know that the function done execution successully
				RetVar = GSM_Manage_OK;
				//put the module into sleep 
				GSM_ManageSleep();

				StartFlag = 1;

				SoftWareRstFlag = 0;

			}
		}
		//if the command done executing but with error 
		else if (GSM_Flag == GSM_Manage_NOK)
		{
			//reset GSM_Check value to the idle value to be able to execute the next commands
			GSM_Check = GSM_NOK;
			//set the return value to not ok for the manager to handle the error
			RetVar = GSM_Manage_NOK;
			//reset the GSM_Flag to the idle value to be able to execute the next commands
			GSM_Flag = GSM_Manage_InProgress;

			//put the module into sleep 
			GSM_ManageSleep();
		}
		else{;/*MISRA*/}
	}
	//if the command transmission wasn't successfull
	else
	{
		//set the return value to not ok for the manager to handle the error
		RetVar = GSM_Manage_NOK;
		//put the module into sleep 
		GSM_ManageSleep();
	}

	return RetVar;
}





/*
 * This function used to configure the GSM module with the necessary settings
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageStart(void)
{
	//variables declarations 
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate indecate the state of the AT command
    static GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the AT command start transmitting
    static uint16_t Counter = 0;//acounter to count how meny times this function was called to calculate time delays
    static uint8_t State = 0;//a variable to hold the state of the FSM

    //start the FSM
    switch (State)
    {
    	//state 0 establish the communication with "AT" AT command
    	case 0 :
    	{
    		//a condition to start the command transmision onley once	
            if (GSM_Check == GSM_NOK)
            {
            	//wake up the module for communication 
            	GSM_ManageCheck = GSM_ManageWakeUp();
            	
            	//if the wakeup was successfull
            	if (GSM_ManageCheck == GSM_Manage_OK)
            	{
            		//start the command transmission
            		GSM_Check = GSM_ATCommand_AT();
            	}
            	else{;/*MISRA*/}
                
            }
            else {;/*MISRA*/}

            //if the command transmission was successfull
    		if(GSM_Check == GSM_OK)
    		{
    			//check if the commad was executed successfully
    			if(GSM_Flag == GSM_Manage_OK)
    			{
    				//reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //change the state to move to the next command
    				State = 1;
    				//reset the GSM_Flag to the idle value to be able to execute the next commands
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			//if the command done executing but with error 
    			else if (GSM_Flag == GSM_Manage_NOK)
    			{
    				//reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //set the return value to not ok for the manager to handle the error
    				RetVar = GSM_Manage_NOK;
    				//change the state to 0 to be able to start this function from the begining if called again 
    				State = 0;
    				//reset the GSM_Flag to the idle value to be able to execute the next commands
    				GSM_Flag = GSM_Manage_InProgress;

    				//put the module into sleep 
    				GSM_ManageSleep();
    			}
    			else{;/*MISRA*/}
    		}
    		//if the command transmission wasn't successfull
    		else
    		{
    			//set the return value to not ok for the manager to handle the error
    			RetVar = GSM_Manage_NOK;
    			//change the state to 0 to be able to start this function from the begining if called again
    			State = 0;
    			//put the module into sleep 
    			GSM_ManageSleep();
    		}

    		break;	
    	}

    	//state 1 stop the command echoing 
    	case 1 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{
    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_StopEcho();
	            }
	            else {;/*MISRA*/}
    			
    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 2;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}
    		
    		break;	
    	}

    	//state 2 fix the module baudrate
    	case 2 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{
    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_BRFix();
	            }
	            else {;/*MISRA*/}

    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 0;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;
    					//set the return value to ok for the manager to know that the function done execution successully
	    				RetVar = GSM_Manage_OK;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}
    		
    		break;	
    	}

    	default : {;/*MISRA*/}
    }
	
	return RetVar;
}

/*
 * This function used to read the SMS message in index 1  
 *Inputs:
 		- MsgBuffer : a pointer to an array tohold the read SMS
 		- MsgLengrh	: the length of the expected SMS message 
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageReadSMS(uint8_t* MsgBuffer, uint8_t MsgLength)
{
	//variables declarations 
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate indecate the state of the AT command
    static GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the AT command start transmitting
    static uint8_t Counter = 0;//acounter to count how meny times this function was called to calculate time delays
    static uint8_t State = 0;//a variable to hold the state of the FSM
    uint8_t Index;//loop index

	//start the FSM    
    switch (State)
    {
    	//state 0 set the sms format to text mode 
    	case 0 :
    	{

    		//a condition to start the command transmision onley once	
            if (GSM_Check == GSM_NOK)
            {
            	//wake up the module for communication 
            	GSM_ManageCheck = GSM_ManageWakeUp();
            	
            	//if the wakeup was successfull
            	if (GSM_ManageCheck == GSM_Manage_OK)
            	{
            		//start the command transmission
            		GSM_Check = GSM_ATCommand_SMSFormat(TEXT);
            	}
            	else{;/*MISRA*/}
                
            }
            else {;/*MISRA*/}

    		
    		//if the command transmission was successfull
    		if(GSM_Check == GSM_OK)
    		{
    			//check if the commad was executed successfully
    			if(GSM_Flag == GSM_Manage_OK)
    			{
    				//reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //change the state to move to the next command
    				State = 1;
    				//reset the GSM_Flag to the idle value to be able to execute the next commands
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			//if the command done executing but with error 
    			else if (GSM_Flag == GSM_Manage_NOK)
    			{
    				//reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //set the return value to not ok for the manager to handle the error
    				RetVar = GSM_Manage_NOK;
    				//change the state to 0 to be able to start this function from the begining if called again 
    				State = 0;
    				//reset the GSM_Flag to the idle value to be able to execute the next commands
    				GSM_Flag = GSM_Manage_InProgress;

    				//put the module into sleep 
    				GSM_ManageSleep();
    			}
    			else{;/*MISRA*/}
    		}
    		//if the command transmission wasn't successfull
    		else
    		{
    			//set the return value to not ok for the manager to handle the error
    			RetVar = GSM_Manage_NOK;
    			//change the state to 0 to be able to start this function from the begining if called again
    			State = 0;
    			//put the module into sleep 
    			GSM_ManageSleep();
    		}

    		break;	
    	}

    	//state 1 check for recieved SMS
    	case 1 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{

    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_CheckRecievedSMS();
	            }
	            else {;/*MISRA*/}

    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 2;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}

    		break;	
    	}

    	//state 3 read the SMS
    	case 2 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{
    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_ReadSMS(MsgLength);
	            }
	            else {;/*MISRA*/}

    			
    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//exectract the msg from the response
    					for(Index = 0; Index < MsgLength; Index++)
    					{
    						MsgBuffer[Index] = RecievedResponsePtr[RecievedResponseLength - MsgLength + Index];
    					}

    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 3;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;	
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}
    		
    		break;	
    	}

    	//state 3 reset the SMS format to PDU mode
    	case 3 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{
    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_SMSFormat(PDU);
	            }
	            else {;/*MISRA*/}

    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 4;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}

    		break;	
    	}

    	//state 4 delete the recieved SMS
    	case 4 :
    	{
    		//delay between commands 
    		if(Counter < T1000)
    		{
    			Counter++;
    		}

    		//if the delay is done
    		else
    		{
    			//a condition to start the command transmision onley once	
	            if (GSM_Check == GSM_NOK)
	            {
	            	//start the command transmission
	                GSM_Check = GSM_ATCommand_DeleteSMS();
	            }
	            else {;/*MISRA*/}

    			//if the command transmission was successfull
    			if(GSM_Check == GSM_OK)
    			{
    				//check if the commad was executed successfully
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
    					State = 0;
    					//reset the GSM_Flag to the idle value to be able to execute the next commands
    					GSM_Flag = GSM_Manage_InProgress;
    					//set the return value to ok for the manager to know that the function done execution successully
	    				RetVar = GSM_Manage_OK;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				//if the command done executing but with error 
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					//reset GSM_Check value to the idle value to be able to execute the next commands
                    	GSM_Check = GSM_NOK;
                    	//reset the counter to be able to execute other delays
                        Counter = 0;
    					//set the return value to not ok for the manager to handle the error
	    				RetVar = GSM_Manage_NOK;
	    				//change the state to 0 to be able to start this function from the begining if called again 
	    				State = 0;
	    				//reset the GSM_Flag to the idle value to be able to execute the next commands
	    				GSM_Flag = GSM_Manage_InProgress;
	    				//put the module into sleep 
    					GSM_ManageSleep();
    				}
    				else{;/*MISRA*/}
    			}
    			//if the command transmission wasn't successfull
    			else
    			{
    				//reset the counter to be able to execute other delays
                    Counter = 0;
    				//set the return value to not ok for the manager to handle the error
	    			RetVar = GSM_Manage_NOK;
	    			//change the state to 0 to be able to start this function from the begining if called again
	    			State = 0;
	    			//put the module into sleep 
    				GSM_ManageSleep();
    			}
    		}
    		
    		break;	
    	}

    	default : {;/*MISRA*/}
    }
	
	return RetVar;
}

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

static GSM_Manage_CheckType GSM_ManageSetSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	//variables declarations 
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to hold the return value to indecate the state of the function 
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate indecate the state of the AT command
    static GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the AT command start transmitting
    static uint8_t Counter = 0;//acounter to count how meny times this function was called to calculate time delays
    static uint8_t State = 0;//a variable to hold the state of the FSM

    //start the FSM
    switch (State)
    {
    	//state 0 set the sms format to text mode 
        case 0 :
        {

            //a condition to start the command transmision onley once   
            if (GSM_Check == GSM_NOK)
            {
                //wake up the module for communication 
                GSM_ManageCheck = GSM_ManageWakeUp();
                
                //if the wakeup was successfull
                if (GSM_ManageCheck == GSM_Manage_OK)
                {
                    //start the command transmission
                    GSM_Check = GSM_ATCommand_SMSFormat(TEXT);
									GPIO_Write(5,PIN_MASK_12,HIGH);
									GPIO_Write(5,PIN_MASK_13,HIGH);
                }
                else{;/*MISRA*/}
                
            }
            else {;/*MISRA*/}

            
            //if the command transmission was successfull
            if(GSM_Check == GSM_OK)
            {
                //check if the commad was executed successfully
                if(GSM_Flag == GSM_Manage_OK)
                {
									GPIO_Write(5,PIN_MASK_13,LOW);
                    //reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //change the state to move to the next command
                    State = 2;
                    //reset the GSM_Flag to the idle value to be able to execute the next commands
                    GSM_Flag = GSM_Manage_InProgress;
                }
                //if the command done executing but with error 
                else if (GSM_Flag == GSM_Manage_NOK)
                {
                    //reset GSM_Check value to the idle value to be able to execute the next commands
                    GSM_Check = GSM_NOK;
                    //set the return value to not ok for the manager to handle the error
                    RetVar = GSM_Manage_NOK;
                    //change the state to 0 to be able to start this function from the begining if called again 
                    State = 0;
                    //reset the GSM_Flag to the idle value to be able to execute the next commands
                    GSM_Flag = GSM_Manage_InProgress;

                    //put the module into sleep 
                    GSM_ManageSleep();
                }
                else{;/*MISRA*/}
            }
            //if the command transmission wasn't successfull
            else
            {
                //set the return value to not ok for the manager to handle the error
                RetVar = GSM_Manage_NOK;
                //change the state to 0 to be able to start this function from the begining if called again
                State = 0;
                //put the module into sleep 
                GSM_ManageSleep();
            }

            break;  
        }

        //state 1 set the characture set of the module
    	case 1 :
    	{
    		//delay between commands 
            if(Counter < T1000)
            {
                Counter++;
            }

            //if the delay is done
            else
    		{
					
    			//a condition to start the command transmision onley once   
                if (GSM_Check == GSM_NOK)
                {
                    //start the command transmission
                    GSM_Check = GSM_ATCommand_CharSet();
										GPIO_Write(5,PIN_MASK_12,LOW);
										GPIO_Write(5,PIN_MASK_13,HIGH);
                }
                else {;/*MISRA*/}

    			//if the command transmission was successfull
                if(GSM_Check == GSM_OK)
                {
                    //check if the commad was executed successfully
                    if(GSM_Flag == GSM_Manage_OK)
                    {
								GPIO_Write(5,PIN_MASK_13,LOW);
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
											GPIO_Write(5,PIN_MASK_14,LOW);
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
                        State = 2;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                    }
                    //if the command done executing but with error 
                    else if (GSM_Flag == GSM_Manage_NOK)
                    {
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //set the return value to not ok for the manager to handle the error
                        RetVar = GSM_Manage_NOK;
                        //change the state to 0 to be able to start this function from the begining if called again 
                        State = 0;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                        //put the module into sleep 
                        GSM_ManageSleep();
											GPIO_Write(5,PIN_MASK_14,LOW);
                    }
                    else{GPIO_Write(5,PIN_MASK_14,HIGH);}
                }
                //if the command transmission wasn't successfull
                else
                {
                    //reset the counter to be able to execute other delays
                    Counter = 0;
                    //set the return value to not ok for the manager to handle the error
                    RetVar = GSM_Manage_NOK;
                    //change the state to 0 to be able to start this function from the begining if called again
                    State = 0;
                    //put the module into sleep 
                    GSM_ManageSleep();
                }
            }
    		
    		break;	
    	}

    	//state 2 set the phone number to send the SMS to
    	case 2 :
    	{
    		//delay between commands 
            if(Counter < T1000)
            {
                Counter++;
            }

            //if the delay is done
            else
    		{
    			//a condition to start the command transmision onley once   
                if (GSM_Check == GSM_NOK)
                {
                    //start the command transmission
                    GSM_Check = GSM_ATCommand_SetSMSMobNum (PhoneNum);
									GPIO_Write(5,PIN_MASK_12,HIGH);
									GPIO_Write(5,PIN_MASK_13,HIGH);
                }
                else {;/*MISRA*/}

    			//if the command transmission was successfull
                if(GSM_Check == GSM_OK)
                {
                    //check if the commad was executed successfully
                    if(GSM_Flag == GSM_Manage_OK)
                    {
								GPIO_Write(5,PIN_MASK_13,LOW);
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
                        State = 3;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;   
                    }
                    //if the command done executing but with error 
                    else if (GSM_Flag == GSM_Manage_NOK)
                    {
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //set the return value to not ok for the manager to handle the error
                        RetVar = GSM_Manage_NOK;
                        //change the state to 0 to be able to start this function from the begining if called again 
                        State = 0;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                        //put the module into sleep 
                        GSM_ManageSleep();
                    }
                    else{;/*MISRA*/}
                }
                //if the command transmission wasn't successfull
                else
                {
                    //reset the counter to be able to execute other delays
                    Counter = 0;
                    //set the return value to not ok for the manager to handle the error
                    RetVar = GSM_Manage_NOK;
                    //change the state to 0 to be able to start this function from the begining if called again
                    State = 0;
                    //put the module into sleep 
                    GSM_ManageSleep();
                }
            }
    		
    		break;	
    	}

    	//state 3 set the message to be sent
    	case 3 :
    	{
    		//delay between commands 
            if(Counter < T1000)
            {
                Counter++;
            }

            //if the delay is done
            else
    		{
    			//a condition to start the command transmision onley once   
                if (GSM_Check == GSM_NOK)
                {
                    //start the command transmission
                    GSM_Check = GSM_ATCommand_SetSMSWriteMsg(Msg, MsgLength);
									GPIO_Write(5,PIN_MASK_12,LOW);
									GPIO_Write(5,PIN_MASK_13,HIGH);
                }
                else {;/*MISRA*/}
    			
    			//if the command transmission was successfull
                if(GSM_Check == GSM_OK)
                {
                    //check if the commad was executed successfully
                    if(GSM_Flag == GSM_Manage_OK)
                    {
								GPIO_Write(5,PIN_MASK_13,LOW);
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
                        State = 4;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                    }
                    //if the command done executing but with error 
                    else if (GSM_Flag == GSM_Manage_NOK)
                    {
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //set the return value to not ok for the manager to handle the error
                        RetVar = GSM_Manage_NOK;
                        //change the state to 0 to be able to start this function from the begining if called again 
                        State = 0;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                        //put the module into sleep 
                        GSM_ManageSleep();
                    }
                    else{;/*MISRA*/}
                }
                //if the command transmission wasn't successfull
                else
                {
                    //reset the counter to be able to execute other delays
                    Counter = 0;
                    //set the return value to not ok for the manager to handle the error
                    RetVar = GSM_Manage_NOK;
                    //change the state to 0 to be able to start this function from the begining if called again
                    State = 0;
                    //put the module into sleep 
                    GSM_ManageSleep();
                }
            }
    		
    		break;	
    	}

    	//state 3 reset the SMS format to PDU mode
    	case 4 :
    	{
    		//delay between commands 
            if(Counter < T1000)
            {
                Counter++;
            }

            //if the delay is done
            else
            {
                //a condition to start the command transmision onley once   
                if (GSM_Check == GSM_NOK)
                {
                    //start the command transmission
                    GSM_Check = GSM_ATCommand_SMSFormat(PDU);
									GPIO_Write(5,PIN_MASK_12,HIGH);
									GPIO_Write(5,PIN_MASK_13,HIGH);
                }
                else {;/*MISRA*/}

    			//if the command transmission was successfull
                if(GSM_Check == GSM_OK)
                {
                    //check if the commad was executed successfully
                    if(GSM_Flag == GSM_Manage_OK)
                    {
									GPIO_Write(5,PIN_MASK_13,LOW);
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //change the state to move to the next command
                        State = 0;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                        //set the return value to ok for the manager to know that the function done execution successully
                        RetVar = GSM_Manage_OK;
                        //put the module into sleep 
                        GSM_ManageSleep();
                    }
                    //if the command done executing but with error 
                    else if (GSM_Flag == GSM_Manage_NOK)
                    {
                        //reset GSM_Check value to the idle value to be able to execute the next commands
                        GSM_Check = GSM_NOK;
                        //reset the counter to be able to execute other delays
                        Counter = 0;
                        //set the return value to not ok for the manager to handle the error
                        RetVar = GSM_Manage_NOK;
                        //change the state to 0 to be able to start this function from the begining if called again 
                        State = 0;
                        //reset the GSM_Flag to the idle value to be able to execute the next commands
                        GSM_Flag = GSM_Manage_InProgress;
                        //put the module into sleep 
                        GSM_ManageSleep();
                    }
                    else{;/*MISRA*/}
                }
                //if the command transmission wasn't successfull
                else
                {
                    //reset the counter to be able to execute other delays
                    Counter = 0;
                    //set the return value to not ok for the manager to handle the error
                    RetVar = GSM_Manage_NOK;
                    //change the state to 0 to be able to start this function from the begining if called again
                    State = 0;
                    //put the module into sleep 
                    GSM_ManageSleep();
                }
            }
    		
    		break;	
    	}

    	default : {;/*MISRA*/}
    }

	return RetVar;
}


/**********************************************************************************************/


