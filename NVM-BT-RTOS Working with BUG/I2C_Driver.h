/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/


#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H


#include <stdint.h>
#include "I2C_Config.h"
#include "GPIO.h"

#define MAX_SCL_FREQUENCY_SM		(100000U)
#define MAX_SCL_FREQUENCY_FM		(400000U)
#define WRITE 						(0U)
#define READ						(1U)
#define	I2C_DELAY_PARAMETER			(50U)

typedef volatile uint32_t* const I2C_RegisterAddressType;

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            						BASES						              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
#define PERIPHERAL_BASE    						(0x40000000U)
#define RCC_BASE1								(0x40023800U)
#define APB1PERIPH_BASE1   				 		PERIPHERAL_BASE
#define I2C_BASE								(APB1PERIPH_BASE1+0x5400U)

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            						OFFSETS						              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
#define I2C_CR1_OFFSET  						(0x00)  
#define I2C_CR2_OFFSET  						(0x04)  
#define I2C_OAR1_OFFSET 						(0x08)  
#define I2C_OAR2_OFFSET 						(0x0C)  
#define I2C_DR_OFFSET   						(0x10)  
#define I2C_SR1_OFFSET  						(0x14)  
#define I2C_SR2_OFFSET  						(0x18)  
#define I2C_CCR_OFFSET  						(0x1C)  
#define I2C_TRISE_OFFSET						(0x20)  
/******************************************************************/
#define I2C_MAXIMUM_NUMBER						(3U)	//Number of Maximum I2C peripherals used




/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            						ADDRESSES					              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

#define I2C_RCC_APB1EN															*((I2C_RegisterAddressType)	(RCC_BASE1 + RCC_APB1ENR_OFFSET))
#define I2C_CR1(PERIPHERAL_ID)													*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_CR1_OFFSET))
#define I2C_CR2(PERIPHERAL_ID)													*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_CR2_OFFSET))
#define I2C_OAR1(PERIPHERAL_ID)													*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_OAR1_OFFSET))
#define I2C_OAR2(PERIPHERAL_ID)   												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_OAR2_OFFSET))
#define I2C_DR(PERIPHERAL_ID)     												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_DR_OFFSET))
#define I2C_SR1(PERIPHERAL_ID)    												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_SR1_OFFSET))
#define I2C_SR2(PERIPHERAL_ID)    												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_SR2_OFFSET))
#define I2C_CCR(PERIPHERAL_ID)    												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_CCR_OFFSET))
#define I2C_TRISE(PERIPHERAL_ID)  												*((I2C_RegisterAddressType) (I2C_BASE + (0x400*(PERIPHERAL_ID-1U)) + I2C_TRISE_OFFSET))




/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            						BIT POSITION				              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */
#define I2C_SR1_BERR_POS                  					      	(8U)
#define I2C_SR1_ARLO_POS                  					      	(9U)
#define I2C_SR1_AF_POS                    					      	(10U)
#define I2C_SR1_BTF_POS          								  	(2U)                                         
#define I2C_SR1_ADDR_POS 										  	(1U)      

#define I2C_SR1_RXNE_POS         								  	(6U)                                         
#define I2C_SR1_TXE_POS         								  	(7U)                                         
#define I2C_SR1_SB_POS          								  	(0U)  //Start Bit Position in status register                                      
#define I2C_CR1_POS_POS  											(11U)
#define I2C_SR2_MSL_POS												(0U)
#define I2C_CR1_START_POS  											(8U)                                         
#define I2C_CR1_STOP_POS   											(9U)                                         
#define I2C_CR1_ENGC_POS  											(6U)      //General Call Enable Bit position                                   									
#define I2C_CR1_SMBUS_POS											(1U)                                         																							
#define I2C_CR1_PE_POS  											(0U)                                         													
#define I2C_CR2_ITERREN_POS       				 					(8U)                                         
#define I2C_CR2_ITEVTEN_POS       				 					(9U)                                         
#define I2C_CR2_ITBUFEN_POS       				 					(10U)
#define I2C_CCR_FS_POS        										(15U)                                        
#define I2C_CR1_ACK_POS           									(10U)   
#define I2C_SR2_BUSY_POS											(1U)
#define RCC_APB1ENR_I2CEN_POS(PERIPHERAL_ID)              (21U+PERIPHERAL_ID-1)             

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *        enums containing:						              				  *
 *               - I2C_CheckType                                              *
 *               - enums for configuration by the user                        *
 *                                                                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

typedef enum 
{
	I2C_NOK=0,
	I2C_OK=1,
	I2C_BUSY
}I2C_CheckType;

typedef enum
{	
	PERIPHERAL_DISABLE=0,
	PERIPHERAL_ENABLE=1
}PeripheralEnable;
	
typedef enum 
{
	STANDARD_MODE=0,
	FAST_MODE=1
}SpeedMode;

typedef enum 
{
	I2C_MODE=0,
	SMBUS_MODE=1
}BusMode;

typedef enum 
{
	DISABLE_GENERAL_CALL=0,
	ENABLE_GENERAL_CALL=1
}GeneralCall;

typedef enum
{
	I2C_1 = 1,
	I2C_2 = 2,
	I2C_3 = 3
}Peripheral_ID;

