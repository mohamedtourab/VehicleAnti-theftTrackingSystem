/*
 *
 *        Authors: Wessam Adel and Mohamed Mamdouh
 *        Date: 18/2/2018
 *        Last Edited : 16/3/2018
 *        Microcontroller: STM32F407VG
 *        
 *        Description: This file contains the implementations of initialization of
 *                                                         I2C1 peripheral for single master and single slave and the 
 *                                                         helper functions. This code is designed for single byte write
 *                                                         or single byte read.
*/

#include "I2C_Driver.h"

#if(I2C_PERIPHERAL_NUMBER>I2C_MAXIMUM_NUMBER)
#error "Invalid Peripheral Number"
#endif

/*
        I2C initialization function
*/
I2C_CheckType I2C_Init(void)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        uint8_t LoopIndex;
        for(LoopIndex = 0; LoopIndex < I2C_PERIPHERAL_NUMBER; LoopIndex++)
        {
                ConfigPtr = &I2C_ConfigParam[LoopIndex];
                
                /*Checking for a valid configuration*/
                if(        (ConfigPtr->I2C_PeripheralFrequency>=PERIPHERAL_CLOCK_2MHz)                                                     &&
                                (ConfigPtr->I2C_PeripheralFrequency<=PERIPHERAL_CLOCK_50MHz)                                               &&
                                ((ConfigPtr->I2C_FrequencyOf_SCL<=MAX_SCL_FREQUENCY_SM && !ConfigPtr->I2C_SpeedMode)                        ||
                                (ConfigPtr->I2C_FrequencyOf_SCL<=MAX_SCL_FREQUENCY_FM && ConfigPtr->I2C_SpeedMode))                                        
                        )
                        {
                                
                                I2C_RCC_APB1EN |= (((uint32_t)1)<<RCC_APB1ENR_I2CEN_POS(ConfigPtr->I2C_Peripheral_ID));
                                I2C_CR1(ConfigPtr->I2C_Peripheral_ID)  &=~ (((uint16_t)ConfigPtr->I2C_PeripheralEnable)<<I2C_CR1_PE_POS);
                                                                                
                                GPIO_SetAlternFuntion(LoopIndex,AF_I2C1);
                                GPIO_SetAlternFuntion(LoopIndex+1,AF_I2C1);
                                                                                
                                                                                                        /*SET PERIPHERAL FREQUENCY*/
                                I2C_CR2(ConfigPtr->I2C_Peripheral_ID)         |= ((uint16_t)ConfigPtr->I2C_PeripheralFrequency);
                                /*SET SPEED MODE IN CCR*/        
                                I2C_CCR(ConfigPtr->I2C_Peripheral_ID)         |= ((ConfigPtr->I2C_SpeedMode)<< I2C_CCR_FS_POS);
                                /*SET VALUE OF CCR IN CRR REGISTER*/        
                                I2C_CCR(ConfigPtr->I2C_Peripheral_ID)         |= (((ConfigPtr->I2C_PeripheralFrequency)*1000000)/((ConfigPtr->I2C_FrequencyOf_SCL)));
                                /*COMPUTE AND ASSIGN MAXIMUM RISE TIME*/
                                I2C_TRISE(ConfigPtr->I2C_Peripheral_ID)       |= ((uint16_t)ConfigPtr->I2C_PeripheralFrequency)+1;
                                /*Enable/Disable Peripheral*/
                                I2C_CR1(ConfigPtr->I2C_Peripheral_ID)         |= (((uint16_t)ConfigPtr->I2C_PeripheralEnable)<<I2C_CR1_PE_POS);
                                /*Enable/Disable Acknowledge*/        
                                I2C_CR1(ConfigPtr->I2C_Peripheral_ID)         |= (((uint16_t)ConfigPtr->I2C_Acknowledge)<<I2C_CR1_ACK_POS);
                                /*Enable/Disable I2C mode*/        
                                I2C_CR1(ConfigPtr->I2C_Peripheral_ID)         |= (((uint16_t)ConfigPtr->I2C_BusMode)<<I2C_CR1_SMBUS_POS);
                                /*Enable / Disbale General Call*/        
                                I2C_CR1(ConfigPtr->I2C_Peripheral_ID)         |= (((uint16_t)ConfigPtr->I2C_GeneralCall)<<I2C_CR1_ENGC_POS);

                                #if(I2C_USE_INT_TO_HANDLE==1)
                                        I2C_CR2(ConfigPtr->I2C_Peripheral_ID) |= 1<<I2C_CR2_ITBUFEN_POS;
                                        I2C_CR2(ConfigPtr->I2C_Peripheral_ID) |= 1<<I2C_CR2_ITEVTEN_POS;
                                        I2C_CR2(ConfigPtr->I2C_Peripheral_ID) |= 1<<I2C_CR2_ITERREN_POS;
                                #endif
                                RetVal = I2C_OK;
                        }
                        else
                        {
                                RetVal = I2C_NOK;
                        }
        }
                return RetVal;
}

