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
*		- If the user requests new reception whiule the old one is not finished the function return BUSY
*		- GPS_ManageOngoingOperation function should be called periodically till-
*		  it execute the Found call back function defined-
*		  in GPS_ConfigType structure.if the data of GPS is invalid data this function-
*		  return execute the error call back function defined in GPS_ConfigType structure
*		- User should define a variable from type location in which the GPS_Location will be stored when-
*		  Passing its address to the function GetData after GPS_ManageOngoingOperation function return OK.
*		- GPS_ReceptionCallBack function is the RxCallBack Function that should be defined-
*		  in the UART configuration parameter
******************************************************************************************************/

#include "GPS.h"
#include "UART.h"
#include "Parse.h"
#include "GPS_Cfg.h"
#include "GPIO.h"


// The required delay in the initialization sequence
#define REQUIREDDELAY		(100/CyclicTime)
// extra delay at the first reading 
#define MAXRECEPTIONDELAY	(2000/CyclicTime)
// GPS Module Update its reading each 3 second
#define UPDATETIME			(3000/CyclicTime)

/*************************************************************************
**********			Global Variables definetion					**********
*************************************************************************/
/* the configuration Parameter defined by used*/
extern const GPS_Config GPS_Parameter;

/*The GPS State*/
static GPS_States State;

/*Flag to start reception from the GPS module*/
// if one ==> Start Recepton from GPS
static uint8_t StartReceptionFlag;

/*Flag to indicate that reception from GPS finished*/
// if zero ==> Receptions did not finish
// if one ==> Reception finished
static uint8_t ReceptionDoneFlag;

// Structure in which we store the location that we read from GPS
static Location ParsedData;

// cunter to count the number of Managing function calls to handle delay needed in the initialization sequence
static uint16_t Counter;

// Buffer in which we store the data we read from GPS
static uint8_t GPS_Data[768];

// Flag to check the first valid data (to overcome the invalid period in the start)
static uint8_t StartFlag;
/*-----------------------------------------------------------------------------------------------------------*/

/*************************************************************************
**********				Functions Implementation				**********
*************************************************************************/
/*
 * This function is used to initialize the global flags and states
 * the initial state is that GPS is Unitialized
*/
void GPS_Init(void)
{
	uint8_t LoopIndex;
	// initialize State to be GPS_UNINITIALIED
	State = GPS_UNINIT;
	// initialize Start reception flag to 0
	StartReceptionFlag = 0;
	// initialize Reception flag
	ReceptionDoneFlag = 0;
	// initialze Parsed data structure
	ParsedData.LatitudeDir = 0;
	ParsedData.LongitudeDir = 0;
	for (LoopIndex = 0; LoopIndex < 9; LoopIndex++)
	{
		ParsedData.Longitude[LoopIndex] = 0;
		ParsedData.Latitude[LoopIndex] = 0;
	}
	// initialize counter
	Counter = 0;
	StartFlag = 0;
}

/*
 * This function is used to set flag which indicates that user need to start reception form GPS
 * The function return busy if there is a request to start new reception while an old reception has not finished
*/
GPS_CheckType StartRead(void)
{
	GPS_CheckType RetVar;
	// check if there is reception inprogress or no
	if (StartReceptionFlag == 0)
	{
		// set the start reception flag
		StartReceptionFlag = 1;
		// return ok
		RetVar = GPS_OK;
	}
	else
	{
		// return busy
		RetVar = GPS_BUSY;
	}
	return RetVar;
}

/*
 * This function is used to get the readings of the GPS (Longitude and Latitude)
 * Inputs:
 *	- Map: Pointer to Location structure in which the function return tha readings of GPS
*/
void GetData(Location* Map)
{
	uint8_t LoopIndex = 0;
	for (LoopIndex = 0; LoopIndex < 9; LoopIndex++)
	{
		// Fill the longitude array
		Map->Longitude[LoopIndex] = ParsedData.Longitude[LoopIndex];
		// Fill te Latitude array
		Map->Latitude[LoopIndex] = ParsedData.Latitude[LoopIndex];
	}
	Map->LongitudeDir = ParsedData.LongitudeDir;
	Map->LatitudeDir = ParsedData.LatitudeDir;
}

