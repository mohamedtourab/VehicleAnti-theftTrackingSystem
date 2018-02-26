/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/


#ifndef I2C_MANAGER_H

#define I2C_MANAGER_H

typedef enum
{
	I2C_IDLE,
	I2C_GENERATE_START,
	I2C_SEND_SLAVE_ADDRESS,
	I2C_SEND_LOCATION_ADDRESS,
	I2C_SEND_DATA,
	I2C_REPEATED_START,
	I2C_SET_SLAVE_ADDRESS_AND_GET_DATA,
	I2C_GENERATE_STOP
}States;


States I2C_Write(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t DataWrite);
States I2C_Read(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t *DataRead);

#endif /*I2C_MANAGER_H*/
