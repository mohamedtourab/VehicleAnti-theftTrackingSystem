
#include "I2C_Manager.h"

static uint8_t GlobalWriteErrorFlag = 0;
static uint8_t GlobalReadErrorFlag = 0;
static uint8_t GlobalTransmissionFlag = 0;
static uint8_t GlobalWritePeripheral_ID = 0;
static uint8_t GlobalWriteDataPtr = 0;
static uint8_t GlobalWriteByteCount = 0;
static uint8_t GlobalWriteLocationAddress = 0;
static uint8_t GlobalWriteSlaveAddress = 0;

static uint8_t GlobalReceptionFlag = 0;
static uint8_t GlobalReadPeripheral_ID = 0;
static uint8_t GlobalReadDataPtr = 0;
static uint8_t GlobalReadByteCount = 0;
static uint8_t GlobalReadLocationAddress = 0;
static uint8_t GlobalReadSlaveAddress = 0;

void I2C_Manager(void)
{
    static I2C_ManagerStates ManageState = I2C_MANAGER_IDLE;
    static I2C_States ReadReturnState;
    static I2C_States WriteReturnState;
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
            WriteReturnState = I2C_Write(GlobalWritePeripheral_ID,GlobalWriteDataPtr,GlobalWriteByteCount,GlobalWriteLocationAddress,GlobalWriteSlaveAddress);
            ManageState = I2C_MANAGER_GET_STATUS;
        }
        break;

        case I2C_MANAGER_RECEIVE:
        {
            ReadReturnState = I2C_Read(GlobalReadPeripheral_ID,GlobalReadDataPtr,GlobalReadByteCount,GlobalReadLocationAddress,GlobalReadSlaveAddress);
            ManageState = I2C_MANAGER_GET_STATUS;
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
    else
    {
        ;//MISRA
    }
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
    else
    {
        ;//MISRA
    }

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

void I2C_SendData(uint8_t Peripheral_ID, uint8_t* DataToBeSent, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    GlobalTransmissionFlag = 1;
    GlobalWritePeripheral_ID = Peripheral_ID;
    GlobalWriteDataPtr = DataToBeSent;
    GlobalWriteByteCount = NumberOfBytes;
    GlobalWriteLocationAddress = LocationAddress;
    GlobalWriteSlaveAddress = SlaveAddress;
}

void I2C_Receive(uint8_t Peripheral_ID, uint8_t* DataToBeReceived, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress)
{
    GlobalReceptionFlag = 1;
    GlobalReadPeripheral_ID = Peripheral_ID;
    GlobalReadDataPtr = DataToBeReceived;
    GlobalReadByteCount = NumberOfBytes;
    GlobalReadLocationAddress = LocationAddress;
    GlobalReadSlaveAddress = SlaveAddress;
}