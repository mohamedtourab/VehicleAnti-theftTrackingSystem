#ifndef UART_CFG_H
#define UART_CFG_H

#define UART_PERIF_NUM 1U
#define UART_USE_INT_TO_HANDLE 1U

void UART_TransmissionDone(void);
void UART_ReceptionDone(void);

#endif
