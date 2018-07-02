#include "BT_Driver.h"
#include "BT_Config.h"

const BT_ConfigType BT_ConfigParam = 
{
    /*
        Enter here, between double quotes, the name of the Bluetooth module
    */
    "LAB TEST",
	
    /*
        Enter here the index of the structure inside the array of structures for the UART configuration file
    */
    CHANNEL_ID_0,
    /*
        Enter here the name of the function to be called after transmission is done
    */
    0,
    /*
        Enter here the name of the function to be called after reception is done
    */
    BT_ReceptionDoneCallBack,

    BT_DisconnectedCallBack,
	
	BT_StatusCallBack,

    Error_CallBack
};

