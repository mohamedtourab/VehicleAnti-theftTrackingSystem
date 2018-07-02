#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "I2C_Driver.h"

typedef enum
{
  I2C_WRITE = 0,
	I2C_READ  = 1
}I2C_Direction;



typedef enum
{
    I2C_UN_INIT                     = 0 ,
    I2C_IDLE                        = 1 ,
    I2C_GENERATE_START              = 2 ,
    I2C_START_STATUS                = 3 ,
    I2C_SEND_SLAVE_ADDRESS          = 4 ,
    I2C_CLEAR_ADDR_BIT              = 5 ,
    I2C_SEND_SLAVE_ADDRESS_STATUS   = 6 ,
    I2C_PLACE_DATA                  = 7 ,
    I2C_PLACE_DATA_STATUS           = 8 ,
    I2C_PLACE_DATA_MANAGER          = 9 ,
    I2C_GET_DATA_MANAGER            = 10,
    I2C_GET_SINGLE_BYTE_STATUS      = 11,
    I2C_GET_SINGLE_BYTE             = 12,
    I2C_GET_MULTI_BYTE_STATUS       = 13,
    I2C_GET_MULTI_BYTES             = 14
}I2C_States;


void I2C_Manager(void);
void I2C_RequestWrite(uint8_t SlaveAddress, uint8_t* DataPointer, uint8_t NumOfBytes);
void I2C_RequestRead(uint8_t SlaveAddress, uint8_t* DataPointer, uint8_t NumOfBytes);

#endif