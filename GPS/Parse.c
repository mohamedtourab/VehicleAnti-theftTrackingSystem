/**************************************************************************************
*	File name: Parse.c
*	Author: Moamen Ali & Moamen Ramadan
*	Date: 6/3/2018
*	Description:
*		- Parse function is used to search for any data in a certain buffer and return-
*		  the index of the start character in the buffer 
*	Usage:
*		- User define the configuration parameter in a GPS_Cfg strycture which are:
*			. The Buffer pointer where we will search in.
*			. The length of the buffer where we will search in.
*			. pointer to the data that we will search about it.
*			. Length of the data.
*		- The function return the index in the buffer of the first character of-
*		- the string we searching about it.
***************************************************************************************/
#include "Parse.h"

/*
 * This function is used to Parse any buffer and search for data in this buffer
 * Inputs:
 	- ParseParameter: pointer to the structure in which user determine the pointer and length of buffer and data
 * Outputs:
 	- PARSE_OK : in case we find the data that we searching for it in the buffer
 	- PARSE_NOK : in case we didnot find the data
*/
Parse_CheckType Parse (Parse_Cfg* ParseParameter)
{
	Parse_CheckType RetVar;
	// counter to get the number of the characters which are founded in the buffer
	uint8_t FoundCounter =0; 
	uint32_t LoopIndex = 0;
	// check the validity of the Lengths
	if ((ParseParameter-> BufferLength) >= (ParseParameter-> DataLength))
	{
		// loop till found the total data in the buffer or reach buffer length without finding data
		while ((LoopIndex < (ParseParameter-> BufferLength)) && (FoundCounter < (ParseParameter-> DataLength)))
		{
			// search for data character by character in the buffer 
			if (*((ParseParameter-> DataPtr) + FoundCounter) == *((ParseParameter-> BufferPtr) + LoopIndex))
			{
				
				FoundCounter++;
			}
			else
			{
				FoundCounter = 0;
			}
			LoopIndex++;
		}

		// if we found the data in the buffer
		if (FoundCounter == (ParseParameter-> DataLength))
		{
			// store the index of the first character of the data that is found and return OK
			ParseParameter->Index = (LoopIndex - (ParseParameter-> DataLength));
			RetVar = PARSE_OK;
		}
		else
		{
			RetVar = PARSE_NOK;
		} 
	}
	else
	{
		RetVar = PARSE_NOK;
	}
	return RetVar;
}