typedef enum
{
	PERIPHERAL_CLOCK_2MHz=2,
	PERIPHERAL_CLOCK_3MHz=3,
	PERIPHERAL_CLOCK_4MHz=4,
	PERIPHERAL_CLOCK_5MHz=5,
	PERIPHERAL_CLOCK_6MHz=6,
	PERIPHERAL_CLOCK_7MHz=7,
	PERIPHERAL_CLOCK_8MHz=8,
	PERIPHERAL_CLOCK_9MHz=9,
	PERIPHERAL_CLOCK_10MHz=10,
	PERIPHERAL_CLOCK_11MHz=11,
	PERIPHERAL_CLOCK_12MHz=12,
	PERIPHERAL_CLOCK_13MHz=13,
	PERIPHERAL_CLOCK_14MHz=14,
	PERIPHERAL_CLOCK_15MHz=15,
	PERIPHERAL_CLOCK_16MHz=16,
	PERIPHERAL_CLOCK_17MHz=17,
	PERIPHERAL_CLOCK_18MHz=18,
	PERIPHERAL_CLOCK_19MHz=19,
	PERIPHERAL_CLOCK_20MHz=20,
	PERIPHERAL_CLOCK_21MHz=21,
	PERIPHERAL_CLOCK_22MHz=22,
	PERIPHERAL_CLOCK_23MHz=23,
	PERIPHERAL_CLOCK_24MHz=24,
	PERIPHERAL_CLOCK_25MHz=25,
	PERIPHERAL_CLOCK_26MHz=26,
	PERIPHERAL_CLOCK_27MHz=27,
	PERIPHERAL_CLOCK_28MHz=28,
	PERIPHERAL_CLOCK_29MHz=29,
	PERIPHERAL_CLOCK_30MHz=30,
	PERIPHERAL_CLOCK_31MHz=31,
	PERIPHERAL_CLOCK_32MHz=32,
	PERIPHERAL_CLOCK_33MHz=33,
	PERIPHERAL_CLOCK_34MHz=34,
	PERIPHERAL_CLOCK_35MHz=35,
	PERIPHERAL_CLOCK_36MHz=36,
	PERIPHERAL_CLOCK_37MHz=37,
	PERIPHERAL_CLOCK_38MHz=38,
	PERIPHERAL_CLOCK_39MHz=39,
	PERIPHERAL_CLOCK_40MHz=40,
	PERIPHERAL_CLOCK_41MHz=41,
	PERIPHERAL_CLOCK_42MHz=42,
	PERIPHERAL_CLOCK_43MHz=43,
	PERIPHERAL_CLOCK_44MHz=44,
	PERIPHERAL_CLOCK_45MHz=45,
	PERIPHERAL_CLOCK_46MHz=46,
	PERIPHERAL_CLOCK_47MHz=47,
	PERIPHERAL_CLOCK_48MHz=48,
	PERIPHERAL_CLOCK_49MHz=49,
	PERIPHERAL_CLOCK_50MHz=50
}PeripheralFrequency;

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            				Configuration Structure				              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

typedef void (*I2C_CallBackFunctionType)(void);

typedef struct
{
	uint8_t								I2C_GPIO_Group_ID;
	/*used to choose from I2Cx, x=1,2,3*/
	Peripheral_ID						I2C_Peripheral_ID;
	/*SCL operating frequency */
	uint32_t							I2C_FrequencyOf_SCL;
	/*I2C operating frequency */
	PeripheralFrequency		I2C_PeripheralFrequency;
	/**/
	PeripheralEnable			I2C_PeripheralEnable;
	/**/
	SpeedMode							I2C_SpeedMode;
	/**/
	BusMode								I2C_BusMode;
	/**/
	GeneralCall						I2C_GeneralCall;
	/**/
	I2C_CallBackFunctionType	I2C_TransmissionDoneCallBack;

	I2C_CallBackFunctionType	I2C_ReceptionDoneCallBack;
	
}I2C_ConfigType;

extern const I2C_ConfigType I2C_ConfigParam[I2C_PERIPHERAL_NUMBER];

/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            				Functions Prototypes				              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

I2C_CheckType I2C_Init(void);
void I2C_GenerateStart(uint8_t Peripheral_ID);
I2C_CheckType I2C_StartStatus(uint8_t Peripheral_ID);

void I2C_SendSlaveAddress(uint8_t SlaveAddress,uint8_t WriteOrRead, uint8_t Peripheral_ID);
I2C_CheckType I2C_SendSlaveAddressStatus(uint8_t Peripheral_ID);
void I2C_Clear_ADDR(uint8_t Peripheral_ID);

void I2C_PlaceData(uint8_t* DataPtr, uint8_t Peripheral_ID);
I2C_CheckType I2C_PlaceDataStatus(uint8_t Peripheral_ID);

void I2C_GetData(uint8_t *Data, uint8_t Peripheral_ID);
I2C_CheckType I2C_GetDataStatus(uint8_t Peripheral_ID);

void I2C_GenerateStop(uint8_t Peripheral_ID);

I2C_CheckType I2C_ErrorCheck(uint8_t Peripheral_ID);

void I2C_TurnOffAcknowledge(uint8_t Peripheral_ID);
void I2C_TurnOnAcknowledge(uint8_t Peripheral_ID);

void I2C_TurnOffPos(uint8_t Peripheral_ID);
void I2C_TurnOnPos(uint8_t Peripheral_ID);

I2C_CheckType I2C_Get_BTF_Status(uint8_t Peripheral_ID);


/*
 ******************************************************************************
 *                                                                            *
 *                                                                            *
 *            				Call Back Function					              *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 */

extern uint8_t I2C_InitFlag;

#endif /*end of I2C_DRIVER_H*/


