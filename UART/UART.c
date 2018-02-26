/**************************************************************************************
*        File name: UART.c
*        Author: Moamen Ali & Moamen Ramadan
*        Date: 18/2/2018
*        Description:
*                        This file contains:
*                        - macros to access the UART register
*                        - prototypes of functions used to access UART register
*        Microcontroller: STM32F407VG
***************************************************************************************/
#include <stdint.h>
#include "UART_cfg.h"
#include "UART.h"
#include "GPIO.h"
#include "STM32F4_MemMap.h"

typedef volatile uint32_t* const UART_RegAddType; // typedef for the uart register addreses which are pointer to 32 bits 

// UARTs Base adresses
#define APB1_BASE                          0x40000000U
#define UART22_BASE                        (APB1_BASE + 0x4400)
#define UART33_BASE                        (APB1_BASE + 0x4800)
#define UART44_BASE                        (APB1_BASE + 0x4C00)
#define UART55_BASE                        (APB1_BASE + 0x5000)

#define APB2_BASE                           0x40010000U
#define UART11_BASE                        (APB2_BASE + 0x1000)
#define UART66_BASE                        (APB2_BASE + 0x1400)

// STM32F407VG Support only 6 UARTs
#define UART_MAX_NUM 6U

static const uint32_t UARTsBaseAddressLut[UART_MAX_NUM] =
{
        UART22_BASE,
        UART33_BASE,
        UART44_BASE,
        UART55_BASE,
        UART11_BASE,
        UART66_BASE
};

#define USART_SR_OFFSET                0x00
#define USART_DR_OFFSET                0x04
#define USART_BRR_OFFSET               0x08
#define USART_CR1_OFFSET               0x0C
#define USART_CR2_OFFSET               0x10
#define USART_CR3_OFFSET               0x14

#define UART_ID 0



// check the number of used UARTs by user and display warning while compilation-
// if Number of used UARts greater than the available UARTs
#if UART_PERIF_NUM > UART_MAX_NUM
#warning "Invalid Channel Number"
#endif // UART_CHANNELS_NUM

// Dereference the UART registers
// calculate the address of each register and cast it to the Typedef data type (UART_RegAddType)
// then dereference that address to be ready to write in the required register 
#define USART_SR(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_SR_OFFSET))
#define USART_DR(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_DR_OFFSET))
#define USART_BRR(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_BRR_OFFSET))
#define USART_CR1(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_CR1_OFFSET))
#define USART_CR2(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_CR2_OFFSET))
#define USART_CR3(UART_ID)        *((UART_RegAddType)(UARTsBaseAddressLut[UART_ID] + USART_CR3_OFFSET))

// macros to define the status of the UARt
#define UART_STATE_UNINIT        0x00
#define UART_STATE_INIT          0x01
#define UART_STATE_TX_INPROGRESS 0x02
#define UART_STATE_RX_INPROGRESS 0x04

// structure to save the number of pins that used as Tx and Rx for all UARts
typedef struct
{
        uint8_t TxAltFunction;
        uint8_t RxAltFunction;
}GpioAlternFunc;
static const GpioAlternFunc UartChannelsAlternFun[UART_MAX_NUM]
= {
   {0x07,0x07},
   {0x07,0x07},
   {0x08,0x08},
   {0x08,0x08},
   {0x07,0x07},
   {0x08,0x08}};

// Array to save the status of all UARTs
uint8_t UartDriverState[UART_MAX_NUM] = {0};
// Array to save the length of transmitted data
uint32_t UartTxLength[UART_MAX_NUM];
// array to save the number of transmitted bytes
uint32_t UartTxCount[UART_MAX_NUM];
// Array of pointers to save the pointers of data to be transmitted
uint8_t* UartTxBuffPtr[UART_MAX_NUM];
// Array to save the length of received data
uint32_t UartRxLength[UART_MAX_NUM];
// array to save the number of received bytes
uint32_t UartRxCount[UART_MAX_NUM];
// Array of pointers to save the pointers in which the received data will be saved
uint8_t* UartRxBuffPtr[UART_MAX_NUM];
// Array of pointers to function to save the RxDone Callback function address
UART_CallBackPtrType RxCallback[UART_MAX_NUM];
// Array of pointers to function to save the TxDone Callback function address
UART_CallBackPtrType TxCallback[UART_MAX_NUM]; 

static void RxManage(uint8_t ChannelId);
static void TxManage(uint8_t ChannelId);

#if (UART_USE_INT_TO_HANDLE == 1)
static void IntManage(UART_PerifID UARTx);
#endif