/*
 *********************************************************************************************************************        
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                         *
 *                                                                                                                                                                        I2C HELPER FUNCTIONS                                                                                                                                                                                                                 *
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                         *
 *********************************************************************************************************************
*/

/*
        this function generates a start bit to initiate communication and it is used to generate a repeated start condition
*/
I2C_CheckType I2C_GenerateStart(uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        
        /*Set Stat bit in CR1*/
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1<<I2C_CR1_START_POS;
        
        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)) & (1<<I2C_SR1_SB_POS))
        {
                RetVal = I2C_OK;
        }
        else
        {
                RetVal = I2C_NOK;
        }
        return RetVal;
}

/*
        this functoin sends the slave address through the SDA line
*/

I2C_CheckType I2C_SetSlaveAddres(uint8_t SlaveAddress,uint8_t WriteOrRead, uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = SlaveAddress|WriteOrRead;
        
        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_ADDR_POS)) || (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_TXE_POS)) )
        {
                RetVal = I2C_OK;
        }
        else
        {
                RetVal = I2C_NOK;
        }
        return RetVal;
}
/*
        this function sends data to the slave
*/
I2C_CheckType I2C_PlaceData(uint8_t Data, uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        
        /*Put data in the data register*/
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = (Data);
        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_ADDR_POS)) || (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_TXE_POS)) )
        {
                if(I2C_DR(ConfigPtr->I2C_Peripheral_ID) == (Data))
                {
                        RetVal = I2C_OK;
                }
        }
        else
        {
                RetVal = I2C_NOK;
        }
        return RetVal;
}
/*
        this function sends the specified memory location in which data is written to or read from
*/
I2C_CheckType I2C_SetLocationAddress(uint8_t LocationAddress, uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        
        /*Put data in the data register*/
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = (LocationAddress);
        

        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_ADDR_POS)) || (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_TXE_POS)) )
        {
                RetVal = I2C_OK;
        }
        else
        {
                RetVal = I2C_NOK;
        }

        return RetVal;
}
/*
 ***************************************************************************************************************************
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 *
 *                                        BUG HERE                                                                         *
 *     What happens is that the data register does not catch the data transmitted from the slave                                                    *
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 *
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 *
 ***************************************************************************************************************************                                                                                                                                                                                                                                                                                                                                
*/

/*
                this function sets the slave address and send it through the SDA and gets data from slave
*/
I2C_CheckType I2C_SetSlaveAddressAndGetData(uint8_t SlaveAddress,uint8_t *Data, uint8_t Peripheral_ID)
{
        
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) &=~ ((1)<<I2C_CR1_ACK_POS);
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = (SlaveAddress|1);

        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_ADDR_POS)) || (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_RXNE_POS)) )//|(1<<I2C_SR1_RXNE_POS)
        {
                        *Data=I2C_DR(ConfigPtr->I2C_Peripheral_ID);
                        RetVal = I2C_OK;
        }
        else
        {
                RetVal = I2C_NOK;
        }
        return RetVal;
}
/*
        this function generates a stop bit to close the communication
*/
void I2C_GenerateStop(uint8_t Peripheral_ID)
{
        volatile const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= 1<<I2C_CR1_STOP_POS;
}
/*
        this function reads Status Register#1
*/
uint16_t I2C_SR1_Read(uint8_t Peripheral_ID)
{
        volatile const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        return (I2C_SR1(ConfigPtr->I2C_Peripheral_ID));
}

/*
        this function reads Status Register#2
*/
uint16_t I2C_SR2_Read(uint8_t Peripheral_ID)
{
        volatile const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        return (I2C_SR2(ConfigPtr->I2C_Peripheral_ID));
}

I2C_CheckType I2C_ErrorCheck(uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr;
        ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID))&((1<<I2C_SR1_AF_POS)|(1<<I2C_SR1_ARLO_POS)|(1<<I2C_SR1_BERR_POS)))
        {
                (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)) &=~ ((uint32_t)((1<<I2C_SR1_AF_POS)|(1<<I2C_SR1_ARLO_POS)|(1<<I2C_SR1_BERR_POS)));
                RetVal = I2C_NOK;
        }
        else
        {
                RetVal = I2C_OK;
        }
        return RetVal;
}