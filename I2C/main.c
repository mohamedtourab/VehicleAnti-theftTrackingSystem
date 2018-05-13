#include "I2C_Driver.h"
#include "I2C_Manager.h"

int main(void)
{
        ReturnStruct WriteReturn;
        ReturnStruct ReadReturn;
        uint8_t DataOutput=0;

        GPIO_Init();
				I2C_Init();
        GPIO_Write(2,1);//Green
        do
        {
                WriteReturn = I2C_Write(0xA0,0x07,0x52,0);
        }while(WriteReturn.CurrentState != I2C_WRITE_DONE);
        GPIO_Write(3,1);//Orange
        do
        {
						ReadReturn = I2C_Read(0xA0,0x07,&DataOutput,0);
        }while(ReadReturn.CurrentState != I2C_READ_DONE);
				if(DataOutput==0x52)
				{
					GPIO_Write(4,1);//Red
				}
				else if(DataOutput==0xA1)
				{
					GPIO_Write(5,1);//Blue
				}
       
       

 }