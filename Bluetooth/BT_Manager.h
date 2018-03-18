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
    CHECK_RECEPTION_DONE = 1,
    SET_COMMAND_MODE = 2,
    SET_FACTORY_SETTINGS = 3,
    SET_DEVICE_NAME = 4,
    SET_SLAVE_MODE = 5,
    SET_NO_AUTHENTICATION = 6,
    SET_SCAN_TIME_SI = 7,
    SET_SCAN_TIME_SJ = 8,
    DISABLE_REMOTE_CONFIGURATION = 9,
    EXIT_COMMAND_MODE=10,
    REBOOT = 11,
    CHECK_RECEIVED_DATA = 12
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

BT_Response MemoryCompare(unsigned char* AT_Command , uint8_t Length);
BT_CheckType BT_Configure(void);
void BT_Init(void);
#endif
