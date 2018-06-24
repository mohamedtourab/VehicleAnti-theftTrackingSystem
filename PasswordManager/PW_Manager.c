#include "PW_Manager.h"
#include "PW_ManagerConfig.h"


#define TIMEOUT		10000U


// Manager States
typedef enum {IDLE_STATE = 0, BT_STATUS_STATE, BT_RECEPTION_STATE, KEY_READ_STATE, DECRYPTION_STATE, NON_AUTH_STATE, AUTH_STATE}PW_ManagerStates;

// helper functions states
typedef enum {PW_CMD = 0, PW_WAIT}HelperFnStates;

// enum for the return type of helper functions
typedef enum {PW_OK = 0, PW_BUSY, PW_NOK}PW_CheckType;


// global variable to store the manager state
static PW_ManagerStates ManagerState;

// global variable to store the helper functions state
static HelperFnStates HelperState;

// flag to start authentication process
static uint8_t StartAuth;

// globall variable to get the bluetooth status
static uint8_t BT_Status;

// global variable to be set in the callback function of reception done
static uint8_t BT_FinishReceiveing;

// global variable to be set in the callback function of NVM reading done
static uint8_t NVM_FinishReading;

// global variable to be set in the callback function when the key is changed
static uint8_t KeyChanged;

// global variable to be set in the BT status callback function
static uint8_t BT_StatusCallBackFlag;

// global array to store the received encrypted data from bluetooth
static uint8_t BT_Data[PW_LENGTH];

// global array to store the key that will be read from NVM
static uint8_t Key[PW_LENGTH];

// global array to store the decrypted password by xoring
static uint8_t DecryptedData[PW_LENGTH];


// the structure of the configuration parameter
extern const PW_ConfigParameters	PW_Configuration;


static PW_CheckType GetBluetoothStatus (void);
static PW_CheckType PW_BluetoothReception (void);
static PW_CheckType NVM_KeyRead (void);

// function to initialize the state and global variables
void PW_Init(void)
{
	uint8_t LoopIndex = 0;
	ManagerState = IDLE_STATE;
	StartAuth = 0;
	BT_Status = 0; // not connected
	BT_FinishReceiveing = 0;
	HelperState = PW_CMD;
	BT_StatusCallBackFlag = 0;
	for (LoopIndex = 0; LoopIndex < PW_LENGTH; LoopIndex++)
	{
		BT_Data[LoopIndex] = 0;
		DecryptedData[LoopIndex] = 0;
	}
	Key[0] = '5';
	Key[1] = '0';
	Key[2] = '6';
	Key[3] = '0';
	NVM_FinishReading = 0;
	KeyChanged = 0;
}

// API for application to request the authentication status
void GetPasswordStatus (void)
{
	StartAuth = 1;
}

// cyclic function to to check the authentication status
void PW_Manage (void)
{
	PW_CheckType Check = PW_BUSY;
	uint8_t LoopIndex;
	static uint8_t ErrorPassword = 0;
	switch (ManagerState)
	{
		case IDLE_STATE:
		{
			if (StartAuth)
			{
				StartAuth = 0;
				ManagerState = BT_STATUS_STATE;
			}
			else{/*wait in idle state till application request an authentication*/}
		}
		break;

		case BT_STATUS_STATE:
		{
			// check the bluetooth connection status
			Check = GetBluetoothStatus();
			// check if if th bluetooth return the status or timout occured
			// if bluetooth return the status
			if (Check == PW_OK)
			{
				// check the connection status
				// if disconnected
				if (BT_Status == 0)
				{
					// go to non authenticated state
					ManagerState = NON_AUTH_STATE;
				}
				// if connected
				else
				{
					ManagerState = BT_RECEPTION_STATE;
				}
			}
			// if timeout error occured
			else if (Check == PW_NOK)
			{
				// go to non authenticated state
				ManagerState = NON_AUTH_STATE;
			}
			else {/*The bluetooth is busy so wait till bluetooth return the status or timeout ocuures*/}
		}
		break;

		case BT_RECEPTION_STATE:
		{
			Check = PW_BluetoothReception();
			// check if the bluetooth finish reception or timeout error ocuured
			// if reception finished
			if (Check == PW_OK)
			{
				// go to NVM read state 
				ManagerState = KEY_READ_STATE;
			}
			// if timeout error occured
			else if (Check == PW_NOK)
			{
				// go to non authenticated state
				ManagerState = NON_AUTH_STATE;
			}
			else {/*The bluetooth is busy so wait till bluetooth finish reception or timeout ocuures*/}
		}
		break;

		case KEY_READ_STATE:
		{
			Check = NVM_KeyRead();
			// check if the NVM finish reading or timeout error ocuured
			// if reception finished
			if (Check == PW_OK)
			{
				// go to decryption state 
				ManagerState = DECRYPTION_STATE;
			}
			// if timeout error occured
			else if (Check == PW_NOK)
			{
				// go to non authenticated state
				ManagerState = NON_AUTH_STATE;
			}
			else {/*The NVM is busy so wait till NVM finish reading or timeout ocuures*/}
		}
		break;

		case DECRYPTION_STATE:
		{
			// loop over the key array and the received encrypted data array and peroform XOR between each element in the twoo arrays
			for (LoopIndex = 0; ((LoopIndex < PW_LENGTH) && (ErrorPassword == 0)); LoopIndex++)
			{
				DecryptedData[LoopIndex] = Key[LoopIndex] ^ BT_Data[LoopIndex];
				// if the decrypted digit is not equal to the corresponding digit in the constant Password in config file 
				if (DecryptedData[LoopIndex] != PW_Configuration.Password[LoopIndex])
				{
					// set the error flag
					ErrorPassword = 1;
				}
			}

			// if the used key in the mobile application is not the same as in the device
			if (ErrorPassword == 1)
			{
				// go to not authenticated state
				ManagerState = NON_AUTH_STATE;
			}
			// if the used key in mobile application is the same as in device
			else
			{
				// go to authenticated state
				ManagerState = AUTH_STATE;
			}
		}
		break;

		case NON_AUTH_STATE:
		{
			// execute the callback function withe non authenticated status as argument
			PW_Configuration.AuthCallBackPtr(NON_AUTH);
			// go to IDLE state
			ManagerState = IDLE_STATE;
		}
		break;

		case AUTH_STATE:
		{
			// execute the callback function withe authenticated status as argument
			PW_Configuration.AuthCallBackPtr(AUTH);
			// go to IDLE state
			ManagerState = IDLE_STATE;
		}
		break;

		default:{/*Default state is IDLE State*/}
	}
}


