/**************************************************************************************
*        File name: GPIO_Config.c
*Auther :edited by the user
*        Date: 18/2/2018
*        Description:
*                        This file contains:
*                        - An array of structurs that containes the croups to initialize the GPIO
*        Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "GPIO.h"
#include "GPIO_Config.h"

const GPIO_ConfigType GPIO_ConfigParam [GPIO_NUMBER_OF_GROUPS] =
{
        
        {
                //set portA as an altirnative function
        PORT_GPIOD, // the choosen port
        PIN_MASK_5, // sellect the pin number of the port (9)
        MODE_ALTERNATIVE, //set the mode to alternative function
        DEFAULT, //set the output type to the defalt value 
        DEFAULT, //set the output speed to the defalt value 
        DEFAULT  //set the pullup pulldown to the defalt value 
    },

    {
                //set portA as an altirnative function
        PORT_GPIOD, // the choosen port
        PIN_MASK_6, // sellect the pin number of the port (10)
        MODE_ALTERNATIVE, //set the mode to alternative function
        DEFAULT, //set the output type to the defalt value 
        DEFAULT, //set the output speed to the defalt value 
        DEFAULT  //set the pullup pulldown to the defalt value 
    }/*,

    {
            //set portD as an output 
        PORT_GPIOD, // the choosen port
        PIN_MASK_12,// sellect the pin number of the port (12)
        MODE_OUTPUT, //set the mode output
        OTYPE_PUSHPULL, //set the output type to pushpull
        OSPEED_LOW, //set the output speed to low speed
        PUPD_NOPUPD //set the pullup pulldown to no pulldown/pulldown
    },

    {
            //set portE as an input 
        PORT_GPIOE, // the choosen port
        PIN_MASK_12, // sellect the pin number of the port (12)
        MODE_INPUT,        //set the mode input
        DEFAULT, //set the output type to the defalt value 
        DEFAULT, //set the output speed to the defalt value 
        PUPD_PULLDOWN  //set the pullup pulldown to pulldown
    } */
   
};