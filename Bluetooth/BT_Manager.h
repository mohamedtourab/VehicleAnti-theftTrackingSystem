#ifndef BT_MANAGER_H
#define BT_MANAGER_H

#include "BT_Driver.h"

void BT_StartSendingData(char* DataToBeSent, uint8_t DataLength);
void BT_StartReceivingData(char* ReceivedData, uint8_t DataLength);
void BT_Manager(void);

typedef enum
{
    BT_INIT,
    BT_IDLE,
    BT_CONFIGURE,
    BT_TRANSMIT,
    BT_RECEIVE
}BT_ManagerState;

#endif 