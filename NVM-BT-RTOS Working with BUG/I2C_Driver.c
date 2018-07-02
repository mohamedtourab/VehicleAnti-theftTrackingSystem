/*
 ********************************************************************************************
 *                                                                                          *
 *        Authors: Wessam Adel and Mohamed Mamdouh                                          *
 *        Date: 11/6/2018                                                                   *
 *        Microcontroller: STM32F407VG                                                      *
 *                                                                                          *
 *        Description: This file contains the implementations of initialization of          *
 *                     I2C peripheral for single master and single slave and the            *
 *                     helper functions. This code is designed for single/multi byte        *
 *                     write/read                                                           *
 *            				                                                    *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */
#include "I2C_Driver.h"

/*
 ********************************************************************************************
 *                                                                                          *
 *     This Function is used to initilaize the I2C peripherals                              *
 *      - It sets the clock gating for the I2C peripherals                                  *
 *      - It checks for valid I2C peripheral clock speed                                    *
 *      - It sets the GPIO Alternating function                                             *
 *      - It initializes proper registers if user asked for interrupts                      *
 *      - It returns I2C_OK for for successful initialization, and I2C_NOK for failure      *
 *                                                                                          *
 ********************************************************************************************
 */

uint8_t I2C_InitFlag = 0;

I2C_CheckType I2C_Init(void)
{
    I2C_CheckType RetVal;
    const I2C_ConfigType* ConfigPtr;
    uint8_t LoopIndex;
    for(LoopIndex = 0; LoopIndex < I2C_PERIPHERAL_NUMBER; LoopIndex++)
    {
            ConfigPtr = &I2C_ConfigParam[LoopIndex];
            
            /*Checking for a valid configuration*/
            if(        (ConfigPtr->I2C_PeripheralFrequency>=PERIPHERAL_CLOCK_2MHz)                              &&
                        (ConfigPtr->I2C_PeripheralFrequency<=PERIPHERAL_CLOCK_50MHz)                            &&
                        ((ConfigPtr->I2C_FrequencyOf_SCL<=MAX_SCL_FREQUENCY_SM && !ConfigPtr->I2C_SpeedMode)    ||
                        (ConfigPtr->I2C_FrequencyOf_SCL<=MAX_SCL_FREQUENCY_FM && ConfigPtr->I2C_SpeedMode))                                        
                    )
                    {
                            
                            GPIO_SetAlternFuntion(ConfigPtr->I2C_GPIO_Group_ID,AF_I2C1);
			    I2C_RCC_APB1EN |= (((uint32_t)1)<<RCC_APB1ENR_I2CEN_POS(ConfigPtr->I2C_Peripheral_ID));
                            I2C_CR1(ConfigPtr->I2C_Peripheral_ID)  &=~ (((uint16_t)ConfigPtr->I2C_PeripheralEnable)<<I2C_CR1_PE_POS);
                                                                            
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
                            I2C_CR1(ConfigPtr->I2C_Peripheral_ID)         |= (((uint16_t)1)<<I2C_CR1_ACK_POS);
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
                            I2C_InitFlag = 1;
                    }
                    else
                    {
                            RetVal = I2C_NOK;
                            I2C_InitFlag = 0;
                    }
    }
    return RetVal;
}



void I2C_GenerateStart(uint8_t Peripheral_ID)
{
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
    
    /*Set Stat bit in CR1*/
    I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= ((uint16_t)1<<I2C_CR1_START_POS);
}

