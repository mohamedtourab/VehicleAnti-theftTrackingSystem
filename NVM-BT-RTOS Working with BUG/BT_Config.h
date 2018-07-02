#ifndef BT_CONFIG_H
#define BT_CONFIG_H

#include <stdint.h>

#define BT_CYCLIC_TIME (100U)

void BT_TransmissionnDoneCallBack(void);
void BT_ReceptionDoneCallBack(void);
void BT_DisconnectedCallBack(void);
void BT_StatusCallBack(uint8_t);
void Error_CallBack(uint8_t);

#endif /*end of BT_CONFIG_H*/

