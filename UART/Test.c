#include "UART.h"
#include "GPIO.h"

int main (void)
{
        UART_ChkType Ret;
        uint8_t Txx[6] = "Hello";
        uint8_t RX;
        uint32_t Count=0;
        GPIO_Init();
        Ret = UART_Init();


        if (Ret == UART_OK)
        {

                UART_StartSilentTransmission ("LAB",3,0);

                while (Count <3)
                {
                        UART_GetNumberOfTxBytes(0,&Count);
                        UART_ManageOngoingOperation(0);
                        Delay_ms(500);

                }

                while (1)
                {

                        Count = 0;
                        Ret = UART_StartSilentReception(&RX,1,0);
                        if (Ret == UART_OK)
                        {

                                while (Count < 1)
                                {
                                        UART_GetNumberOfRxBytes(0,&Count);
                                        UART_ManageOngoingOperation(0);
                                }


                        }
                        Count = 0;
                        UART_StartSilentTransmission (&RX,1,0);

                        while (Count<1)
                        {
                                UART_GetNumberOfTxBytes(0,&Count);
                                UART_ManageOngoingOperation(0);
                        }
                }
                
        }
        return 0;
}