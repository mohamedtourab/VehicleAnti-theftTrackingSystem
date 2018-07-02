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
 *					-BT_ConnectionStatus
 *                  -BT_KillConnection
 */

#include "BT_Manager.h"

#define BT_TIME_OUT (1000000U)
#define BT_ERROR_ID (0U)

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *                          Globals Declaration                               *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
static uint8_t*     TransmittedDataPtr;
static uint8_t*     ReceivedDataPtr;
static uint8_t      TransmittedDataCount;
static uint8_t      ReceivedDataCount;
static uint8_t      StartTransmissionFlag;
static uint8_t      StartReceptionFlag;
static uint8_t      KillConnectionStatusFlag;
static uint8_t	    GetConnectionStatusFlag;
static uint64_t     BT_TimeOut = (BT_TIME_OUT/BT_CYCLIC_TIME);


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
    BT_CheckType SendDataRetVal;
    BT_CheckType GetDataRetVal;
	BT_CheckType ConnectionStatusRetVal;
    BT_CheckType KillConnectionRetVal;
    static uint64_t ErrorCounter = 0;
	const BT_ConfigType* ConfigPtr = &BT_ConfigParam;
    switch(ManagerState)
    {
        case BT_INIT:
        {
            if(BT_InitFlag == 1)
            {
                ManagerState = BT_IDLE;
            }
            else
            {
                ManagerState = BT_INIT;
            }

        }
        break;
        
        case BT_IDLE:
        {
            if(GetConnectionStatusFlag == 1)
			{
				ManagerState = BT_GET_CONNECTION_STATUS;
			}
			else if(StartTransmissionFlag == 1)
            {
                ManagerState = BT_TRANSMIT;
            }
            else if(StartReceptionFlag == 1)
            {
                ManagerState = BT_RECEIVE;
            }
            else if(KillConnectionStatusFlag == 1)
            {
                ManagerState = BT_KILL_CONNECTION;
            }
            else
            {
                ManagerState = BT_IDLE;
            }
        }
        break;

        case BT_TRANSMIT:
        {
            if(ErrorCounter > BT_TimeOut)
            {
                ManagerState = BT_ERROR;
            }
            else
            {
                SendDataRetVal = BT_SendData(TransmittedDataPtr, TransmittedDataCount, 0);

                if(SendDataRetVal == BT_OK)
                {
                    ManagerState = BT_IDLE;
                    StartTransmissionFlag = 0;
                    (*(ConfigPtr->BT_TransmissionCallBackPtr))();
                }
                else
                {
                    ManagerState = BT_TRANSMIT;
                    ErrorCounter++;
                }
            }
        }
        break;

        case BT_RECEIVE:
        {
            if(ErrorCounter > BT_TimeOut)
            {
                ManagerState = BT_ERROR;
            }
            else
            {
                GetDataRetVal = BT_GetData(ReceivedDataPtr, ReceivedDataCount, 0);

                if(GetDataRetVal == BT_OK)
                {
                    ManagerState = BT_IDLE;
                    StartReceptionFlag = 0;
                    (*(ConfigPtr->BT_ReceptionCallBackPtr))();
                }
                else
                {
                    ManagerState = BT_RECEIVE;
                    ErrorCounter++;
                }
            }
        }
        break;
		
		case BT_GET_CONNECTION_STATUS:
		{
            if(ErrorCounter > BT_TimeOut)
            {
                ManagerState = BT_ERROR;
            }
            else
            {            
                ConnectionStatusRetVal = BT_GetConnectionStatus(0);
                
                if(ConnectionStatusRetVal == BT_OK)
                {
                    ManagerState = BT_IDLE;
                    GetConnectionStatusFlag = 0;
                    /*This indicates that the Bluetooth module is connected*/
                    (*(ConfigPtr->BT_ConnectionStatusCallBackPtr))((uint8_t)1);
                }
                else if(ConnectionStatusRetVal == BT_NOK)
                {
                    ManagerState = BT_IDLE;
                    GetConnectionStatusFlag = 0;
                    /*This indicates that the Bluetooth module is not connected*/
                    (*(ConfigPtr->BT_ConnectionStatusCallBackPtr))((uint8_t)0);
                }
                else
                {
                    ManagerState = BT_GET_CONNECTION_STATUS;
                    ErrorCounter++;
                }
            }
		}
		break;

        case BT_KILL_CONNECTION:
        {
            if(ErrorCounter > BT_TimeOut)
            {
                ManagerState = BT_ERROR;
            }
            else
            {
                KillConnectionRetVal = BT_StopCommunication(0);

                if(KillConnectionRetVal == BT_OK)
                {
                    ManagerState = BT_IDLE;
                    KillConnectionStatusFlag = 0;
                    /*This indicates that the Bluetooth module is not connected*/
                    (*(ConfigPtr->BT_KillConnectionStatusCallBackPtr))();
                }
                else
                {
                    ManagerState = BT_KILL_CONNECTION;
                    ErrorCounter++;
                }
            }
        }
        break;

        case BT_ERROR:
        {
            ManagerState = BT_IDLE;
            ErrorCounter = 0;
            StartTransmissionFlag = 0;
            StartReceptionFlag = 0;
            GetConnectionStatusFlag = 0;
            KillConnectionStatusFlag = 0;
			BT_GetData(0,0,1);
			BT_SendData(0,0,1);
			BT_GetConnectionStatus(1);
			BT_StopCommunication(1);
            (*(ConfigPtr->BT_ErrorCallBackPtr))((uint8_t)BT_ERROR_ID);
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

void BT_StartSendingData(uint8_t* DataToBeSent, uint8_t DataLength)
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

void BT_StartReceivingData(uint8_t* ReceivedData, uint8_t DataLength)
{
    StartReceptionFlag = 1;
    ReceivedDataPtr    = ReceivedData;
    ReceivedDataCount  = DataLength;
}

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *    API to be used by the user to request getting module connection status  *
 *                                                                            *
 *         -When the GetConnectionStatusFlag is set to ONE the manager        *
 *          is notified and so it starts calling the driver's function        *
 *          to start the operation                                            *
 *                                                                            *
 *         -When the Get Connection Status process ends successfully,	      *
 *			the manager clears the GetConnectionStatusFlag, and the           *
 *          call back function choosen by the user in the BT_Config.c         *
 *          file is called with an argument of "0" indicating module          *
 *          not connected and argument of "1" indicating module is            *
 *			connected, the implementaion of the call back function is         *
 *			left for the user                                         	      *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

void BT_ConnectionStatus(void)
{
    GetConnectionStatusFlag = 1;
}

void BT_KillConnection(void)
{
    KillConnectionStatusFlag = 1;
}

