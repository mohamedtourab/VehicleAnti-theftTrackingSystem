#include "BT_Driver.h"

//void BT_TxDone(void);
//void BT_RxDone(void);
//void BT_ConnectionKilled(void);
//void BT_CheckConnection(uint8_t);

const BT_ConfigType BT_ConfigParam = 
{
    /*
        Enter here, between double quotes, the name of the Bluetooth module
    */
    "doo7a",
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
    0,

    0,
	
	0
};