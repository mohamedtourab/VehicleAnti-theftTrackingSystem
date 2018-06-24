/**************************************************************************************
*File name: ELM327.h
*Auther : 
*		-Abdelrhman Hosny
*		-Amr Mohamed
*Date: 30/4/2018
*Description:
*	This file contains:
*		- implementation of functions used to manage the ELM327 OBD module
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "ELM327.h"

/***********************************************************************************
**********							Macros									********
***********************************************************************************/

#define ELM327_EXPIRE_TIME	100000U

#define ELM327_T_EXP	(ELM327_EXPIRE_TIME/ELM327_CYCLIC_TIME)


/*****Development mode Macro*****/
//#define DEVELOPMENT_MODE_ENABLE
/********************************/

/*AT Commands lengths Response lengths Macros*/
#define SYSTEM_RESET_CMD_LENGTH				4U
#define VEHICLE_SPEED_CMD_LENGTH			5U
#define VEHICLE_RPM_CMD_LENGTH				5U
#define BATTERY_STATE_CMD_LENGTH			5U


/*AT Commands Response lengths Macros*/
#define SYSTEM_RESET_RRES_LENGHTH			17U
#define VEHICLE_SPEED_RRES_LENGTH			9U
#define VEHICLE_RPM_RRES_LENGTH				9U
#define BATTERY_STATE_RRES_LENGTH			6U


/*AT Commands expected Response lengths Macros*/
#define SYSTEM_RESET_ERES_LENGHTH			17U
#define VEHICLE_SPEED_ERES_LENGTH			1U
#define VEHICLE_RPM_ERES_LENGTH				1U
#define BATTERY_STATE_ERES_LENGTH			1U



/******************************************************/
#define MAX_RESPONSE_LENGTH				100U
#define MAX_EXPECTED_RESPONSE_LENGTH	100U

/******************************************************/
#define SEND_CMD				0U
#define PROCCESS_RECEIVED_RES	1U

#define ELM327_MANAGE_UNINIT					0U
#define ELM327_MANAGE_IDLE						1U
#define ELM327_MANAGE_READ_SPEED				2U
#define ELM327_MANAGE_READ_RPM					3U
#define ELM327_MANAGE_READ_BATTERY_STATE		4U
#define ELM327_MANAGE_ERROR						5U

/******************************************************/
#define ASCII_OF_ZERO			48U
#define ASCII_OF_NINE			57U
#define ASCII_OF_A				65U








/***********************************************************************************
**********				GSM Helper functions prototypes						********
***********************************************************************************/

/*
 * This function used to establish the communication with the ELM327 module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_System_Reset(void);

/*
 * This function used to read the SPEED of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Vehicle_Speed(void);

/*
 * This function used to read the RPM of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Vehicle_RPM(void);

/*
 * This function used to read the battery state of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Battery_State(void);

/*
 *	This function is used to stop the Uart transmission
 *	Inputs:NONE
 *	Output:NONE
 **/
static void ELM327_ATCMD_Stop(void);

/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings        
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length);

/*********************************************************************************/
/*
 * This function is a FSM to reset the system to default
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ResetDefaults(void);

/*
 * This function is a FSM to read the speed of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ReadSpeed(void);

/*
 * This function is a FSM to read the rpm of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType Read_RPM(void);

/*
 * This function is a FSM to read the rpm of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ReadBatteryState(void);


/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/

static uint8_t ReceivedResponse[MAX_RESPONSE_LENGTH];//array to hold the received response
static uint8_t ReceivedResponseLength;//the length of the RecievedResponse

static uint8_t ExpectedResponse[MAX_EXPECTED_RESPONSE_LENGTH];//the expected response
static uint8_t ExpectedResponseLength;//the length of the expected Response

static uint8_t ReceiveDoneFlag;	//flag to tell if the receiption is done
static uint8_t ReceiveSuccessFlag;	//flag to tell if the received response meet the requirements

static uint8_t ResetDefualtsFlag;		//flag to change the state to reset defaults
static uint8_t ReadSpeedFlag;		//flag to change the state to read speed state
static uint8_t Read_RPM_Flag;		//flag to change the state to read RPM state
static uint8_t ReadBatterStateFlag;	//flag to change the state to read battery state 

static uint8_t HelperState;		//to set the state of the helper functions
static uint8_t ManageState;		//set the state of Manager function

static uint16_t MaximumResponseTimeCounter;		//counter used to break from waiting response if the response time exceeds a certain limit

/***********************************************************************************
**********				GSM  Driver functions' bodies                      ********
***********************************************************************************/


