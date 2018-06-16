/**************************************************************************************
*        File name: TIMER.c
*        Author: Moamen Ali & Moamen Ramadan
*        Date: 1/5/2018
*        Description:
*                        This file contains:
*                        - Base Addresses of The TIMER peripherals
*                        - Implementation of functions used to access TIMER registers.
*        Microcontroller: STM32F407VG
***************************************************************************************/
#include <stdint.h>
#include "GPIO.h"
#include "STM32F4_MemMap.h"
#include "TIMER_Cfg.h"
#include "TIMER.h"

typedef volatile uint32_t* const TIMER_RegAddType; // typedef for the TIMER register addreses which are pointer to 32 bits

/*****************************************************************
******             TIMER Periferals on APB1 bus             ******
*****************************************************************/
#define APB1_BASE                 	         0x40000000U
#define TIMER_2_BASE                        (APB1_BASE + 0x0000)
#define TIMER_3_BASE                        (APB1_BASE + 0x0400)
#define TIMER_4_BASE                        (APB1_BASE + 0x0800)
#define TIMER_5_BASE                        (APB1_BASE + 0x0C00)

// STM32F407VG Support 4 TIMERS 
#define TIMERS_MAX_NUM 4U

static const uint32_t TIMERsBaseAddressLut[TIMERS_MAX_NUM] =
{
    TIMER_2_BASE,
    TIMER_3_BASE,
    TIMER_4_BASE,
    TIMER_5_BASE
};

/*------------------------------------------------------------
******             TIMERs Registers Offset               ******
------------------------------------------------------------*/
#define TIMx_CR1_OFFSET				0x00 
#define TIMx_SR_OFFSET 				0x10
#define TIMx_EGR_OFFSET				0x14
#define TIMx_CCMRx_OFFSET			0x18
#define TIMx_CCER_OFFSET			0x20
#define TIMx_CNT_OFFSET				0x24
#define TIMx_PSC_OFFSET				0x28
#define TIMx_ARR_OFFSET				0x2C
#define TIMx_CCRx_OFFSET 			0x34

#define TIMER_ID 0

// check the number of used TIMER Channels by user and display warnning during compilation-
// if Number of used Channels greater than the available Channels
#if TTIMER_CHANNEL_NUM > (TIMERS_MAX_NUM << 2)
#warning "Invalid Channel Number"
#endif // TIMERS_CHANNELS_NUM

/*-----------------------------------------------------------------------------------------------------
************                            Accessing the TIMERs Registers                      ************
------------------------------------------------------------------------------------------------------*/
// calculate the address of each register and cast it to the Typedef data type (TIMER_RegAddType)
// then dereference that address to be ready to write in the required register.
// TIMER_ID is the index of the required TIMER periferal in TIMERsBaseAddressLut -
// to get the base address of the required Peripheral. 
#define TIMx_CR1(TIMER_ID)    	     			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_CR1_OFFSET))
#define TIMx_SR(TIMER_ID)       	 			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_SR_OFFSET))
#define TIMx_EGR(TIMER_ID)     		 			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_EGR_OFFSET))
#define TIMx_CCMRx(TIMER_ID,CHANNELID)     		 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_CCMRx_OFFSET + CHANNELID*0x04))
#define TIMx_CCER(TIMER_ID)          			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_CCER_OFFSET))
#define TIMx_CNT(TIMER_ID)        	 			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_CNT_OFFSET))
#define TIMx_PSC(TIMER_ID)        	 			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_PSC_OFFSET))
#define TIMx_ARR(TIMER_ID)        	 			 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_ARR_OFFSET))
#define TIMx_CCRx(TIMER_ID,CHANNELID)        	 *((TIMER_RegAddType)(TIMERsBaseAddressLut[TIMER_ID] + TIMx_CCRx_OFFSET + CHANNELID*0x04))

/********************			BIT OPERATIONS			************************/
#define GET_BIT(REG,BIT_NUM)				((REG >> BIT_NUM) & 1)
#define SET_BIT(REG,BIT_NUM)				(REG |= 1<<(BIT_NUM))
#define CLEAR_BIT(REG,BIT_NUM)				(REG &= ~(1<<(BIT_NUM)))

/********************		LOW NIBBLE OPERATIONS			************************/
#define GET_LOWNIBBLE(REG)					(REG & 0x00FFU)
#define SET_LOWNIBBLE(REG)					(REG |= 0x00FFU)
#define CLEAR_LOWNIBBLE(REG)				(REG &= ~(0x00FFU))


