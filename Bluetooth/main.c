
#include "BT_Manager.h"
//#include "stm32f407xx.h"




void main (void)
{
    unsigned char ReceivedData[6]={0,0,0,0,0,0};

    GPIO_Init();
    Delay_ms(100);
    UART_Init();
    BT_Init();
    Delay_ms(600);

        while(BT_Configure() == BT_NOK);
        GPIO_Write(0,1);
    while(BT_GetData(ReceivedData,1)==BT_NOK);



    GPIO_Write(1,1);

    if(ReceivedData[0] == 'h')
    {
        GPIO_Write(2,1);
    }

     while(BT_SendData("WELCOME\n",8)==BT_NOK);

        GPIO_Write(3,1);
     while(BT_SendData("Enter your user name: ",22)==BT_NOK);

     GPIO_Write(4,1);

    while(BT_GetData(ReceivedData,6)==BT_NOK);

    if(ReceivedData[0]=='u'&&ReceivedData[1]=='s'&&ReceivedData[2]=='e'&&ReceivedData[3]=='r'&&ReceivedData[4]=='0'&&ReceivedData[5]=='0')
    {
        GPIO_Write(5,1);
    }

   while(BT_SendData("User Found !",12)==BT_NOK);



}

