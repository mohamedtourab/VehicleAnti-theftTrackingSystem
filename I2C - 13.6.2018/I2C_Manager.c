
#include "I2C_Manager.h"

static uint8_t GlobalTransmissionFlag = 0;

static uint8_t GlobalPeripheral_ID = 0;
static uint8_t GlobalSlaveAddress = 0;
static uint8_t GlobalLocationAddress = 0;

static uint8_t GlobalWriteData = 0;
static uint8_t GlobalWriteByteCount = 0;
static uint8_t RepeatedStartFlag = 0;

static uint8_t GlobalWriteErrorFlag = 0;

static uint8_t GlobalReceptionFlag = 0;
static uint8_t* GlobalReadDataPtr;
static uint8_t GlobalReadByteCount = 0;


static uint8_t GlobalReadErrorFlag = 0;

void I2C_SendData(uint8_t Peripheral_ID, uint8_t DataToBeSent, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    GlobalTransmissionFlag = 1;
    GlobalPeripheral_ID = Peripheral_ID;
    GlobalWriteData = DataToBeSent;
    GlobalWriteByteCount = NumberOfBytes;
    GlobalLocationAddress = LocationAddress;
    GlobalSlaveAddress = SlaveAddress;
}

void I2C_Receive(uint8_t Peripheral_ID, uint8_t* DataToBeReceived, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    GlobalReceptionFlag = 1;
    GlobalPeripheral_ID = Peripheral_ID;
    GlobalReadDataPtr = DataToBeReceived;
    GlobalReadByteCount = NumberOfBytes;
    GlobalLocationAddress = LocationAddress;
    GlobalSlaveAddress = SlaveAddress;
}

