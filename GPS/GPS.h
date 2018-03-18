#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>


/* The cyclic time definetion in (ms)*/
#define CyclicTime	10U

/*****************************************************************************************
**********						Data Type Definetion							**********
*****************************************************************************************/

/*Call back function to be executed after parsing*/
typedef void (*GPS_CallBackFn)(void);

/*Structure to get the Configuration Parameters of GPS_Module*/
typedef struct 
{
	uint8_t UART_ChannelId;	// The index of the UART channel in the Configuration Parameter array defined in UART_Cfg.c
	uint8_t GPIO_ChannelId;	// The index of the GPIO pins channel in the configuration parameter defined in GPIO_Cfg.c
	uint8_t ForcePinMask;	// The pin mask used for Force pin
	uint8_t RstPinMask;		// The pin mask used for Reset pin
	GPS_CallBackFn FoundCallBack;	// CallBack function that will be executed in case of finding the data in GPS readeings
	GPS_CallBackFn ErrorCallBack;	// CallBack function that will be executed in case the data is not found
}GPS_Config;

/*Structure to store the Longitude and Latitude*/
typedef struct 
{
	uint8_t Longitude[9];
	uint8_t LongitudeDir;
	uint8_t Latitude[9];
	uint8_t LatitudeDir;
}Location;

/*enum for the return type*/
typedef enum {GPS_OK = 0, GPS_NOK = 1}GPS_CheckType;

/*enum for the GPS States*/
typedef enum {GPS_UNINIT = 0, GPS_IDLE = 1, GPS_WAIT = 2, GPS_PARSE = 3, GPS_ERROR = 4}GPS_States;
/*--------------------------------------------------------------------------------------*/

/*****************************************************************************************
**********						Functions ProtoType								**********
*****************************************************************************************/
/*
 * This function is used to initialize the global flags and states
 * the initial state is that GPS is Unitialized
*/
void GPS_Init(void);

/*
 * This function is used to set flag which indicates that user need to start reception form GPS
*/
void StartRead(void);

/*
 * This function is used to get the readings of the GPS (Longitude and Latitude)
 * Inputs:
 *	- Map: Pointer to Location structure in which the function return tha readings of GPS
*/
void GetData(Location* Map);

/*
 * This function is used to manage the operation of the GPS in different states
 * The function return:
 *	- GPS_OK: in case of GPS data is valid and we get the Location.
 *	- GPS_NOK: in case of GPS data is invalid.
*/
GPS_CheckType GPS_ManagOnGoingOperation(void);

/*
 * The CallBack function That is executed at the end of receiving the data from GPS module
*/
void GPS_ReceptionCallBack(void);
/*--------------------------------------------------------------------------------------*/

#endif
