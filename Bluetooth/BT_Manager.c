/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 13/3/2018
 *  Last Edited: 18/3/2018
 *	Microcontroller: STM32F407VG
 *  Description: This File contains the Bluetooth Initialization and Configuration, and it needs around 700 ms to be finished
*/

#include "BT_Manager.h"

static uint8_t TransmissionDone;
static uint8_t ReceptionDone;


/*
 ****************************************************************************** 
 *                                                                            *      
 *                                                                            *      
 *            Transmission and Reception Flags Initialization                 *  
 *                                                                            *  
 *                                                                            *  
 ******************************************************************************
 */

void BT_Init(void)
{
    TransmissionDone = 0;
    ReceptionDone    = 0;
}


/*
 ****************************************************************************** 
 *                                                                            *      
 *                                                                            *      
 *              Bluetooth Manager (BLuetooth Configuration)                   *  
 *                                                                            *  
 *                                                                            *  
 ******************************************************************************
 */

BT_CheckType BT_Configure(void)
{
    static BT_State State = SET_COMMAND_MODE;
    static BT_State OldState = SET_COMMAND_MODE;
    static unsigned char ReceivedArray[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    BT_CheckType RetVal = BT_NOK;

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
                State = CHECK_RECEIVED_DATA;
                ReceptionDone = 0;
            }
            else
            {
                UART_ManageOngoingOperation(0);
            }

        }
        break;

        case CHECK_RECEIVED_DATA:
        {
            switch(MemoryCompare(ReceivedArray,LENGTH_OF_BT_RESPONSE))
            {
                case CMD:
                {
                    if(OldState == SET_COMMAND_MODE)
                    {
                        State = SET_FACTORY_SETTINGS;
                    }
                    else{/*MISRA*/};
                }
                break;
                case AOK:
                {
                    State = OldState + 1;
                }
                break;
                case ERR:
                {
                    State = REBOOT;
                }
                break;
                case END:
                {
                    State = OldState + 1;
                }
                break;
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

            UART_StartSilentTransmission("SN,aloalo\r",23,0);
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
            if(OldState != EXIT_COMMAND_MODE )
            {
                State = SET_COMMAND_MODE;
                OldState = SET_COMMAND_MODE;
            }
            else
            {
                OldState = State;
                RetVal = BT_OK;

            }

        }
        break;

    }

    return RetVal;
}

/*
 ****************************************************************************** 
 *                                                                            *      
 *                                                                            *      
 *            UART Call Back Functions for Transmission and Reception         *  
 *                                                                            *  
 *                                                                            *  
 ******************************************************************************
 */

void BluetoothTxDone(void)
{
    TransmissionDone = 1;
}

void BluetoothRxDone(void)
{
    ReceptionDone = 1;
}


/*
 ****************************************************************************** 
 *                                                                            *      
 *                                                                            *      
 *                        Memory Compare Function                             *  
 *                                                                            *  
 *                                                                            *  
 ******************************************************************************
 */

BT_Response MemoryCompare(unsigned char * AT_Command, uint8_t Length)
{
    const unsigned char Command[NUMBER_OF_COMMANDS][3] =    {
                                                                {'C','M','D'},
                                                                {'A','O','K'},
                                                                {'E','R','R'},
                                                                {'E','N','D'}    
                                                            };

    BT_Response RetVal;

    switch(AT_Command[0])
    {
        case Command[0][0]:
        {
            if((AT_Command[1] == Command[0][1]) && (AT_Command[2] == Command[0][2]) )
            {
                RetVal = CMD;
            }
        }
        break;
        case Command[1][0]:
        {
            if((AT_Command[1] == Command[1][1]) && (AT_Command[2] == Command[1][2]) )
            {
                RetVal = AOK;
            }
        }
        break;
        case Command[2][0]:
        {
            if((AT_Command[1] == Command[2][1]) && (AT_Command[2] == Command[2][2]) )
            {
                RetVal = ERR;
            }
            else if((AT_Command[1] == Command[3][1]) && (AT_Command[2] == Command[3][2]) )
            {
                RetVal = END;
            }
        }
        break;
        default: break;
    }
    return RetVal;
}