// callback of get the bluetooth status
void BT_StatusCallBack (uint8_t Status)
{
	BT_Status = Status;
	BT_StatusCallBackFlag = 1;
}

// callback function to indicate finish reception through bluetooth
void BT_ReceptionDoneCallBack (void)
{
	BT_FinishReceiveing = 1;
}

// callback function to indicate finish reading from NVM
void NVM_ReadingDoneCallBack (void)
{
	NVM_FinishReading = 1;
}

// callback function to indicate that key is changed from default case
void KeyChangedCallBack (void)
{
	KeyChanged = 1;
}



// helper function to request the connection status of bluetooth and wait the callback
static PW_CheckType GetBluetoothStatus (void)
{
	static uint32_t TimeOutCounter;
	PW_CheckType  RetVar = PW_BUSY;
	switch (HelperState)
	{
		case PW_CMD:
		{
			// request the BT connection from BT manager
			BT_ConnectionStatus();
			// change state to wait the callback from BT manager
			HelperState = PW_WAIT;
		}
		break;

		case PW_WAIT:
		{
			// check the time out error counter
			// if it does not reach the predetermined time out
			if (TimeOutCounter < (TIMEOUT/PW_MANAGER_CYCLIC_CTIME))
			{
				// check if the callback function is executed or no
				if (BT_StatusCallBackFlag)
				{
					// clear the flag
					BT_StatusCallBackFlag = 0;
					HelperState = PW_CMD;
					RetVar = PW_OK;
				}
				else{/*wait the call back from BT manager*/}
				TimeOutCounter++;
			}
			// if the counter reaches the predetermined timeout
			else
			{
				TimeOutCounter = 0;
				HelperState = PW_CMD; 
				RetVar = PW_NOK;
			}
		}
		break;
	}
	return RetVar;
}

// helper function to request the reception bluetooth and wait the callback
static PW_CheckType PW_BluetoothReception (void)
{
	static uint32_t TimeOutCounter;
	PW_CheckType  RetVar = PW_BUSY;
	switch (HelperState)
	{
		case PW_CMD:
		{
			// request the reception from BT manager
			BT_StartReceivingData(BT_Data, PW_LENGTH);
			// change state to wait the callback from BT manager
			HelperState = PW_WAIT;
		}
		break;

		case PW_WAIT:
		{
			// check the time out error counter
			// if it does not reach the predetermined time out
			if (TimeOutCounter < (TIMEOUT/PW_MANAGER_CYCLIC_CTIME))
			{
				// check if the callback function is executed or no
				if (BT_FinishReceiveing)
				{
					// clear the flag
					BT_FinishReceiveing = 0;
					HelperState = PW_CMD;
					RetVar = PW_OK;
				}
				else{/*wait the call back from BT manager*/}
				TimeOutCounter++;
			}
			// if the counter reaches the predetermined timeout
			else
			{
				TimeOutCounter = 0;
				HelperState = PW_CMD; 
				RetVar = PW_NOK;
			}
		}
		break;
	}
	return RetVar;
}

// helper function to request the reading from NVM and wait the callback
static PW_CheckType NVM_KeyRead (void)
{
	static uint32_t TimeOutCounter;
	PW_CheckType  RetVar = PW_BUSY;
	switch (HelperState)
	{
		case PW_CMD:
		{
			// if the key was changed
			if (KeyChanged)
			{
				// request reading from NVM manager
				NVM_Read(PW_Configuration.NVM_BlockId, Key);
				// change state to wait the callback from NVM manager
				HelperState = PW_WAIT;
			}
			else
			{
				RetVar = PW_OK;
			}
		}
		break;

		case PW_WAIT:
		{
			// check the time out error counter
			// if it does not reach the predetermined time out
			if (TimeOutCounter < (TIMEOUT/PW_MANAGER_CYCLIC_CTIME))
			{
				// check if the callback function is executed or no
				if (NVM_FinishReading)
				{
					// clear the flag
					NVM_FinishReading = 0;
					HelperState = PW_CMD;
					RetVar = PW_OK;
				}
				else{/*wait the call back from NVM manager*/}
				TimeOutCounter++;
			}
			// if the counter reaches the predetermined timeout
			else
			{
				TimeOutCounter = 0;
				HelperState = PW_CMD; 
				RetVar = PW_NOK;
			}
		}
		break;
	}
	return RetVar;
}