/*
 * This function is used to initialize the flags for the functions called by the client 
 * Inputs:NONE
 * Output:NONE
*/
void ELM327_Init(void)
{
	ReceiveDoneFlag = 0;
	ReceiveSuccessFlag = 0;

	ResetDefualtsFlag = 0;
	ReadSpeedFlag = 0;
	Read_RPM_Flag = 0;
	ReadBatterStateFlag = 0;

	HelperState = SEND_CMD;
	ManageState = ELM327_MANAGE_UNINIT;

	MaximumResponseTimeCounter = 0;
}


/*
 * This function is used to Reset to defaults
 *Inputs:NONE
 * Output:
 *			-indication of success of the function
*/
ELM327_CheckType ELM327_ResetDefaults(void)
{
	ELM327_CheckType RetVar = ELM327_BUSY;

	if(ResetDefualtsFlag == 0)	//check if the flag is raised already
	{
		ResetDefualtsFlag = 1;	//rise the flag

		RetVar = ELM327_OK;		//return ok if the flag is raised
	}
	else
	{
		/*the return value is initialized to busy so no action is needed here*/
	}

	return RetVar;
}


/*
 * This function is used to Read the speed
 *Inputs:NONE
 * Output:
 *			-indication of success of the function
*/
ELM327_CheckType ELM327_GetVehicleSpeed(void)
{
	ELM327_CheckType RetVar = ELM327_BUSY;

	if(ReadSpeedFlag == 0)						//check if the flag is raised already
	{
		ReadSpeedFlag = 1;						//rise the flag

		RetVar = ELM327_OK;						//return ok if the flag is raised
	}
	else
	{
		/*the return value is initialized to busy so no action is needed here*/
	}

	return RetVar;
}



/*
 * This function is used to Read the RPM 
 *Inputs:NONE
 * Output:
 *			-indication of success of the function
*/
ELM327_CheckType ELM327_GetVehicleRPM(void)
{
	ELM327_CheckType RetVar = ELM327_BUSY;

	if(Read_RPM_Flag == 0)					//check if the flag is raised already
	{
		Read_RPM_Flag = 1;					//rise the flag

		RetVar = ELM327_OK;					//return ok if the flag is raised
	}
	else
	{
		/*the return value is initialized to busy so no action is needed here*/
	}

	return RetVar;
}



/*
 * This function is used to Read the state of the battery
 *Inputs:NONE
 * Output:
 *			-indication of success of the function
*/
ELM327_CheckType ELM327_GetVehicleBatteryState(void)
{
	ELM327_CheckType RetVar = ELM327_BUSY;

	if(ReadBatterStateFlag == 0)				//check if the flag is raised already
	{
		ReadBatterStateFlag = 1;				//rise the flag	

		RetVar = ELM327_OK;						//return ok if the flag is raised
	}
	else
	{
		/*the return value is initialized to busy so no action is needed here*/
	}

	return RetVar;
}