// calculate the period coressponding to required freq
#define FREQUENCY 	0U 
#define TIMER_FREQUENCY	16000000
#define PERIOD(FREQUENCY)	((TIMER_FREQUENCY/FREQUENCY)-1)


//calculate Value corresponding to DutyCycle
#define	DUTYCYCLE 	0U
#define	PERIODTIME 		0U
#define PULSE_LENGTH(PERIODTIME,DUTYCYCLE)	((((PERIODTIME+1)*DUTYCYCLE)/100)-1)

/*-------------------------------------
****        TIMERs States           ****
-------------------------------------*/
#define TIMER_STATE_UNINIT        0x00
#define TIMER_STATE_INIT          0x01
#define TIMER_STATE_INPROGRESS	  0x02
/*----------------------------------*/

// Array to the States of All Channels
static uint8_t ChannelsState[TIMER_CHANNEL_NUM] = {TIMER_STATE_UNINIT};
// Array to store the Starting dutycycle of each channel that determined by USER in TIMER_Cfg.c
static uint32_t Duty[TIMER_CHANNEL_NUM];
// Array to store the state of counting of each channel either upcounting till 100% or downcount till 0
static uint8_t Up[TIMER_CHANNEL_NUM];

// Array to hold the alternative function value of each TIMER module
static const uint32_t TIMERChannelsAlternFun[TIMERS_MAX_NUM]
= {
   AF_TIM2, // Alternative function of TIMER2 is 0x01 
   AF_TIM3, // Alternative function of TIMER3 is 0x02 
   AF_TIM4, // Alternative function of TIMER4 is 0x02
   AF_TIM5  // Alternative function of TIMER5 is 0x02
  };

/*A function to initialize all the TIMER channels with the configuration set in TIMER_Cfg*/
TIMER_ChkType TIMER_Init (void)
{
	TIMER_ChkType RetVar; // variable for the return value
	const TIMER_ConfigType* CfgPtr; // pointer to TIMER_ConfigType to save the addres of the configuration structure 
    uint8_t LoopIndex;
    // loop over all defined TIMER Channels in TIMEr_ConfigParam array
    for (LoopIndex = 0; LoopIndex < TIMER_CHANNEL_NUM; LoopIndex++)
    {
    	CfgPtr = &TIMER_ConfigParam[LoopIndex];
    	// check the validity of configuration parameters
    	if ( ((CfgPtr->PWMFrequency) <= 42000000) &&
    		 ((CfgPtr->PWMFrequency) >= 630)	  &&
    		 ((CfgPtr->DutyCycle) <= 100)		  &&
    		 ((CfgPtr->Step) < 100))
    	{
    		// Enable Clock gating of the module used
    		SET_BIT(RCC_APB1ENR,(CfgPtr->TimerId));

    		// set the GPIO alternative function
    		GPIO_SetAlternFuntion(CfgPtr->GPIOChannelId,TIMERChannelsAlternFun[CfgPtr->TimerId]);

    		// Set the mode of timer to PWM and set the channel to be output
            // CCxS Bits = 00 -> Channel Configured as output
            // OCxPE Bit  = 1 -> Output compare preload enable
            // OCxM Bits == 110 -> PWM mode 1
    		TIMx_CCMRx(CfgPtr->TimerId,(((CfgPtr->TimerChannel)-1)/2)) |= (0x68 << ((((CfgPtr->TimerChannel)-1)&0x01)<<3));

    		// Enable the ARPE bit
    		CLEAR_LOWNIBBLE(TIMx_CR1(CfgPtr->TimerId));
    		SET_BIT(TIMx_CR1(CfgPtr->TimerId),ARPE);

    		// set the prescaller to work with the max of the bus frequency
    		TIMx_PSC(CfgPtr->TimerId) = 0;

    		//calculate the period corresponding to the required frequency and set the ARR register with the calculated period
    		TIMx_ARR(CfgPtr->TimerId) = PERIOD(CfgPtr->PWMFrequency);

    		// calculate the required value corresponding to the duty cycle and set the CCRx register with the calculated value
            Duty[LoopIndex] = CfgPtr->DutyCycle;
    		TIMx_CCRx(CfgPtr->TimerId,((CfgPtr->TimerChannel)-1)) = PULSE_LENGTH(PERIOD(CfgPtr->PWMFrequency),0);

    		// reset the counter register
    		TIMx_CNT(CfgPtr->TimerId) = 0;

    		// generate an update event to move the initialiations to the access register
    		SET_BIT(TIMx_EGR(CfgPtr->TimerId),UG);
    		TIMx_PSC(CfgPtr->TimerId) = 0;

    		// enable the counter
    		SET_BIT(TIMx_CR1(CfgPtr->TimerId),CEN);
				
				// Enable the output pin mode
        	SET_BIT(TIMx_CCER(CfgPtr->TimerId),(((CfgPtr->TimerChannel)-1)<<2));
            // Change the state of the Channel to be TIMER_STATE_INIT
    		ChannelsState[LoopIndex] = TIMER_STATE_INIT;

    		// return TIMER_OK
    		RetVar = TIMER_OK;
    	}
    	else
        // if the configuration is invalid
    	{
            // return TIMER_NOK
    		RetVar = TIMER_NOK;
    	}

    }
	return RetVar;
}


