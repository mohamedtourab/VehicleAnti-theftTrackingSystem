/**********************************************************************************	
*	File name: GPIO.h
*	Auther : 
			-Abdelrhman Hosny
			-Amr Mohamed
*	Date : 18/2/2018
*	Discription :
			this file contains:
				-macros to access GPIO registers
				-prototype of the APIs of the GPIO 
*	Target : STM32F407VG
**********************************************************************************/

#ifndef GPIO_H
#define GPIO_H

#include "STM32F4_MemMap.h"
#include "GPIO_Config.h"


/*************************************************************************************
**********                     Macros to access GPIO                        **********
*************************************************************************************/

#define	DEFAULT 			0U

/*************************************************************************************
**********				Macros To SET GPIO PORT number	                    **********
*************************************************************************************/
#define	PORT_GPIOA			0U
#define	PORT_GPIOB			1U
#define	PORT_GPIOC			2U
#define	PORT_GPIOD			3U
#define	PORT_GPIOE			4U
#define	PORT_GPIOF			5U
#define	PORT_GPIOG			6U
#define	PORT_GPIOH			7U
#define	PORT_GPIOI			8U

/***********************************************************************************
**********				Macros to mask the required pin						********
***********************************************************************************/
#define	PIN_MASK_0		0x0001U
#define	PIN_MASK_1		0x0002U
#define	PIN_MASK_2		0x0004U
#define	PIN_MASK_3		0x0008U
#define	PIN_MASK_4		0x0010U
#define	PIN_MASK_5		0x0020U
#define	PIN_MASK_6		0x0040U
#define	PIN_MASK_7		0x0080U
#define	PIN_MASK_8		0x0100U
#define	PIN_MASK_9		0x0200U
#define	PIN_MASK_10		0x0400U
#define	PIN_MASK_11		0x0800U
#define	PIN_MASK_12		0x1000U
#define	PIN_MASK_13		0x2000U
#define	PIN_MASK_14		0x4000U
#define	PIN_MASK_15		0x8000U
#define	PIN_MASK_ALL	0xFFFFU


/*************************************************************************************
**********				Macros To access GPIO mode register                 **********
*************************************************************************************/
#define	MODE_INPUT			0x00000000U
#define	MODE_OUTPUT			0x55555555U
#define	MODE_ALTERNATIVE	0xAAAAAAAAU
#define	MODE_ANALOG			0xFFFFFFFFU

/*************************************************************************************
**********			Macros to access Output Type register					**********
*************************************************************************************/
#define	OTYPE_PUSHPULL		0U
#define	OTYPE_OPENDRAIN		1U

/*************************************************************************************
**********			Macros to access Output speed register					**********
*************************************************************************************/
#define	OSPEED_LOW			0x00000000U
#define	OSPEED_MEDIUM		0x55555555U
#define OSPEED_HIGH			0xAAAAAAAAU
#define	OSPEED_VERYHIGH		0xFFFFFFFFU

/************************************************************************************
**********			Macros to access pull-up/pull-down register				*********
************************************************************************************/
#define	PUPD_NOPUPD			0x00000000U
#define	PUPD_PULLUP			0x55555555U
#define	PUPD_PULLDOWN		0xAAAAAAAAU

/************************************************************************************
**********			Macros to access Alternative functions register			*********
************************************************************************************/
#define	AF_SYSTEM			0x00000000U

#define	AF_TIM1				0x11111111U
#define	AF_TIM2				0x11111111U

#define	AF_TIM3				0x22222222U
#define	AF_TIM4				0x22222222U
#define	AF_TIM5				0x22222222U

#define	AF_TIM8				0x33333333U
#define	AF_TIM9				0x33333333U
#define	AF_TIM10			0x33333333U
#define	AF_TIM11			0x33333333U

#define	AF_I2C1				0x44444444U
#define	AF_I2C2				0x44444444U
#define	AF_I2C3				0x44444444U

#define	AF_SPI1				0x55555555U
#define	AF_SPI2				0x55555555U

#define	AF_SPI3				0x66666666U