/*
 * This function is a FSM to manage the on going operations of the GSM module
 *Inputs:NONE
 * Output:NONE
*/
void ELM327_ManageOngoingOperation(void)
{
	ELM327_CheckType ELM327_Check = ELM327_BUSY;					//variable to indicate the success of the command

	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the ELM327_ConfigType
	
	int16_t VehicleData = 0; //avariable to hold any data from the vehicle after processing it
	OBD_VehicleStatus VehicleStatus = 0;	//variable to hold the state of the car

	switch(ManageState)
	{
		case ELM327_MANAGE_UNINIT :
		{
			ELM327_Check = ResetDefaults();

			if(ELM327_Check == ELM327_OK)			//in this case the module response is as expected 
			{
				ResetDefualtsFlag = 0;				//clear the flag

				ManageState = ELM327_MANAGE_IDLE;	//go to idle state

				HelperState = SEND_CMD;				//return back the helper state to it's initilized state
			}
			else if(ELM327_Check == ELM327_NOK)		//in this case the module response is not as expected 
			{
				ResetDefualtsFlag = 0;				//clear the flag

				ManageState = ELM327_MANAGE_ERROR;	//go to error state

				HelperState = SEND_CMD;				//return back the helper state to it's initilized state
			}
			else
			{
				/*ELM327 is initialized with busy so no action is needed here*/
			}
			break;
		}

		case ELM327_MANAGE_IDLE :
		{
			/*check for flags in priority battern*/
			if(ResetDefualtsFlag == 1)
			{
				ManageState = ELM327_MANAGE_UNINIT;
			}
			else if(ReadSpeedFlag == 1)
			{
				ManageState = ELM327_MANAGE_READ_SPEED;
			}
			else if(Read_RPM_Flag == 1)
			{
				ManageState = ELM327_MANAGE_READ_RPM;
			}
			else if(ReadBatterStateFlag == 1)
			{
				ManageState = ELM327_MANAGE_READ_BATTERY_STATE;
			}
			else
			{
				/*there's no other flag to be checked stay in idle case*/
			}
			break;
		}

		case ELM327_MANAGE_READ_SPEED :
		{
			ELM327_Check = ReadSpeed();

			if(ELM327_Check == ELM327_OK)
			{
				if(ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-1] == '\r')		//check if the vehicle started 
				{
					/*this means that the vehicle not yet started*/
					VehicleData = -1;									//return -1 for used as indication that the car not started
					VehicleStatus = OBD_PARKED_STATE;
				}
				else 															//this means the car is started (speed may = 0 [if not yet moved] , or speed > zero [started to move])
				{
					/*this algorithm used to convert the ASCII answer of the module to decimel numbers*/
					if((ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-1] >= ASCII_OF_ZERO) && (ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-1] <= ASCII_OF_NINE))
					{
						VehicleData = ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-1]-ASCII_OF_ZERO;
					}
					else
					{
						VehicleData = (ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-1]-ASCII_OF_A)+10;
					}

					if((ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-2] >= ASCII_OF_ZERO) && (ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-2] <= ASCII_OF_NINE))
					{
						VehicleData += (ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-2]-ASCII_OF_ZERO)*16;
					}
					else
					{
						VehicleData += ((ReceivedResponse[VEHICLE_SPEED_RRES_LENGTH-2]-ASCII_OF_A)+10)*16;
					}

					if (VehicleData == 0)
					{
						VehicleStatus = OBD_STARTED_STATE;

					}
					else
					{
						VehicleStatus = OBD_MOVING_STATE;
					}
				}

				ReadSpeedFlag = 0;

				ManageState = ELM327_MANAGE_IDLE;

				HelperState = SEND_CMD;

				ConfigPtr->GetVehicleStatusCallBackFn(VehicleStatus);
				ConfigPtr->GetVehicleSpeedCallBackFn(VehicleData);
			}
			else if(ELM327_Check == ELM327_NOK)
			{
				ReadSpeedFlag = 0;

				ManageState = ELM327_MANAGE_ERROR;

				HelperState = SEND_CMD;				
			}
			else
			{
				/*ELM327 is initialized with busy so no action is needed here*/
			}			
			break;
		}

		case ELM327_MANAGE_READ_RPM :
		{
			ELM327_Check = Read_RPM();

			if(ELM327_Check == ELM327_OK)
			{
				
				if(ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-1] == '\r')			//check if the vehicle started
				{
					/*means vehicle not started yet*/
					VehicleData = -1;			//retun -1 as indication of not started
				}
				else
				{
					/*means that the vehicle already started */
					if((ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-1] >= ASCII_OF_ZERO) && (ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-1] <= ASCII_OF_NINE))
					{
						VehicleData = ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-1]-ASCII_OF_ZERO;
					}
					else
					{
						VehicleData = (ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-1]-ASCII_OF_A)+10;
					}

					if((ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-2] >= ASCII_OF_ZERO) && (ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-2] <= ASCII_OF_NINE))
					{
						VehicleData += (ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-2]-ASCII_OF_ZERO)*16;
					}
					else
					{
						VehicleData += ((ReceivedResponse[VEHICLE_RPM_RRES_LENGTH-2]-ASCII_OF_A)+10)*16;
					}

					VehicleData <<= 6;	//equation from data sheet to estimate the RPM from output parameters
				}
				
				Read_RPM_Flag = 0;

				ManageState = ELM327_MANAGE_IDLE;

				HelperState = SEND_CMD;

				ConfigPtr-> GetVehicleRPMCallBackFn(VehicleData);
			}
			else if(ELM327_Check == ELM327_NOK)
			{
				Read_RPM_Flag = 0;

				ManageState = ELM327_MANAGE_ERROR;

				HelperState = SEND_CMD;				
			}
			else
			{
				/*ELM327 is initialized with busy so no action is needed here*/
			}	
			break;
		}

		case ELM327_MANAGE_READ_BATTERY_STATE :
		{
			ELM327_Check = ReadBatteryState();

			if(ELM327_Check == ELM327_OK)
			{

				if(ReceivedResponse[BATTERY_STATE_RRES_LENGTH-1] == '\r')		//check if the device is connected to the car
				{
					/*means that the device is not connected*/
					VehicleData = 0;	//indication that the device is not connected
				}
				else
				{
					/*means that the device is connected to the car*/
					VehicleData = 1;	//indication that the device is connected
				}

				ReadBatterStateFlag = 0;

				ManageState = ELM327_MANAGE_IDLE;

				HelperState = SEND_CMD;

				ConfigPtr -> GetVehicleBatteryStateCallBackFn(VehicleData);
			}
			else if(ELM327_Check == ELM327_NOK)
			{
				ReadBatterStateFlag = 0;

				ManageState = ELM327_MANAGE_ERROR;

				HelperState = SEND_CMD;				
			}
			else
			{
				/*ELM327 is initialized with busy so no action is needed here*/
			}				
			break;
		}

		case ELM327_MANAGE_ERROR :
		{
			ConfigPtr -> ErrorCallBackFn(ELM327_ERROR_ID);
			ManageState = ELM327_MANAGE_IDLE;


			break;
		}

		default :
		{

		}
	}
}



