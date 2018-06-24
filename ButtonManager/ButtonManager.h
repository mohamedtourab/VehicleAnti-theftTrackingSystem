#ifndef	BUTTON_MANAGER_H
#define	BUTTON_MANAGER_H

#include "GPIO.h"
#include "ButtonManager_Config.h"

// enum of the button states
typedef enum {OFF = 0, JUST_PRESSED, ON, JUST_RELEASED}ButtonStates;

// enum for the return value of the functions
typedef enum {BUTTON_OK = 0, BUTTON_NOK}ButtonCheckType;

// structure for button configuration
typedef struct 
{
	uint8_t	ChannelId;	// the index of the button in the configuration file of GPIO
	uint16_t PinMask;	// the pin mask of the used pin
	/*
		the allowed values:
		PUPD_PULLUP		0x55555555U
		PUPD_PULLDOWN	0xAAAAAAAAU
	*/
	uint32_t PUPD;		// the initial state of the button either pulled up or pull down
}ButtonConfigParameters;

// initialize the state of the button
void Button_Init (void);

/* function to get the state of the button either on or off
 * Inputs:
 		- ButtonChannelID: the index of the required button in the ButtonManager_Config.c file
 		- State 	: pointer to buttonstate variable to return the state of the required button
 			. OFF => Button is not presssed.
 			. JUST_PRESSED
 			. JUST_RELEASED
 			. ON  => Button is pressed
 * Outputs:
 		- BUTTON_OK => if the entered channel id is valid
 		- BUTTON_NOK => if the entered channel id is invalid
*/
ButtonCheckType GetButtonState (uint8_t ButtonChannelId, ButtonStates* BState);

/* Cyclic function to manage the states of the button
 * Inputs:
  		- ButtonChannelId: the index of the required button in the ButtonManager_Config.c file
 * Outputs:
 		- BUTTON_OK => if the entered channel id is valid
 		- BUTTON_NOK => if the entered channel id is invalid
*/
ButtonCheckType ButtonManage (uint8_t ButtonChannelId);

#endif