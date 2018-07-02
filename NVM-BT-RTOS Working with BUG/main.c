#include "GPIO.h"
#include "UART.h"
#include "BT_Driver.h"
#include "BT_Manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "I2C_Manager.h"
#include "NVM_Manager.h"


#define STACK_SIZE 1000

uint8_t TxDone;
uint8_t NVM_Data[6] = {'1','1','1','1','1'};
uint8_t NVM_RecArr[4] = {0};
uint8_t r;
uint8_t BT_RecArr[4] = {0};
uint8_t RxDone;
uint8_t GetStatusDone;
uint8_t WriteDone= 0;
uint8_t ReadDone=0;
uint8_t index__ = 0;

void TestManager(void)
{
	static uint8_t x = 0;
	static uint8_t Check = 1;
	static uint32_t cnt = 0;

	switch(x)
	{
		case 0:
		{
			NVM_Write(0,NVM_Data);
			x = 1;
		}
		break;

		case 1:
		{
			if(WriteDone == 1)
			{
				x = 2;
				WriteDone = 0;
			}
		}
		break;
		
		case 99:
		{
			cnt++;
			
			if(cnt == 1000)
			{
				x = 2;
			}
		}
		break;
			
		case 2:
		{
			BT_ConnectionStatus();
			x = 3;
		}
		break;

		case 3:
		{
			if(GetStatusDone == 2)
			{
				GetStatusDone = 0;
				x = 4;
			}
			else
			{
				x = 2;
			}
		}
		break;

		case 4:
		{
			BT_StartReceivingData(BT_RecArr,4);
			x = 5;
		}
		break;

		case 5:
		{
			if(RxDone == 1)
			{
				x = 6;
				RxDone = 0;
			}
		}
		break;

		case 6:
		{
			NVM_Read(0,NVM_RecArr);
			x = 7;
		}
		break;

		case 7:
		{
			if( (NVM_RecArr[0] == BT_RecArr[0]) && (NVM_RecArr[1] == BT_RecArr[1]) && (NVM_RecArr[2] == BT_RecArr[2]) && (NVM_RecArr[3] == BT_RecArr[3]) )
			{
				x = 20;
				ReadDone = 0;
				GPIO_Write(1,PIN_MASK_10,HIGH);
			}
		}
		break;

		case 8:
		{
			for(index__ = 0; index__ < 4; index__++)
			{
				if(NVM_RecArr[index__] != BT_RecArr[index__])
				{
					Check = 0;
				}
			}
			x = 9;
		}
		break;

		case 9:
		{
			if(Check == 1)
			{
				GPIO_Write(1,PIN_MASK_15,HIGH);
			}
			else
			{
				GPIO_Write(1,PIN_MASK_12,HIGH);
			}
		}
		break;
	}
}

void NVM_WriteSuccess()
{
	WriteDone = 1;
	GPIO_Write(1,PIN_MASK_15,HIGH);
}

void NVM_ReadingDoneCallBack(void)
{
	ReadDone = 1;
	GPIO_Write(1,PIN_MASK_3,HIGH);
}

void BT_StatusCallBack(uint8_t Connected)
{
	if(Connected == 1)
	{
		GPIO_Write(1,PIN_MASK_12,HIGH);
		GPIO_Write(1,PIN_MASK_13,LOW);
		GetStatusDone = 2;
	}
	else if(Connected == 0)
	{
		GPIO_Write(1,PIN_MASK_13,HIGH);
		GPIO_Write(1,PIN_MASK_12,LOW);
		GetStatusDone = 3;
	}
}

void BT_ReceptionDoneCallBack(void)
{
	RxDone = 1;
}

void BT_Task(void* PvParameter)
{
    TickType_t PreviousWakeTime;
    const TickType_t BT_Cyclic = 100;

    PreviousWakeTime = xTaskGetTickCount();
	
    while(1)
    {
        BT_Manager();
        vTaskDelayUntil(&PreviousWakeTime, BT_Cyclic);
    }
}

void TestTask(void* PvParameter)
{
    TickType_t PreviousWakeTime;
    const TickType_t TestCyclic = 5000;
	
    PreviousWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		TestManager();
        vTaskDelayUntil(&PreviousWakeTime, TestCyclic);
    }
}

void I2C_Task(void* PvParameter)
{
    TickType_t PreviousWakeTime;
    const TickType_t I2C_Cyclic = 100;
	
    PreviousWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		I2C_Manager();
        vTaskDelayUntil(&PreviousWakeTime, I2C_Cyclic);
    }
}

void NVM_Task(void* PvParameter)
{
    TickType_t PreviousWakeTime;
    const TickType_t NVM_Cyclic = 100;
	
    PreviousWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		NVM_Manager();
        vTaskDelayUntil(&PreviousWakeTime, NVM_Cyclic);
    }
}



int main(void)
{
    volatile uint16_t LoopIndex;
    TaskHandle_t BT_Handler = NULL;
    TaskHandle_t TestHandler = NULL;
	TaskHandle_t I2C_Handler = NULL;
	TaskHandle_t NVM_Handler = NULL;
    /*Initialization*/

    GPIO_Init();//Zero to be written in the memory to make the PW manager use the default key value 
    for(LoopIndex = 0; LoopIndex < 5000; LoopIndex++);
    UART_Init();
	I2C_Init();
	for(LoopIndex = 0; LoopIndex < 5000; LoopIndex++);
	BT_Init();
	for(LoopIndex = 0; LoopIndex < 5000; LoopIndex++);

	NVM_Init();

    NVIC_SetPriorityGrouping(4);
	//GPIO_Write(1,PIN_MASK_10,HIGH);
	//GPIO_Write(1,PIN_MASK_11,HIGH);
    xTaskCreate(I2C_Task,"I2C",STACK_SIZE,NULL,1,&I2C_Handler);
	xTaskCreate(NVM_Task,"NVM",STACK_SIZE,NULL,1,&NVM_Handler);
	xTaskCreate(BT_Task,"BT",STACK_SIZE,NULL,1,&BT_Handler);
	xTaskCreate(TestTask,"TEST",STACK_SIZE,NULL,2,&TestHandler);
    
    vTaskStartScheduler();

    while(1)
	{}
}

/*
void BT_TransmissionnDoneCallBack(void)
{
	TxDone = 1;
}
void BT_ReceptionDoneCallBack(void)
{
	RxDone = 1;
}

void BT_DisconnectedCallBack(void)
{
	GPIO_Write(1,PIN_MASK_13,HIGH);
}

void BT_StatusCallBack(uint8_t Connected)
{
	if( Connected == 1)
	{
		GPIO_Write(1,PIN_MASK_15,HIGH);
		GPIO_Write(1,PIN_MASK_14,LOW);
		GetStatusDone = 2;
	}
	else
	{
		GPIO_Write(1,PIN_MASK_14,HIGH);
		GPIO_Write(1,PIN_MASK_15,LOW);
		GetStatusDone = 3;
	}
}
*/
void Error_CallBack(uint8_t ID)
{
	GPIO_Write(1,PIN_MASK_14,HIGH);
}

void BT_DisconnectedCallBack(void)
{}
