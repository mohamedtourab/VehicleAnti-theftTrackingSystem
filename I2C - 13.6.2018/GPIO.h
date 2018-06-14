/**********************************************************************************        
*        File name: GPIO.h
*        Auther : 
                        -Abdelrhman Hosny
                        -Amr Mohamed
*        Date : 18/2/2018
*        Discription :
                        this file contains:
                                -macros to access GPIO registers
                                -prototype of the APIs of the GPIO 
*        Target : STM32F407VG
**********************************************************************************/

#ifndef GPIO_H
#define GPIO_H

#include "STM32F4_MemMap.h"
#include "GPIO_Config.h"


/*************************************************************************************
**********                     Macros to access GPIO                            ******
*************************************************************************************/

#define        DEFAULT                         0U

/*************************************************************************************
**********                      Macros To SET GPIO PORT number                  ******
*************************************************************************************/
#define        PORT_GPIOA                        0U
#define        PORT_GPIOB                        1U
#define        PORT_GPIOC                        2U
#define        PORT_GPIOD                        3U
#define        PORT_GPIOE                        4U
#define        PORT_GPIOF                        5U
#define        PORT_GPIOG                        6U
#define        PORT_GPIOH                        7U
#define        PORT_GPIOI                        8U

/***********************************************************************************
**********					Macros to mask the required pin     	            ****
***********************************************************************************/
#define        PIN_MASK_0                0U
#define        PIN_MASK_1                1U
#define        PIN_MASK_2                2U
#define        PIN_MASK_3                3U
#define        PIN_MASK_4                4U
#define        PIN_MASK_5                5U
#define        PIN_MASK_6                6U
#define        PIN_MASK_7                7U
#define        PIN_MASK_8                8U
#define        PIN_MASK_9                9U
#define        PIN_MASK_10				10U
#define        PIN_MASK_11				11U
#define        PIN_MASK_12				12U
#define        PIN_MASK_13				13U
#define        PIN_MASK_14				14U
#define        PIN_MASK_15				15U


/*************************************************************************************
**********                      Macros To access GPIO mode register             ******
*************************************************************************************/
#define        MODE_INPUT                         0U
#define        MODE_OUTPUT                        1U
#define        MODE_ALTERNATIVE                   2U
#define        MODE_ANALOG                        3U

/*************************************************************************************
**********              Macros to access Output Type register                   ******
*************************************************************************************/
#define        OTYPE_PUSHPULL           0U
#define        OTYPE_OPENDRAIN          1U

/*************************************************************************************
**********              Macros to access Output speed register                  ******
*************************************************************************************/
#define         OSPEED_LOW              0U
#define         OSPEED_MEDIUM           1U
#define         OSPEED_HIGH             2U
#define         OSPEED_VERYHIGH         3U

/************************************************************************************
**********              Macros to access pull-up/pull-down register             *****
************************************************************************************/
#define        PUPD_NOPUPD              0U
#define        PUPD_PULLUP              1U
#define        PUPD_PULLDOWN            2U

/************************************************************************************
**********              Macros to access Alternative functions register         *****
************************************************************************************/
#define        AF_SYSTEM                0U

#define        AF_TIM1                  1U
#define        AF_TIM2                  1U

#define        AF_TIM3                  2U
#define        AF_TIM4                  2U
#define        AF_TIM5                  2U

#define        AF_TIM8                  3U
#define        AF_TIM9                  3U
#define        AF_TIM10                 3U
#define        AF_TIM11                 3U

#define        AF_I2C1                  4U
#define        AF_I2C2                  4U
#define        AF_I2C3                  4U

#define        AF_SPI1                  5U
#define        AF_SPI2                  5U

#define        AF_SPI3                  6U

#define        AF_USART1                7U
#define        AF_USART2                7U
#define        AF_USART3                7U

#define        AF_USART4                8U
#define        AF_USART5                8U
#define        AF_USART6                8U

#define        AF_CAN1                  9U
#define        AF_CAN2                  9U
#define        AF_TIM12                 9U
#define        AF_TIM13                 9U
#define        AF_TIM14                 9U

#define        AF_OTG_FS                10U
#define        AF_OTG_HS                10U

#define        AF_ETH                   11U

#define        AF_FSMC                  12U
#define        AF_SDIO                  12U
#define        AF_OTG_HS_C              12U

#define        AF_DCMI                  13U

#define        AF_AF14                  14U

#define        AF_EVENTOUT              15U

/*************************************************************************************
**********                      Macros To SET Output Data                       ******
*************************************************************************************/
#define        LOW              0U
#define        HIGH             1U

/***********************************************************************************
**********                      Defined data types                              ****
***********************************************************************************/

//structure to be filled by the user to configure the GPIO
typedef struct 
{
        //port and pin sellection
        uint8_t Port;
        uint8_t Pin;

        //mode sellection
        /*
                Input           00
                Output          01
                Alt-Func        10
                Analog          11
        */
        uint8_t Mode;

        //set the output type
        /*
                PushPull        0
                Open Drain      1        
        */
        uint8_t OutputType;

        //set the output Speed
        /*
                Low             00
                Medium          01
                High            10
                Very High       11
        */
        uint8_t OutputSpeed;

        //set the pull up and pull down
        /*
                NPUPD           00
                PU              01
                PD              10
                Reservied       11
        */
        uint8_t PUPD;
} GPIO_ConfigType;

//--------------------------------------------------------------------------------

//enumerate data type for the functions' return values
typedef enum {GPIO_OK =0, GPIO_NOK} GPIO_CheckType;

//--------------------------------------------------------------------------------

//extern constant array of GPIO_ConfigType structure of the GPIO groups
extern const GPIO_ConfigType GPIO_ConfigParam [GPIO_NUMBER_OF_GROUPS];

/***********************************************************************************
**********                      Functions prototypes                            ****
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
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - Data                   : the data to write to that specific group
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_Write(uint8_t GroupId, uint8_t Data);


/*
 * This function used to Digital read data from a specific group
 * Inputs:
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - DataPtr                   : a pointer to a variable to store the data read from that specific group
 * Output:
         - an indication of the success of the function
*/

GPIO_CheckType GPIO_Read(uint8_t GroupId, uint8_t* DataPtr);


/*
 * This function used to select which peripheral will be connected to a GPIO pin
 * Inputs:
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - AlternFuncId   : the ID of the alternative function
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_SetAlternFuntion(uint8_t GroupId, uint8_t AlternFuncId);

/*********************************************************************************/

#endif




