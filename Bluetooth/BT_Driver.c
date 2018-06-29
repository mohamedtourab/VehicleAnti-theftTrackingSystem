/*
 *
 *        Authors: Wessam Adel and Mohamed Mamdouh
 *        Date: 13/3/2018
 *  Last Edited: 18/3/2018
 *        Microcontroller: STM32F407VG
 *  Description: This File contains the Bluetooth Initialization and Configuration
*/

#include "BT_Driver.h"

uint8_t BT_InitFlag = 0;

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
    const BT_ConfigType* ConfigPtr = & BT_ConfigParam;
    TransmissionDone  = 0;
    ReceptionDone     = 0;
    BT_InitFlag       = 0;

    while(BT_Configure() != BT_OK)
    {
        UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
    }
    while(BT_ChangeBaudRate() != BT_OK)
    {
        UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
    }
    if(sizeof(ConfigPtr->BT_Name) <= 25)
    {
        BT_InitFlag      = 1;
    }
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

BT_CheckType BT_ChangeBaudRate(void)
{
    static BT_State State = SET_COMMAND_MODE;
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
    uint8_t ReceivedArray[5] = {'\0','\0','\0','\0','\0'};
    BT_Response MemoryCompareRetVal;
    BT_CheckType RetVal = BT_BUSY;

    switch(State)
    {
        case SET_COMMAND_MODE:
        {
            UART_StartSilentTransmission((uint8_t*)"$$$",3,ConfigPtr->BT_ChannelID);
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case CHECK_TRANSMISSION_DONE:
        {
            if(TransmissionDone == 1)
            {
                TransmissionDone = 0;
                State = CHECK_RECEPTION_DONE;
                UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
            }
        }
        break;

        case CHECK_RECEPTION_DONE:
        {
            if(ReceptionDone == 1)
            {
                State = CHECK_RECEIVED_DATA;
                ReceptionDone = 0;
            }
            else{/*MISRA*/};
        }
        break;

        case CHECK_RECEIVED_DATA:
        {
            MemoryCompareRetVal = MemoryCompare(ReceivedArray,LENGTH_OF_BT_RESPONSE);

            if(MemoryCompareRetVal == CMD)
            {
                RetVal = BT_BUSY;
                State = CHANGE_BR;
            }
            else if(MemoryCompareRetVal == AOK)
            {
                RetVal = BT_OK;
            }
            else
            {
                RetVal = BT_NOK;
            }
        }
        break;

        case CHANGE_BR:
        {
            UART_StartSilentTransmission((uint8_t*)"U,9600,N",8,(ConfigPtr->BT_ChannelID));
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
    }
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
    uint8_t NumberOfCharacters;
    uint8_t OutputName[29];
    BT_Response MemoryCompareRetVal;
    BT_CheckType RetVal = BT_NOK;
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;

    switch(State)
    {
        case SET_COMMAND_MODE:
        {
            UART_StartSilentTransmission((uint8_t*)"$$$",3,ConfigPtr->BT_ChannelID);
			
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        case CHECK_TRANSMISSION_DONE:
        {
            if(TransmissionDone == 1)
            {

                TransmissionDone = 0;
                State = CHECK_RECEPTION_DONE;
                UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
            }
            else{/*MISRA*/};
        }
        break;
        case CHECK_RECEPTION_DONE:
        {
            if(ReceptionDone == 1)
            {
                State = CHECK_RECEIVED_DATA;
                ReceptionDone = 0;
            }
            else{/*MISRA*/};
        }
        break;

        case CHECK_RECEIVED_DATA:
        {
            MemoryCompareRetVal = MemoryCompare(ReceivedArray,LENGTH_OF_BT_RESPONSE);
            
            switch(MemoryCompareRetVal)
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
				
				default: break;
            }
        }
        break;

        case SET_FACTORY_SETTINGS:
        {
            UART_StartSilentTransmission((uint8_t*)"SF,1\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
					
        }
        break;

        case SET_DEVICE_NAME:
        {
            NumberOfCharacters = Parser(ConfigPtr->BT_Name, (char*)OutputName);
            UART_StartSilentTransmission(OutputName,NumberOfCharacters,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_SLAVE_MODE:
        {

            UART_StartSilentTransmission((uint8_t*)"SM,0\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_NO_AUTHENTICATION:
        {

        UART_StartSilentTransmission((uint8_t*)"SA,1\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case DISABLE_REMOTE_CONFIGURATION:
        {
            UART_StartSilentTransmission("ST,255\r",7,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
				
        case EXIT_COMMAND_MODE:
        {

            UART_StartSilentTransmission((uint8_t*)"---\r",4,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        
        case REBOOT:
        {

            UART_StartSilentTransmission((uint8_t*)"R,1\r",4,ConfigPtr->BT_ChannelID);
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

        default: break;
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

static BT_Response MemoryCompare(unsigned char * AT_Command, uint8_t Length)
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
        case 'C':
        {
            if((AT_Command[1] == Command[0][1]) && (AT_Command[2] == Command[0][2]) )
            {
                RetVal = CMD;
            }
        }
        break;
        case 'A':
        {
            if((AT_Command[1] == Command[1][1]) && (AT_Command[2] == Command[1][2]) )
            {
                RetVal = AOK;
            }
        }
        break;
        case 'E':
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

        case '1':
        {
            RetVal = CONN;
        }
        break;

        case '0':
        {
            RetVal = NOT_CONN;
        }
        break;

        case 'K':
        {
            if( (AT_Command[1] == 'I') && (AT_Command[2] == 'L') && (AT_Command[3] == 'L') )
            {
                RetVal = KILL;
            }
        }
        break;

        default: break;
    }
    return RetVal;
}

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *                        Get Data Function                                   *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

BT_CheckType BT_GetData(uint8_t* DataReceived,uint8_t NoOfBytes)
{
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
    static BT_DataStates state = BT_DATA_IDLE;
    BT_CheckType RetVal = BT_NOK;
	switch(state)
	{
	    case BT_DATA_IDLE:
		{
		    UART_StartSilentReception(DataReceived,NoOfBytes,ConfigPtr->BT_ChannelID);
		    state = BT_DATA_BUSY;
		    RetVal = BT_NOK;
		}
		break;
		case BT_DATA_BUSY:
		{

		    if(ReceptionDone == 1)
			{
				state = BT_DATA_IDLE;
				ReceptionDone = 0;
				RetVal = BT_OK;
			}
			else
			{
				state = BT_DATA_BUSY;
				RetVal = BT_NOK;
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
 *                        Send Data Function                                  *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */


BT_CheckType BT_SendData(uint8_t* DataToBeSent, uint8_t NoOfBytes)
{
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
    static BT_DataStates State = BT_DATA_IDLE;
    BT_CheckType RetVal = BT_NOK;

    switch(State)
    {
        case BT_DATA_IDLE:
        {
            UART_StartSilentTransmission(DataToBeSent,NoOfBytes,ConfigPtr->BT_ChannelID);
            
            State = BT_DATA_BUSY;
            RetVal = BT_NOK;
            TransmissionDone = 0;
        }
        break;
        
        case BT_DATA_BUSY:
        {
            if(TransmissionDone == 1)
            {
                State = BT_DATA_IDLE;
                TransmissionDone = 0;
                RetVal = BT_OK;
            }
            else
            {
                State = BT_DATA_BUSY;
                RetVal = BT_NOK;
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
 *   Function Used to modify the user's naming of the module to add the       *  
 *   required AT Command                                                      *  
 *                                                                            *  
 ******************************************************************************
 */


static unsigned int Parser (const char InputName[],char OutputName[])
{
    OutputName[0]= 'S';
    OutputName[1]= 'N';
    OutputName[2]= ',';
    uint8_t i=0;
    uint8_t Counter=0;

    while(InputName[i] !='\0'){
        Counter++;
        i++;
    }
    i=3;
    int j=0;
    for(j=0;j<Counter;j++)
    {
        OutputName[i] = InputName[j];
        i++;
    }
    OutputName[i]='\r';
    return (i+1);
}

 /*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *                  Get Connection Status Function                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

BT_CheckType BT_GetConnectionStatus(void)
{
	const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
	static BT_ConnectionStates State = BT_CONN_SET_CMD;
    static BT_ConnectionStates OldState = BT_CONN_SET_CMD;
    BT_CheckType RetVal = BT_BUSY;
	static uint8_t DoneFlag = 0;
    static uint8_t ReceivedArray[5] = {'\0','\0','\0','\0','\0'};
    BT_Response FunctionReturn;

    switch(State)
    {
        case BT_CONN_SET_CMD:
        {
            OldState = BT_CONN_SET_CMD;
            UART_StartSilentTransmission((uint8_t*)"$$$",3,ConfigPtr->BT_ChannelID);
            State = BT_CONN_BUSY;
        }
        break;
		
		case BT_CONN_SEND_GK:
		{
			OldState = BT_CONN_SEND_GK;
			UART_StartSilentTransmission((uint8_t*)"GK\r",3,ConfigPtr->BT_ChannelID);
			State = BT_CONN_BUSY;
		}
		
		break;
		
		case BT_CONN_EXIT_CMD:
		{
			
			OldState = BT_CONN_EXIT_CMD;
			UART_StartSilentTransmission((uint8_t*)"---\r",4,ConfigPtr->BT_ChannelID);
			State = BT_CONN_BUSY;
		}
		break;
		
        case BT_CONN_BUSY:
        {
			if(TransmissionDone == 1)
			{
				State = BT_CONN_START_RECEPTION;
				TransmissionDone = 0;
			}
			else if(ReceptionDone == 1)
			{
				State = BT_CONN_CHECK_RECEIVED_DATA;
				ReceptionDone = 0;
			}
			else
			{
				State = BT_CONN_BUSY;
			}
        }
        
        break;

		case BT_CONN_START_RECEPTION:
		{
			switch(OldState)
			{
				case BT_CONN_SET_CMD:
				{
					UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
					State = BT_CONN_BUSY;
				}
				break;
					
				case BT_CONN_SEND_GK:
				{
					UART_StartSilentReception(ReceivedArray,1,ConfigPtr->BT_ChannelID);
					State = BT_CONN_BUSY;
				}
				break;
				
				case BT_CONN_EXIT_CMD:
				{
					UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
					State = BT_CONN_BUSY;
				}
				break;
				
				default: break;
			}
		}
		break;

        case BT_CONN_CHECK_RECEIVED_DATA:
        {
            switch(OldState)
			{
				case BT_CONN_SET_CMD:
				{
					FunctionReturn = MemoryCompare(ReceivedArray,LENGTH_OF_BT_RESPONSE);
					
					if( FunctionReturn == CMD )
					{
						State = BT_CONN_SEND_GK;
					}
					else{/*MISRA*/};
				}
				break;
				
				case BT_CONN_SEND_GK:
				{
					FunctionReturn = MemoryCompare(ReceivedArray,1);
					if( FunctionReturn == CONN )
					{
						State = BT_CONN_EXIT_CMD;
						DoneFlag = 1;
					}
					else if(FunctionReturn == NOT_CONN)
					{
						State = BT_CONN_EXIT_CMD;
						DoneFlag = 2;
					}
				}
				break;
				
				case BT_CONN_EXIT_CMD:
				{
					FunctionReturn = MemoryCompare(ReceivedArray,3);
					if( FunctionReturn == END )
					{
						State = BT_CONN_SET_CMD;
						
						if(DoneFlag == 1)
						{
							DoneFlag = 0;
							RetVal = BT_OK;
						}
						else if(DoneFlag == 2)
						{
							RetVal = BT_NOK;
							DoneFlag = 0;
						}
						else{/*MISRA*/};
					}
					 
				}
				break;
					
				default: break;
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
 *                      Kill Connection Function                              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

BT_CheckType BT_StopCommunication(void)
{
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
    static BT_StopCommunicationStates State = BT_STOP_COMM_SET_CMD;
    static BT_StopCommunicationStates OldState = BT_STOP_COMM_SET_CMD;
    BT_CheckType RetVal = BT_BUSY;
    static uint8_t ReceivedArray[4] = {'\0','\0','\0','\0'};
    BT_Response FunctionReturn;
	static uint8_t DoneFlag = 0;

    switch(State)
    {
        case BT_STOP_COMM_SET_CMD:
        {
            OldState = BT_STOP_COMM_SET_CMD;
            UART_StartSilentTransmission((uint8_t*)"$$$",3,ConfigPtr->BT_ChannelID);
            State = BT_STOP_COMM_BUSY;
        }
        break;

        case BT_STOP_COMM_SEND_K:
        {
            OldState = BT_STOP_COMM_SEND_K;
            UART_StartSilentTransmission((uint8_t*)"K,\r",3,ConfigPtr->BT_ChannelID);
            State = BT_STOP_COMM_BUSY;
        }
        break;
		
		case BT_STOP_COMM_EXIT_CMD:
		{
			OldState = BT_STOP_COMM_EXIT_CMD;
            UART_StartSilentTransmission((uint8_t*)"---\r",4,ConfigPtr->BT_ChannelID);
            State = BT_STOP_COMM_BUSY;
		}
		break;

        case BT_STOP_COMM_BUSY:
        {
            if(TransmissionDone == 1)
			{
				State = BT_STOP_COMM_START_RECEPTION;
				TransmissionDone = 0;
			}
			else if(ReceptionDone == 1)
			{
				State = BT_STOP_COMM_CHECK_RECEIVED_DATA;
				ReceptionDone = 0;
			}
			else
			{
				State = BT_STOP_COMM_BUSY;
			}   
        }
        break;

        case BT_STOP_COMM_CHECK_RECEIVED_DATA:
        {
            switch(OldState)
			{
				case BT_STOP_COMM_SET_CMD:
				{
					FunctionReturn = MemoryCompare(ReceivedArray,LENGTH_OF_BT_RESPONSE);
					
					if( FunctionReturn == CMD )
					{
						State = BT_STOP_COMM_SEND_K;
					}
					else{/*MISRA*/};
				}
				break;
				
				case BT_STOP_COMM_SEND_K :
				{
					FunctionReturn = MemoryCompare(ReceivedArray,4);
					if( FunctionReturn == KILL )
					{
						DoneFlag = 1;
					}
					else
                    {
                        DoneFlag = 2;
                    }
					State = BT_STOP_COMM_EXIT_CMD;
				}
				break;
				
				case BT_STOP_COMM_EXIT_CMD:
				{
					FunctionReturn = MemoryCompare(ReceivedArray,3);
					if( FunctionReturn == END )
					{
						if(DoneFlag == 1)
						{
							RetVal = BT_OK;
							DoneFlag = 0;
						}
						else if(DoneFlag == 2)
						{
							RetVal = BT_NOK;
							DoneFlag = 0;
						}
						else{/*MISRA*/}
						State = BT_STOP_COMM_SET_CMD;
						
					}
				}
				break;
				
				default: break;
        }
        break;
	}
		break;
	
        case BT_STOP_COMM_START_RECEPTION:
        {
            switch(OldState)
			{
				case BT_STOP_COMM_SET_CMD:
				{
					UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
					State = BT_STOP_COMM_BUSY;
				}
				break;
					
				case BT_STOP_COMM_SEND_K:
				{
					UART_StartSilentReception(ReceivedArray,4,ConfigPtr->BT_ChannelID);
					State = BT_STOP_COMM_BUSY;
				}
				break;
				
				case BT_STOP_COMM_EXIT_CMD:
				{
					UART_StartSilentReception(ReceivedArray,3,ConfigPtr->BT_ChannelID);
					State = BT_STOP_COMM_BUSY;
				}
				break;
				default: break;
			}
        }
        break;
    }
    return RetVal;
}