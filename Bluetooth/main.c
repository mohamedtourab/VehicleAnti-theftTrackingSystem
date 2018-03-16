
#include "GPIO.h"
#include "UART.h"
//#include "stm32f407xx.h"
int Bluetooth_Configure(void);

static uint8_t TransmissionDone = 0;
static uint8_t ReceptionDone = 0;
//volatile uint8_t State = 0;


typedef enum
{
    CHECK_TRANSMISSION_DONE = 0,
    CHECK_RECEPTION_DONE = 1,
    SET_COMMAND_MODE = 2,
    SET_FACTORY_SETTINGS = 3,
    SET_DEVICE_NAME = 4,
    SET_SLAVE_MODE = 5,
    SET_NO_AUTHENTICATION = 6,
    SET_SCAN_TIME_SI = 7,
    SET_SCAN_TIME_SJ = 8,
    DISABLE_REMOTE_CONFIGURATION = 9,
	EXIT_COMMAND_MODE=10,
    REBOOT = 11,
    CHECK_RECEIVED_DATA = 12
}BT_State;





void BluetoothTxDone(void)
{
    TransmissionDone++;
}

void BluetoothRxDone(void)
{
    ReceptionDone++;
}


void main (void)
{
    GPIO_Init();
    Delay_ms(100);
    UART_Init();
    Delay_ms(100);
    while(Bluetooth_Configure());

    //Delay_ms(1000);
}


