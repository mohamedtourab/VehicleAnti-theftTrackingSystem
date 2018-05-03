/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 19/3/2018
 *	Microcontroller: STM32F407VG
 *
*/




#include "ADC_Driver.h"

typedef volatile uint32_t* const ADC_RegisterAddressType;

#define PERIPH_BASE              		0x40000000U 
#define RCC_BASE                 		(0x40023800U)
#define APB2PERIPH_BASE          		(PERIPH_BASE + 0x00010000U)
#define ADC_BASE			     		(APB2PERIPH_BASE+0x2000U)
		
		
#define ADC_SR_OFFSET 	  		 		(0x00)
#define ADC_CR1_OFFSET 	   		 		(0x04)
#define ADC_CR2_OFFSET 	   		 		(0x08)
#define ADC_SMPR1_OFFSET   		 		(0x0C)
#define ADC_SMPR2_OFFSET   		 		(0x10)
#define ADC_JOFR1_OFFSET   		 		(0x14)
#define ADC_JOFR2_OFFSET   		 		(0x18)
#define ADC_JOFR3_OFFSET   		 		(0x1C)
#define ADC_JOFR4_OFFSET   		 		(0x20)
#define ADC_HTR_OFFSET	       	 		(0x24)
#define ADC_LTR_OFFSET	         		(0x28)
#define ADC_SQR1_OFFSET 		 		(0x2C)
#define ADC_SQR2_OFFSET 		 		(0x30)
#define ADC_SQR3_OFFSET 		 		(0x34)
#define ADC_JSQR_OFFSET 		 		(0x38)
#define ADC_JDR1_OFFSET 		 		(0x3C)
#define ADC_JDR2_OFFSET 		 		(0x40)
#define ADC_JDR3_OFFSET 		 		(0x44)
#define ADC_JDR4_OFFSET 		 		(0x48)
#define ADC_DR_OFFSET			 		(0x4C)
		
#define ADC_MAXIMUM_NUMBER       		(3U)

#define ADC_RCC_APB2EN                  *((ADC_RegisterAddressType)	(RCC_BASE + RCC_APB2ENR_OFFSET))
#define ADC_SR(PERIPHERAL_ID)           *((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SR_OFFSET))
#define ADC_CR1(PERIPHERAL_ID)          *((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_CR1_OFFSET))
#define ADC_CR2(PERIPHERAL_ID)          *((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_CR2_OFFSET))
#define ADC_SMPR1(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SMPR1_OFFSET))				
#define ADC_SMPR2(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SMPR2_OFFSET))
#define ADC_JOFR1(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JOFR1_OFFSET))
#define ADC_JOFR2(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JOFR2_OFFSET))
#define ADC_JOFR3(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JOFR3_OFFSET))
#define ADC_JOFR4(PERIPHERAL_ID)		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JOFR4_OFFSET))
#define ADC_HTR(PERIPHERAL_ID)			*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_HTR_OFFSET))
#define ADC_LTR(PERIPHERAL_ID)  		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_LTR_OFFSET))
#define ADC_SQR1(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SQR1_OFFSET))
#define ADC_SQR2(PERIPHERAL_ID)			*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SQR2_OFFSET)) 
#define ADC_SQR3(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_SQR3_OFFSET))
#define ADC_JSQR(PERIPHERAL_ID)			*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JSQR_OFFSET)) 
#define ADC_JDR1(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JDR1_OFFSET))
#define ADC_JDR2(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JDR2_OFFSET))
#define ADC_JDR3(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JDR3_OFFSET))
#define ADC_JDR4(PERIPHERAL_ID) 		*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_JDR4_OFFSET))
#define ADC_DR(PERIPHERAL_ID)			*((ADC_RegisterAddressType) (ADC_BASE + (0x100*(PERIPHERAL_ID-1U)) + ADC_DR_OFFSET))  



