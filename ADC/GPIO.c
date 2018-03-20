/**************************************************************************************
*        File name: GPIO.c
*Auther : 
                        -Abdelrhman Hosny
                        -Amr Mohamed
*        Date: 18/2/2018
*        Description:
*                        This file contains:
*                        - implementation of functions used to access GPIO register
*        Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "GPIO.h"

// number of available ports 
#define NUMBER_OF_PORTS     9U 

/*************************************************************************************
**********                      GPIO Memory Map                                 ******
*************************************************************************************/

/**********************************GPIO******************************************/
#define GPIO_BASE_ADD       0x40020000U
/******************************GPIOx offsets*************************************/
#define GPIOA_OFFSET        0x0000U
#define GPIOB_OFFSET        0x0400U
#define GPIOC_OFFSET        0x0800U
#define GPIOD_OFFSET        0x0C00U
#define GPIOE_OFFSET        0x1000U
#define GPIOF_OFFSET        0x1400U
#define GPIOG_OFFSET        0x1800U
#define GPIOH_OFFSET        0x1C00U
#define GPIOI_OFFSET        0x2000U
/***************************GPIO ports base addresses****************************/
//address for the GPIOA 
#define GPIOA_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOA_OFFSET))
//address for the GPIOB 
#define GPIOB_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOB_OFFSET))
//address for the GPIOC 
#define GPIOC_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOC_OFFSET))
//address for the GPIOD 
#define GPIOD_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOD_OFFSET))
//address for the GPIOE 
#define GPIOE_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOE_OFFSET))
//address for the GPIOF 
#define GPIOF_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOF_OFFSET))
//address for the GPIOG 
#define GPIOG_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOG_OFFSET))
//address for the GPIOH 
#define GPIOH_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOH_OFFSET))
//address for the GPIOI 
#define GPIOI_BASE_ADD ((uint32_t)(GPIO_BASE_ADD + GPIOI_OFFSET))
/******************************GPIO registers offsets****************************/
#define GPIO_MODER_OFFSET           0x00U
#define GPIO_OTYPER_OFFSET          0x04U
#define GPIO_OSPEEDR_OFFSET         0x08U
#define GPIO_PUPDR_OFFSET           0x0CU
#define GPIO_IDR_OFFSET             0x10U
#define GPIO_ODR_OFFSET             0x14U
#define GPIO_BSRR_OFFSET            0x18U
#define GPIO_LCKR_OFFSET            0x1CU
#define GPIO_AFRL_OFFSET            0x20U
#define GPIO_AFRH_OFFSET            0x24U


/***********************************************************************************
**********						Defined data types							********
***********************************************************************************/

/*define a datatype that is volatile (to privent compiler optimization)
 *and constat becouse it carries a constant address
*/
typedef volatile uint32_t* const GPIO_RegAddType;

/***********************************************************************************
**********                      Declare Globals                             ********
***********************************************************************************/
/*a static variable to hold the state if the GPIO groups
        1 => initialized
        0 => not initialized
*/
static uint8_t GPIO_GroupState[GPIO_NUMBER_OF_GROUPS] = {0};

//array of the base addresses of the GPIO ports to acess them by an index
static const uint32_t GPIOx_BaseAddress[NUMBER_OF_PORTS] = 
{
        GPIOA_BASE_ADD,
        GPIOB_BASE_ADD,
        GPIOC_BASE_ADD,
        GPIOD_BASE_ADD,
        GPIOE_BASE_ADD,
        GPIOF_BASE_ADD,
        GPIOG_BASE_ADD,
        GPIOH_BASE_ADD,
        GPIOI_BASE_ADD
};
/***********************************************************************************
**********              Macro like function to control the GPIO             ********
***********************************************************************************/
 
//macro like function to generate the addres of the used register in the gpio port
#define GPIO_REG_ADD(PORT_ID,REG_OFFSET) (GPIOx_BaseAddress[PORT_ID] + REG_OFFSET)

