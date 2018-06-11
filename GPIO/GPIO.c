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
#define NUMBER_OF_PORTS 9U 

/*************************************************************************************
**********                             GPIO Memory Map                                 **********
*************************************************************************************/

/**********************************GPIO******************************************/
#define GPIO_BASE_ADD 0x40020000U
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
#define GPIO_MODER_OFFSET 		0x00U
#define GPIO_OTYPER_OFFSET		0x04U
#define GPIO_OSPEEDR_OFFSET		0x08U
#define GPIO_PUPDR_OFFSET		0x0CU
#define GPIO_IDR_OFFSET			0x10U
#define GPIO_ODR_OFFSET			0x14U
#define GPIO_BSRR_OFFSET		0x18U
#define GPIO_LCKR_OFFSET		0x1CU
#define GPIO_AFRL_OFFSET		0x20U
#define GPIO_AFRH_OFFSET		0x24U


/***********************************************************************************
**********					Defined data types								********
***********************************************************************************/

/*define a datatype that is volatile (to privent compiler optimization)
 *and constat becouse it carries a constant address
*/
typedef volatile uint32_t* const GPIO_RegAddType;

/***********************************************************************************
**********					Declare Globals									********
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
**********				Macro like function to control the GPIO				********
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

//macro like function to write to the pin
#define GPIO_WRITE(PORT, MASK, DATA) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_ODR_OFFSET)) = (DATA) & (MASK)
#define GPIO_WRITE_ONE(PORT, MASK, DATA) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_ODR_OFFSET)) |= ((DATA) & (MASK))
#define GPIO_WRITE_ZERO(PORT, MASK, DATA) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_ODR_OFFSET)) &= ~((DATA) & (MASK))

//macro like function to read from the pin
#define GPIO_READ(PORT) *((GPIO_RegAddType)GPIO_REG_ADD(PORT, GPIO_IDR_OFFSET)) 


/***********************************************************************************
**********						Helper functions							********
***********************************************************************************/

/*
 * This function used to upsample the pin mask by 2
 Inputs:
         - Pin				: the pin mask to upsample
         - Mask				: a pointer to a variable to store the upsampled mask
 * Output:NONE
*/
static void UpSample_2Bits(uint16_t Pin, uint32_t* Mask)
{        
        //declarations
        uint8_t i; //loop index
        uint32_t Count = 1;//a variable to check the pin mask bits
        *Mask = 0; //initialize the upsampled mask
          
        //loop on the pin mask bits
        for(i=0;i<16;i++)
        {
                //check if the bit is one
                if (Pin & Count)
                {
                        /*replace that one by two ones 
                        in the corresponding place in the upsampled mask*/ 
                        *Mask |= (Count << i);
                        *Mask |= (Count << (i+1));
                } 
                else{;/*MISRA*/}
                Count = Count << 1; //shift the count to check the next bit
        }
}

/*
 * This function used to upsample the pin mask by 4 for the alternative function high and low registers
 Inputs:
         - Pin                 : the index of the structure in the GPIO_ConfogParam array
         - AFL                   : a pointer to a variable to store the upsampled AFL
         - AFH                   : a pointer to a variable to store the upsampled AFH
 * Output:NONE
*/
static void UpSample_4Bits(uint16_t Pin, uint32_t* AFL, uint32_t* AFH)
{        
        //declarations
        uint8_t i; //loop index
        uint32_t Count = 1;//a variable to check the pin mask bits
        uint8_t PinLow = 0;//a variable to carry the Low byte of the the pin mask
        uint8_t PinHigh = 0;//a variable to carry the High byte of the the pin mask

        *AFL = 0;//initialize the AFL upsampled mask 
        *AFH = 0;//initialize the AFH upsampled mask 

        PinLow = Pin & 0x00FFU;//mask the Low byte of the the pin mask
        PinHigh = (Pin >> 8);//mask the High byte of the the pin mask

//--------------------------------upsample AFL---------------------------------------

        //loop on the pin mask bits
        for(i=0; i<8; i++)
        {
                //check if the bit is one
                if (PinLow & Count)
                {
                        /*replace that one by four ones 
                        in the corresponding place in the upsampled mask*/ 
                        *AFL |= (Count<<(i*3));
                        *AFL |= (Count<<((i*3)+1));
                        *AFL |= (Count<<((i*3)+2));
                        *AFL |= (Count<<((i*3)+3));
                } 
                else{;/*MISRA*/}
                Count = Count << 1;//shift the count to check the next bit
        }

        Count = 1;//resert the Count to upsample AFH

//--------------------------------upsample AFH---------------------------------------

        //loop on the pin mask bits
        for(i=0; i<8; i++)
        {
                //check if the bit is one
                if (PinHigh & Count)
                {
                        /*replace that one by four ones 
                        in the corresponding place in the upsampled mask*/ 
                        *AFH |= (Count << (i*3));
                        *AFH |= (Count << ((i*3)+1));
                        *AFH |= (Count << ((i*3)+2));
                        *AFH |= (Count << ((i*3)+3));
                } 
                else{;/*MISRA*/}
                Count = Count << 1;//shift the count to check the next bit
        }
}



