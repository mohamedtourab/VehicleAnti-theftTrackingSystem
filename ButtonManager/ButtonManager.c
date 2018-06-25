#include "ButtonManager.h"

// array of structures which contain the Button configuration parameters
extern ButtonConfigParameters	Buttons[NUMBER_OF_BUTTONS];
// global variable to hold the state of the button
static ButtonStates	State[NUMBER_OF_BUTTONS];
// global variable to get the initial values of all button
static uint8_t InitialValues[NUMBER_OF_BUTTONS];


// initialize the state of the button
void Button_Init (void)
{
	ButtonConfigParameters* ButtonPtr;
	// initialize the state of the buttons to be off
	uint8_t LoopIndex = 0;
	for (LoopIndex = 0; LoopIndex < NUMBER_OF_BUTTONS; LoopIndex++)
	{
		ButtonPtr = &Buttons[LoopIndex];
		// initialize the state to be OFF
		State[LoopIndex] = 0;
		// variable to get the initial value of the pin
		// initially '1' in case of pullup
		// initially '0' in case of pulldown 
		if ((ButtonPtr->PUPD) == PUPD_PULLUP)
		{
			InitialValues[LoopIndex] = 1;
		}
		else
		{
			InitialValues[LoopIndex] = 0;
		}
	}
}

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
ButtonCheckType GetButtonState (uint8_t ButtonChannelId, ButtonStates* BState)
{
	ButtonCheckType RetVar;
	// check the validity of the entered channel Id
	if (ButtonChannelId < NUMBER_OF_BUTTONS)
	{
		// return the state of the required button
		*BState = State[ButtonChannelId];
		RetVar = BUTTON_OK;
	}
	else
	{
		RetVar = BUTTON_NOK;
	}
	return RetVar;
}

/* Cyclic function to manage the states of the button
 * Inputs:
  		- ButtonChannelId: the index of the required button in the ButtonManager_Config.c file
 * Outputs:
 		- BUTTON_OK => if the entered channel id is valid
 		- BUTTON_NOK => if the entered channel id is invalid
*/
ButtonCheckType ButtonManage (uint8_t ButtonChannelId)
{
	ButtonCheckType RetVar;
	uint16_t PortData;
	uint8_t PinData; 
	ButtonConfigParameters* ButtonPtr;
	// check the validity of the entered channel id
	if (ButtonChannelId < NUMBER_OF_BUTTONS)
	{
		ButtonPtr = &Buttons[ButtonChannelId];
		// read the value of the port
		GPIO_Read((ButtonPtr->ChannelId), &PortData);
		// mask the data was read to get the value of the pin by masking the register by the enetered Pin Mask
		// if Non zero
		if ((PortData & (ButtonPtr->PinMask)))
		{
			PinData = 1;
		}
		// if zero
		else
		{
			PinData = 0;
		}
		// switch the state of the required button
		switch (State[ButtonChannelId])
		{
			// in case of it is not pressed
			case OFF:
			{
				// check the Pin value
				// if it is not equal the initial value
				if (PinData != InitialValues[ButtonChannelId])
				{
					// change state to be JUST_PRESSED
					State[ButtonChannelId] = JUST_PRESSED;
				}
				else{/*State still Off*/}
			}
			break;
			// in case of just pressed
			case JUST_PRESSED:
			{
				// check the Pin value
				// if it still is not equal the initial value
				if (PinData != InitialValues[ButtonChannelId])
				{
					// change state to be ON
					State[ButtonChannelId] = ON;
				}
				// if it is equal the initial value
				else
				{
					// change state to be OFF
					State[ButtonChannelId] = OFF; 
				}
			}
			break;
			// in case of on 
			case ON:
			{
				// check the Pin value
				// if it is  equal the initial value
				if (PinData == InitialValues[ButtonChannelId])
				{
					// change state to be JUST_RELEASED
					State[ButtonChannelId] = JUST_RELEASED;
				}
				else{/*state remains ON*/}
			}
			break;
			// in case of JUST_RELEASED
			case JUST_RELEASED:
			{
				// check the Pin value
				// if it still is equal the initial value
				if (PinData == InitialValues[ButtonChannelId])
				{
					// change state to be OFF
					State[ButtonChannelId] = OFF;
				}
				// if it is not equal the initial value
				else
				{
					State[ButtonChannelId] = ON;
				}
			}
			break;
			default:{/* All Button States is covered and the default case is OFF*/}
		}
		RetVar = BUTTON_OK;
	}
	else
	{
		RetVar = BUTTON_NOK;
	}
	return RetVar;
}