int Bluetooth_Configure(void)
{
    static BT_State State = SET_COMMAND_MODE;
    static BT_State OldState = SET_COMMAND_MODE;
    static unsigned char ReceivedArray[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    volatile int RetVal = 1;
    
    switch(State)
    {
        case SET_COMMAND_MODE:
        {
            UART_StartSilentTransmission("$$$",3,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        case CHECK_TRANSMISSION_DONE:
        {
            if(TransmissionDone)
            {

				TransmissionDone = 0;
                State = CHECK_RECEPTION_DONE;
                UART_StartSilentReception(ReceivedArray,4,0);
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
            
        }
        break;
        case CHECK_RECEPTION_DONE:
        {
            if(ReceptionDone)
            {
                State=CHECK_RECEIVED_DATA;
                ReceptionDone=0;
				if(OldState == EXIT_COMMAND_MODE)
				{
					GPIO_Write(2,1);
				}
            }
            else
            {
                if(OldState == EXIT_COMMAND_MODE)
				{
					GPIO_Write(0,1);
				}
                UART_ManageOngoingOperation(0);
            }

        }
        break;

        case CHECK_RECEIVED_DATA:
        {
            if(OldState==SET_COMMAND_MODE && ReceivedArray[0]=='C' && ReceivedArray[1]=='M' && ReceivedArray[2]=='D')
            {
                State = SET_FACTORY_SETTINGS;
            }
            else if( (ReceivedArray[0]=='A' &&  ReceivedArray[1]=='O' && ReceivedArray[2]=='K') || (ReceivedArray[0]=='E' &&  ReceivedArray[1]=='N' && ReceivedArray[2]=='D'))
            {
                State = OldState + 1;
            }
            else if(ReceivedArray[0]=='E' &&  ReceivedArray[1]=='R' && ReceivedArray[2]=='R')
            {
                GPIO_Write(1,1);
				State = REBOOT;
            }
        }
        break;

        case SET_FACTORY_SETTINGS:
        {

            
			UART_StartSilentTransmission("SF,1\r",5,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        
        case SET_DEVICE_NAME:
        {
            
            UART_StartSilentTransmission("SN,Authenticate-Device\r",23,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_SLAVE_MODE:
        {
            
			UART_StartSilentTransmission("SM,0\r",5,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_NO_AUTHENTICATION:
        {
            
			UART_StartSilentTransmission("SA,0\r",5,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_SCAN_TIME_SI:
        {
           
			UART_StartSilentTransmission("SI,0800\r",8,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_SCAN_TIME_SJ:
        {
            UART_StartSilentTransmission("SJ,0800\r",8,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        case DISABLE_REMOTE_CONFIGURATION:
        {
            UART_StartSilentTransmission("ST,0\r",5,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
		case EXIT_COMMAND_MODE:
		{
			
			UART_StartSilentTransmission("---\r",4,0);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
		}
        break;
        case REBOOT:
        {
            
            UART_StartSilentTransmission("R,1\r",4,0);
            if(OldState != DISABLE_REMOTE_CONFIGURATION )
            {
                State = SET_COMMAND_MODE;
                OldState = SET_COMMAND_MODE;
            }
            else
            {
                OldState = State;
                RetVal = 0;
            }
            
        }
        break;
        
    }


    return RetVal;
}
























/*
int Bluetooth_Init(void)
{
    static unsigned char ReceivedArray[10]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    static int State = 0;
    volatile unsigned int i;
    int RetVal = 1;
    switch(State)
    {
        case 0:
        {
            UART_StartSilentTransmission("$$$",3,0);
            State=1;
        }
        break;
        case 1:
        {
            if(TransmissionDone)
            {
                TransmissionDone=0;
                State=2;
                UART_StartSilentReception(ReceivedArray,3,0);
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 2:
        {
            if(ReceptionDone)
            {
                State=3;
                ReceptionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 3:
        {
            if(ReceivedArray[0]=='C' && ReceivedArray[1]=='M' && ReceivedArray[2]=='D')
            {
                State= 4 ;
            }
            else
            {
                State = 5;  //State 5 is for rebooting
            }
        }
        break;
        case 4: 
        {
            UART_StartSilentTransmission("SF,1",4,0);
            State=6;
        }
        break;
        case 5:
        {
            UART_StartSilentTransmission("R,1",3,0);
            State = 19;
        }
        case 6:
        {
            if(TransmissionDone)
            {
                State = 7;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 7:
        {
            UART_StartSilentTransmission("SN,Graduation",13,0);
            State=8;
        }
        break;
        case 8:
        {
            if(TransmissionDone)
            {
                State = 9;
                TransmissionDone=0;
            }
            else
            { 
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 9:
        {
            UART_StartSilentTransmission("SM,0",4,0);
            State=10;
        }
        break;
        case 10:
        {
            if(TransmissionDone)
            {
                State = 11;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 11:
        {
            UART_StartSilentTransmission("SA,0",4,0);
            State=12;
        }
        break;
        case 12:
        {
            if(TransmissionDone)
            {
                State = 13;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 13:
        {
            UART_StartSilentTransmission("SI,0800",7,0);
            State=14;
        }
        break;
        case 14:
        {
            if(TransmissionDone)
            {
                State = 15;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 15:
        {
            UART_StartSilentTransmission("SJ,0800",7,0);
            State=16;
        }
        break;
        case 16:
        {
            if(TransmissionDone)
            {
                State = 17;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 17:
        {
            UART_StartSilentTransmission("ST,0",4,0);
            State=18;
        }
        break;
        case 18:
        {
            if(TransmissionDone)
            {
                State = 20;
                TransmissionDone=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 19:
        {
            if(TransmissionDone)
            {
                TransmissionDone=0;
                State=0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 20:
        {
            UART_StartSilentTransmission("R,1",3,0);
            State = 21;
        }
        break;
        case 21:
        {
            if(TransmissionDone)
            {
                TransmissionDone=0;
                State=22;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }
        }
        break;
        case 22:
        {
            RetVal = 0;
        }
        break;
    }
    return RetVal;
}
*/

/*void main(void)
{
    unsigned char Received[1] = {'\0','\0','\0','\0','\0'};
    GPIO_Init();
    Delay_ms(100);
    UART_Init();
    Delay_ms(100);
    while(1)
    {
        switch(State)
        {
            case 0:
            {
                UART_StartSilentTransmission("$$$",3,0);
                State = 1;
            }
            break;
            case 1:
            {
                if(TransmissionDone)
                {
                    State = 2;
                    TransmissionDone = 0;
                    UART_StartSilentReception(Received,3,0);
                }
                else
                {
                    UART_ManageOngoingOperation(0);
                }
            }
            break;
            case 2:
            {
                if(ReceptionDone)
                {
                    State = 3;
                    ReceptionDone = 0;
                }
                else
                {
                    UART_ManageOngoingOperation(0);
                }
            }
            break;
            case 3:
            {
                if(Received[0] == 'C' && Received[1] == 'M' && Received[2] == 'D')
                {
                    GPIO_Write(6,0);
                    GPIO_Write(4,1);
                    State = 5;
                }
                else
                {
                    GPIO_Write(5,1);
                    State = 4;
                    UART_StartSilentTransmission("R,1",3,0);
                }
            }
            break;
            case 4:
            {
                if(TransmissionDone)
                {
                    GPIO_Write(6,1);
                    State = 0;
                    TransmissionDone = 0;
                }
                else
                {
                    GPIO_Write(5,1);
                    Delay_ms(10);
                    GPIO_Write(5,0);
                    Delay_ms(10);
                    UART_ManageOngoingOperation(0);
                    State = 6;
                }
            }
            break;
            case 5:
            {
                
                GPIO_Write(5,0);
                GPIO_Write(6,0);
                Delay_ms(1000);
                GPIO_Write(4,0);
                Delay_ms(1000);
                GPIO_Write(4,1);
                UART_StartSilentTransmission("---\r",4,0);
                State = 6;
            }
            break;
            case 6:
            {
                if(TransmissionDone)
                {
                    GPIO_Write(6,1);
                    State = 7;
                    TransmissionDone = 0;
                }
                else
                {
                    UART_ManageOngoingOperation(0);
                }
            }
            break;
            case 7:
            {
                GPIO_Write(4,0);
                GPIO_Write(5,0);
                Delay_ms(100);
                GPIO_Write(6,0);
                Delay_ms(100);
                GPIO_Write(6,1);
            }
        }
    }
}

*/