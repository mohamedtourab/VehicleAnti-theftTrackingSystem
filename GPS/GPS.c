#include "GPS.h"
#include "UART.h"
#include "GPIO.h"

#define RequiredDelay	(100/CyclicTime)

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
static uint8_t Counter;

// Buffer in which we store the data we read from GPS
static uint8_t GPS_Data[768];
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
	// initialize State to be GPS_UNINITIALIED
	State = GPS_UNINIT;
	// initialize Start reception flag to 0
	StartReceptionFlag = 0;
	// initialize Reception flag
	ReceptionDoneFlag = 0;
	// initialze Parsed data structure
	ParsedData = {{0},0,{0},0};
	// initialize counter
	Counter = 0;
}

/*
 * This function is used to set flag which indicates that user need to start reception form GPS
*/
void StartRead(void)
{
	// set the start reception flag
	StartReceptionFlag = 1;
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
 * The function return:
 *	- GPS_OK: in case of GPS data is valid and we get the Location.
 *	- GPS_NOK: in case of GPS data is invalid.
*/
GPS_CheckType GPS_ManagOnGoingOperation(void)
{
	GPS_CheckType RetVar;
	GPS_Config* GPS_Ptr;
	Parse_CheckType ParseRet;
	Parse_Cfg ParseParameter = {0,0,0,0,0};
	uint8_t ParseData[6] = "GNRMC";
	uint8_t StartIndex = 0, EndIndex = 0;

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
				GPIO_Write((GPS_Ptr->GPIO_ChannelId),(GPS_Ptr->ForcePinMask),HIGH);
				// write '0' on RST pin
				GPIO_Write((GPS_Ptr->GPIO_ChannelId),(GPS_Ptr->RstPinMask),LOW);
			}
			else
			{
				if (Counter == (RequiredDelay))
				{
					// write '1' on RST pin
					GPIO_Write((GPS_Ptr->GPIO_ChannelId),(GPS_Ptr->RstPinMask),HIGH);
					// change the state to be GPS_IDLE
					State = GPS_IDLE;
				}
				else{;/*MISRA*/}
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
			else{;/*MISRA*/}
		}
		break;
		// if Wait
		case GPS_WAIT:
		{
			// check the ReceptionDoneFlag
			// if '1'
			if (ReceptionDoneFlag == 1)
			{
				ReceptionDoneFlag = 0;
				// change the state to be GPS_PARSE
				State = GPS_PARSE;
			}
			else{;/*MISRA*/}
		}
		break;
		// if Parse
		case GPS_PARSE:
		{
			// Search for "GNRMC" in the data we read fom GPS
			ParseParameter.BufferPtr = GPS_Data;
			ParseParameter.BufferLength = 768;
			ParseParameter.DataPtr = ParsedData;
			ParseParameter.DataLength = 5;
			ParseRet = Parse(&ParseParameter)
			// if found 
			if (ParseRet == PARSE_OK)
			{
				// search for Data validity ",A,"
				ParseParameter.BufferPtr = GPS_Data + (ParseParameter.Index) + 5;
				ParsedData[0] = ',';
				ParsedData[1] = 'A';
				ParsedData[2] = ',';
				ParseParameter.DataPtr = ParsedData;
				ParseParameter.DataLength = 3;
				ParseRet = Parse(&ParseParameter)
				// if found 
				if (ParseRet == PARSE_OK)
				{
					// mark the Longitude start index
					StartIndex = ParseParameter.Index;
					//search for Longitude direction ",N," or ",S,"
					ParsedData[0] = ',';
					ParsedData[1] = 'N';
					ParsedData[2] = ',';
					ParseParameter.DataPtr = ParsedData;
					ParseParameter.DataLength = 3;
					ParseRet = Parse(&ParseParameter)
					// if found 
					if (ParseRet == PARSE_OK)
					{
						// mark the end index of Longitude
						EndIndex = ParseParameter.Index;
						for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
						{
							ParsedData.Longitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
						}
						ParsedData.LongitudeDir = ParseParameter.BufferPtr[EndIndex+1];
						// Start of Latitude is the end of longitude
						StartIndex = EndIndex;
						LongitudeFlag = 1;
					}
					// else
					else
					{
						//search for Longitude direction ",N," or ",S,"
						ParsedData[0] = ',';
						ParsedData[1] = 'S';
						ParsedData[2] = ',';
						ParseParameter.DataPtr = ParsedData;
						ParseParameter.DataLength = 3;
						ParseRet = Parse(&ParseParameter)
						// if found 
						if (ParseRet == PARSE_OK)
						{
							// mark the end index of Longitude
							EndIndex = ParseParameter.Index;
							for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
							{
								ParsedData.Longitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
							}
							ParsedData.LongitudeDir = ParseParameter.BufferPtr[EndIndex+1];
							// Start of Latitude is the end of longitude
							StartIndex = EndIndex;
							LongitudeFlag = 1;
						}
						else
						{
							LongitudeFlag = 0;
						}
					}

					//search for Longitude direction ",E,"
					ParsedData[0] = ',';
					ParsedData[1] = 'E';
					ParsedData[2] = ',';
					ParseParameter.DataPtr = ParsedData;
					ParseParameter.DataLength = 3;
					ParseRet = Parse(&ParseParameter)
					// if found 
					if (ParseRet == PARSE_OK)
					{
						// mark the end index of Latitude
						EndIndex = ParseParameter.Index;
						for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
						{
							ParsedData.Latitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
						}
						ParsedData.LatitudeDir = ParseParameter.BufferPtr[EndIndex+1];
						LatitudeFlag = 1;
					}
					// else
					else
					{
						//search for Longitude direction ",W,"
						ParsedData[0] = ',';
						ParsedData[1] = 'W';
						ParsedData[2] = ',';
						ParseParameter.DataPtr = ParsedData;
						ParseParameter.DataLength = 3;
						ParseRet = Parse(&ParseParameter)
						// if found 
						if (ParseRet == PARSE_OK)
						{
							// mark the end index of Latitude
							EndIndex = ParseParameter.Index;
							for (LoopIndex = 0; LoopIndex <=(EndIndex - StartIndex -4); LoopIndex++)
							{
								ParsedData.Latitude[LoopIndex] = ParseParameter.BufferPtr[StartIndex+LoopIndex+3];
							}
							ParsedData.LatitudeDir = ParseParameter.BufferPtr[EndIndex+1];
							LatitudeFlag = 1;
						}
						else
						{
							LatitudeFlag = 0;
						}	
					}

					if (LongitudeFlag && LatitudeFlag)
					{
						RetVar = GPS_OK;
						// execute call back function
						GPS_Ptr->FoundCallBack();
						// change the state to be IDLE
						State = GPS_IDLE;
					}
					else
					{
						// change the state to be error
						State = GPS_ERROR;
					}

				}
				// else
				else
				{
					// Change the state to be Error
					State = GPS_ERROR;
				}
			}
			//else
			else
			{
				// Change the state to be Error
				State = GPS_ERROR;
			}
		}
		break;
		// if Error
		case GPS_ERROR:
		{
			// return GPS_NOk
			RetVar = GPS_NOk;
			// execute the error call back function
			GPS_Ptr->ErrorCallBack();
			// change the state to be IDLE
			State = GPS_IDLE;
		}
	}
	return RetVar;
}

/*
 * The CallBack function That is executed at the end of receiving the data from GPS module
*/
void GPS_ReceptionCallBack(void)
{
	ReceptionDoneFlag = 1;
}
/*----------------------------------------------------------------------*/