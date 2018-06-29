#ifndef BT_CONFIG_H
#define BT_CONFIG_H

#define BT_CYCLIC_TIME (10U)

void BT_TransmissionnDoneCallBack(void);
void BT_ReceptionDoneCallBack(void);
void BT_DisconnectedCallBack(void);
void BT_StatusCallBack(uint8_t);
void BT_ErrorCallBack(uint8_t);

#endif /*end of BT_CONFIG_H*/

