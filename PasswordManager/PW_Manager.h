#ifndef PW_MANAGER_H
#define PW_MANAGER_H
#include "BT_Manager.h"
#include "NVM_Manager.h"
#include "PW_ManagerConfig.h"

// authentication states
typedef enum {NON_AUTH = 0, AUTH}AuthenticationStatus;

// callback function to get the authentication status
typedef void (*AuthStatusCallBackFn)(AuthenticationStatus);

// configuration parameters of PW manager
typedef struct 
{
	// the Password in ascii format
	uint8_t Password[PW_LENGTH];

	// the index of the required block in NVM to read the ket
	uint8_t NVM_BlockId;

	// pointer to function which is executed to determine the status of authentication
	AuthStatusCallBackFn AuthCallBackPtr;
}PW_ConfigParameters;

// function to initialize the state and global variables
void PW_Init(void);

// API for application to request the authentication status
void GetPasswordStatus (void);

// cyclic function to to check the authentication status
void PW_Manage (void);

// callback of get the bluetooth status
void BT_StatusCallBack (uint8_t Status);

// callback function to indicate finish reception through bluetooth
void BT_ReceptionDoneCallBack (void);

// callback function to indicate finish reading from NVM
void NVM_ReadingDoneCallBack (void);

// callback function to indicate that key is changed from default case
void KeyChangedCallBack (void);




#endif