#include "ButtonManager.h"
#include "ButtonManager_Config.h"

ButtonConfigParameters	Buttons[NUMBER_OF_BUTTONS]=
{
	{
		2,				// Channel ID of the button in GPIO_Cfg.c
		PIN_MASK_10,		// the pin mask of the used pin
		PUPD_PULLUP		// the state of button pulled up or down
	}
};