/*A function to initialize all the UART channels with the configuration set in UART_Cfg*/
UART_ChkType UART_Init(void)
{
        UART_ChkType RetVar; // variable for the return value
        const UART_ConfigType* CfgPtr; 
        uint8_t LoopIndex;
        // loop over all defined UARts in UART_ConfigParam array
        for(LoopIndex =0; LoopIndex < UART_PERIF_NUM; LoopIndex ++)
        {
                CfgPtr = &UART_ConfigParam[LoopIndex];
                /*Checking for a valid configuration*/
                if((CfgPtr->DataSize <= UART_9_BITS) &&
                   (CfgPtr->Parity <= ODDPARITY) &&
                   (CfgPtr->StopBits <=STOP_TWO))
                {
                        //Enable the GPIO Group
                        GPIO_SetAlternFuntion(CfgPtr->TxPortID,UartChannelsAlternFun[CfgPtr->UartPerifID].TxAltFunction);
                        GPIO_SetAlternFuntion(CfgPtr->RxPortID,UartChannelsAlternFun[CfgPtr->UartPerifID]).RxAltFunction;

                        // enable clock gating
                        // check the used UART on APB1 Bus or APB2 Bus
                        if (CfgPtr->UartPerifID <4)
                        {
                                // enable clock gating on APB1 bus for UART2 & UART3 & UART4 & UART5
                                RCC_APB1ENR |= (0x00020000 << CfgPtr->UartPerifID);
                        }
                        else if ( CfgPtr->UartPerifID == 4 || CfgPtr->UartPerifID == 5)
                        {
                                // enable clock gating on bus APB2 for UART1 and UART6
                                RCC_APB2ENR |= 1 << CfgPtr->UartPerifID; 
                        }
                        else 
                        {;/*MISRA*/}
                        // set the baudrate 
                        USART_BRR(CfgPtr->UartPerifID) = CfgPtr->BaudRate;
                        // set the Number of bits
                        USART_CR1(CfgPtr->UartPerifID) &=~ (1 << M);
                        USART_CR1(CfgPtr->UartPerifID) |= (CfgPtr->DataSize << M);
                        // set the number of stop bits
                        USART_CR2(CfgPtr->UartPerifID) &=~ (3 << STOP);
                        USART_CR2(CfgPtr->UartPerifID) |= (CfgPtr->StopBits << STOP);
                        // set the parity bits
                        USART_CR1((CfgPtr->UartPerifID)) |= ((CfgPtr->Parity) << PS);
                        /*Enable TXE and RXNE and TC Interrupts*/
                        #if(UART_USE_INT_TO_HANDLE == 1)
                        USART_CR1(CfgPtr->UartPerifID) |= 0xE0;
                        #endif
                        // set the oversampling by 16
                        USART_CR1(CfgPtr->UartPerifID) &=~ (1 << OVER8);
                        // enable UART
                        USART_CR1(CfgPtr->UartPerifID) |= (1 << UE);
                        /*Change the UART State*/
                        UartDriverState[CfgPtr->UartPerifID] = UART_STATE_INIT;
                        // store the rx callback function address
                        RxCallback[CfgPtr->UartPerifID] = CfgPtr->RxDoneCallBackPtr;
                        // store the Tx callback function address
                        TxCallback[CfgPtr->UartPerifID] = CfgPtr->TxDoneCallBackPtr;
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        return RetVar;
}

/*A function to Request a silent transmission*/
/*
 Inputs:
         - TxBuffPtr: Pointer to the data to be transmitted
         - TxLength : length of the data to be transmittted
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StartSilentTransmission (uint8_t* TxBuffPtr,
                                           uint32_t TxLength,
                                          uint8_t ChannelId)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                /*Check if the channel Ready to start transmission*/
                if(UartDriverState[CfgPtr->UartPerifID] == UART_STATE_INIT)
                {
                        // enable transmitter 
                        USART_CR1(CfgPtr->UartPerifID) |= 1 << TE;
                        // UART state is Tx inprogress
                        UartDriverState[CfgPtr->UartPerifID] = UART_STATE_TX_INPROGRESS;
                        // save the pointer (TxBuffPtr) of the transmitted data 
                        UartTxBuffPtr[CfgPtr->UartPerifID] = TxBuffPtr;
                        // save the length of transmitted data in the global array
                        UartTxLength[CfgPtr->UartPerifID] = TxLength;
                        // initialize Tx counter to zero
                        UartTxCount[CfgPtr->UartPerifID] = 0;
                        // return UART Ok
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function to stop a current transmission*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StopCrntTransmission(uint8_t ChannelId)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                // check the state of the UART channel
                if ((UartDriverState[CfgPtr->UartPerifID]) == UART_STATE_TX_INPROGRESS)
                {
                        // disable transmitter 
                        USART_CR1(CfgPtr->UartPerifID) &=~ (1 << TE);
                        UartDriverState[CfgPtr->UartPerifID] = UART_STATE_INIT;
                        // return UART Ok
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function to get number of transmitted bytes in an ongoing transmission*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
         - NumPtr   : pointer to variable in which the function save the number of the transmitted bytes 
*/
UART_ChkType UART_GetNumberOfTxBytes(uint8_t ChannelId,uint32_t* NumPtr)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                // check the state of the UART channel
                if ((UartDriverState[CfgPtr->UartPerifID] ) == UART_STATE_TX_INPROGRESS)
                {
                        *NumPtr = UartTxCount[CfgPtr->UartPerifID];
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function to Request a silent Reception*/
/*
 Inputs:
         - RxBuffPtr: Pointer to the location in which the received data will be saved
         - RxLength : length of the data will be received
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StartSilentReception(uint8_t* RxBuffPtr,
                                       uint32_t RxLength,
                                       uint8_t ChannelId)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                // check the state of UART
                if (UartDriverState[CfgPtr->UartPerifID] == UART_STATE_INIT)
                {
                        // enable receiver
                        USART_CR1(CfgPtr->UartPerifID) |= 1 << RE;
                        // save the pointer RxBuffPtr where store the received data
                        UartRxBuffPtr[CfgPtr->UartPerifID] = RxBuffPtr;
                        // save the length of the received data
                        UartRxLength[CfgPtr->UartPerifID] = RxLength;
                        // initialize Rx counter by zero to start receiving
                        UartRxCount[CfgPtr->UartPerifID] = 0;
                        // change the state of UART to be Rx in progress
                        UartDriverState[CfgPtr->UartPerifID] = UART_STATE_RX_INPROGRESS;
                        // return UART Ok
                        RetVar = UART_OK;         
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else 
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function to stop a current Reception*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
*/
UART_ChkType UART_StopCrntReception(uint8_t ChannelId)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                // check the state of UART
                if ((UartDriverState[CfgPtr->UartPerifID]) == UART_STATE_RX_INPROGRESS)
                {
                        // disable the Rx 
                        USART_CR1(CfgPtr->UartPerifID) &=~ (1 << RE);
                        // change the state of UART to be UART_INIT
                        UartDriverState[CfgPtr->UartPerifID] = UART_STATE_INIT;
                        // return UART ok
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function to Get the number of received bytes in an ongoing reception*/
/*
 Inputs:
         - ChannelId: index of the used UART in UART_ConfigParam array
         - NumPtr   : pointer to variable in which the function save the number of the received bytes 
*/
UART_ChkType UART_GetNumberOfRxBytes(uint8_t ChannelId,uint32_t* NumPtr)
{
        UART_ChkType RetVar;
        const UART_ConfigType* CfgPtr;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                // address of the required structure corresponding to ChannelId
                CfgPtr = &UART_ConfigParam[ChannelId];
                // check the state of UART
                if ((UartDriverState[CfgPtr->UartPerifID] ) == UART_STATE_RX_INPROGRESS)
                {
                        *NumPtr = UartRxCount[CfgPtr->UartPerifID];
                        RetVar = UART_OK;
                }
                else
                {
                        RetVar = UART_NOK;
                }
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/*A function manage transmission and recpetion handling*/
#if (UART_USE_INT_TO_HANDLE == 0)
UART_ChkType UART_ManageOngoingOperation(uint8_t ChannelId)
{
        UART_ChkType RetVar;
        /*Check the validity of channel ID*/
        if(ChannelId < UART_PERIF_NUM)
        {
                TxManage(ChannelId);
                RxManage(ChannelId);
                RetVar = UART_OK;
        }
        else
        {
                RetVar = UART_NOK;
        }
        return RetVar;
}

/* 
 * This function used to complete the recpetion of the string 
 * increment counter to receive character by character till reach the length of string
 * Inputs:
         - ChannelId: the index of the used UART in the UART_ConfigParam array of structures
*/
static void RxManage(uint8_t ChannelId)
{
        const UART_ConfigType* CfgPtr;
        // address of the required structure corresponding to ChannelId
        CfgPtr = &UART_ConfigParam[ChannelId];
        // check the state of UART
        if ((UartDriverState[CfgPtr->UartPerifID]) == UART_STATE_RX_INPROGRESS)
        {
            // check Rx Done or not
            if (UartRxCount[CfgPtr->UartPerifID] < UartRxLength[CfgPtr->UartPerifID])
            {
               // check the RXNE flag which set when data ready to be read
                if (((USART_SR(CfgPtr->UartPerifID) >> RXNE) & ((uint32_t)1)) == 1)
                {
                    // receive data from data register in the pointer specified in UartRxBuffPtr
                    *(UartRxBuffPtr[CfgPtr->UartPerifID] + UartRxCount[CfgPtr->UartPerifID]) = (USART_DR(CfgPtr->UartPerifID)&(0x00FF));
                    // increment counter to receive the next character
                    UartRxCount[CfgPtr->UartPerifID]++;
                    // clear RXNE flag
                    USART_SR(CfgPtr->UartPerifID) &=~ (1 << RXNE);
                }
                else {;/*MISRA*/}
            }
            else 
            {
                // disable receiver
                USART_CR1(CfgPtr->UartPerifID) &=~ (1 << RE);
                UartDriverState[CfgPtr->UartPerifID] = UART_STATE_INIT;
            }
        }
        else {;/*MISRA*/}
}

/* 
 * This function used to complete the transmission of the string 
 * increment counter to send character by character till reach the length of string
 * Inputs:
         - ChannelId: the index of the used UART in the UART_ConfigParam array of structures
*/
static void TxManage(uint8_t ChannelId)
{
        const UART_ConfigType* CfgPtr;
        // address of the required structure corresponding to ChannelId
        CfgPtr = &UART_ConfigParam[ChannelId];
        // check the state of UART
        if ((UartDriverState[CfgPtr->UartPerifID]) == UART_STATE_TX_INPROGRESS)
        {
                // check the TXE flag which is set when the data register is empty and ready to receive new byte
                if ((USART_SR(CfgPtr->UartPerifID) >> TXE) & ((uint32_t)1))
                { 
                        // check Tx Done or not
                        if(UartTxCount[CfgPtr->UartPerifID] < UartTxLength[CfgPtr->UartPerifID])
                        {
                                // send the next character
                                USART_DR(CfgPtr->UartPerifID) = *(UartTxBuffPtr[CfgPtr->UartPerifID] + UartTxCount[CfgPtr->UartPerifID]);
                                // increment counter to send the next character
                                UartTxCount[CfgPtr->UartPerifID]++;
                        }
                        else
                        {
                                while (!(((USART_SR((CfgPtr->UartPerifID))) >> TC) & ((uint32_t)1)));
                                // disable transmitter 
                                USART_CR1(CfgPtr->UartPerifID) &=~ (1 << TE);
                                UartDriverState[CfgPtr->UartPerifID] = UART_STATE_INIT;
                        }         
                }
                else{;/*MISRA*/}
        }
        else{;/*MISRA*/}
}
#endif

/*
 * This function used to handle TXE and RXNE interrupt
 * Input:
         - UARTx : UART id which trigger the interrupt
*/ 
#if (UART_USE_INT_TO_HANDLE == 1)
static void IntManage(UART_PerifID UARTx)
{
        // check the TXE flag which is set when the data register is empty and ready to receive new byte
        if ((USART_SR(UARTx) >> TXE) & ((uint32_t)1))
        {
            // check Tx Done or not
            if(UartTxCount[UARTx] < UartTxLength[UARTx])
            {
                // send the next character
                USART_DR(UARTx) = *(UartTxBuffPtr[UARTx] + UartTxCount[UARTx]);
                // increment counter to send the next character
                UartTxCount[UARTx]++;
            }
            else {;/*MISRA*/}
        }

        if ((USART_SR(UARTx) >> TC) & ((uint32_t)1))
        {
            // disable transmitter 
            USART_CR1(UARTx) &=~ (1 << TE);
            UartDriverState[UARTx] = UART_STATE_INIT;
            // execute the call back function
            TxCallback[UARTx]();
        }

        if ((USART_SR(UARTx) >> RXNE) & ((uint32_t)1))
        {
            // clear RXNE flag
            USART_SR(UARTx) &=~ 1 << RXNE;
            // check Rx Done or not
            if (UartRxCount[UARTx] < UartRxLength[UARTx])
            {
                // receive data from data register in the pointer specified in UartRxBuffPtr
                *(UartRxBuffPtr[UARTx] + UartRxCount[UARTx]) = USART_DR(UARTx);
                // increment counter to receive the next character
                UartRxCount[UARTx]++;

                if (UartRxCount[UARTx] == UartRxLength[UARTx])
                {
                	// disable receiver
                	USART_CR1(UARTx) &=~ (1 << RE);
                	UartDriverState[UARTx] = UART_STATE_INIT;
                	// execute the call back function
                	RxCallback[UARTx]();
                }
            }        
        }
}



// interrupt service routine of UART1 
void USART1_IRQHandler (void)
{
        IntManage(UART11);
}

// interrupt service routine of UART2 
void USART2_IRQHandler (void)
{
        IntManage(UART22);
}

// interrupt service routine of UART3 
void USART3_IRQHandler (void)
{
        IntManage(UART33);
}

// interrupt service routine of UART4 
void USART4_IRQHandler (void)
{
        IntManage(UART44);
}

// interrupt service routine of UART5 
void USART5_IRQHandler (void)
{
        IntManage(UART55);
}

// interrupt service routine of UART4 
void USART6_IRQHandler (void)
{
        IntManage(UART66);
}
#endif