/*
 * This function is used to manage the operation of the GPS in different states
 * The function calls FoundCallBack function defined in configuration file 
 * in case of GPS data is valid and we get the Location and calls ErrorCallBack function 
 * in case of GPS data is invalid.
*/
void GPS_ManagOnGoingOperation(void)
{
	const GPS_Config* GPS_Ptr;
	Parse_CheckType ParseRet;
	Parse_Cfg ParseParameter = {0,0,0,0,0};
	uint8_t ParseData[6] = "GNRMC";
	uint8_t StartIndex = 0, EndIndex = 0;
	uint8_t LoopIndex;

	// flags used to check if Longitude and latitude found or not
	uint8_t LongitudeFlag = 0, LatitudeFlag = 0;

	GPS_Ptr = &GPS_Parameter;

	// check the State of the GPS
	switch (State)
	{
		// if Uninit
		case GPS_UNINIT:
		{
			if (Counter == 0)
			{
				// write '1' on force pin
				GPIO_Write((GPS_Ptr->Force_ChannelId),(GPS_Ptr->ForcePinMask),HIGH);
				// write '0' on RST pin
				GPIO_Write((GPS_Ptr->RST_ChannelId),(GPS_Ptr->RstPinMask),LOW);
			}
			else
			{
				if (Counter >= (REQUIREDDELAY))
				{
					// write '1' on RST pin
					GPIO_Write((GPS_Ptr->RST_ChannelId),(GPS_Ptr->RstPinMask),HIGH);
					// wait an additional delay to start reception from GPS
					if (Counter == (REQUIREDDELAY + MAXRECEPTIONDELAY))
					{
						Counter = 0;
						// change the state to be GPS_IDLE
						State = GPS_IDLE;
					}
				}
				else{;/*Finish the required initialization sequence*/}
			}
			// increment counter till reach RequiredDelay
			Counter++;
		}
		break;
		// if IDLE
		case GPS_IDLE:
		{
			// check StartReceptionFlag
			// if '1'
			if (StartReceptionFlag == 1)
			{
				// UART start reception
				UART_StartSilentReception(GPS_Data,768,(GPS_Ptr->UART_ChannelId));
				// change the state to be GPS_WAIT
				State = GPS_WAIT;
			}
			else{;/*Stay in IDLE state as the user did not require a reception from GPS module*/}
		}
		break;
		// if Wait
		case GPS_WAIT:
		{
			// Check if the GPS module does not receive data for the defined update time,
			// in case of not receiving change state to be error
			if (Counter <= UPDATETIME)
			{
				// check the ReceptionDoneFlag which indicates that reception of 768 finished
				// if '1'
				if (ReceptionDoneFlag == 1)
				{
					Counter = 0;
					ReceptionDoneFlag = 0;
					// change the state to be GPS_PARSE
					State = GPS_PARSE;
				}
				else{;/*Stay in wait state as the GPS module still receives data*/}
			}
			else
			{
				// Stop the current reception
				UART_StopCrntReception(GPS_Ptr->UART_ChannelId);
				// clear the counter
				Counter = 0;
				// Change State to be GPS_ERROR
				State = GPS_ERROR;
			}
			Counter++;
		}
		break;
		// if Parse
		case GPS_PARSE:
		{
			// Search for "GNRMC" in the data we read fom GPS
			ParseParameter.BufferPtr = GPS_Data;
			ParseParameter.BufferLength = 768;
			ParseParameter.DataPtr = ParseData;
			ParseParameter.DataLength = 5;
			ParseRet = Parse(&ParseParameter);
			// if found 
			if (ParseRet == PARSE_OK)
			{
				// search for Data validity ",A,"
				ParseParameter.BufferPtr = GPS_Data + (ParseParameter.Index) + 5; // change the start of buffer to be searched after GNRMC
				// change data we need to search about it
				ParseData[0] = ',';
				ParseData[1] = 'A';
				ParseData[2] = ',';
				ParseParameter.DataPtr = ParseData;
				ParseParameter.DataLength = 3;
				ParseRet = Parse(&ParseParameter);
				// if found 
				if (ParseRet == PARSE_OK)
				{
					// mark the Longitude start index
					StartIndex = ParseParameter.Index;
					//search for Longitude direction ",N,"
					ParseData[0] = ',';
					ParseData[1] = 'N';
					ParseData[2] = ',';
					ParseParameter.DataPtr = ParseData;
					ParseParameter.DataLength = 3;
					ParseRet = Parse(&ParseParameter);
					// if found 
					if (ParseRet == PARSE_OK)
					{
						// mark the end index of Longitude
						EndIndex = ParseParameter.Index;
						// fill data on the parseddata global variable
						for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
						{
							ParsedData.Longitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
						}
						ParsedData.LongitudeDir = ParseParameter.BufferPtr[EndIndex+1];
						// Start of Latitude is the end of longitude
						StartIndex = EndIndex;
						// set the Longitude flag to be '1' to indicate that we get the valid longitude
						LongitudeFlag = 1;
					}
					// else
					else
					{
						//search for Longitude direction ",S,"
						ParseData[0] = ',';
						ParseData[1] = 'S';
						ParseData[2] = ',';
						ParseParameter.DataPtr = ParseData;
						ParseParameter.DataLength = 3;
						ParseRet = Parse(&ParseParameter);
						// if found 
						if (ParseRet == PARSE_OK)
						{
							// mark the end index of Longitude
							EndIndex = ParseParameter.Index;
							// fill data on the parseddata global variable
							for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
							{
								ParsedData.Longitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
							}
							ParsedData.LongitudeDir = ParseParameter.BufferPtr[EndIndex+1];
							// Start of Latitude is the end of longitude
							StartIndex = EndIndex;
							// set the Longitude flag to be '1' to indicate that we get the valid longitude
							LongitudeFlag = 1;
						}
						else
						{
							// set the Longitude flag to be '0' to indicate that we did not get the valid longitude
							LongitudeFlag = 0;
						}
					}

					//search for Longitude direction ",E,"
					ParseData[0] = ',';
					ParseData[1] = 'E';
					ParseData[2] = ',';
					ParseParameter.DataPtr = ParseData;
					ParseParameter.DataLength = 3;
					ParseRet = Parse(&ParseParameter);
					// if found 
					if (ParseRet == PARSE_OK)
					{
						// mark the end index of Latitude
						EndIndex = ParseParameter.Index;
						// fill data on the parseddata global variable
						for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
						{
							ParsedData.Latitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
						}
						ParsedData.LatitudeDir = ParseParameter.BufferPtr[EndIndex+1];
						// set the Latitude flag to be '1' to indicate that we get the valid latitude
						LatitudeFlag = 1;
					}
					// else
					else
					{
						//search for Longitude direction ",W,"
						ParseData[0] = ',';
						ParseData[1] = 'W';
						ParseData[2] = ',';
						ParseParameter.DataPtr = ParseData;
						ParseParameter.DataLength = 3;
						ParseRet = Parse(&ParseParameter);
						// if found 
						if (ParseRet == PARSE_OK)
						{
							// mark the end index of Latitude
							EndIndex = ParseParameter.Index;
							// fill data on the parseddata global variable
							for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
							{
								ParsedData.Latitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
							}
							ParsedData.LatitudeDir = ParseParameter.BufferPtr[EndIndex+1];
							// set the Latitude flag to be '1' to indicate that we get the valid latitude
							LatitudeFlag = 1;
						}
						else
						{
							// set the Latitude flag to be '0' to indicate that we did not get the valid latitude
							LatitudeFlag = 0;
						}	
					}

					// check if we get the valid longitude and latitude or not
					if (LongitudeFlag && LatitudeFlag)
					{
						// Clear StartReceptionFlag to indicate that reception finished and module ready to receive ne request
						StartReceptionFlag = 0;
						// Set the StartFlag
						StartFlag = 1;
						// execute call back function
						GPS_Ptr->FoundCallBack();
						// change the state to be IDLE
						State = GPS_IDLE;
					}
					// if the data is invalid but module was read a valid data before
					else if (StartFlag == 1)
					{
						// change the state to be error
						State = GPS_ERROR;
					}
					// if the module does not read any valid data
					// this case as the module read invalid data form some time after reseting
					// so we do not need to report that as an error 
					else
					{
						// Change the state to be IDLE to receive new frames
						State = GPS_IDLE;
					}

				}
				// else
				else
				{
					// if the data is invalid but module was read a valid data before
					if (StartFlag == 1)
					{
						// change the state to be error
						State = GPS_ERROR;
					}
					// if the module does not read any valid data
					// this case as the module read invalid data form some time after reseting
					// so we do not need to report that as an error 
					else
					{
						// Change the state to be IDLE to receive new frames
						State = GPS_IDLE;
					}
				}
			}
			//else
			else
			{
				// if the data is invalid but module was read a valid data before
				if (StartFlag == 1)
				{
					// change the state to be error
					State = GPS_ERROR;
				}
				// if the module does not read any valid data
				// this case as the module read invalid data form some time after reseting
				// so we do not need to report that as an error 
				else
				{
					// Change the state to be IDLE to receive new frames
					State = GPS_IDLE;
				}
			}
		}
		break;
		// if Error
		case GPS_ERROR:
		{
			// execute the error call back function
			GPS_Ptr->ErrorCallBack(ERROR_ID);
			// change the state to be IDLE
			State = GPS_IDLE;
		}
	}
}

/*
 * The CallBack function That is executed at the end of receiving the data from GPS module
*/
void GPS_ReceptionCallBack(void)
{
	ReceptionDoneFlag = 1;
}
/*----------------------------------------------------------------------*/