#define	AF_USART1			0x77777777U
#define	AF_USART2			0x77777777U
#define	AF_USART3			0x77777777U

#define	AF_USART4			0x88888888U
#define	AF_USART5			0x88888888U
#define	AF_USART6			0x88888888U

#define	AF_CAN1				0x99999999U
#define	AF_CAN2				0x99999999U
#define	AF_TIM12			0x99999999U
#define	AF_TIM13			0x99999999U
#define	AF_TIM14			0x99999999U

#define	AF_OTG_FS			0xAAAAAAAAU
#define	AF_OTG_HS			0xAAAAAAAAU

#define	AF_ETH 				0xBBBBBBBBU

#define	AF_FSMC				0xCCCCCCCCU
#define	AF_SDIO				0xCCCCCCCCU
#define	AF_OTG_HS_C			0xCCCCCCCCU

#define	AF_DCMI				0xDDDDDDDDU

#define	AF_AF14				0xEEEEEEEEU

#define	AF_EVENTOUT			0xFFFFFFFFU

/************************************************************************************
**********						Macros to Write data 						*********
************************************************************************************/
#define LOW 		0U
#define HIGH 		1U
#define FULL_DATA 	2U

/***********************************************************************************
**********						Defined data types							********
***********************************************************************************/

//structure to be filled by the user to configure the GPIO
typedef struct 
{
	//port and pin sellection
	uint8_t Port;
	uint16_t Pin;

	//mode sellection
	/*
		Input 		00
		Output 		01
		Alt-Func 	10
		Analog 		11
	*/
	uint32_t Mode;

	//set the output type
	/*
		PushPull		0
		Open Drain 		1	
	*/
	uint16_t OutputType;

	//set the output Speed
	/*
		Low 		00
		Medium 		01
		High	 	10
		Very High	11
	*/
	uint32_t OutputSpeed;

	//set the pull up and pull down
	/*
		NPUPD 		00
		PU  		01
		PD 		 	10
		Reservied	11
	*/
	uint32_t PUPD;
} GPIO_ConfigType;

//--------------------------------------------------------------------------------

//enumerate data type for the functions' return values
typedef enum {GPIO_OK =0, GPIO_NOK} GPIO_CheckType;

//--------------------------------------------------------------------------------

//extern constant array of GPIO_ConfigType structure of the GPIO groups
extern const GPIO_ConfigType GPIO_ConfigParam [GPIO_NUMBER_OF_GROUPS];


/***********************************************************************************
**********						Functions prototypes						********
***********************************************************************************/

/*
 * This function used to initialize all the GPIO Groups in the configurations
 * Inputs:NONE
 * Output:
 	- an indication of the success of the function
*/
GPIO_CheckType GPIO_Init(void);


/*
 * This function used to Digital Write data to a specific group
 * Inputs:
 	- GroupId		 : the index of the structure in the GPIO_ConfogParam array
 	- Data  		 : the data to write to that specific group
 	- State 		 : set if pin is HIGH or LOW or Control ALL pins(FULL_DATA)
 * Output:
 	- an indication of the success of the function
*/
GPIO_CheckType GPIO_Write(uint8_t GroupId, uint16_t Data, uint8_t State);


/*
 * This function used to Digital read data from a specific group
 * Inputs:
 	- GroupId		 : the index of the structure in the GPIO_ConfogParam array
 	- DataPtr  		 : a pointer to a variable to store the data read from that specific group
 * Output:
 	- an indication of the success of the function
*/

GPIO_CheckType GPIO_Read(uint8_t GroupId, uint16_t* DataPtr);


/*
 * This function used to select which peripheral will be connected to a GPIO pin
 * Inputs:
 	- GroupId		 : the index of the structure in the GPIO_ConfogParam array
 	- AlternFuncId   : the ID of the alternative function
 * Output:
 	- an indication of the success of the function
*/
GPIO_CheckType GPIO_SetAlternFuntion(uint8_t GroupId, uint32_t AlternFuncId);

/*********************************************************************************/

#endif