#include "GPIO.h"
#include "UART.h"
#include "delay.h"
#include "GPS.h"
#include "Parse.h"

uint8_t FoundFlag = 0;
uint8_t ErrorFlag = 0;
uint8_t UART1_Flag = 0;
void Test (void)
{
	UART1_Flag = 1;
}

void GPS_CallBack (void)
{
	FoundFlag = 1;
}

void Error_CallBack(void)
{
	ErrorFlag = 1;
	GPIO_Write(4, PIN_MASK_12, HIGH);
}


int main (void)
{
	uint8_t State = 0;
	Location Map = {{0},0,{0},0};
	GPIO_Init();
	Delay_ms(10);
	GPS_Init();
	UART_Init();

	StartRead();
	while (1)
	{
		switch (State)
		{
			case 0:
			{
				if (FoundFlag == 0)
				{
					GPS_ManagOnGoingOperation();
					Delay_ms(10);
				}
				else
				{
					FoundFlag = 0;
					GetData(&Map);
					State = 1;
				}
			}
			break;
			case 1:
			{
				UART_StartSilentTransmission("Longitude:",10,0);
				State = 2;
			}
			break;
			case 2:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission(Map.Longitude,9,0);
					State = 3;
				}
			}
			break;
			case 3:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission(&(Map.LongitudeDir),1,0);
					State = 4;
				}
			}
			break;
			case 4:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission("\r\n",2,0);
					State = 5;
				}
			}
			break;
			case 5:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission("Latitude:",9,0);
					State = 6;
				}
			}
			break;
			case 6:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission(Map.Latitude,9,0);
					State = 7;
				}
			}
			break;
			case 7:
			{
				if (UART1_Flag != 0)
				{
					UART1_Flag = 0;
					UART_StartSilentTransmission(&(Map.LatitudeDir),1,0);
					State = 8;
				}
			}
			break;
			case 8:
			{}
		}
	}

}