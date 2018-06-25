#include "I2C_Manager.h"
#include "delay.h"

static uint8_t TransmissionDone = 0;
static uint8_t ReceptionDone = 0;

void I2C_TxDone(void)
{
    TransmissionDone = 1;
}
void I2C_RxDone(void)
{
    ReceptionDone = 1;
}

int main(void)
{
    uint8_t ReceivedData=0;
    uint8_t ReceivedArr[7] = {0,0,0,0,0,0,0};
    uint8_t Data[8]={0x78,0x66,0x23,0x45,0x87,0x66,0x66,0x66};
    uint8_t SlaveAddress = 0xA0;
    
	/* For writing single/multi-byte and reading single byte put x = 0 */
    /* For only writing single/multi-byte put x = 10 */
    /* For only reading single byte put x = 13 */
    /* For only reading multi-bytes put x = 19 */
    
	uint8_t x = 19;

    GPIO_Init();
    I2C_Init();
	
    while(1)
    {
        I2C_Manager();
        
        switch(x)
        {
            case 0:
            {
                I2C_RequestWrite(SlaveAddress, Data, 2);
                x = 1;
            }
            break;

            case 1:
            {
                if(TransmissionDone == 1)
                {
                    TransmissionDone = 0;
                    x = 2;
                }
                else
                {
                    x = 1;
                }
            }
            break;

            case 2:
            {
				I2C_GenerateStop(0);
				GPIO_Write(5,1);
                x = 3;
            }
            break;

			case 3:
			{
				Delay_ms(5);
				x = 4;
			}
			break;
			
			case 4:
            {
                I2C_RequestWrite(SlaveAddress, Data, 1);
                x = 5;
            }
            break;
			
			case 5:
			{
				if(TransmissionDone == 1)
                {
                    TransmissionDone = 0;
                    x = 6;
                }
                else
                {
                    x = 5;
                }
			}
			break;

			
			case 6:
			{
				I2C_RequestRead(SlaveAddress, &ReceivedData, 1);
				x = 7;
			}
            break;
			
			case 7:
			{
				if(ReceptionDone == 1)
				{
					ReceptionDone = 0;
					x = 8;
				}
				else
				{
					x = 7;
				}
			}
			break;
			
			case 8:
			{
				GPIO_Write(2,1);
                x = 9;
			}
			break;
			
			case 9:
			{
				if(ReceivedData == 0x66)
				{
					GPIO_Write(4,1);
					x = 255;
				}
			}
			break;
/************************************************************************/
            case 10:
            {
                I2C_RequestWrite(SlaveAddress, Data, 8);
                x = 11;
            }
            break;

            case 11:
            {
                if(TransmissionDone == 1)
                {
                    TransmissionDone = 0;
                    x = 12;
                }
                else
                {
                    x = 11;
                }
            }
            break;

            case 12:
            {
				I2C_GenerateStop(0);
				GPIO_Write(5,1);
                x = 255;
            }
            break;
/********************************************************************************/
            case 13:
            {
                I2C_RequestWrite(SlaveAddress, Data, 1);
                x = 14;
            }
            break;
			
			case 14:
			{
				if(TransmissionDone == 1)
                {
                    TransmissionDone = 0;
                    x = 15;
                }
                else
                {
                    x = 14;
                }
			}
			break;

			
			case 15:
			{
				I2C_RequestRead(SlaveAddress, &ReceivedData, 1);
				x = 16;
			}
            break;
			
			case 16:
			{
				if(ReceptionDone == 1)
				{
					ReceptionDone = 0;
					x = 17;
				}
				else
				{
					x = 16;
				}
			}
			break;
			
			case 17:
			{
				GPIO_Write(2,1);
                x = 18;
			}
			break;
			
			case 18:
			{
				if(ReceivedData == 0x66)
				{
					GPIO_Write(4,1);
					x = 255;
				}
			}
			break;
/***********************************************************************************/
            case 19:
            {
                I2C_RequestWrite(SlaveAddress, Data, 1);
                x = 20;
            }
            break;
			
			case 20:
			{
				if(TransmissionDone == 1)
                {
                    TransmissionDone = 0;
                    x = 21;
                }
                else
                {
                    x = 20;
                }
			}
			break;

			
			case 21:
			{
				I2C_RequestRead(SlaveAddress, ReceivedArr, 6);
				x = 22;
			}
            break;
			
			case 22:
			{
				if(ReceptionDone == 1)
				{
					ReceptionDone = 0;
					x = 23;
				}
				else
				{
					x = 22;
				}
			}
			break;
			
			case 23:
			{
				GPIO_Write(2,1);
                x = 24;
			}
			break;
			
			case 24:
			{
				if( (ReceivedArr[0] == 0x66) &&
                    (ReceivedArr[1] == 0x23) &&
                    (ReceivedArr[2] == 0x45) &&
					(ReceivedArr[3] == 0x87) &&
				    (ReceivedArr[4] == 0x66) &&
				    (ReceivedArr[5] == 0x66) &&
					(ReceivedArr[5] == 0x66)	)
				{
					GPIO_Write(4,1);
					x = 255;
				}
			}
			break;
        }

    }
	return 0;
}		