/* A function that is called to enable the output pin and each time called increases the DutyCycle 
   by the determined Step till reach 100% and then decreased by the sam step till reach 0 (Used to Dim the led) */
/*
 Inputs:
    - ChannelId : index of the used Timer in TIMER_ConfigParam array
*/
TIMER_ChkType PWM_Start (uint32_t ChannelId)
{
    TIMER_ChkType RetVar;
    const TIMER_ConfigType* CfgPtr;
    /*Check the validity of channel ID*/
    if (ChannelId < TIMER_CHANNEL_NUM)
    {
        CfgPtr = &TIMER_ConfigParam[ChannelId];
        // check teh Channel State
        // if state is Init which means that it is the first time to start the PWM
        if (ChannelsState[ChannelId] == TIMER_STATE_INIT)
        {
        	
            // change the state to be INPROGRESS to indicate that this is not the first time to call the function
        	ChannelsState[ChannelId] = TIMER_STATE_INPROGRESS;
        }
        // if state is INPROGRESS which means that the function called befor
        else
        {
            // check the state of the counting either UP or Down
       		if (Up[ChannelId] == 0)
        	{
				// increase the DutyCycle by the specified step
	          	Duty[ChannelId] +=CfgPtr->Step;
    	      	TIMx_CCRx(CfgPtr->TimerId,((CfgPtr->TimerChannel)-1)) = PULSE_LENGTH(PERIOD(CfgPtr->PWMFrequency),Duty[ChannelId]); 
				if (Duty[ChannelId] >= 95)
				{
					Up[ChannelId] = 1;
				}
    	    }
        	else
	        {
				// decrese the dutycycle
        	  	Duty[ChannelId] -=CfgPtr->Step;
	        	TIMx_CCRx(CfgPtr->TimerId,((CfgPtr->TimerChannel)-1)) = PULSE_LENGTH(PERIOD(CfgPtr->PWMFrequency),Duty[ChannelId]);
				if (Duty[ChannelId] < 5)
				{
					Up[ChannelId] = 0;
				}
        	}
        }
 	    RetVar = TIMER_OK;
    }
    else
    {
        RetVar = TIMER_NOK;
    }
    return RetVar;
}


/* A function that is called to fix the Dutycycle to zero
   Inputs:
   	  - ChannelId : index of the used Timer in Timer_ConfigParam
*/
TIMER_ChkType PWM_Clear (uint32_t ChannelId)
{
    TIMER_ChkType RetVar;
    const TIMER_ConfigType* CfgPtr;
    /*Check the validity of channel ID*/
    if (ChannelId < TIMER_CHANNEL_NUM)
    {
        CfgPtr = &TIMER_ConfigParam[ChannelId];
        // check the Channel State
        if (ChannelsState[ChannelId] == TIMER_STATE_INPROGRESS)
        {
            TIMx_CCRx(CfgPtr->TimerId,((CfgPtr->TimerChannel)-1)) = PULSE_LENGTH(PERIOD(CfgPtr->PWMFrequency),0);
            ChannelsState[ChannelId] = TIMER_STATE_INIT;
        }
        else{/*Error Cleared so fix the duty cycle to zero*/}
        RetVar = TIMER_OK;
    }
    else
    {
        /*error channel used*/
        RetVar = TIMER_NOK;
    }
    return RetVar;
}
