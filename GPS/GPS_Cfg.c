#include "GPS.h"
#include "GPIO.h"
void GPS_CallBack (void);
void Error_CallBack(void);
const GPS_Config GPS_Parameter = 
{
	1,	// The index of the UART channel in the Configuration Parameter array defined in UART_Cfg.c
	2,	// The index of the Force pin channel in the configuration parameter defined in GPIO_Cfg.c
	PIN_MASK_13,	// The pin mask used for Force pin
	3, // The index of the Reset pin channel in the configuration parameter defined in GPIO_Cfg.c
	PIN_MASK_2,	// The pin mask used for Reset pin
	GPS_CallBack,	// CallBack function that will be executed in case of finding the data in GPS readeings
	Error_CallBack	// CallBack function that will be executed in case the data is not found
};