/**************************************************************************************
*        File name: UART.h
*        Author: Moamen Ali & Moamen Ramadan
*        Date: 18/2/2018
*        Description:
*                        This file contains:
*                        - macros to access the UART register
*                        - prototypes of functions used to access UART register
*        Microcontroller: STM32F407VG
***************************************************************************************/
#ifndef UART_H
#define UART_H
#include <stdint.h>
#include "UART_cfg.h"

//enum for UartPerifID 
typedef enum {UART22 = 0, UART33 = 1, UART44 = 2, UART55 = 3, UART11 = 4, UART66 = 5}UART_PerifID;
// enum for the return value of functions either Ok or NOK
typedef enum {UART_OK =0, UART_NOK}UART_ChkType; 
// enum for Parity parameter in UART_ConfigType structure
typedef enum {NOPARITY =0, NU = 1, EVENPARITY = 2, ODDPARITY = 3} UART_ParityType;
// enum for StopBits parameter in UART_ConfigType structure  
typedef enum {STOP_ONE =0, STOP_TWO}UART_StopBitsType;
// enum for DataSize parameter in UART_ConfigType structure
typedef enum {UART_8_BITS=0,UART_9_BITS=1}UART_DataSizeType;
// call back function
typedef void (*UART_CallBackPtrType)(void);

//Macros for BAUD RATE values for frequency 16 MHz 
#define BR9600          0x683
#define BR19200         0x341
#define BR38400         0x1A1
#define BR115200        0x08B
#define BR460800        0x023


//Macros To access UART status register 
#define RXNE        5U
#define TC          6U
#define TXE         7U


//Macros To access Control register1                  
#define RE            2U
#define TE            3U
#define RXNEIE        5U
#define TCIE          6U
#define TXEIE         7U
#define PS            9U
#define PCE           10U
#define M             12U
#define UE            13U
#define OVER8         15U

//Macros To access Control register2 
#define STOP         12U


/*A structure type to contain all the required configuration*/
typedef struct
{
        /*Uart Peripheral number*/
        uint32_t UartPerifID;
        /*Tx Pin GPIO channel ID*/
        uint8_t TxPortID;
        /*Rx Pin GPIO ChannelId*/
        uint8_t RxPortID;
        /*Channel BaudRate*/
        uint32_t BaudRate;
        /*Channel ParityType*/
        UART_ParityType Parity;
        /*Channel Stop Bits*/
        UART_StopBitsType StopBits;
        /*Channel frame Data Size*/
        UART_DataSizeType DataSize;
        /*Channel TxDone Callback function*/
        UART_CallBackPtrType TxDoneCallBackPtr;
        /*Channel RxDone CallBack function*/
        UART_CallBackPtrType RxDoneCallBackPtr; 
}UART_ConfigType;

extern const UART_ConfigType UART_ConfigParam[UART_PERIF_NUM];


/*A function to initialize all the UART channels with the configuration set in UART_Cfg*/
UART_ChkType UART_Init(void);

/*A function to Request a silent transmission*/
/*
 Inputs:
         - TxBuffPtr: Pointer to the data to be transmitted
         - TxLength : length of the data to be transmittted
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StartSilentTransmission (uint8_t* TxBuffPtr,
                                           uint32_t TxLength,
                                           uint8_t ChannelId);

/*A function to stop a current transmission*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StopCrntTransmission(uint8_t ChannelId);

/*A function to get number of transmitted bytes in an ongoing transmission*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
         - NumPtr   : pointer to variable in which the function save the number of the transmitted bytes 
*/
UART_ChkType UART_GetNumberOfTxBytes(uint8_t ChannelId,uint32_t* NumPtr);

/*A function to Request a silent Reception*/
/*
 Inputs:
         - RxBuffPtr: Pointer to the location in which the received data will be saved
         - RxLength : length of the data will be received
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StartSilentReception(uint8_t* RxBuffPtr,
                                       uint32_t RxLength,
                                       uint8_t ChannelId);

/*A function to stop a current Reception*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StopCrntReception(uint8_t ChannelId);

/*A function to Get the number of received bytes in an ongoing reception*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
         - NumPtr   : pointer to variable in which the function save the number of the received bytes 
*/
UART_ChkType UART_GetNumberOfRxBytes(uint8_t ChannelId,uint32_t* NumPtr);

/*A function manage transmission and recpetion handling*/
#if (UART_USE_INT_TO_HANDLE == 0)
UART_ChkType UART_ManageOngoingOperation(uint8_t ChannelId);
#endif
#endif