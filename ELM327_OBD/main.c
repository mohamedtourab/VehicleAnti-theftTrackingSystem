#include "GPIO.h"
#include "UART.h"
#include "delay.h"
#include "ELM327.h"



void main (void)
{
	int16_t VehicleSpeed=0;
	int16_t VehicleRPM=0;
	uint8_t BatteryState=0;


	GPIO_Init();
	Delay_ms(10);
	UART_Init();
	Delay_ms(10);
	ELM327_Init();

		ELM327_GetVehicleSpeed(&VehicleSpeed);
		ELM327_GetVehicleRPM(&VehicleRPM);
		ELM327_GetVehicleBatteryState(&BatteryState);


	while(1)
	{

		ELM327_ManageOngoingOperation();

		Delay_ms(10);
	}
}