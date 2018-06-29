#ifndef BT_MANAGER_H
#define BT_MANAGER_H

#include "BT_Driver.h"

void BT_StartSendingData(uint8_t* DataToBeSent, uint8_t DataLength);
void BT_StartReceivingData(uint8_t* ReceivedData, uint8_t DataLength);
void BT_ConnectionStatus(void);
void BT_Manager(void);

typedef enum
{
    BT_INIT,
    BT_IDLE,
    BT_TRANSMIT,
    BT_RECEIVE,
	BT_GET_CONNECTION_STATUS,
    BT_KILL_CONNECTION,
    BT_ERROR
}BT_ManagerState;

#endif 