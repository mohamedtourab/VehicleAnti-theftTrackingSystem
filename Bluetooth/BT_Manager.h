#ifndef BT_CONFIG_H
#define BT_CONFIG_H

#include <stdint.h>


#include "GPIO.h"
#include "UART.h"

#define NUMBER_OF_COMMANDS      4U
#define LENGTH_OF_BT_RESPONSE   3U
typedef enum
{
    CHECK_TRANSMISSION_DONE = 0,
    CHECK_RECEPTION_DONE ,
    SET_COMMAND_MODE ,
    SET_FACTORY_SETTINGS ,
    SET_DEVICE_NAME ,
    SET_SLAVE_MODE ,
    SET_NO_AUTHENTICATION ,
   // SET_SCAN_TIME_SI ,
   //SET_SCAN_TIME_SJ ,
    DISABLE_REMOTE_CONFIGURATION ,
    EXIT_COMMAND_MODE,
    REBOOT ,
    CHECK_RECEIVED_DATA 
}BT_State;

typedef enum
{
    BT_OK  = 0,
    BT_NOK = 1
}BT_CheckType;

typedef enum
{
    CMD = 0,
    AOK,
    ERR,
    END
}BT_Response;

typedef enum
{
	BT_IDLE = 0,
	BT_BUSY
}BT_DataStates;

BT_CheckType BT_GetData(char* DataReceived,uint8_t NoOfBytes);
BT_CheckType BT_SendData(char* DataSent, uint8_t NoOfBytes);
BT_Response MemoryCompare(unsigned char* AT_Command , uint8_t Length);
BT_CheckType BT_Configure(void);
void BT_Init(void);
#endif