/*
 ********************************************************************************************
 *                                                                                          *
 *     This Function is used by the GetStatus function in the FSM of the manager            *
 *      - It checks that start bit is sent successfully or not                              *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

I2C_CheckType I2C_StartStatus(uint8_t Peripheral_ID)
{
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];

    I2C_CheckType RetVal;
    
    if( ((I2C_SR1(ConfigPtr->I2C_Peripheral_ID)) & (1<<I2C_SR1_SB_POS)) == (1<<I2C_SR1_SB_POS) )
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
 ********************************************************************************************
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

void I2C_SendSlaveAddress(uint8_t SlaveAddress,uint8_t WriteOrRead, uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = (SlaveAddress|WriteOrRead);
}

I2C_CheckType I2C_SendSlaveAddressStatus(uint8_t Peripheral_ID)
{
    I2C_CheckType RetVal;
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
    
    if ( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1<<I2C_SR1_ADDR_POS)) == (1<<I2C_SR1_ADDR_POS))
    {
        RetVal = I2C_OK;
    }
    else
    {
        RetVal = I2C_NOK;
    }
    return RetVal;
}

void I2C_Clear_ADDR(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_SR1(ConfigPtr->I2C_Peripheral_ID);
        I2C_SR2(ConfigPtr->I2C_Peripheral_ID);
}

/*
 ********************************************************************************************
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

void I2C_PlaceData(uint8_t* DataPtr, uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        /*Put data in the data register*/
        I2C_DR(ConfigPtr->I2C_Peripheral_ID) = (*DataPtr);
}

I2C_CheckType I2C_PlaceDataStatus(uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        
       if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1<<I2C_SR1_BTF_POS)))
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
 ********************************************************************************************
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

void I2C_GetData(uint8_t *Data, uint8_t Peripheral_ID)
{
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
    *Data = I2C_DR(ConfigPtr->I2C_Peripheral_ID);
}


I2C_CheckType I2C_GetDataStatus(uint8_t Peripheral_ID)
{
        
    I2C_CheckType RetVal;
    const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];

    if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID) & (1<<I2C_SR1_RXNE_POS)) )
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
 ********************************************************************************************
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

void I2C_GenerateStop(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= (1<<I2C_CR1_STOP_POS);
}

void I2C_TurnOffAcknowledge(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) &=~ (1<<I2C_CR1_ACK_POS);
}

void I2C_TurnOnAcknowledge(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= (1<<I2C_CR1_ACK_POS);
}

void I2C_TurnOnPos(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) |= (1<<I2C_CR1_POS_POS);
}


void I2C_TurnOffPos(uint8_t Peripheral_ID)
{
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        I2C_CR1(ConfigPtr->I2C_Peripheral_ID) &=~ (1<<I2C_CR1_POS_POS);
}

I2C_CheckType I2C_Get_BTF_Status(uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];
        if( (I2C_SR1(ConfigPtr->I2C_Peripheral_ID)&(1<<I2C_SR1_BTF_POS)) == (1<<I2C_SR1_BTF_POS) )
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
 ********************************************************************************************
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 *                                                                                          *
 ********************************************************************************************
 */

I2C_CheckType I2C_ErrorCheck(uint8_t Peripheral_ID)
{
        I2C_CheckType RetVal;
        const I2C_ConfigType* ConfigPtr = &I2C_ConfigParam[Peripheral_ID];

        if((I2C_SR1(ConfigPtr->I2C_Peripheral_ID))  &
            ((1<<I2C_SR1_AF_POS)|(1<<I2C_SR1_ARLO_POS)|(1<<I2C_SR1_BERR_POS)))
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
/*
void I2C_Test(void)
{
        volatile uint16_t Temp;
        I2C_CR1(0) |= 1 << I2C_CR1_START_POS;
	while (!((I2C_SR1(0) & (1 << I2C_SR1_SB_POS))
		& (I2C_SR2(0) & ((1 << I2C_SR2_MSL_POS) | (1<< I2C_SR2_BUSY_POS)))));
                I2C_DR(0) = 0xa0;
                while (!(I2C_SR1(0) & (1 << I2C_SR1_ADDR_POS)));
                Temp = I2C_SR2(0);
                I2C_DR(0) = 0x00;
                while (!(I2C_SR1(0) & ((1 << I2C_SR1_BTF_POS))));
                I2C_DR(0) = 0x55;
                while (!(I2C_SR1(0) & ((1 << I2C_SR1_BTF_POS))));
                I2C_CR1(0) |= 1 << I2C_CR1_STOP_POS;
                Delay_ms(1000);
}
*/