/*
 *
 *        Authors: Wessam Adel and Mohamed Mamdouh
 *        Date: 11/6/2018
 *        Microcontroller: STM32F407VG
 * 
 *  Description: This file contains the manager of the Bluetooth module, it manages the following states:
 * 
 *                  -Initilization
 *                  -Configuration
 *                  -Transmission
 *                  -Reception
 *                  -Idle
 * 
 *               This file contains three functions:
 *
 *                  -BT_Manager
 *                  -BT_StartSendingData
 *                  -BT_StartReceivingData
*/

#include "BT_Manager.h"

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *                          Globals Declaration                               *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
static char*    TransmittedDataPtr;
static char*    ReceivedDataPtr;
static uint8_t  TransmittedDataCount;
static uint8_t  ReceivedDataCount;
static uint8_t  StartTransmissionFlag;
static uint8_t  StartReceptionFlag;


/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *      This function is responsible for managing the different               *
 *      requests by the user for using the bluetooth module                   *
 *      it uses the driver's helper function                                  *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
void BT_Manager(void)
{
    static BT_ManagerState ManagerState = BT_INIT;
    BT_CheckType ConfigRetVal;
    BT_CheckType SendDataRetVal;
    BT_CheckType GetDataRetVal;

    switch(ManagerState)
    {
        case BT_INIT:
        {
            if(BT_InitFlag == 1)
            {
                ManagerState = BT_CONFIGURE;
				
            }
            else
            {
                ManagerState = BT_INIT;
            }

        }
        break;

        case BT_CONFIGURE:
        {
            ConfigRetVal = BT_Configure();
            
            if(ConfigRetVal == BT_OK)
            {
                ManagerState = BT_IDLE;
            }
            else
            {
                ManagerState = BT_CONFIGURE;
            }
        }
        break;

        case BT_IDLE:
        {
            if(StartTransmissionFlag == 1)
            {
                ManagerState = BT_TRANSMIT;
            }
            else if(StartReceptionFlag == 1)
            {
                ManagerState = BT_RECEIVE;
            }
            else
            {
                ManagerState = BT_IDLE;
            }
        }
        break;

        case BT_TRANSMIT:
        {
            SendDataRetVal = BT_SendData(TransmittedDataPtr, TransmittedDataCount);

            if(SendDataRetVal == BT_OK)
            {
                ManagerState = BT_IDLE;
                StartTransmissionFlag = 0;
            }
            else
            {
                ManagerState = BT_TRANSMIT;
            }
        }
        break;

        case BT_RECEIVE:
        {
            GetDataRetVal = BT_GetData(ReceivedDataPtr, ReceivedDataCount);

            if(GetDataRetVal == BT_OK)
            {
                ManagerState = BT_IDLE;
                StartReceptionFlag = 0;
            }
            else
            {
                ManagerState = BT_RECEIVE;
            }
        }
        break;
    }
}

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *             API to be used by the user to request sending data             *
 *                                                                            *
 *             -When the StartTransmissionFlag is set to ONE the manager      *
 *              is notified and so it starts calling the driver's function    *
 *              to start the operation                                        *
 *                                                                            *
 *             -When the transmission process ends successfully, the manager  *
 *              clears the StartTransmissionFlag, and the call back function  *
 *              choosen by the user in the BT_Config.c file is called,        *
 *              the implementaion of the call back function                   *
 *              is left for the user                                          *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

void BT_StartSendingData(char* DataToBeSent, uint8_t DataLength)
{
    StartTransmissionFlag = 1;
    TransmittedDataPtr    = DataToBeSent;
    TransmittedDataCount  = DataLength;
}

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *             API to be used by the user to request receiving data           *
 *                                                                            *
 *             -When the StartReceptionFlag is set to ONE the manager         *
 *              is notified and so it starts calling the driver's function    *
 *              to start the operation                                        *
 *                                                                            *
 *             -When the Reception process ends successfully, the manager     *
 *              clears the StartReceptionFlag, and the call back function     *
 *              choosen by the user in the BT_Config.c file is called,        *
 *              the implementaion of the call back function                   *
 *              is left for the user                                          *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

void BT_StartReceivingData(char* ReceivedData, uint8_t DataLength)
{
    StartReceptionFlag = 1;
    ReceivedDataPtr    = ReceivedData;
    ReceivedDataCount  = DataLength;
}