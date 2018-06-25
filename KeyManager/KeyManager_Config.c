#include "KeyManager.h"
#include "PW_Manager.h"
KeyConfigType KeyConfigurationParameter = 
{
	0,			 // The Button index of the used Pin to set the Key
	0, 			 // The UART index of the used Pin to set the Key
	0,			 // The block id in NVM to store the new key
	KeyChangedCallBack			 // CallBack function in case of valid location
};