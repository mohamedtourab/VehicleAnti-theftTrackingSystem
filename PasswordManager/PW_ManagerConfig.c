#include "PW_Manager.h"
void AuthenticattionStatusCallBackFn (AuthenticationStatus ST);

const PW_ConfigParameters	PW_Configuration = 
{
	"1234",					 // password
	0,									//block ID in NVM where the key is stored
	AuthenticattionStatusCallBackFn		// callback function to get the authentication status
};