#include "GSM_Manager.h"
#define T30		(30/CYCLIC_TIME)
#define T800	(800/CYCLIC_TIME)
#define T1000	(1000/CYCLIC_TIME)
#define T1100	(1100/CYCLIC_TIME)
#define T5000	(5000/CYCLIC_TIME)
#define T7000	(7000/CYCLIC_TIME)

GSM_Manage_CheckType GSM_Flag;

uint8_t SoftWareRstFlag;
uint8_t StartFlag;
uint8_t SendMsgFlag;
uint8_t RecieveMsgFlage;
uint8_t* MsgGlob;
uint8_t MsgLengthGlob;
uint8_t* PhoneNumGlob;

uint8_t* RecievedResponsePtr;

uint16_t RecievedResponseLength;

void GSM_Init(void)
{
	GSM_Flag = GSM_Manage_InProgress;
	StartFlag = 0;
	SendMsgFlag = 0;
	SoftWareRstFlag = 0;
	RecieveMsgFlage = 0;
}

void SendSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	SendMsgFlag = 1;
	MsgGlob = Msg;
	MsgLengthGlob = MsgLength;
	PhoneNumGlob=PhoneNum;
}

void StartCommunication(void)
{
	StartFlag = 1;
}

void SoftWareReset(void)
{
	SoftWareRstFlag = 1;
}


void GSM_CallBack(GSM_CheckType GSM_Check, uint8_t* RecievedResponse, uint16_t ResponseLength)
{
	if(GSM_Check == GSM_OK)
	{
		GSM_Flag = GSM_Manage_OK;
		RecievedResponsePtr = RecievedResponse;
		RecievedResponseLength = ResponseLength;
	}
	else
	{
		GSM_Flag = GSM_Manage_NOK;
	}

}


static GSM_Manage_CheckType GSM_ManagePowerOn(void)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to indicate the success of the reset
    static GPIO_CheckType GPIO_Check;
    const GSM_ConfigType* GSM_ConfigPtr =  &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
    static uint16_t CounterPwr;
    if (CounterPwr == 0)
    {
    	//RTS LOW for software handling
    	GPIO_Check = GPIO_Write(GSM_ConfigPtr->RTSGroupId, GSM_ConfigPtr->RTSPinMask, LOW);
		if(GPIO_Check == GPIO_OK)
		{
			//set DTR HIGH for the sleep mode
			GPIO_Check = GPIO_Write(GSM_ConfigPtr->DTRGroupId, GSM_ConfigPtr->DTRPinMask, HIGH);
			if(GPIO_Check == GPIO_OK)
			{
				GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);//PWRK HIGH
				if(GPIO_Check == GPIO_OK)
				{
					RetVar = GSM_Manage_InProgress;
				}
				else
				{
					RetVar = GSM_Manage_NOK;
				}
			}
			else
			{
				RetVar = GSM_Manage_NOK;
			}
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}    	
	}

	else if(CounterPwr == T30 && GPIO_Check == GPIO_OK)
	{
		GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, LOW);//PWRK LOW

		if(GPIO_Check == GPIO_OK)
		{
			RetVar = GSM_Manage_InProgress;
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}

	}

	else if( (CounterPwr == T1100 + T30) && (GPIO_Check == GPIO_OK) )
	{
		GPIO_Check = GPIO_Write(GSM_ConfigPtr->PWRKeyGroupId, GSM_ConfigPtr->PWRKeyPinMask, HIGH);//PWRK HIGH
	

		if(GPIO_Check == GPIO_OK)
		{
			RetVar = GSM_Manage_InProgress;
		}
		else
		{
			RetVar = GSM_Manage_NOK;
		}
	}
	else if(CounterPwr == T1100 + T30 + T7000)
	{
		RetVar = GSM_Manage_OK;
	}
	else{;/*MISRA*/}

	CounterPwr++;

	return RetVar;

} 


