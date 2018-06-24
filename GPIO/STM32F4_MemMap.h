/**********************************************************************************	
*	File name: STM32F4_MemMap.h
*	Auther : 
*	Date : 18/2/2018
*	Discription :
			this file contains:
				-macros to access the memory and registers of the uc
*	Target : STM32F407VG
**********************************************************************************/

#ifndef STM32F4_MEM_MAP_H
#define STM32F4_MEM_MAP_H

#include <stdint.h>

/*define a datatype that is volatile (to privent compiler optimization)
 *and constat becouse it carries a constant address
*/
typedef volatile uint32_t* const STM32F4_PrefRegType;

/******************************clock gating registers********************************/

//base address for the reset and clock control register (in AHB1 bus)
#define RCC_BASE_ADD	0x40023800U
/***********************************RCC offsets************************************/
#define RCC_AHB1ENR_OFFSET	0x30U
#define RCC_AHB2ENR_OFFSET	0x34U	
#define RCC_AHB3ENR_OFFSET	0x38U
#define RCC_APB1ENR_OFFSET	0x40U
#define RCC_APB2ENR_OFFSET	0x44U
/***************************clock gating base addresses****************************/
//address for the AHB1 buss RCC enable 
#define RCC_AHB1ENR *((STM32F4_PrefRegType)(RCC_BASE_ADD + RCC_AHB1ENR_OFFSET))
//address for the AHB2 buss RCC enable 
#define RCC_AHB2ENR *((STM32F4_PrefRegType)(RCC_BASE_ADD + RCC_AHB2ENR_OFFSET))
//address for the AHB3 buss RCC enable 
#define RCC_AHB3ENR *((STM32F4_PrefRegType)(RCC_BASE_ADD + RCC_AHB3ENR_OFFSET))
//address for the APB1 buss RCC enable 
#define RCC_APB1ENR *((STM32F4_PrefRegType)(RCC_BASE_ADD + RCC_APB1ENR_OFFSET))
//address for the APB2 buss RCC enable 
#define RCC_APB2ENR *((STM32F4_PrefRegType)(RCC_BASE_ADD + RCC_APB2ENR_OFFSET))

/************************************************************************/


#endif