#define ADC_SR_AWD_POS          	    (0U)                                         
#define ADC_SR_EOC_POS          	    (1U)                                          
#define ADC_SR_STRT_POS         	    (4U)                                         
#define ADC_SR_OVR_POS          	    (5U)                                         
#define ADC_CR1_EOCIE_POS       	    (5U)                                         
#define ADC_CR1_SCAN_POS        	    (8U)                                         
#define ADC_CR1_DISCEN_POS      	    (11U)                                        
#define ADC_CR1_RES_POS         	    (24U)                                        
#define ADC_CR1_OVRIE_POS       	    (26U)                                        
#define ADC_CR2_ADON_POS        	    (0U)                                         
#define ADC_CR2_CONT_POS        	    (1U)                                         
#define ADC_CR2_EOCS_POS        	    (10U)                                        
#define ADC_CR2_ALIGN_POS       	    (11U)                                        
#define ADC_CR2_SWSTART_POS     	    (30U)                                        
#define ADC_SMPR1_SMP10_POS     	    (0U)                                         
#define ADC_SMPR1_SMP11_POS     	    (3U)                                         
#define ADC_SMPR1_SMP12_POS     	    (6U)                                         
#define ADC_SMPR1_SMP13_POS     	    (9U)                                         
#define ADC_SMPR1_SMP14_POS     	    (12U)                                        
#define ADC_SMPR1_SMP15_POS     	    (15U)                                        
#define ADC_SMPR1_SMP16_POS     	    (18U)                                        
#define ADC_SMPR1_SMP17_POS     	    (21U)                                        
#define ADC_SMPR1_SMP18_POS     	    (24U)                                        
#define ADC_SMPR2_SMP0_POS      	    (0U)                                         
#define ADC_SMPR2_SMP1_POS      	    (3U)                                         
#define ADC_SMPR2_SMP2_POS      	    (6U)                                         
#define ADC_SMPR2_SMP3_POS      	    (9U)                                         
#define ADC_SMPR2_SMP4_POS      	    (12U)                                         
#define ADC_SMPR2_SMP5_POS      	    (15U)                                         
#define ADC_SMPR2_SMP6_POS      	    (18U)                                         
#define ADC_SMPR2_SMP7_POS      	    (21U)                                         
#define ADC_SMPR2_SMP8_POS      	    (24U)                                         
#define ADC_SMPR2_SMP9_POS      	    (27U)   
#define RCC_APB2ENR_ADCEN_POS(PERIPHERAL_ID)         (8U+PERIPHERAL_ID-1) 

#if(ADC_PERIPHERAL_NUMBER>ADC_MAXIMUM_NUMBER)
#error "Invalid Peripheral Number"
#endif




ADC_CheckType ADC_Init(void)
{
	ADC_CheckType RetVal;
	const ADC_ConfigType* ConfigPtr;
	ConfigPtr = &ADC_ConfigParam[0];

	ADC_RCC_APB2EN |= ( ((uint32_t)1)<<RCC_APB2ENR_ADCEN_POS(ConfigPtr->ADC_Peripheral_ID) );
	//I don't know how to Initialize GPIO as its not alternative function it's an Analog Mode
	ADC_CR1(ConfigPtr->ADC_Peripheral_ID) |= ((ConfigPtr->ADC_ScanMode)<<ADC_CR1_SCAN_POS);
	ADC_CR1(ConfigPtr->ADC_Peripheral_ID) |= ((ConfigPtr->ADC_Resolution)<<ADC_CR1_RES_POS);
	//the line below doesn't configured in the configuration structure(to be edited)
	ADC_CR2(ConfigPtr->ADC_Peripheral_ID) |= 1<<ADC_CR2_EOCS_POS; //The EOC bit is set at the end of each regular conversion. Overrun detection is enabled.

	ADC_CR2(ConfigPtr->ADC_Peripheral_ID) |= ((ConfigPtr->ADC_DataAlignment)<<ADC_CR2_ALIGN_POS);

	
	
	
	


}