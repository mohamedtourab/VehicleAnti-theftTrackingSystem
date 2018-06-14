#include "BT_Driver.h"

void BT_TxDone(void);
void BT_RxDone(void);

const BT_ConfigType BT_ConfigParam = 
{
    /*
        Enter here, between double quotes, the name of the Bluetooth module
    */
    "Wessam",
    /*
        Enter here the index of the structure inside the array of structures for the UART configuration file
    */
    CHANNEL_ID_0,
    /*
        Enter here the name of the function to be called after transmission is done
    */
    BT_TxDone,
    /*
        Enter here the name of the function to be called after reception is done
    */
    BT_RxDone
};