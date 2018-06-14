#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "I2C_Driver.h"

typedef enum
{
    I2C_MANAGER_IDLE,
    I2C_MANAGER_GET_STATUS,
    I2C_MANAGER_SEND,
    I2C_MANAGER_RECEIVE,
}I2C_ManagerStates;

typedef enum
{
    I2C_GENERATE_START,
    I2C_SEND_SLAVE_ADDRESS,
    I2C_PLACE_DATA,
    I2C_REPEATED_START,
    I2C_SEND_LOCATION_ADDRESS,
    I2C_GET_DATA,
    I2C_GENERATE_STOP
}I2C_States;



I2C_States I2C_Write(uint8_t Peripheral_ID, uint8_t* DataToBeSent, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress);
I2C_States I2C_Read(uint8_t Peripheral_ID, uint8_t* DataToBeReceived, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress);

#endif