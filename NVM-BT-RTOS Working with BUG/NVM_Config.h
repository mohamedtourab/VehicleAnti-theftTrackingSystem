#ifndef NVM_CONFIG_H
#define NVM_CONFIG_H

#define NO_OF_NVM_USED  (2U)
#define MEMORY_SIZE 	(1024)
#define NVM_CYCLIC_TIME (1U)

void NVM_WriteSuccess(void);
void NVM_ReadingDoneCallBack(void);
void Error_CallBack(uint8_t ID);

#endif