static GSM_Manage_CheckType GSM_ManageStart(void)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to indicate the success of the reset
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate the success of the reset
	static GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the reset
    const GSM_ConfigType* GSM_ConfigPtr =  &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
    static uint8_t Counter;
    static uint8_t State;

    switch (State)
    {
    	case 0 :
    	{	
            if (GSM_Check == GSM_NOK)
            {
                GSM_Check = GSM_ATCommand_AT();
            }
            else {;/*MISRA*/}

    		if(GSM_Check == GSM_OK)
    		{
    			if(GSM_Flag == GSM_Manage_OK)
    			{
                    GSM_Check = GSM_NOK;
    				State = 1;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else if (GSM_Flag == GSM_Manage_NOK)
    			{
                    GSM_Check = GSM_NOK;
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else{;/*MISRA*/}
    		}
    		else
    		{
    			RetVar = GSM_Manage_NOK;
    			State = 0;
    		}

    	break;	
    	}

    	case 1 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_StopEcho();
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
                        Counter = 0;
    					State = 2;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
                        Counter = 0;
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
                    Counter = 0;
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}
    		}
    		
    		break;	
    	}

    	case 2 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_BRFix();
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    					RetVar = GSM_Manage_OK;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}
    		Counter = 0;
    		}
    		
    		break;	
    	}
    }
	
	return RetVar;
}

static GSM_Manage_CheckType GSM_ManageReadSMS(uint8_t* MsgBuffer, uint8_t MsgLength)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to indicate the success of the reset
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate the success of the reset
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the reset
    const GSM_ConfigType* GSM_ConfigPtr =  &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
    static uint8_t Counter;
    static uint8_t State;
    uint8_t Index;


    switch (State)
    {
    	case 0 :
    	{	
    		GSM_Check = GSM_ATCommand_SMSFormat(TEXT);
    		if(GSM_Check == GSM_OK)
    		{
    			if(GSM_Flag == GSM_Manage_OK)
    			{
    				State = 1;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else if (GSM_Flag == GSM_Manage_NOK)
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else{;/*MISRA*/}
    		}
    		else
    		{
    			RetVar = GSM_Manage_NOK;
    			State = 0;
    		}

    	break;	
    	}

    	case 1 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_CheckRecievedSMS();
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 2;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 2 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_ReadSMS(MsgLength);
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					for(Index = 0; Index < MsgLength; Index++)
    					{
    						MsgBuffer[Index] = RecievedResponsePtr[RecievedResponseLength - MsgLength + Index];
    					}

    					State = 3;
    					GSM_Flag = GSM_Manage_InProgress;
    					
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}
    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 3 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_SMSFormat(PDU);
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 4;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 4 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_DeleteSMS();
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    					RetVar = GSM_Manage_OK;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}



    }
	
	return RetVar;
}

static GSM_Manage_CheckType GSM_ManageSetSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to indicate the success of the reset
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_OK;// variable to indicate the success of the reset
	GSM_CheckType GSM_Check = GSM_NOK;// variable to indicate the success of the reset
    const GSM_ConfigType* GSM_ConfigPtr =  &GSM_ConfigParam;//declare a pointer to structur of the GSM_ConfigType
    static uint8_t Counter;
    static uint8_t State;


    switch (State)
    {
    	case 0 :
    	{	
    		GSM_Check = GSM_ATCommand_SMSFormat(TEXT);
    		if(GSM_Check == GSM_OK)
    		{
    			if(GSM_Flag == GSM_Manage_OK)
    			{
    				State = 1;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else if (GSM_Flag == GSM_Manage_NOK)
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    				GSM_Flag = GSM_Manage_InProgress;
    			}
    			else{;/*MISRA*/}
    		}
    		else
    		{
    			RetVar = GSM_Manage_NOK;
    			State = 0;
    		}

    	break;	
    	}

    	case 1 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_CharSet();
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 2;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 2 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_SetSMSMobNum (PhoneNum);
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 3;
    					GSM_Flag = GSM_Manage_InProgress;				
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}
    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 3 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_SetSMSWriteMsg(Msg, MsgLength);
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 4;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}

    	case 4 :
    	{
    		if(Counter != T1000)
    		{
    			Counter++;
    		}
    		else
    		{
    			GSM_Check = GSM_ATCommand_SMSFormat(PDU);
    			if(GSM_Check == GSM_OK)
    			{
    				if(GSM_Flag == GSM_Manage_OK)
    				{
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    					RetVar = GSM_Manage_OK;
    				}
    				else if (GSM_Flag == GSM_Manage_NOK)
    				{
    					RetVar = GSM_Manage_NOK;
    					State = 0;
    					GSM_Flag = GSM_Manage_InProgress;
    				}
    				else{;/*MISRA*/}
    			}
    			else
    			{
    				RetVar = GSM_Manage_NOK;
    				State = 0;
    			}

    		Counter = 0;
    		}
    		
    		break;	
    	}
    }
	
	return RetVar;
}


