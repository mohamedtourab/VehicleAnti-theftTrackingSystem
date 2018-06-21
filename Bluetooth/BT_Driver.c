/*
 *
 *        Authors: Wessam Adel and Mohamed Mamdouh
 *        Date: 13/3/2018
 *  Last Edited: 18/3/2018
 *        Microcontroller: STM32F407VG
 *  Description: This File contains the Bluetooth Initialization and Configuration, and it needs around 700 ms to be finished
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

BT_CheckType BT_Configure(void)
{
    static BT_State State = SET_COMMAND_MODE;
    static BT_State OldState = SET_COMMAND_MODE;
    static unsigned char ReceivedArray[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    uint8_t NumberOfCharacters;
    unsigned char OutputName[29];
    BT_CheckType RetVal = BT_NOK;
    const BT_ConfigType* ConfigPtr = &BT_ConfigParam;

    switch(State)
    {
        case SET_COMMAND_MODE:
        {
            UART_StartSilentTransmission("$$$",3,ConfigPtr->BT_ChannelID);
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
                UART_StartSilentReception(ReceivedArray,4,ConfigPtr->BT_ChannelID);
            }
            else
            {
                #if(BT_USE_INT_TO_HANDLE==0)
                    UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
                #endif
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
                #if(BT_USE_INT_TO_HANDLE==0)
                    UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
                #endif
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
            UART_StartSilentTransmission("SF,1\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_DEVICE_NAME:
        {
            NumberOfCharacters = Parser(ConfigPtr->BT_Name, OutputName);
            UART_StartSilentTransmission(OutputName,NumberOfCharacters,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_SLAVE_MODE:
        {

            UART_StartSilentTransmission("SM,0\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case SET_NO_AUTHENTICATION:
        {

        UART_StartSilentTransmission("SA,1\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;

        case DISABLE_REMOTE_CONFIGURATION:
        {
            UART_StartSilentTransmission("ST,0\r",5,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        case EXIT_COMMAND_MODE:
        {

            UART_StartSilentTransmission("---\r",4,ConfigPtr->BT_ChannelID);
            OldState = State;
            State = CHECK_TRANSMISSION_DONE;
        }
        break;
        
        case REBOOT:
        {

            UART_StartSilentTransmission("R,1\r",4,ConfigPtr->BT_ChannelID);
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

 BT_CheckType BT_GetData(char* DataReceived,uint8_t NoOfBytes)
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
			#if(BT_USE_INT_TO_HANDLE==0)
                UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
            #endif
			if(ReceptionDone == 1)
			{
				state = BT_DATA_IDLE;
				ReceptionDone = 0;
				RetVal = BT_OK;
                (*(ConfigPtr->BT_ReceptionCallBack))();
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

 BT_CheckType BT_SendData(char* DataToBeSent, uint8_t NoOfBytes)
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
			#if(BT_USE_INT_TO_HANDLE==0)
                UART_ManageOngoingOperation(ConfigPtr->BT_ChannelID);
            #endif
			if(TransmissionDone == 1)
			{
				State = BT_DATA_IDLE;
				TransmissionDone = 0;
				RetVal = BT_OK;
                (*(ConfigPtr->BT_TransmissionCallBack))();
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
 ************************************************************************************** 
 *                                                                                    *  
 * Function Used to Check the current status of the module connected or not connected *     
 * 1,0,0 indicates the device is connected                                            *
 * 0,0,0 indicates that it is not connected                                           *                                     *  
 *                                                                                    *  
 **************************************************************************************
 */

BT_ConnectionStatus BT_GetConnectionStatus (void)
{
    BT_ConnectionStatus RetVal;
    static unsigned char ReceivedArray[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};

    UART_StartSilentTransmission("$$$",3,ConfigPtr->BT_ChannelID);
    UART_StartSilentTransmission("GK",2,ConfigPtr->BT_ChannelID);
    UART_StartSilentReception(ReceivedArray,6,ConfigPtr->BT_ChannelID);

    switch(ReceivedArray[0])
    {
        case '1': //connected
        {
            RetVal = CONNECTED;
            return RetVal;
        }
        break;

        case '0': //not connected
        {
            RetVal = NOT_CONNECTED;
            return RetVal;
        }
        break;

        default:
        break;
    }

}

/*
 ****************************************************************************** 
 *                                                                            *  
 *     Function Used to disconnect the device the current connection          *      
 *                                                                            *  
 *                                                                            *  
 ******************************************************************************
 */

void BT_KillConnection (void)
{
    UART_StartSilentTransmission("$$$",3,ConfigPtr->BT_ChannelID);
    UART_StartSilentTransmission("K",1,ConfigPtr->BT_ChannelID);
}