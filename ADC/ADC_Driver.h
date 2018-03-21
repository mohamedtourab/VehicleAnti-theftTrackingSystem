/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 19/3/2018
 *	Microcontroller: STM32F407VG
 *
*/








#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include "GPIO.h"
#include <stdint.h>

typedef enum
{
	ADC_OK,
	ADC_NOK
	
}ADC_CheckType;


typedef enum
{
    RESOLUTION_12=0,
    RESOLUTION_10,
    RESOLUTION_8,
    RESOLUTION_6
    
}Resolution;

typedef enum
{
    DISABLE_DISCONTINUOUS_MODE,
    ENABLE_DISCONTINUOUS_MODE
}DiscontinuousMode;

typedef enum
{
    DISABLE_SCAN_MODE=0,
    ENABLE_SCAN_MODE
}ScanMode;


typedef enum
{
    RIGHT_DATA_ALIGNMENT,
    LEFT_DATA_ALIGNMENT
}DataAlignment;

typedef enum
{
    SINGLE_MODE,
    CONTINUOUS_MODE
}ContinuousConversion;

typedef enum
{
    PRESCALER_2,
    PRESCALER_4,
    PRESCALER_6,
    PRESCALER_8
}Prescaler;


typedef struct
{
    uint8_t                    ADC_Peripheral_ID;
    Resolution                 ADC_Resolution;
    DiscontinuousMode          ADC_DiscontinuousMode;
    ScanMode                   ADC_ScanMode;
    DataAlignment              ADC_DataAlignment;
    ContinuousConversion       ADC_ContinuousConversion;
    Prescaler                  ADC_Prescaler;

}ADC_ConfigType;

ADC_CheckType ADC_Init(void);




#endif
