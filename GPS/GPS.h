/*****************************************************************************************************
*	File name: GPS.h
*	Author: Moamen Ali & Moamen Ramadan
*	Date: 18/3/2018
*	Description:
*		- GPS Functions to initialize the GPS and start to get the whole GPS reading
*		  and then parse the GPS reading to get the Longitude and latitude. 
*	Usage:
*		- User define the configuration parameter in a GPS_ConfigType structure which are:
*			. the index of the used UART in the UART_Cfg.c file.
*			. The index of the used GPIO for Force in GPIO_Cfg.c file.
*			. The Pin mask of the used pin for Force in GPIO_Cfg.c file
*			. The index of the used GPIO for Reset pin on GPIO_Cfg.c file.
*			. The Pin mask of the used pin for Force in GPIO_Cfg.c file.
*			. pointer to function to be executed when finding the valid data of GPS.
*			. pointer to function to be executed when GPS data is invalid or error ocuured.
*		- User should call GPS_Init function to initialize all flags and states.
*		- User call Start Read once when he need to start read GPS data.
*		- GPS_ManageOngoingOperation function should be called periodically till-
*		  it return GPS_Ok and execute the Found call back function defined-
*		  in GPS_ConfigType structure.if the data of GPS is invalid data this function-
*		  return GPS_NOk and execute the error call back function defined in GPS_ConfigType structure
*		- User should define a variable from type location in which the GPS_Location will be stored when-
*		  Passing its address to the function GetData after GPS_ManageOngoingOperation function return OK.
*		- GPS_ReceptionCallBack function is the RxCallBack Function that should be defined-
*		  in the UART configuration parameter
******************************************************************************************************/

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

/*Callback function in case of error*/
typedef void (*GPS_ErrorCallBackFn)(uint8_t);

// enum for the return value of functions either Ok or BUSY
typedef enum {GPS_OK = 0, GPS_BUSY = 1}GPS_CheckType;

/*Structure to get the Configuration Parameters of GPS_Module*/
typedef struct 
{
	uint8_t UART_ChannelId;				// The index of the UART channel in the Configuration Parameter array defined in UART_Cfg.c
	uint8_t Force_ChannelId;			// The index of the Force pin channel in the configuration parameter defined in GPIO_Cfg.c
	uint16_t ForcePinMask;				// The pin mask used for Force pin
	uint8_t RST_ChannelId;				// The index of the Reset pin channel in the configuration parameter defined in GPIO_Cfg.c
	uint16_t RstPinMask;				// The pin mask used for Reset pin
	GPS_CallBackFn FoundCallBack;		// CallBack function that will be executed in case of finding the data in GPS readeings
	GPS_ErrorCallBackFn ErrorCallBack;	// CallBack function that will be executed in case the data is not found
}GPS_Config;

/*Structure to store the Longitude and Latitude*/
typedef struct 
{
	uint8_t Longitude[9];
	uint8_t LongitudeDir;
	uint8_t Latitude[9];
	uint8_t LatitudeDir;
}Location;

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
 * The function return busy if there is a request to start new reception while an old reception has not finished
*/
GPS_CheckType StartRead(void);

/*
 * This function is used to get the readings of the GPS (Longitude and Latitude)
 * Inputs:
 *	- Map: Pointer to Location structure in which the function return tha readings of GPS
*/
void GetData(Location* Map);

/*
 * This function is used to manage the operation of the GPS in different states
 * The function calls FoundCallBack function defined in configuration file 
 * in case of GPS data is valid and we get the Location and calls ErrorCallBack function 
 * in case of GPS data is invalid.
*/
void GPS_ManagOnGoingOperation(void);

/*
 * The CallBack function That is executed at the end of receiving the data from GPS module
*/
void GPS_ReceptionCallBack(void);
/*--------------------------------------------------------------------------------------*/

#endif
