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
 *************************************************************************************************************************
 *                                                                                                                       *        
 *                                                BYTE WRITE FSM                                                         *         
 *                                                                                                                       *         
 *************************************************************************************************************************   
 */
ReturnStruct I2C_Write(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t DataWrite, uint8_t Peripheral_ID)
{
      //  static States I2C_WriteStates = I2C_GENERATE_START;
      //  static States I2C_OldState;
      static ReturnStruct ReturnValue = {I2C_GENERATE_START,I2C_GENERATE_START};
      //ReturnValue.CurrentState = I2C_GENERATE_START;

        switch(ReturnValue.CurrentState)
        {
                case I2C_ERROR_CHECK:
                        if(I2C_ErrorCheck(Peripheral_ID)==I2C_OK)
                        {
                                
                                ReturnValue.CurrentState = ReturnValue.OldState;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_STOP;
                        }
                        ReturnValue.OldState = I2C_ERROR_CHECK;
                        break;
                        
                case I2C_GENERATE_START:
                        if(I2C_GenerateStart(Peripheral_ID)==I2C_OK)
                        {

                                ReturnValue.CurrentState = I2C_SEND_SLAVE_ADDRESS;
                        }        
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
															
                        }
                        ReturnValue.OldState = I2C_GENERATE_START;
                        break;

                case I2C_SEND_SLAVE_ADDRESS:
                        if(I2C_SetSlaveAddres(SlaveAddress,WRITE,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_SEND_LOCATION_ADDRESS;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
																								

                        }
                        ReturnValue.OldState = I2C_SEND_SLAVE_ADDRESS;
                        break;

                case I2C_SEND_LOCATION_ADDRESS:
                        if(I2C_SetLocationAddress(LocationAddress,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_SEND_DATA;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_SEND_LOCATION_ADDRESS;
                        break;

                case I2C_SEND_DATA:
                        if(I2C_PlaceData(DataWrite,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_STOP;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_SEND_DATA;
                        break;
                                                        
                case I2C_GENERATE_STOP:
                        I2C_GenerateStop(Peripheral_ID);
                        if(ReturnValue.OldState == I2C_ERROR_CHECK)
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_START;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_WRITE_DONE;
                        }
                        ReturnValue.OldState = I2C_GENERATE_STOP;
                        break;
                case I2C_WRITE_DONE:
                break;
               
        }
        return ReturnValue;
}

/* 
 *************************************************************************************************************************
 *                                                                                                                       *        
 *                                                  BYTE READ FSM                                                        *         
 *                                                                                                                       *         
 *************************************************************************************************************************   
 */

ReturnStruct I2C_Read(uint8_t SlaveAddress,uint8_t LocationAddress,uint8_t *DataRead, uint8_t Peripheral_ID)
{
        // static States I2C_ReadState = I2C_GENERATE_START;
        // static States I2C_OldState;
        //static ReturnStruct ReturnValue;
				static ReturnStruct ReturnValue = {I2C_GENERATE_START,I2C_GENERATE_START};
        switch(ReturnValue.CurrentState)
        {
                case I2C_ERROR_CHECK:
                        if(I2C_ErrorCheck(Peripheral_ID)==I2C_OK)
                        {
                                
                                ReturnValue.CurrentState = ReturnValue.OldState;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_STOP;
                        }
                        ReturnValue.OldState = I2C_ERROR_CHECK;
                        break;

                case I2C_GENERATE_START:
                        if(I2C_GenerateStart(Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_SEND_SLAVE_ADDRESS;
                        }        
                        else
                        {
                                
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_GENERATE_START;
                        break;

                case I2C_SEND_SLAVE_ADDRESS:
                        if(I2C_SetSlaveAddres(SlaveAddress,WRITE,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_SEND_LOCATION_ADDRESS;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_SEND_SLAVE_ADDRESS;
                        break;

                case I2C_SEND_LOCATION_ADDRESS:
                        if(I2C_SetLocationAddress(LocationAddress,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_REPEATED_START;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_SEND_LOCATION_ADDRESS;
                        break; 

                case I2C_REPEATED_START:
                        if(I2C_GenerateStart(Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_SET_SLAVE_ADDRESS_AND_GET_DATA;
                        }        
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_REPEATED_START;
                        break;

                case I2C_SET_SLAVE_ADDRESS_AND_GET_DATA:
                        if(I2C_SetSlaveAddressAndGetData(SlaveAddress,DataRead,Peripheral_ID)==I2C_OK)
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_STOP;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_ERROR_CHECK;
                        }
                        ReturnValue.OldState = I2C_SET_SLAVE_ADDRESS_AND_GET_DATA;
                        break;
                        
                case I2C_GENERATE_STOP:
                        I2C_GenerateStop(Peripheral_ID);
                        if(ReturnValue.OldState == I2C_ERROR_CHECK)
                        {
                                ReturnValue.CurrentState = I2C_GENERATE_START;
                        }
                        else
                        {
                                ReturnValue.CurrentState = I2C_READ_DONE;
                        }
                        ReturnValue.OldState = I2C_GENERATE_STOP;
                        break;
                case I2C_READ_DONE:break;


                        
                
        }
        return ReturnValue;
}