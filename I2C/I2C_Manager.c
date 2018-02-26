/*
 *
 *        Authors: Wessam Adel and Mohamed Mamdouh
 *        Date: 18/12/2017
 *        Microcontroller: STM32F407VG
 *        
 *        Description: This file contains the implementation of the finite state machine
 *                                                         that is used for single byte write or single byte read.
 *
*/


#include "I2C_Driver.h"
#include "I2C_Manager.h"


/*
                ******************************************************************************
                *                                                                                                                BYTE WRITE FSM                                                     *
                ******************************************************************************                        
*/
States I2C_Write(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t DataWrite)
{
        static States I2C_WriteStates = I2C_IDLE;
        
        switch(I2C_WriteStates)
        {
                case I2C_IDLE:
                        if(I2C_SR2_Read() & I2C_SR2_BUSY_POS)
                        {
                                I2C_WriteStates = I2C_IDLE;
                        }
                        else
                        {
                                I2C_WriteStates = I2C_GENERATE_START;
                        }
                        break;
                        
                case I2C_GENERATE_START:
                        if(I2C_GenerateStart()==I2C_OK)
                        {
                                I2C_WriteStates = I2C_SEND_SLAVE_ADDRESS;
                        }        
                        else
                        {
                                I2C_WriteStates = I2C_GENERATE_START;
                        }
                        break;

                case I2C_SEND_SLAVE_ADDRESS:
                        if(I2C_SetSlaveAddres(SlaveAddress,WRITE)==I2C_OK)
                        {
                                I2C_WriteStates = I2C_SEND_LOCATION_ADDRESS;
                        }
                        else
                        {
                                I2C_WriteStates = I2C_SEND_SLAVE_ADDRESS;
                        }
                        break;

                case I2C_SEND_LOCATION_ADDRESS:
                        if(I2C_SetLocationAddress(LocationAddress)==I2C_OK)
                        {
                                I2C_WriteStates = I2C_SEND_DATA;
                        }
                        else
                        {
                                I2C_WriteStates = I2C_SEND_LOCATION_ADDRESS;
                        }
                        break;

                case I2C_SEND_DATA:
                        if(I2C_PlaceData(DataWrite)==I2C_OK)
                        {
                                I2C_WriteStates = I2C_GENERATE_STOP;
                        }
                        else
                        {
                                I2C_WriteStates = I2C_SEND_DATA;
                        }
                        break;
                                                        
                case I2C_GENERATE_STOP:
                        I2C_GenerateStop();
                        I2C_WriteStates = I2C_IDLE;
                        break;
                default:
                        I2C_WriteStates = I2C_IDLE;
                        break;
        }
		I2C_Delay();
        return I2C_WriteStates;
}
/*
                ******************************************************************************
                *                                                                                                                BYTE READ FSM                                                             *
                ******************************************************************************                        
*/
States I2C_Read(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t *DataRead)
{
        static States I2C_ReadStates = I2C_IDLE;
        
        switch(I2C_ReadStates)
        {
                case I2C_IDLE:
                        if(I2C_SR2_Read() & I2C_SR2_BUSY_POS)
                        {
                                I2C_ReadStates = I2C_IDLE;
                        }
                        else
                        {
                                I2C_ReadStates = I2C_GENERATE_START;
                        }
                        break;

                case I2C_GENERATE_START:
                        if(I2C_GenerateStart()==I2C_OK)
                        {
                                I2C_ReadStates = I2C_SEND_SLAVE_ADDRESS;
                        }        
                        else
                        {
                                I2C_ReadStates = I2C_GENERATE_START;
                        }
                        break;

                case I2C_SEND_SLAVE_ADDRESS:
                        if(I2C_SetSlaveAddres(SlaveAddress,WRITE)==I2C_OK)
                        {
                                I2C_ReadStates = I2C_SEND_LOCATION_ADDRESS;
                        }
                        else
                        {
                                I2C_ReadStates = I2C_SEND_SLAVE_ADDRESS;
                        }
                        break;

                case I2C_SEND_LOCATION_ADDRESS:
                        if(I2C_SetLocationAddress(LocationAddress)==I2C_OK)
                        {
                                I2C_ReadStates = REPEATED_START;
                        }
                        else
                        {
                                I2C_ReadStates = I2C_SEND_LOCATION_ADDRESS;
                        }
                        break;                
                case REPEATED_START:
                        if(I2C_GenerateStart()==I2C_OK)
                        {
                                I2C_ReadStates = SET_SLAVE_ADDRESS_AND_GET_DATA;
                        }        
                        else
                        {
                                I2C_ReadStates = I2C_GENERATE_START;
                        }
                        break;
                case SET_SLAVE_ADDRESS_AND_GET_DATA:
                        if(I2C_SetSlaveAddressAndGetData(SlaveAddress,DataRead)==I2C_OK)
                        {
                                I2C_ReadStates = I2C_GENERATE_STOP;
                        }
                        else
                        {
                                I2C_ReadStates = SET_SLAVE_ADDRESS_AND_GET_DATA;
                        }
                        break;
                        
                case I2C_GENERATE_STOP:
                        I2C_GenerateStop();
                        I2C_ReadStates = I2C_IDLE;
                        break;        

                default:
                        I2C_ReadStates = I2C_IDLE;
                        break;
        }
        return I2C_ReadStates;
}