/***********************************************************************************
**********				ELM327 Driver UART call back functions					********
***********************************************************************************/

/*
 * This function callback function for the ELM327 for UART TX
 * Inputs:NONE
 * Output:NONE
*/

void ELM327_TxCallBackFn(void)
{
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the ELM327_ConfigType

	//start receiving the response of the command 
	UART_StartSilentReception(ReceivedResponse, ReceivedResponseLength, ConfigPtr->UartChannelId);
}


/*
 * This function callback function for the GSM driver UART RX
 * Inputs:NONE
 * Output:NONE
*/

void ELM327_RxCallBackFn(void)
{
	ELM327_CheckType ELM327_Check = ELM327_NOK;					//variable to indicate the success of the reception begin

	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	//compare the received response with the expected response 
	ELM327_Check = StrComp(ReceivedResponse, ExpectedResponse, ExpectedResponseLength);

	ReceiveDoneFlag = 1 ;

	if(ELM327_Check == ELM327_OK)
	{
		ReceiveSuccessFlag = 1 ;
	}

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	UART_StartSilentTransmission (ReceivedResponse,ReceivedResponseLength,1);

	#endif
}



/***********************************************************************************
**********					Helper functions' bodies						********
***********************************************************************************/

/*
 * This function is a FSM to reset the system to default
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ResetDefaults(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_BUSY;			//variable to indicate the success of the AT command

	switch(HelperState)
	{
		case SEND_CMD :
		{
			ATCMD_System_Reset();					//send the command

			HelperState = PROCCESS_RECEIVED_RES;	//go to the next state

			MaximumResponseTimeCounter=0;			//initilize the response time counter with 0

			break;
		}

		case PROCCESS_RECEIVED_RES :
		{
			if(ReceiveDoneFlag == 1)			//check if response is received from the module
			{
				ReceiveDoneFlag=0;				//response is received, so we clear the flag for the next use
				
				if(ReceiveSuccessFlag == 1)		//check if the response is received correctly
				{
					ReceiveSuccessFlag = 0;		//response received correctly , se clear the flag

					RetVar = ELM327_OK;			//return ok for the check in the main manage function
				}
				else
				{
					RetVar = ELM327_NOK;		//return not ok for the check in the main manage function
				}
			}
			else 								//incase response not received yet
			{
				if(MaximumResponseTimeCounter<ELM327_T_EXP)		//if the counter is less than the expected maximum response time
				{
					MaximumResponseTimeCounter++;				//increase the counter to reach the maximum expected response time
				}
				else 											//in this case the counter exceeds the limits
				{
					MaximumResponseTimeCounter = 0 ;	//clear the counter
					ELM327_ATCMD_Stop();				//stop the reception
					RetVar = ELM327_NOK;				//return not ok for the check in the main manage function
				}
			}

			break;
		}

		default :
		{
			RetVar = ELM327_NOK;
		}
	}
	return RetVar;
}

/*
 * This function is a FSM to read the speed of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ReadSpeed(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_BUSY;						//variable to indicate the success of the AT command

	switch(HelperState)
	{
		case SEND_CMD :
		{
			ATCMD_Vehicle_Speed();

			HelperState = PROCCESS_RECEIVED_RES;

			MaximumResponseTimeCounter=0;

			break;
		}

		case PROCCESS_RECEIVED_RES :
		{
			if(ReceiveDoneFlag == 1)
			{
				ReceiveDoneFlag=0;

				if(ReceiveSuccessFlag == 1)
				{

					ReceiveSuccessFlag = 0;		//reset the flag for next use

					RetVar = ELM327_OK;			//return ok for manager check
				}
				else
				{
					RetVar = ELM327_NOK;		//return not ok for the manager check
				}
			}
			else
			{
				if(MaximumResponseTimeCounter<ELM327_T_EXP)		//check for expiration time 
				{
					MaximumResponseTimeCounter++;
				}
				else
				{
					MaximumResponseTimeCounter = 0 ;

					ELM327_ATCMD_Stop();

					RetVar = ELM327_NOK;
				}
			}

			break;
		}

		default :
		{
			RetVar = ELM327_NOK;
		}
	}
	return RetVar;
}


/*
 * This function is a FSM to read the rpm of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType Read_RPM(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_BUSY;						//variable to indicate the success of the AT command

	switch(HelperState)
	{
		case SEND_CMD :
		{
			ATCMD_Vehicle_RPM();

			HelperState = PROCCESS_RECEIVED_RES;

			MaximumResponseTimeCounter=0;

			break;
		}

		case PROCCESS_RECEIVED_RES :
		{
			if(ReceiveDoneFlag == 1)
			{
				ReceiveDoneFlag=0;

				if(ReceiveSuccessFlag == 1)
				{
					ReceiveSuccessFlag = 0;

					RetVar = ELM327_OK;
				}
				else
				{
					RetVar = ELM327_NOK;
				}
			}
			else
			{
				if(MaximumResponseTimeCounter<ELM327_T_EXP)
				{
					MaximumResponseTimeCounter++;
				}
				else
				{
					MaximumResponseTimeCounter = 0 ;

					ELM327_ATCMD_Stop();

					RetVar = ELM327_NOK;
				}
			}

			break;
		}

		default :
		{
			RetVar = ELM327_NOK;
		}
	}
	return RetVar;
}


/*
 * This function is a FSM to read the rpm of the vehicle
 * Inputs:NONE
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType ReadBatteryState(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_BUSY;						//variable to indicate the success of the AT command

	switch(HelperState)
	{
		case SEND_CMD :
		{
			ATCMD_Battery_State();

			HelperState = PROCCESS_RECEIVED_RES;

			MaximumResponseTimeCounter=0;

			break;
		}

		case PROCCESS_RECEIVED_RES :
		{
			if(ReceiveDoneFlag == 1)
			{
				ReceiveDoneFlag=0;

				if(ReceiveSuccessFlag == 1)
				{
					ReceiveSuccessFlag = 0;
					RetVar = ELM327_OK;
				}
				else
				{
					RetVar = ELM327_OK;
				}
			}
			else
			{
				if(MaximumResponseTimeCounter<ELM327_T_EXP)
				{
					MaximumResponseTimeCounter++;
				}
				else
				{
					MaximumResponseTimeCounter = 0 ;

					ELM327_ATCMD_Stop();
					
					RetVar = ELM327_NOK;
				}
			}

			break;
		}

		default :
		{
			RetVar = ELM327_NOK;
		}
	}
	return RetVar;
}

//**********************************************************************************//

