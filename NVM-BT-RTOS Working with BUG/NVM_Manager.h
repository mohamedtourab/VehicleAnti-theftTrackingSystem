#ifndef NVM_MANAGER_H
#define NVM_MANAGER_H
#include <stdint.h>
#include "I2C_Manager.h"
#include "NVM_Config.h"


typedef void (*NVM_CallBackPointerType)(void);
typedef void (*NVM_ErrorCallBackPointerType)(uint8_t);




typedef enum
{
    NVM_UN_INIT,
    NVM_IDLE,
    NVM_WRITE,
    NVM_READ,
    NVM_WAIT,
    NVM_ERROR
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
    uint8_t NVM_LocationAddress;
    NVM_CallBackPointerType NVM_WriteDoneCallBackFunction;
    NVM_CallBackPointerType NVM_ReadDoneCallBackFunction;
    NVM_ErrorCallBackPointerType NVM_ErrorCallBackFunction;

}NVM_ConfigType;



NVM_CheckType NVM_Init(void);
NVM_CheckType NVM_Write(uint8_t ConfigStructure_ID, uint8_t* DataPointer);
NVM_CheckType NVM_Read(uint8_t ConfigStruct, uint8_t* DataPointer);
void NVM_Manager(void);
static void Concatenator(uint8_t GlobalConfigStructure_ID, uint8_t* OriginalArray);

extern uint8_t I2C_InitFlag ;
extern NVM_ConfigType NVM_ConfigParam [NO_OF_NVM_USED];
#endif

