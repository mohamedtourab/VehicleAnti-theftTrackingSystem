/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/
//

#ifndef I2C_MANAGER_H

#define I2C_MANAGER_H

typedef enum
{
	I2C_ERROR_CHECK,
	I2C_GENERATE_START,
	I2C_SEND_SLAVE_ADDRESS,
	I2C_SEND_LOCATION_ADDRESS,
	I2C_SEND_DATA,
	I2C_REPEATED_START,
	I2C_SET_SLAVE_ADDRESS_AND_GET_DATA,
	I2C_GENERATE_STOP,
	I2C_WRITE_DONE,
	I2C_READ_DONE
}States;

typedef struct{
	States CurrentState;
	States OldState;
}ReturnStruct;



ReturnStruct I2C_Write(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t DataWrite, uint8_t Peripheral_ID);
ReturnStruct I2C_Read(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t *DataRead, uint8_t Peripheral_ID);

#endif /*I2C_MANAGER_H*/
