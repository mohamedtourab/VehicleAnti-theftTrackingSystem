#include "UART.h"
#include "UART_cfg.h"
#include "KeyManager.h"
const UART_ConfigType UART_ConfigParam[UART_PERIF_NUM] =
{
                {
                                UART_3,0,
                                BR9600,NOPARITY,STOP_ONE,UART_8_BITS,
                                UART_ReceptionDone,UART_ReceptionDone
                }
                };