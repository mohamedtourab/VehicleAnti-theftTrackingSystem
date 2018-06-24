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

/*---------------------------------------------------------------------------------------------------
**********					Enums Used by user to Configure the UART periferal			   **********
---------------------------------------------------------------------------------------------------*/
// enum for the return value of functions either Ok or NOK
typedef enum {UART_OK =0, UART_NOK}UART_ChkType;

//enum for UartPerifID 
typedef enum {UART_2 = 0, UART_3 = 1, UART_4 = 2, UART_5 = 3, UART_1 = 4, UART_6 = 5}UART_PerifID; 
// enum for Parity parameter in UART_ConfigType structure
typedef enum {NOPARITY =0, EVENPARITY = 2, ODDPARITY = 3} UART_ParityType;
// enum for StopBits parameter in UART_ConfigType structure  
typedef enum {STOP_ONE =0, STOP_TWO}UART_StopBitsType;
// enum for DataSize parameter in UART_ConfigType structure
typedef enum {UART_8_BITS=0,UART_9_BITS=1}UART_DataSizeType;
// call back function
typedef void (*UART_CallBackPtrType)(void);
/*-----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
**********		A structure type to contain all the required configuration		**********
----------------------------------------------------------------------------------------*/
typedef struct
{
        /*Uart Peripheral number*/
		/* Values of UartPerifID can be:
			- UART_1
			- UART_2
			- UART_3
			- UART_4
			- UART_5
			- UART_6*/
        uint32_t UartPerifID;

        /*Tx and Rx Pins GPIO channel ID*/
        /* the index of Tx and Rx pins GPIO configuration structure in GPIO_Cfg.c*/
        uint8_t GPIO_PortID;

        /*Channel BaudRate*/
        // BR9600 	==> 9600 	bps
        // BR19200 	==> 19200 	bps
        // BR38400 	==> 38400 	bps
        // BR115200 ==> 115200 	bps
        // BR460800 ==> 460800 	bps
        uint32_t BaudRate;

        /*Channel ParityType*/
        /* Values Can be:
        	- NOPARITY
        	- EVENPARITY
        	- ODDPARITY*/
        UART_ParityType Parity;

        /*Channel Stop Bits*/
        /* Values Can be:
        	- STOP_ONE
        	- STOP_TWO*/
        UART_StopBitsType StopBits;

        /*Channel frame Data Size*/
        /* Values Can be:
        	- UART_8_BITS	==> 8 bits word length
        	- UART_9_BITS	==> 9 bits word length*/
        UART_DataSizeType DataSize;
        
        /*Channel TxDone Callback function*/
        UART_CallBackPtrType TxDoneCallBackPtr;
        /*Channel RxDone CallBack function*/
        UART_CallBackPtrType RxDoneCallBackPtr; 
}UART_ConfigType;
/*--------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------
**********					UART Registers Bit definetions					**********
------------------------------------------------------------------------------------*/

/*************************************************************************************
**********		Macros for BAUD RATE values for frequency 16 MHz            **********
*************************************************************************************/ 
#define BR9600          0x683	// Hexadecimal value which is saved in BRR register 
#define BR19200         0x341
#define BR38400         0x1A1
#define BR115200        0x08B
#define BR460800        0x023


/*************************************************************************************
**********				Macros To access UART status register               **********
*************************************************************************************/
//		Flag		bit Number in SR register 
#define RXNE        		5U	
#define TC          		6U
#define TXE         		7U


/*************************************************************************************
**********				Macros To access Control register1                  **********
*************************************************************************************/
//		Bit name	Bit Number in CR1 Register                  
#define RE            		2U	// 1-> Enable receiver , 0-> Disable receiver
#define TE            		3U	// 1-> Enable transmitter, 0-> Disable transmitter
#define RXNEIE        		5U	// 1-> Enable RXNE interrupt, 0-> Disable RXNE interrupt
#define TCIE          		6U	// 1-> Enable TC interrupt, 0-> Disable TC interrupt
#define TXEIE         		7U	// 1-> Enable TXE interrupt, 0-> Disable TXE interrupt
#define PS            		9U	// 1-> ODD parity, 0-> Even Parity
#define PCE           		10U	// 1-> Enable Parity, 0-> Disable Parity
#define M             		12U	// 1-> Word length = 9 bits, 0-> Word Length = 8 bits
#define UE            		13U	// 1-> Enable UART periferal, 0-> Disable UART periferal
#define OVER8         		15U	// 1-> Enable oversample by 8, 0-> Enable oversample by 16

/*************************************************************************************
**********				Macros To access Control register2                  **********
*************************************************************************************/ 
//		Bit Name 	Bit Number in CR2 Register
#define STOP         			12U // 0-> One stop bit, 2-> Two stop bits
/*---------------------------------------------------------------------------------------*/


/* The Array of structure in which the user configure the required UART periferal in UART_Cfg.c*/
extern const UART_ConfigType UART_ConfigParam[UART_PERIF_NUM];


/*-------------------------------------------------------------------------------------------
**********								Driver APIs								   ********** 
--------------------------------------------------------------------------------------------/
// Return type of all functions is UART_ChkType which can be either UART_OK or UART_NOK

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