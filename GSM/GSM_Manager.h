#include "GSM.h"

#define CYCLIC_TIME 10

typedef enum {GSM_Manage_OK = 0, GSM_Manage_NOK, GSM_Manage_InProgress} GSM_Manage_CheckType;

typedef enum {UNINIT=0,IDLE,START,SWRESET,SENDSMS,RECIEVESMS,ERROR} GSM_State;

void GSM_Init(void);
void SendSMS(uint8_t* Msg, uint8_t MsgLength, uint8_t* PhoneNum);
void StartCommunication(void);
void SoftWareReset(void);


GSM_Manage_CheckType GSM_ManageOngoingOperation(void);