GSM_Manage_CheckType GSM_ManageOngoingOperation(void)
{
	GSM_Manage_CheckType RetVar = GSM_Manage_InProgress;// variable to indicate the success of the reset
	GSM_Manage_CheckType GSM_ManageCheck = GSM_Manage_InProgress;// variable to indicate the success of the reset
	static uint8_t ManageState;


	switch(ManageState)
	{
		case UNINIT :
		{
			GSM_ManageCheck = GSM_ManagePowerOn();
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				ManageState = IDLE;
                StartFlag = 1;
				RetVar=GSM_Manage_InProgress;
			}
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				ManageState = ERROR;
				RetVar=GSM_Manage_NOK;
			}
			else{;/*MISRA*/}

			break;
		}

		case IDLE :
		{
			if(SoftWareRstFlag == 1)
			{
				ManageState = SWRESET;
			}
			else{;/*MISRA*/}

			if(StartFlag == 1)
			{
				ManageState = START;
			}
			else{;/*MISRA*/}

			if(SendMsgFlag == 1)
			{
				ManageState = SENDSMS;
			}
			else{;/*MISRA*/}

			if(RecieveMsgFlage == 1)
			{
				ManageState = RECIEVESMS;
			}
			else{;/*MISRA*/}

			break;
		}

		case START :
		{
			GSM_ManageCheck = GSM_ManageStart();
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				ManageState = IDLE;
                StartFlag=0;
				RetVar=GSM_Manage_OK;
			}
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				RetVar=GSM_Manage_NOK;
                StartFlag=0;
				ManageState = ERROR;
			}
			else{;/*MISRA*/}

			break;
		}

		case SWRESET :
		{
			//GSM_ManageCheck = GSM_ManageSWReset();
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				ManageState = IDLE;
				RetVar=GSM_Manage_InProgress;
			}
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				RetVar=GSM_Manage_NOK;
				ManageState = ERROR;
			}
			else{;/*MISRA*/}

			StartFlag=1;
			SoftWareRstFlag = 0;
			break;
		}


		case SENDSMS :
		{
			GSM_ManageCheck = GSM_ManageSetSMS(MsgGlob,MsgLengthGlob,PhoneNumGlob);
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				ManageState = IDLE;
				RetVar=GSM_Manage_OK;
				//#####################################################################	
			}
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				RetVar=GSM_Manage_NOK;
				ManageState = ERROR;
			}
			else{;/*MISRA*/}

			SendMsgFlag = 0;
			break;
		}



		case RECIEVESMS :
		{
			//GSM_ManageCheck = GSM_ManageReadSMS(uint8_t* MsgBuffer, uint8_t MsgLength);//######################################
			if(GSM_ManageCheck == GSM_Manage_OK)
			{
				ManageState = IDLE;
				RetVar=GSM_Manage_InProgress;
				//#####################################################################
			}
			else if(GSM_ManageCheck == GSM_Manage_NOK)
			{
				RetVar=GSM_Manage_NOK;
				ManageState = ERROR;
			}
			else{;/*MISRA*/}

			RecieveMsgFlage = 0;
			break;
		}

	}
}





/*
	SOFTWARE reset function
	HARDWARE reset function

	Manager call back functions


	Configration of the manager
		Reade msg
		msg length
		call back function of user
		error call back function

	Error call back functions
*/