/***********************************************************************************
**********						GPIO functions' bodies						********
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
        uint32_t Mask; //variable to hold the upsampled data
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
                        RCC_AHB1ENR |= (1 << (ConfigPtr->Port));

                        //upsample the pin mask to configure 2bit accesed registers
                        UpSample_2Bits(ConfigPtr->Pin, &Mask);

                        //set the mode
                        MODE_REG(ConfigPtr->Port) |= (Mask) & (ConfigPtr->Mode);

                        //set the output type
                        OTYPE_REG(ConfigPtr->Port) |= (ConfigPtr->Pin) & (ConfigPtr->OutputType);

                        //set the output speed
                        OSPEED_REG(ConfigPtr->Port) |= (Mask) & (ConfigPtr->OutputSpeed);

                        //set the pullup/pulldown
                        PUPD_REG(ConfigPtr->Port) |= (Mask) & (ConfigPtr->PUPD);

                        //change the state of this group to initialized
                        GPIO_GroupState[i] = 1;

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
 	- GroupId		 : the index of the structure in the GPIO_ConfogParam array
 	- Data  		 : the data to write to that specific group
 	- State 		 : set if pin is HIGH or LOW or Control ALL pins(FULL_DATA)
 * Output:
 	- an indication of the success of the function
*/
GPIO_CheckType GPIO_Write(uint8_t GroupId, uint16_t Data, uint8_t State)
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
                if ((ConfigPtr->Mode == MODE_OUTPUT) && (GPIO_GroupState[GroupId] == 1))
                {
                    if(State == HIGH)
                    {
                      //write the data to the group
                        GPIO_WRITE_ONE(ConfigPtr->Port, ConfigPtr->Pin,Data);

                        //writing done successfully
                        RetVar = GPIO_OK;
  
                    }
                    else if(State == LOW)
                    {
                        //write the data to the group
                        GPIO_WRITE_ZERO(ConfigPtr->Port, ConfigPtr->Pin,Data);

                        //writing done successfully
                        RetVar = GPIO_OK;

                    }
                    else if(State == FULL_DATA)
                    {
                        //write the data to the group
                        GPIO_WRITE(ConfigPtr->Port, ConfigPtr->Pin,Data);

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
GPIO_CheckType GPIO_Read(uint8_t GroupId, uint16_t* DataPtr)
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
                        *DataPtr = GPIO_READ(ConfigPtr->Port); 
                        
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
GPIO_CheckType GPIO_SetAlternFuntion(uint8_t GroupId, uint32_t AlternFuncId)
{
        //declarations
        GPIO_CheckType RetVar = GPIO_OK;// variable to indicate the success of the reading
        const GPIO_ConfigType* ConfigPtr;//declare a pointer to structur of the GPIO_ConfigType
        uint32_t AFL; //variable to hold the upsampled data for the AFRL
        uint32_t AFH; //variable to hold the upsampled data for the AFRH

        //check if the group number withen the allowed range
        if(( GroupId < GPIO_NUMBER_OF_GROUPS))
        {
                //assign a pointer to the configuration structure
                ConfigPtr = &GPIO_ConfigParam[GroupId];

                //check if the group mode is altirnative function and the group is initialized
                if( (ConfigPtr->Mode == MODE_ALTERNATIVE) && (GPIO_GroupState[GroupId] == 1) )
                {
                        //upsample the pin mask to configure 4bit accesed altirnative function registers registers
                        UpSample_4Bits(ConfigPtr->Pin, &AFL, &AFH);

                        //set the altirnative function low registers with the upsampled AFL mask
                        AFRL_REG(ConfigPtr->Port) |= (AlternFuncId & AFL);
                        //set the altirnative function high registers with the upsampled AFH mask
                        AFRH_REG(ConfigPtr->Port) |= (AlternFuncId & AFH);

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