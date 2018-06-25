#include "I2C_Manager.h"

static uint8_t I2C_Write;
static uint8_t I2C_Read;
static uint8_t I2C_SlaveAddress;
static uint8_t I2C_ByteCount;
static uint8_t* I2C_DataPointer;

void I2C_RequestWrite(uint8_t SlaveAddress, uint8_t* DataPointer, uint8_t NumOfBytes)
{
    I2C_Write = 1;
    I2C_SlaveAddress = SlaveAddress;
    I2C_DataPointer = DataPointer;
    I2C_ByteCount = NumOfBytes;
}
void I2C_RequestRead(uint8_t SlaveAddress, uint8_t* DataPointer, uint8_t NumOfBytes)
{
    I2C_Read = 1;
    I2C_SlaveAddress = SlaveAddress;
    I2C_DataPointer = DataPointer;
    I2C_ByteCount = NumOfBytes;
}

void I2C_Manager(void)
{
    I2C_CheckType DriverReturnStatus = I2C_NOK;
    static I2C_States ManagerState = I2C_UN_INIT;
    static I2C_States OldState = I2C_UN_INIT;
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[0];
    
    switch(ManagerState)
    {
        case I2C_UN_INIT:
        {
            if(I2C_InitFlag == 1)
            {
                ManagerState = I2C_IDLE;
            }
            else
            {
                ManagerState = I2C_UN_INIT;
            }
        }
        break;

        case I2C_IDLE:
        {
            if(I2C_Write == 1)
            {
                ManagerState = I2C_GENERATE_START;
            }
            else if(I2C_Read == 1)
            {
                ManagerState = I2C_GENERATE_START;
            }
            else
            {
                ManagerState = I2C_IDLE;
            }
        }
        break;

        case I2C_GENERATE_START:
        {
            I2C_GenerateStart(0);
            ManagerState = I2C_START_STATUS;
        }
        break;

        case I2C_START_STATUS:
        {
            DriverReturnStatus = I2C_StartStatus(0);

            if(DriverReturnStatus == I2C_OK)
            {
                ManagerState = I2C_SEND_SLAVE_ADDRESS;
            }
            else
            {
                ManagerState = I2C_START_STATUS;
            }
        }
        break;

        case I2C_SEND_SLAVE_ADDRESS:
        {
            if(I2C_Write == 1)
            {
                I2C_SendSlaveAddress(I2C_SlaveAddress,I2C_WRITE,0);
            }
            else if(I2C_Read == 1)
            {
                I2C_SendSlaveAddress(I2C_SlaveAddress,I2C_READ,0);
            }
            else{/*MISRA*/};
            
            ManagerState = I2C_SEND_SLAVE_ADDRESS_STATUS;
        }
        break;

        case I2C_SEND_SLAVE_ADDRESS_STATUS:
        {
            DriverReturnStatus = I2C_SendSlaveAddressStatus(0);

            if(DriverReturnStatus == I2C_OK)
            {
                if(I2C_Write == 1)
                {
                    ManagerState = I2C_CLEAR_ADDR_BIT;
                }
                else if(I2C_Read == 1)
                {
                    OldState = I2C_SEND_SLAVE_ADDRESS_STATUS;
                    ManagerState = I2C_GET_DATA_MANAGER;
                }
                else{/*MISRA*/};
            }
            else
            {
                ManagerState = I2C_SEND_SLAVE_ADDRESS_STATUS;
            }
        }
        break;

        case I2C_CLEAR_ADDR_BIT:
        {
            I2C_Clear_ADDR(0);

            if(I2C_Write == 1)
            {
                ManagerState = I2C_PLACE_DATA;
            }
            else if(I2C_Read == 1)
            {
                OldState = I2C_CLEAR_ADDR_BIT;
                ManagerState = I2C_GET_DATA_MANAGER;
            }
            else{/*MISRA*/};

        }
		break;

        case I2C_PLACE_DATA:
        {
            I2C_PlaceData(I2C_DataPointer, 0);
            ManagerState = I2C_PLACE_DATA_STATUS;
        }
        break;

        case I2C_PLACE_DATA_STATUS:
        {
            DriverReturnStatus = I2C_PlaceDataStatus(0);

            if(DriverReturnStatus == I2C_OK)
            {
                ManagerState = I2C_PLACE_DATA_MANAGER;
            }
            else
            {
                ManagerState = I2C_PLACE_DATA_STATUS;
            }
        }
        break;

        case I2C_PLACE_DATA_MANAGER:
        {
            if(I2C_ByteCount == 1)
            {
                ManagerState = I2C_IDLE;
                I2C_Write = 0;
                I2C_DataPointer = 0;
                I2C_ByteCount = 0;
                (*(ConfigPtr->I2C_TransmissionDoneCallBack))();
            }
            else if(I2C_ByteCount > 1)
            {
                I2C_ByteCount--;
                I2C_DataPointer++;
                ManagerState = I2C_PLACE_DATA;
            }
            else{/*MISRA*/};
        }
        break;

        case I2C_GET_DATA_MANAGER:
        {
            switch(OldState)
            {
                case I2C_SEND_SLAVE_ADDRESS_STATUS:
                {
					if(I2C_ByteCount == 1)
                    {
                        I2C_TurnOffAcknowledge(0);
                        ManagerState = I2C_CLEAR_ADDR_BIT;
                    }
                    else if(I2C_ByteCount > 1)
                    {
                       ManagerState = I2C_CLEAR_ADDR_BIT; 
                    }
                    else{/*MISRA*/};
                }
                break;

                case I2C_CLEAR_ADDR_BIT:
                {
					if(I2C_ByteCount == 1)
                    {
                        ManagerState = I2C_GET_SINGLE_BYTE_STATUS;
                    }
                    else if(I2C_ByteCount > 1)
                    {
                        ManagerState = I2C_GET_MULTI_BYTE_STATUS;
                    }
                    else{/*MISRA*/};
                }
                break;

                case I2C_GET_SINGLE_BYTE:
                {
                    ManagerState = I2C_IDLE;
                    OldState = I2C_UN_INIT;
                    I2C_Read = 0;
                    I2C_ByteCount = 0;
                    I2C_DataPointer = 0;
					I2C_TurnOnAcknowledge(0);
                    (*(ConfigPtr->I2C_ReceptionDoneCallBack))();
                }
                break;

                case I2C_GET_MULTI_BYTES:
                {
                    if(I2C_ByteCount == 1)
                    {
                        I2C_TurnOffAcknowledge(0);
                        ManagerState = I2C_GET_SINGLE_BYTE_STATUS;
                    }
                    else if(I2C_ByteCount > 1)
                    {
                        I2C_ByteCount--;
                        I2C_DataPointer++;
                       ManagerState = I2C_GET_MULTI_BYTE_STATUS; 
                    }
                }
                break;
				
				default: break;
            }
        }
        break;

        case I2C_GET_SINGLE_BYTE_STATUS:
        {
            DriverReturnStatus = I2C_GetDataStatus(0);

            if(DriverReturnStatus == I2C_OK)
            {
                ManagerState = I2C_GET_SINGLE_BYTE;
            }
            else
            {
                ManagerState = I2C_GET_SINGLE_BYTE_STATUS;
            }
        }
        break;

        case I2C_GET_SINGLE_BYTE:
        {
            I2C_GetData(I2C_DataPointer, 0);
            ManagerState = I2C_GET_DATA_MANAGER;
            OldState = I2C_GET_SINGLE_BYTE;
        }
        break;

        case I2C_GET_MULTI_BYTE_STATUS:
        {
            DriverReturnStatus = I2C_GetDataStatus(0);

            if(DriverReturnStatus == I2C_OK)
            {
                ManagerState = I2C_GET_MULTI_BYTES;
            }
            else
            {
                ManagerState = I2C_GET_MULTI_BYTE_STATUS;
            }
        }
        break;

        case I2C_GET_MULTI_BYTES:
        {
            I2C_GetData(I2C_DataPointer, 0);
            ManagerState = I2C_GET_DATA_MANAGER;
            OldState = I2C_GET_MULTI_BYTES;
        }
        break;
    }
}
/*
        case 0:
        {
            I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= ((uint16_t)1<<I2C_CR1_START_POS);
            State = 1;
        }
        break;

        case 1:
        {
            if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_SB_POS))
	        	& (I2C_SR2(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR2_MSL_POS) | (1<< I2C_SR2_BUSY_POS))))
            {
                State = 2;
            }
            else
            {
                State = 1;
            }
        }
        break;

        case 2:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = 0xa0;
            State = 3;
        }
        break;

        case 3:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_ADDR_POS))
            {
                Temp = I2C_SR2(ConfigPtr->I2C_Peripheral_ID);
                State = 4;
            }
            else
            {
                State = 3;
            }
        }
        break;

        case 4:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = LocAddr;
            State = 5;
        }
        break;

        case 5:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR1_BTF_POS)))
            {
                State = 6;
            }
            else
            {
                State = 5;
            }
        }
        break;
        case 6:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = 0x55;
            State = 7;
        }
        break;
        case 7:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR1_BTF_POS)))
            {
                State = 8;
            }
            else
            {
                State = 7;
            }
        }
        break;
        case 8:
        {
            I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1 << I2C_CR1_STOP_POS;
            Delay_ms(1000);
            State = 9;
        }
        break;
        case 9 :
        {
            I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1 << I2C_CR1_START_POS;
            State = 10;
        }
        break;
        case 10:
        {
            if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_SB_POS))
	        	& (I2C_SR2(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR2_MSL_POS) | (1<< I2C_SR2_BUSY_POS))))
            {
                State = 11;
            }
            else
            {
                State = 10;
            }
        }
        break;

        case 11:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = 0xa0;
            State = 12;
        }
        break;

        case 12:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_ADDR_POS))
            {
                Temp = I2C_SR2(ConfigPtr->I2C_Peripheral_ID);
                State = 13;
            }
            else
            {
                State = 12;
            }
        }
        break;

        case 13:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = LocAddr;
            State = 14;
        }
        break;

        case 14:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR1_BTF_POS)))
            {
                State = 15;
            }
            else
            {
                State = 14;
            }
        }
        break;
        case 15:
        {
            I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1 << I2C_CR1_START_POS;
            State = 16;
        }
        break;
        case 16:
        {
            if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_SB_POS))
	        	& (I2C_SR2(ConfigPtr->I2C_Peripheral_ID) & ((1 << I2C_SR2_MSL_POS) | (1<< I2C_SR2_BUSY_POS))))
            {
                State = 17;
            }
            else
            {
                State = 16;
            }
        }
        break;
        case 17:
        {
            I2C_DR(ConfigPtr->I2C_Peripheral_ID) = 0xa1;
            State = 18;
        }
        break;
        case 18:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_ADDR_POS))
            {
                Temp = I2C_SR2(ConfigPtr->I2C_Peripheral_ID);
                State = 19;
            }
            else
            {
                State = 18;
            }
        }
        break;
        case 19:
        {
            if(I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1 << I2C_SR1_RXNE_POS))
            {
                Temp = I2C_DR(ConfigPtr->I2C_Peripheral_ID);
                State = 20;
            }
        }
        break;
        case 20:
        {
            I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1 << I2C_CR1_STOP_POS;
            if(Temp == 0x55)
            {
                GPIO_Write(3,1);
            }
        }
        break;
    }
}
*/
    /*switch(ManagerState)
    {
        case I2C_UN_INIT:
        {
            if(I2C_InitFlag == 1)
            {
                ManagerState = I2C_IDLE;
            }
            else
            {
                ManagerState = I2C_UN_INIT;
            }
            
            OldManagerState = I2C_UN_INIT;
        }
        break;

        case I2C_IDLE:
        {
            if(I2C_InitiateStart == 1)
            {
                I2C_GenerateStart(I2C_PeripheralID);
                ManagerState = I2C_GET_STATUS;
                OldManagerState = I2C_IDLE;
            }
            else if(I2C_SlaveAddressTransmission == 1)
            {
                I2C_SendSlaveAddress(I2C_SlaveAddress, I2C_ReadOrWrite, I2C_PeripheralID);
                ManagerState = I2C_GET_STATUS;
                OldManagerState = I2C_IDLE;
            }
            else if(I2C_WriteRequest == 1)
            {
                ManagerState = I2C_PLACE_DATA;
                OldManagerState = I2C_IDLE;
				Counter = I2C_ByteCount;

            }
            else if(I2C_ReadRequest == 1)
            {
                ManagerState = I2C_GET_STATUS;
                OldManagerState = I2C_GET_SINGLE_BYTE;
            }
            else
            {
                ManagerState = I2C_IDLE;
            }

        }
        break;

        case I2C_PLACE_DATA:
        {
            I2C_PlaceData(I2C_DataPtr, I2C_PeripheralID);
            I2C_DataPtr++;
            Counter--;
            ManagerState = I2C_GET_STATUS;
            OldManagerState = I2C_PLACE_DATA;
        }
        break;
        
        case I2C_GET_MULTIPLE_BYTES:
        {
            I2C_GetData(I2C_DataPtr, I2C_PeripheralID);   
            I2C_DataPtr++;         
            ManagerState = I2C_GET_STATUS;
            OldManagerState = I2C_GET_MULTIPLE_BYTES;
        }
        break;

        case I2C_GET_TWO_BYTES:
        {
            I2C_GetData(I2C_DataPtr, I2C_PeripheralID);
            I2C_DataPtr++;
            I2C_GetData(I2C_DataPtr, I2C_PeripheralID);
            ManagerState = I2C_IDLE;
        }
        break;

        case I2C_GET_SINGLE_BYTE:
        {
            I2C_GetData(I2C_DataPtr, I2C_PeripheralID); 
            OldManagerState = I2C_GET_SINGLE_BYTE;
            ManagerState = I2C_GENERATE_STOP;
					
        }
        break;

        case I2C_GENERATE_STOP:
        {
            I2C_GenerateStop(I2C_PeripheralID);
            OldManagerState = I2C_GENERATE_STOP;
            ManagerState = I2C_IDLE;
        }
        break;


        case I2C_GET_STATUS:
        {
            switch(OldManagerState)
            {
				case I2C_IDLE:
                {
                    if( I2C_InitiateStart == 1 )
                    {
                        DriverReturnStatus = I2C_StartStatus(I2C_PeripheralID);
                        if( DriverReturnStatus == I2C_OK )
                        {
                            I2C_InitiateStart = 0;
                            ManagerState = I2C_IDLE;
                            (*(ConfigPtr->I2C_StartDoneCallBack))();
                        }
                        else if( DriverReturnStatus == I2C_NOK )
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                    else if( I2C_SlaveAddressTransmission == 1)
                    {
                        DriverReturnStatus = I2C_SendSlaveAddressStatus(I2C_PeripheralID);
                        if( DriverReturnStatus == I2C_OK )
                        {
							if(I2C_ReadOrWrite == I2C_READ && I2C_ByteCount == 1)
							{
								I2C_TurnOffAcknowledge(I2C_PeripheralID);
							}
							I2C_SlaveAddressTransmission = 0;
                            ManagerState = I2C_IDLE;
                            (*(ConfigPtr->I2C_SendSlaveAddressDoneCallBack))();
                        }
                        else if( DriverReturnStatus == I2C_NOK )
                        {
                            ManagerState = I2C_GET_STATUS;
                        }
                    }
                }
                break;

                case I2C_PLACE_DATA:
                {
                    DriverReturnStatus = I2C_PlaceDataStatus(I2C_PeripheralID);
                    if( (DriverReturnStatus == I2C_OK) && (Counter > 0) )
                    {
                        ManagerState = I2C_PLACE_DATA;
                    }
                    else if((DriverReturnStatus == I2C_OK) && (Counter == 0))
                    {
                        ManagerState = I2C_IDLE;
                        (*(ConfigPtr->I2C_TransmissionDoneCallBack))();
                    }
                    else
                    {
                        ManagerState = I2C_GET_STATUS;                        
                    }
                }
                break;

                case I2C_GET_MULTIPLE_BYTES:
                {
                    DriverReturnStatus = I2C_GetDataStatus(I2C_PeripheralID);
                    if( DriverReturnStatus == I2C_OK )
                    {
                        if( Counter == 2)
                        {
                            I2C_TurnOffAcknowledge(I2C_PeripheralID);

                        }

                        ManagerState = I2C_GET_MULTIPLE_BYTES;
                        Counter--;

                        if( Counter == 0)
                        {
                            ManagerState = I2C_GENERATE_STOP;
                            I2C_TurnOnAcknowledge(I2C_PeripheralID);
                        }
                    }
                    else
                    {
                        ManagerState = I2C_GET_STATUS;
                    }
                }
                break;

                case I2C_GET_TWO_BYTES:
                {
                    DriverReturnStatus = I2C_Get_BTF_Status(I2C_PeripheralID);
                    if( DriverReturnStatus==I2C_OK )
                    {
                        I2C_GenerateStop(I2C_PeripheralID);
                        ManagerState = I2C_GET_TWO_BYTES;
                    }
                    else
                    {
                        ManagerState = I2C_GET_STATUS;
                    }

                }
                break;


                case I2C_GET_SINGLE_BYTE:
                {
                    DriverReturnStatus = I2C_GetDataStatus(I2C_PeripheralID);
                    if(DriverReturnStatus == I2C_OK)
                    {
                        ManagerState = I2C_GET_SINGLE_BYTE;
                    }
										else
                    {
                        ManagerState = I2C_GET_STATUS;
                    }

                }
                break;
				
				default: break;
            }
        }
        break;
    }
}
*/
/*
We need to manage: -Issuing stop at the end of write/read
                   -Handling errors
*/