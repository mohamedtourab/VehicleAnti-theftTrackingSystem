
#include "BT_Manager.h"

uint8_t TxDone = 0;
uint8_t RxDone = 0;

int main (void)
{
    GPIO_Init();
	UART_Init();
	BT_Init();
	
	
	while(1)
	{
		BT_Manager();
	}
	return 0;
}

void BT_TxDone(void)
{
	TxDone = 1;
}
void BT_RxDone(void)
{
	RxDone = 1;
}