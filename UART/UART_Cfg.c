#include "UART.h"
#include "UART_cfg.h"
const UART_ConfigType UART_ConfigParam[UART_PERIF_NUM] =
{
                {
                                UART_2,0,1,
                                BR9600,NOPARITY,STOP_ONE,UART_8_BITS,
                                0,0
                }
                };