//------------------------------------------------------------------------------

//macro like function to set the mode of the pin
#define MODE_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_MODER_OFFSET))

//macro like function to set the output type of the pin
#define OTYPE_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_OTYPER_OFFSET))

//macro like function to set the output speed of the pin
#define OSPEED_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_OSPEEDR_OFFSET))

//macro like function to set the pullup/pulldown of the pin
#define PUPD_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_PUPDR_OFFSET))

//macro like function to set the AFL of the pin
#define AFRL_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_AFRL_OFFSET))

//macro like function to set the AFH of the pin
#define AFRH_REG(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_AFRH_OFFSET))

//-------------------------------------------------------------------------------

//macro like function to write zero to the pin
#define GPIO_WRITE_ZERO(PORT, MASK, DATA) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_BSRR_OFFSET)) |=  (((uint32_t)DATA) << (MASK + 16U))

//macro like function to write one to the pin
#define GPIO_WRITE_ONE(PORT, MASK, DATA) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_BSRR_OFFSET)) |=  (((uint32_t)DATA) << MASK)

//macro like function to read from the pin
#define GPIO_READ(PORT, MASK) ( ( (*((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_IDR_OFFSET))) & (1U << MASK) ) >> MASK )



/***********************************************************************************
**********                      GPIO functions' bodies                      ********
***********************************************************************************/

