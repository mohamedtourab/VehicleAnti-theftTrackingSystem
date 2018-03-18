#include "GPS.h"

const GPS_Config GPS_Parameter = 
{
	0,	// The index of the UART channel in the Configuration Parameter array defined in UART_Cfg.c
	0,	// The index of the GPIO pins channel in the configuration parameter defined in GPIO_Cfg.c
	0,	// The pin mask used for Force pin
	0,	// The pin mask used for Reset pin
	0,	// CallBack function that will be executed in case of finding the data in GPS readeings
	0	// CallBack function that will be executed in case the data is not found
}