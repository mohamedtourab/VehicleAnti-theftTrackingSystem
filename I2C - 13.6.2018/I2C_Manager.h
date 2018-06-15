#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "I2C_Driver.h"

/*
typedef enum
{
    I2C_MANAGER_IDLE,
    I2C_MANAGER_GET_STATUS,
    I2C_MANAGER_SEND,
    I2C_MANAGER_RECEIVE,
    I2C_MANAGER_ERROR_CHECK
}I2C_ManagerStates;
*/
typedef enum
{
    I2C_INIT,
    I2C_IDLE,
    I2C_GET_STATUS,
    I2C_GENERATE_START,
    I2C_SEND_SLAVE_ADDRESS,
    I2C_PLACE_DATA,
    I2C_REPEATED_START,
    I2C_SEND_LOCATION_ADDRESS,
    I2C_GET_DATA,
    I2C_GENERATE_STOP,
    I2C_ERROR_CHECK
}I2C_States;

I2C_CheckType I2C_Manager(void);
void I2C_SendData(uint8_t Peripheral_ID, uint8_t DataToBeSent, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress);
void I2C_Receive(uint8_t Peripheral_ID, uint8_t* DataToBeReceived, uint8_t NumberOfBytes, uint8_t LocationAddress, uint8_t SlaveAddress);

#endif