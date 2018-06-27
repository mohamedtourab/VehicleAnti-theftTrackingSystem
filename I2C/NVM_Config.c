#include "NVM_Manager.h"
#include "NVM_Config.h"

NVM_ConfigType NVM_ConfigParam [NO_OF_NVM_USED] = 
{
    {
        0xA0,//Slave Address
        1, //Numer of Bytes
        0x00, //Location Address to Read Or Write
        NVM_WriteDone,//Write call back function
        NVM_ReadDone //Read call back function
    }
}