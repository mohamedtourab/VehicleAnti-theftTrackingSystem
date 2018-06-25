
#include "BT_Manager.h"
#include "delay.h"

uint8_t TxDone = 0;
uint8_t RxDone = 0;
uint8_t flag =0;


int main (void)
{
    GPIO_Init();
	UART_Init();
	BT_Init();
	BT_CheckType y = BT_BUSY;
	uint8_t DataReceived[4] = {'\0','\0','\0','\0'};
	uint8_t x = 0;
	
	while(BT_Configure()!=BT_OK)
	{
		UART_ManageOngoingOperation(0);
	}
	GPIO_Write(1,PIN_MASK_12,HIGH);
	

	
	
	while(1)
	{
		UART_ManageOngoingOperation(0);
		
		switch(x)
		{
			case 0:
			{
				if( (BT_GetData(DataReceived,1) == BT_OK) && (DataReceived[0] == 'h') )
				{
					x =1;
				}
				else
				{
					x = 0;
				}	
			}
			break;

			case 1:
			{
				y = BT_GetConnectionStatus(); 
				if(y == BT_OK)
				{
					//GPIO_Write(1, PIN_MASK_14,HIGH);
					x = 2;
				}
				else if(y == BT_NOK)
				{
					//GPIO_Write(1, PIN_MASK_15,HIGH);
					x = 2;
				}
			}
			break;
			case 2:
			{
				if( (BT_GetData(DataReceived,1) == BT_OK) && (DataReceived[0] == 'k') )
				{
					x = 3;
					
				}
			}
			break;
			case 3:
			{
				y = BT_StopCommunication();
				
				if( (y == BT_OK) || (y == BT_NOK) )
				{
					x = 4;
					GPIO_Write(1, PIN_MASK_15,HIGH);
				}
			}
			break;
			case 4:
			{
				Delay_ms(500);
				x=5;
			}
			break;
			case 5:
			{
				y = BT_GetConnectionStatus(); 
				if(y == BT_OK)
				{
					GPIO_Write(1, PIN_MASK_14,HIGH);
					x = 5;
				}
				else if(y == BT_NOK)
				{
					GPIO_Write(1, PIN_MASK_13,HIGH);
					x = 5;
				}
			}
			break;
			
			default: break;
		}
	}
}