/*
 * This function used to establish the communication with the ELM327 module
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_System_Reset(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_OK;						//variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;							//variable to indicate the success of the transmission beginning
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[SYSTEM_RESET_CMD_LENGTH] = {'A','T','Z','\r'};//the command to be sent

	//assign the expected response
	ExpectedResponse[0]  = '\r';
	ExpectedResponse[1]  = '\r';
	ExpectedResponse[2]  = '\r';
	ExpectedResponse[3]  = 'E';
	ExpectedResponse[4]  = 'L';
	ExpectedResponse[5]  = 'M';
	ExpectedResponse[6]  = '3';
	ExpectedResponse[7]  = '2';
	ExpectedResponse[8]  = '7';
	ExpectedResponse[9] = ' ';
	ExpectedResponse[10] = 'v';
	ExpectedResponse[11] = '1';
	ExpectedResponse[12] = '.';
	ExpectedResponse[13] = '5';
	ExpectedResponse[14] = '\r';
	ExpectedResponse[15] = '\r';
	ExpectedResponse[16] = '>';

	ReceivedResponseLength = SYSTEM_RESET_RRES_LENGHTH;//assign the length of the Received Response
	ExpectedResponseLength = SYSTEM_RESET_ERES_LENGHTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, SYSTEM_RESET_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = ELM327_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}



/*
 * This function used to read the SPEED of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Vehicle_Speed(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_OK;						//variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;							//variable to indicate the success of the transmission beginning
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[VEHICLE_SPEED_CMD_LENGTH] = {'0','1','0','D','\r'};//the command to be sent

	//assign the expected response
	ExpectedResponse[0]  = '\r';


	ReceivedResponseLength = VEHICLE_SPEED_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = VEHICLE_SPEED_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, VEHICLE_SPEED_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = ELM327_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}


/*
 * This function used to read the RPM of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Vehicle_RPM(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_OK;						//variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;							//variable to indicate the success of the transmission beginning
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[VEHICLE_RPM_CMD_LENGTH] = {'0','1','0','C','\r'};//the command to be sent

	//assign the expected response
	ExpectedResponse[0]  = '\r';

	ReceivedResponseLength = VEHICLE_RPM_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = VEHICLE_RPM_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, VEHICLE_RPM_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = ELM327_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}


/*
 * This function used to read the battery state of the car
 *Inputs:NONE
 * Output:
 *	- an indication of the success of the function
*/
static ELM327_CheckType ATCMD_Battery_State(void)
{
	//declarations
	ELM327_CheckType RetVar = ELM327_OK;						//variable to indicate the success of the AT command
	UART_ChkType UART_Check = UART_OK;							//variable to indicate the success of the transmission beginning
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	uint8_t CommandToSend[BATTERY_STATE_CMD_LENGTH] = {'A','T','R','V','\r'};//the command to be sent

	//assign the expected response
	ExpectedResponse[0]  = '\r';


	ReceivedResponseLength = BATTERY_STATE_RRES_LENGTH;//assign the length of the Received Response
	ExpectedResponseLength = BATTERY_STATE_ERES_LENGTH;//assign the length of the expectedResponse

	//start the transmission of the command
	UART_Check = UART_StartSilentTransmission(CommandToSend, BATTERY_STATE_CMD_LENGTH, ConfigPtr->UartChannelId);

	#ifdef DEVELOPMENT_MODE_ENABLE //check if the development mode is enabled

	//if the the start of transmission wasn't successful
	if(UART_Check == UART_NOK)
	{
		//transmission wasn't successful
		RetVar = ELM327_NOK;
	}
	else{/*RetVar is initialized to OK so no action is needed here*/}

	#endif

	return RetVar;
}

