#ifndef BT_DRIVER_H
#define BT_DRIVER_H

#include <stdint.h>


#include "GPIO.h"
#include "UART.h"
#include "BT_Config.h"

#define NUMBER_OF_COMMANDS      4U
#define LENGTH_OF_BT_RESPONSE   3U

typedef void (*BT_CallBackPtrType)(void);
/*
 *  This is a new type which is a pointer to function which returns void
 *  and has a uint8_t argument, which holds the connection status of the module
 * 
 */

typedef void (*BT_CS_CallBackPtrType)(uint8_t);

typedef enum
{
    CHECK_TRANSMISSION_DONE = 0,
    CHECK_RECEPTION_DONE ,
    SET_COMMAND_MODE ,
    SET_FACTORY_SETTINGS ,
    SET_DEVICE_NAME ,
    SET_SLAVE_MODE ,
    SET_NO_AUTHENTICATION ,
    DISABLE_REMOTE_CONFIGURATION ,
    EXIT_COMMAND_MODE,
    REBOOT ,
    CHECK_RECEIVED_DATA,
    CHANGE_BR 
}BT_State;

typedef enum
{
    BT_OK  = 0,
    BT_NOK = 1,
	BT_BUSY = 2
}BT_CheckType;

typedef enum
{
    CMD = 0,
    AOK,
    ERR,
    CONN,
    NOT_CONN,
    KILL,
    END
}BT_Response;


typedef enum
{
	BT_DATA_IDLE = 0,
	BT_DATA_BUSY
}BT_DataStates;

typedef enum
{
    BT_CONN_SET_CMD,
    BT_CONN_CHECK_RECEIVED_DATA,
	BT_CONN_SEND_GK,
    BT_CONN_START_RECEPTION,
	BT_CONN_BUSY,
	BT_CONN_EXIT_CMD
}BT_ConnectionStates;

typedef enum
{
    BT_STOP_COMM_SET_CMD,
    BT_STOP_COMM_CHECK_RECEIVED_DATA,
	BT_STOP_COMM_SEND_K,
    BT_STOP_COMM_START_RECEPTION,
	BT_STOP_COMM_BUSY,
	BT_STOP_COMM_EXIT_CMD,
}BT_StopCommunicationStates;

typedef enum
{
    CHANNEL_ID_0 = 0,
    CHANNEL_ID_1 = 1,
    CHANNEL_ID_2 = 2,
    CHANNEL_ID_3 = 3,
    CHANNEL_ID_4 = 4,
    CHANNEL_ID_5 = 5
}BT_UART_ChannelID;

typedef struct
{
    char BT_Name[25];
    uint8_t BT_ChannelID;
    BT_CallBackPtrType BT_TransmissionCallBackPtr;
    BT_CallBackPtrType BT_ReceptionCallBackPtr;
    BT_CallBackPtrType BT_KillConnectionStatusCallBackPtr;
	BT_CS_CallBackPtrType BT_ConnectionStatusCallBackPtr;
}BT_ConfigType;

extern const BT_ConfigType BT_ConfigParam;
extern uint8_t BT_InitFlag;

BT_CheckType BT_GetData(uint8_t* DataReceived,uint8_t NoOfBytes);
BT_CheckType BT_SendData(uint8_t* DataSent, uint8_t NoOfBytes);
BT_CheckType BT_GetConnectionStatus(void);
BT_CheckType BT_StopCommunication(void);
BT_CheckType BT_ChangeBaudRate(void);

static BT_Response MemoryCompare(unsigned char* AT_Command , uint8_t Length);
BT_CheckType BT_Configure(void);
static unsigned int Parser (const char InputName[],char OutputName[]);
void BT_Init(void);
void BluetoothTxDone(void);
void BluetoothRxDone(void);

#endif