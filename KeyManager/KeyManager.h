/*******************************************************************************************************************
*		 File name: KeyManager.h
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


#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "GPIO.h"
#include "UART.h"
#include "ButtonManager.h"
#include "KeyManager_Config.h"
#include "NVM_Manager.h"//////////////////////////

// typedef for the callback function to be executed in case of valid or invalid location
typedef void (*KeyManager_CallBackPtr)(void);


// structure to get the configuration from user
typedef struct 
{
	// Pin to be checked to change the Key
	uint8_t ButtonChannelId;

	// Channel Id of the used UART to enter the new Key
	uint8_t UART_ChannelId;

	// the index of the required block in NVM to write the ket
	uint8_t NVM_BlockId;

	// pointer to Function to be executed if we can change the key
	KeyManager_CallBackPtr NVM_WriteDone;

}KeyConfigType;


// Function to initialize the state
void KeyInit (void);

// Function to Change the key when the pushbutton pressed
void KeyChangeManage (void);

// callback function of the UART reception
void UART_ReceptionDone (void);

// callback function of NVM in case of successful writing
void NVM_WriteSeccess (void);

// callback function of NVM in case of failure writing
void NVM_WriteFail (void);
#endif