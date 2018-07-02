#include "NVM_Manager.h"
#include "NVM_Config.h"

void Error_CallBack(uint8_t ErrorID);

NVM_ConfigType NVM_ConfigParam [NO_OF_NVM_USED] = 
{
    {
        /*EEPROM Slave Address*/
        0xA0,
        /*Numer of Bytes*/
        5,
        /*Location Address to Read Or Write*/
        0x30,
        /*Write Call Back Function*/
        NVM_WriteSuccess,
        /*Read Call Back Function*/
        NVM_ReadingDoneCallBack,
        /*Error Call Back Function*/
        Error_CallBack
    },

    {
        /*EEPROM Slave Address*/
        0xA0,
        /*Numer of Bytes*/
        1,
        /*Location Address to Read Or Write*/
        0x60,
        /*Write Call Back Function*/
        NVM_WriteSuccess,
        /*Read Call Back Function*/
        NVM_ReadingDoneCallBack,
        /*Error Call Back Function*/
        Error_CallBack
    }
};


