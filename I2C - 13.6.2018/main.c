#include "I2C_Manager.h"

void main(void)
{
    uint8_t DataToBeSent = 0x55;
    uint8_t SlaveAddress = 0xA0;
    uint8_t LocationAddress = 0xBB;
    uint8_t ReceivedData = 0;

    GPIO_Init();

    I2C_Init();

    I2C_SendData(0,DataToBeSent,1,LocationAddress,SlaveAddress);
    while(I2C_Manager()	!= I2C_OK);
	  GPIO_Write(2,1);//GREEN LED

	
	
   I2C_SendData(0,DataToBeSent,1,0xBC,SlaveAddress);
    while(I2C_Manager()	!= I2C_OK);
	  GPIO_Write(3,1); //ORANGE LED

	
    I2C_Receive(0,&ReceivedData,1,LocationAddress,SlaveAddress);
    while(I2C_Manager() != I2C_OK);
		
		
	if(ReceivedData == DataToBeSent)
    GPIO_Write(4,1); ///RED LED
	
	else if(ReceivedData == (0xA1))
		GPIO_Write(5,1); ///BLUE LED


}