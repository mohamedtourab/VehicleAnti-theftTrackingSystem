#ifndef NVM_MANAGER_H
#define NVM_MANAGER_H
#include "I2C_Manager.h"
#include "NVM_Config.h"


typedef void (*NVM_CallBackPointerType)(void);

void NVM_WriteDone(void);
void NVM_ReadDone(void);


typedef enum
{
    NVM_UNINIT,
    NVM_IDLE,
    NVM_WRITE,
    NVM_READ,
    NVM_WAIT
}NVM_States;

typedef enum 
{
    NVM_BUSY,
    NVM_NOT_BUSY,
    NVM_OK,
    NVM_NOK
}NVM_CheckType;

typedef struct
{
    uint8_t NVM_SlaveAddress;
    uint8_t NVM_NoOfBytes;
    //uint8_t NVM_LocationAddress;
    NVM_CallBackPointerType NVM_WriteDoneCallBackPtr;
    NVM_CallBackPointerType NVM_ReadDoneCallBackPtr;

}NVM_ConfigType;



NVM_CheckType NVM_Init(void);
NVM_CheckType NVM_Write(uint8_t ConfigStructure_ID, uint8_t* DataPointer);
NVM_CheckType NVM_Read(uint8_t ConfigStruct, uint8_t* DataPointer);
void NVM_Manager(void);

extern NVM_ConfigType NVM_ConfigParam [NO_OF_NVM_USED];
extern uint8_t I2C_InitFlag ;


#endif