I2C_CheckType I2C_Manager(void)
{
    static I2C_CheckType RetVal = I2C_BUSY;
    static I2C_States ManagerState = I2C_INIT;
    static I2C_States OldManagerState = I2C_INIT;
    static I2C_CheckType DriverReturnStatus = I2C_NOK;

    switch(ManagerState)
    {
        case I2C_INIT:
        {
            OldManagerState = I2C_INIT;

            if(I2C_InitFlag == 1)
            {
                ManagerState = I2C_IDLE;
            }
            else
            {
                ManagerState = I2C_INIT;
            }
        }
        break;

        case I2C_IDLE:
        {
            OldManagerState = I2C_IDLE;
						
            
            if( (GlobalTransmissionFlag == 1) || (GlobalReceptionFlag == 1) )
            {
                ManagerState = I2C_GENERATE_START;
								RetVal = I2C_BUSY;
            }
            else
            {
                ManagerState = I2C_IDLE;
            }
        }
        break;
        
        case I2C_GET_STATUS:
        {
            DriverReturnStatus = I2C_ErrorCheck(GlobalPeripheral_ID);
            
            if(DriverReturnStatus == I2C_OK)
            {
                switch(OldManagerState)
                {
                    case I2C_GENERATE_START:
                    {
                        DriverReturnStatus = I2C_StartStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK)
                        {
                            ManagerState = I2C_SEND_SLAVE_ADDRESS;
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    break;

                    case I2C_SEND_SLAVE_ADDRESS:
                    {
                        DriverReturnStatus = I2C_SendSlaveAddressStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK && RepeatedStartFlag == 0)
                        {
                            ManagerState = I2C_SEND_LOCATION_ADDRESS;
                        }
                        else if(DriverReturnStatus == I2C_OK && RepeatedStartFlag == 1)
                        {
                            ManagerState = I2C_GET_DATA;
                            RepeatedStartFlag = 0;
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    break;

                    case I2C_SEND_LOCATION_ADDRESS:
                    {
                        DriverReturnStatus = I2C_SendLocationAddressStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK)
                        {
                            if(GlobalTransmissionFlag == 1)
                            {
                                ManagerState = I2C_PLACE_DATA;
                            }
                            else if(GlobalReceptionFlag == 1)
                            {
                                ManagerState = I2C_REPEATED_START;
                            }
                            else{/*MISRA*/};
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
										break;
                    case I2C_PLACE_DATA:
                    {
                        DriverReturnStatus = I2C_PlaceDataStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK)
                        {
                            ManagerState = I2C_GENERATE_STOP;
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    break;

                    case I2C_REPEATED_START:
                    {
                        DriverReturnStatus = I2C_StartStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK)
                        {
                            ManagerState = I2C_SEND_SLAVE_ADDRESS;
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    break;

                    case I2C_GET_DATA:
                    {
                        DriverReturnStatus = I2C_GetDataStatus(GlobalPeripheral_ID);

                        if(DriverReturnStatus == I2C_OK)
                        {
                            ManagerState = I2C_GENERATE_STOP;
                        }
                        else
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    break;


                }
           }
           else
            {
               ManagerState = I2C_ERROR_CHECK;
            }
        }
        break;

        case I2C_ERROR_CHECK:
        {
            RetVal = I2C_NOK;
        }
        break;

        case I2C_GENERATE_START:
        {
            OldManagerState = I2C_GENERATE_START;
            I2C_GenerateStart(GlobalPeripheral_ID);
            ManagerState = I2C_GET_STATUS;
        }
        break;
        
        case I2C_SEND_SLAVE_ADDRESS:
        {
            OldManagerState = I2C_SEND_SLAVE_ADDRESS;
            if(RepeatedStartFlag == 1)
            {
                I2C_SendSlaveAddress(GlobalSlaveAddress, READ, GlobalPeripheral_ID);
            }
            else
            {
                I2C_SendSlaveAddress(GlobalSlaveAddress, WRITE, GlobalPeripheral_ID);
            } 
            ManagerState = I2C_GET_STATUS;
        }
        break;
        
        case I2C_PLACE_DATA:
        {
            OldManagerState = I2C_PLACE_DATA;
            I2C_PlaceData(GlobalWriteData, GlobalPeripheral_ID);
            ManagerState = I2C_GET_STATUS;
        }
        break;
        
        case I2C_REPEATED_START:
        {
            OldManagerState = I2C_REPEATED_START;
            I2C_GenerateStart(GlobalPeripheral_ID);
            ManagerState = I2C_GET_STATUS;
            RepeatedStartFlag = 1;
        }
        break;
        
        case I2C_SEND_LOCATION_ADDRESS:
        {
            OldManagerState = I2C_SEND_LOCATION_ADDRESS;
            I2C_SendLocationAddress(GlobalLocationAddress, GlobalPeripheral_ID);
            ManagerState = I2C_GET_STATUS;
        }
        break;
        
        case I2C_GET_DATA:
        {
            OldManagerState = I2C_GET_DATA;
            I2C_GetData(GlobalReadDataPtr, GlobalPeripheral_ID);
            ManagerState = I2C_GET_STATUS;
        }
        break;
        
        case I2C_GENERATE_STOP:
        {
            OldManagerState = I2C_GENERATE_STOP;
            I2C_GenerateStop(GlobalPeripheral_ID);
            ManagerState = I2C_IDLE;
            GlobalReceptionFlag = 0;
            GlobalTransmissionFlag = 0;
            RetVal = I2C_OK;
        }
        break;
        
    }
return RetVal;
}
/*
void I2C_Manager(void)
{
    static I2C_ManagerStates ManageState = I2C_MANAGER_IDLE;
    static I2C_States   WriteReturnState;
    static I2C_States   ReadReturnState;
    
    switch(ManageState)
    {
        case I2C_MANAGER_IDLE:
        {
            if(GlobalTransmissionFlag == 1)
            {
                ManageState = I2C_MANAGER_SEND;
            }
            else if(GlobalReceptionFlag == 1)
            {
                ManageState = I2C_MANAGER_RECEIVE;
            }
            else
            {
                ManageState = I2C_MANAGER_IDLE;
            }
        }
        break;

        case I2C_MANAGER_GET_STATUS:
        {
            
        }
        break;

        case I2C_MANAGER_SEND:
        {
            WriteReturnState = I2C_Write(GlobalWritePeripheral_ID, GlobalWriteDataPtr, GlobalWriteByteCount, GlobalWriteLocationAddress, GlobalWriteSlaveAddress);
            ManageState = I2C_MANAGER_GET_STATUS;
        }
        break;

        case I2C_MANAGER_RECEIVE:
        {
            ReadReturnState = I2C_Read(GlobalReadPeripheral_ID, GlobalReadDataPtr, GlobalReadByteCount, GlobalReadLocationAddress, GlobalReadSlaveAddress);
            ManageState = I2C_MANAGER_GET_STATUS;
        }
        break;

        case I2C_MANAGER_ERROR_CHECK:
        {

        }
        break;

    }
}

I2C_States I2C_Write(uint8_t Peripheral_ID, uint8_t* DataToBeSent, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    static I2C_States WriteState = I2C_GENERATE_START;
    
    if(GlobalWriteErrorFlag == 1)
    {
        WriteState = I2C_GENERATE_STOP;
    }
    else{};

    switch(WriteState)
    {
        case I2C_GENERATE_START:
        {
            I2C_GenerateStart(Peripheral_ID);
            WriteState = I2C_SEND_SLAVE_ADDRESS;
        }
        break;

        case I2C_SEND_SLAVE_ADDRESS:
        {
            I2C_SetSlaveAddress(SlaveAddress, WRITE, Peripheral_ID);
            WriteState = I2C_SEND_LOCATION_ADDRESS;
        }
        break;

        case I2C_SEND_LOCATION_ADDRESS:
        {
            I2C_SetLocationAddress(LocationAddress, Peripheral_ID);
            WriteState = I2C_PLACE_DATA;
        }
        break;

        case I2C_PLACE_DATA:
        {
            I2C_PlaceData(DataToBeSent, Peripheral_ID);
            WriteState = I2C_PLACE_DATA;
        }
        break;

        case I2C_GENERATE_STOP:
        {
            I2C_GenerateStop(Peripheral_ID);
            WriteState = I2C_GENERATE_START;
            GlobalWriteErrorFlag = 0;
        }
        break;

        default: break;

    }
    return WriteState;
}

I2C_States I2C_Read(uint8_t Peripheral_ID, uint8_t* DataToBeReceived, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    static I2C_States ReadState = I2C_GENERATE_START;
    static I2C_States OldReadState = I2C_GENERATE_START;

    if(GlobalReadErrorFlag == 1)
    {
        ReadState = I2C_GENERATE_STOP;
    }
    else{};

    switch(ReadState)
    {

        case I2C_GENERATE_START:
        {
            I2C_GenerateStart(Peripheral_ID);
            ReadState = I2C_SEND_SLAVE_ADDRESS;
            OldReadState = I2C_GENERATE_START;
        }
        break;

        case I2C_SEND_SLAVE_ADDRESS:
        {
            if(OldReadState == I2C_REPEATED_START)
            {
                I2C_SetSlaveAddress(SlaveAddress, READ, Peripheral_ID);
                ReadState = I2C_GET_DATA;
            }
            else
            {
                I2C_SetSlaveAddress(SlaveAddress, WRITE, Peripheral_ID);
                ReadState = I2C_SEND_LOCATION_ADDRESS;
            }
            
            OldReadState = I2C_SEND_SLAVE_ADDRESS;
        }
        break;

        case I2C_SEND_LOCATION_ADDRESS:
        {
            I2C_SetLocationAddress(LocationAddress, Peripheral_ID);
            ReadState = I2C_REPEATED_START;
            OldReadState = I2C_SEND_LOCATION_ADDRESS;
        }
        break;

        case I2C_REPEATED_START:
        {
            I2C_GenerateStart(Peripheral_ID);
            ReadState = I2C_SEND_SLAVE_ADDRESS;
            OldReadState = I2C_REPEATED_START;
        }
        break;

        case I2C_GET_DATA:
        {
            I2C_GetData(DataToBeReceived, Peripheral_ID);
            ReadState = I2C_GENERATE_STOP;
            OldReadState = I2C_GET_DATA;
        }
        break;

        case I2C_GENERATE_STOP:
        {
            I2C_GenerateStop(Peripheral_ID);
            ReadState = I2C_GENERATE_START;
            OldReadState = I2C_GENERATE_STOP;
            GlobalReadErrorFlag = 0;
        }
        break;

        default: break;
    }
    return ReadState;
}
*/
