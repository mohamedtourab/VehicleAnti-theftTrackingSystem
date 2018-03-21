/**************************************************************************************
*        File name: GSM.h
*        Auther : 
                        -Abdelrhman Hosny
                        -Amr Mohamed
*        Date: 6/3/2018
*        Description:
*                        this file contains:
                                -macros to control the M95 GSM module
                                -prototype of the APIs of the M95 GSM module 
*        Microcontroller: STM32F407VG
***************************************************************************************/ 
#ifndef GSM_H
#define GSM_H

#include "GPIO.h"
#include "UART.h"
#include "GSM_Config.h"
#include "delay.h"

/*************************************************************************************
**********                     Macros to control GSM                            ******
*************************************************************************************/

/*************************************************************************************
**********              Macros to control GSM SMS format                        ******
*************************************************************************************/

#define PDU         0
#define TEXT        1

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//--------------------------------------------------------------------------------

//enumerate data type for the functions' return values
typedef enum {GSM_OK = 0, GSM_NOK} GSM_CheckType;

//--------------------------------------------------------------------------------

//a pointer to a call back function data type
typedef void (*GSM_CallBackFn)(GSM_CheckType, uint8_t*, uint16_t);

//--------------------------------------------------------------------------------

//structure to be filled by the user to configure the GSM
typedef struct
{
        /*Uart Channel number*/
        uint8_t UartChannelId;

        /*GSM power key Group ID*/
        uint8_t PWRKeyGroupId;
        /*pin mask of the power key in the selected GPIO group*/
        uint16_t PWRKeyPinMask;

        /*GSM ready to send Group ID*/
        uint8_t RTSGroupId;
        /*pin mask of the RTS in the selected GPIO group*/
        uint16_t RTSPinMask;

        /*GSM State Group ID*/
        uint8_t DTRGroupId;
        /*pin mask of the DTR in the selected GPIO group*/
        uint16_t DTRPinMask;

        /*GSM Ring Group ID*/
        uint8_t RingGroupId;
        /*pin mask of the Ring in the selected GPIO group*/
        uint16_t RingPinMask;

        /*a pointer to a call back function*/
        GSM_CallBackFn GSM_CallBackFnPtr;

}GSM_ConfigType;

//------------------------------------------------------------------------------

//extern constant array of GSM_ConfigType structure of the GSM groups
extern const GSM_ConfigType GSM_ConfigParam;

/***********************************************************************************
**********                      Functions prototypes                        ********
***********************************************************************************/

//----------------------------- UART call back functions --------------------------

/*
 * This function callback function for the GSM UART TX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Tx_CallBackFn (void);

/*
 * This function callback function for the GSM UART RX
 * Inputs:NONE
 * Output:NONE
*/

void GSM_Rx_CallBackFn (void);

//------------------------------------- Hardware -----------------------------------

/*
 * This function used to power on the GSM module
 Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_PowerOn (void);

/*
 * This function used to WakeUp the GSM module from sleep mode
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_WakeUp (void);

/*
 * This function used to put the GSM module in sleep mode
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_Sleep (void);

//------------------------------------- software ------------------------------------

/*
 * This function used to establish the communication with the GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_AT (void);

/*
 * This function used to stop echoing the commands from GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_StopEcho (void);

/*
 * This function used to reset all parameters to default
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/


GSM_CheckType GSM_ATCommand_RstDefault (void);

/*
 * This function used to set the baud rate of GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_BRFix (void);

/*
 * This function used to delete all sms messages on the GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_DeleteSMS (void);

/*
 * This function used to set the sms format of GSM module
 *Inputs:
         - Mode		: the sms format (PDU, TEXT)
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SMSFormat (uint8_t Mode);

/*
 * This function used to set character set of GSM module
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_CharSet (void);

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - PhoneNum		: a pointer to the phone number to send the SMS
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SetSMSMobNum (uint8_t* PhoneNum);

/*
 * This function used to send an SMS from GSM module
 *Inputs:
         - Msg 			: a pointer the SMS message + (Ctrl+Z) or (ascii: 26)
         - MsgLengrh	: the length of the SMS message + 1 (Ctrl+Z)
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_SetSMSWriteMsg (uint8_t* Msg, uint8_t MsgLength);

/*
 * This function used to check if there is SMS message in index 1 
 *Inputs:NONE
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_CheckRecievedSMS (void);

/*
 * This function used to read the SMS message in index 1  
 *Inputs:
 		- MsgLengrh	: the length of the expected SMS message 
 * Output:
         - an indication of the success of the function
*/

GSM_CheckType GSM_ATCommand_ReadSMS (uint8_t MsgLength);



/**************************************************************************************/

#endif