/*
 * This function used to initialize all the GPIO Groups in the configurations
 * Inputs:NONE
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_Init(void)
{        
        //declarations
        uint8_t i;//loop index
        GPIO_CheckType RetVar = GPIO_OK; // variable to indicate the success of the initialization
        const GPIO_ConfigType* ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType

        //loop on the number of GPIO pin configurations
        for(i=0; (i < GPIO_NUMBER_OF_GROUPS) && (RetVar == GPIO_OK); i++)
        {
                //check if the port number withen the allowed range
                if(GPIO_ConfigParam[i].Port < NUMBER_OF_PORTS)
                {
                        //assign a pointer to the configuration structure
                        ConfigPtr = &GPIO_ConfigParam[i];

                        //enable clock gating
                        RCC_AHB1ENR |= (1U << (ConfigPtr->Port));

                        //set the mode
                        MODE_REG(ConfigPtr->Port) |=  (((uint32_t)(ConfigPtr->Mode)) << ((ConfigPtr->Pin) << 1U));

                        //set the output type
                        OTYPE_REG(ConfigPtr->Port) |= (((uint16_t)(ConfigPtr->OutputType)) << (ConfigPtr->Pin));

                        //set the output speed
                        OSPEED_REG(ConfigPtr->Port) |= (((uint32_t)(ConfigPtr->OutputSpeed)) << ((ConfigPtr->Pin) << 1U));

                        //set the pullup/pulldown
                        PUPD_REG(ConfigPtr->Port) |= (((uint32_t)(ConfigPtr->PUPD)) << ((ConfigPtr->Pin) << 1U));

                        //change the state of this group to initialized
                        GPIO_GroupState[i] = 1U;

                        //initialization done successfully
                        RetVar = GPIO_OK;
                }

                else 
                {
                        //Invalid GroupId
                        RetVar = GPIO_NOK;
                }
                
        }

        return RetVar;
}


/*
 * This function used to Digital Write data to a specific group
 * Inputs:
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - Data                   : the data to write to that specific group
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_Write(uint8_t GroupId, uint8_t Data)
{
        //declarations
        GPIO_CheckType RetVar;// variable to indicate the success of the writing
        const GPIO_ConfigType* ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType

        //check if the group number withen the allowed range
        if (( GroupId < GPIO_NUMBER_OF_GROUPS))
        {
                //assign a pointer to the configuration structure
                ConfigPtr = &GPIO_ConfigParam[GroupId];

                //check if the group mode is output and the group is initialized
                if ((ConfigPtr->Mode == MODE_OUTPUT) && (GPIO_GroupState[GroupId] == 1U))
                {
                        //if (Data!=0)
                        //        Data = 1;

                        //write the data to the group
                        GPIO_WRITE_ZERO(ConfigPtr->Port, ConfigPtr->Pin, (Data^0x01));
                        GPIO_WRITE_ONE(ConfigPtr->Port, ConfigPtr->Pin, Data);

                        //writing done successfully
                        RetVar = GPIO_OK;
                }
                else 
                {        
                        //the group mode isn't output or the group isn't initialized
                        RetVar = GPIO_NOK; 
                }
        }
        else 
        {
                //the group number isn't withen the allowed range
                RetVar = GPIO_NOK;
        }

        return RetVar;
}


/*
 * This function used to Digital read data from a specific group
 * Inputs:
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - DataPtr                   : a pointer to a variable to store the data read from that specific group
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_Read(uint8_t GroupId, uint8_t* DataPtr)
{
        //declarations
        GPIO_CheckType RetVar;// variable to indicate the success of the reading
        const GPIO_ConfigType* ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType

        //check if the group number withen the allowed range
        if(( GroupId < GPIO_NUMBER_OF_GROUPS))
        {
                //assign a pointer to the configuration structure
                ConfigPtr = &GPIO_ConfigParam[GroupId];

                //check if the group mode is input and the group is initialized
                if( (ConfigPtr->Mode == MODE_INPUT) && (GPIO_GroupState[GroupId] == 1) )
                {
                        //read the data from the group
                        *DataPtr = GPIO_READ(ConfigPtr->Port, ConfigPtr->Pin); 
                        
                        //reading done successfully
                        RetVar = GPIO_OK;
                }
                else 
                {
                        //the group mode isn't input or the group isn't initialized
                        RetVar = GPIO_NOK; 
                }

        }
        else 
        {
                //the group number isn't withen the allowed range
                RetVar = GPIO_NOK; 
        }

        return RetVar;
}



/*
 * This function used to select which peripheral will be connected to a GPIO pin
 * Inputs:
         - GroupId                 : the index of the structure in the GPIO_ConfogParam array
         - AlternFuncId   : the ID of the alternative function
 * Output:
         - an indication of the success of the function
*/
GPIO_CheckType GPIO_SetAlternFuntion(uint8_t GroupId, uint8_t AlternFuncId)
{
        //declarations
        GPIO_CheckType RetVar = GPIO_OK;// variable to indicate the success of the reading
        const GPIO_ConfigType* ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType

        //check if the group number withen the allowed range
        if(( GroupId < GPIO_NUMBER_OF_GROUPS))
        {
                //assign a pointer to the configuration structure
                ConfigPtr = &GPIO_ConfigParam[GroupId];

                //check if the group mode is altirnative function and the group is initialized
                if( (ConfigPtr->Mode == MODE_ALTERNATIVE) && (GPIO_GroupState[GroupId] == 1U) )
                {
                        if ((ConfigPtr->Pin) <= 7U)
                        {
                                //set the altirnative function low registers with the upsampled AFL mask
                                AFRL_REG(ConfigPtr->Port) |= (((uint32_t)AlternFuncId) << ((ConfigPtr->Pin) << 2U));
                        }
                        else
                        {
                                //set the altirnative function high registers with the upsampled AFH mask
                                AFRH_REG(ConfigPtr->Port) |= (((uint32_t)AlternFuncId) << (((ConfigPtr->Pin)-8) << 2U));
                        }
                        
                        //setting alternative function done successfully
                        RetVar = GPIO_OK;
                }
                else 
                {
                        //the group mode isn't alternative function or the group isn't initialized
                        RetVar = GPIO_NOK; 
                }
        }
        else 
        {
                //the group number isn't withen the allowed range
                RetVar = GPIO_NOK; 
        }

        return RetVar;
}
