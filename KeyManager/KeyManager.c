/*******************************************************************************************************************
*		 File name: KeyManager.c
*        Author: Moamen Ali & Moamen Ramadan
*        Date: 16/6/2018
*        Description:
*				Key manager is used to change the USED key in the encryption algorithm for authentication.
*				When the user need to change the USED key, there is a button to be pressed to notify the 
*	 			system to listen to UART and get the new key from the user. So, in the configuration file
*				user should determine the used UART and the pin where the button is connected and aslo 
*			    the length of the key.
*				The key enetered through UART will be stored in a predetermined location in the EEPROM 
*				to be read by the other layes to check the authentications
*				to reset the default key user should enter zeros.
*		USAGE:
*				1- Configure the Parameter in the Config file which are the used UART and button and key length.
*				2- KeyInit function is used to Initialize the used states and global variables.
*				3- KeyChangeManage is the periodic function that is called to handle checking
*				   the state of the button and start reception from UART in case of presseing the button and 
*				   then try to store the key in the predetermined location. in case of valid location we store
*				   the key and execute the callback function predetermined in the configuration file. 
*				   also we store the status of the used key either default or new key in EEPROM.
*******************************************************************************************************************/

#include "KeyManager.h"
#include "KeyManager_Config.h"

#define TIMEOUT 100000U


// helper functions states
typedef enum {KEY_CMD = 0, KEY_WAIT}KeyHelperFnStates;

// enum for the return type of helper functions
typedef enum {KEY_OK = 0, KEY_BUSY, KEY_NOK}Key_CheckType;

// States of the manager
typedef enum {IDLE = 0, UART_WAIT, NVM_WRITE}KeyManagerStates;


// Flag which is set in the reception callback function
static uint8_t ReceptionDoneFlag;
// flag which is set in the NVM success callback function
static uint8_t NVM_SuccesWrite;
// flag which is set in the NVM failure callback function
static uint8_t NVM_FailWrite;
// variable to controle the state machine
static KeyManagerStates KeyState;
// global variable to store the helper functions state
static KeyHelperFnStates HelperState;
// variable to read the state of the button
static ButtonStates Button;
// structure in which user set the configuration parameter
extern KeyConfigType KeyConfigurationParameter;


static Key_CheckType StartUARTReception (uint8_t* KeyPtr);
static Key_CheckType WriteNewKey (uint8_t* KeyPtr);

// Function to initialize the state
void KeyInit (void)
{
	ReceptionDoneFlag = 0;
	NVM_SuccesWrite = 0;
	NVM_FailWrite = 0;
	KeyState = IDLE;
	Button = 0;
	HelperState = KEY_CMD;
}

// Function to Change the key when the pushbutton pressed
void KeyChangeManage (void)
{
	// array of characters to store the new key
	static uint8_t Key[KEY_LENGTH];
	Key_CheckType Check = KEY_BUSY;

	switch (KeyState)
	{
		// State IDLE in which we wait the Pin pressed to start receiving the key from UART
		case IDLE:
		{
			// check the state of the Button
			GetButtonState ((KeyConfigurationParameter.ButtonChannelId), &Button);
			// if Pressed Button = ON
			if (Button == JUST_PRESSED)
			{
				// Change the state to wait the reception finish
				KeyState  = UART_WAIT;
			}
			else {/*Pin is not Pressed*/}
		}
		break;

		// State wait in which we wait the reception from UART finish
		case UART_WAIT:
		{
			Check = StartUARTReception (Key);
			// check if the UART finish reception or timeout error ocuured
			// if reception finished
			if (Check == KEY_OK)
			{
				// go to NVM rWrite state 
				KeyState = NVM_WRITE;
			}
			// if timeout error occured
			else if (Check == KEY_NOK)
			{
				// go to IDLE state
				KeyState = IDLE;
			}
			else {/*The UART is busy so wait till UART finish reception or timeout ocuures*/}
		}
		break;

		// State NVM_WRITE in which we try to write the received data from UART in the required location in EEPROM
		case NVM_WRITE:
		{
			// try to write in the non volatile memory in the block
			Check = WriteNewKey (Key);
			// check if the NVM finish Writing or timeout error ocuured
			// if writing finished
			if (Check == KEY_OK)
			{
				// execute the succes callback function
				KeyConfigurationParameter.NVM_WriteDone();
				KeyState  = IDLE;
			}
			// if timeout error occured
			else if (Check == KEY_NOK)
			{
				KeyState  = IDLE;
			}
			else {/*The NVM is busy so wait till NVM finish writing or timeout ocuures*/}	
		}
		break;

		default:{/*default state is the IDLE State*/}
	}
}


// CallBack function which is executed when UART finish reception
void UART_ReceptionDone (void)
{
	ReceptionDoneFlag = 1;
}

// callback function of NVM in case of successful writing
void NVM_WriteSeccess (void)
{
	NVM_SuccesWrite = 1;
}

// callback function of NVM in case of failure writing
void NVM_WriteFail (void)
{
	NVM_FailWrite = 1;
}


// Helper function to start reception from UART
static Key_CheckType StartUARTReception (uint8_t* KeyPtr)
{
	static uint32_t TimeOutCounter;
	Key_CheckType  RetVar;
	switch (HelperState)
	{
		case KEY_CMD:
		{
			// request reception from UART
			UART_StartSilentReception(KeyPtr, KEY_LENGTH, KeyConfigurationParameter.UART_ChannelId);
			// change state to wait the callback from UART manager
			HelperState = KEY_WAIT;
		}
		break;

		case KEY_WAIT:
		{
			// check the time out error counter
			// if it does not reach the predetermined time out
			if (TimeOutCounter < (TIMEOUT/KEY_MANAGER_CYCLIC_CTIME))
			{
				// check if the callback function is executed or no
				if (ReceptionDoneFlag)
				{
					// clear the flag
					ReceptionDoneFlag = 0;
					HelperState = KEY_CMD;
					RetVar = KEY_OK;
				}
				else{/*wait the call back from UART manager*/}
				TimeOutCounter++;
			}
			// if the counter reaches the predetermined timeout
			else
			{
				TimeOutCounter = 0;
				HelperState = KEY_CMD; 
				RetVar = KEY_NOK;
			}
		}
		break;
	}
	return RetVar;
}

static Key_CheckType WriteNewKey (uint8_t* KeyPtr)
{
	static uint32_t TimeOutCounter;
	Key_CheckType  RetVar;
	switch (HelperState)
	{
		case KEY_CMD:
		{
			// call NVM API to write the new key
			NVM_Write(KeyConfigurationParameter.NVM_BlockId, KeyPtr);
			// change state to wait the callback from NVM manager
			HelperState = KEY_WAIT;
		}
		break;

		case KEY_WAIT:
		{
			// check the time out error counter
			// if it does not reach the predetermined time out
			if (TimeOutCounter < (TIMEOUT/KEY_MANAGER_CYCLIC_CTIME))
			{
				// check if the callback function is executed or no
				if (NVM_SuccesWrite)
				{
					// clear the flag
					NVM_SuccesWrite = 0;
					HelperState = KEY_CMD;
					RetVar = KEY_OK;
				}
				else if (NVM_FailWrite)
				{
					// clear the flag
					NVM_FailWrite = 0;
					HelperState = KEY_CMD;
					RetVar = KEY_NOK;
				}
				else{/*wait the call back from NVM manager*/}
				TimeOutCounter++;
			}
			// if the counter reaches the predetermined timeout
			else
			{
				TimeOutCounter = 0;
				HelperState = KEY_CMD; 
				RetVar = KEY_NOK;
			}
		}
		break;
	}
	return RetVar;	
}