/*
 *	This function is used to stop the Uart transmission
 *	Inputs:NONE
 *	Output:NONE
 **/
static void ELM327_ATCMD_Stop(void)
{
	const ELM327_ConfigType* ConfigPtr = &ELM327_ConfigParam;	//declare a pointer to structure of the GSM_ConfigType

	UART_StopCrntReception(ConfigPtr->UartChannelId);
}


/*
 * This function used to compare two strings
 * Inputs:
 *		- Str1		: A pointer to the first string
 *		- Str2		: A pointer to the second string
 *		- Length	: the length of the two strings       
 * Output:
 *		- an indication of the success of the function
*/
static ELM327_CheckType StrComp(uint8_t* Str1, uint8_t* Str2, uint8_t Length)
{
	//variable declaration
	uint8_t Index; //loop index
	ELM327_CheckType RetVar = ELM327_OK;// variable to indicate the success of the function

	//String compare loop
	for(Index = 0; ((Index < Length) && (RetVar == ELM327_OK)); Index++)
	{
		//compare the ith char of the two strings
		if(Str1[Index] != Str2[Index])
		{
			//the ith chars of the two strings don't match
			RetVar = ELM327_NOK;
		}
		else{/*the loop shall continue till till the last character in the 2 strings or finding a miss match between them*/}
	}

	return RetVar;
}

