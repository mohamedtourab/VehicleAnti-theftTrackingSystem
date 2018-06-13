/**************************************************************************************
*	File name: Parse.h
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

#ifndef PARSE_H_
#define PARSE_H_

#include <stdint.h>

/*************************************************************************************
********** 						Data type definition						**********
*************************************************************************************/

/* the return type of the function will be either PARSE_OK if we found the data in the buffer
   or PARSE_NOK in case we did not find the data in the buffer*/
typedef enum {PARSE_OK = 0, PARSE_NOK = 1}Parse_CheckType;

/* Structure to get the parameters from user*/
typedef struct 
{
	// pointer to the buffer where we will search in
	uint8_t* BufferPtr;

	// Length of the buffer where we will search in for the data.
	uint32_t BufferLength;

	// pointer to the string which we need to search for
	uint8_t* DataPtr;

	// Length of the data which we will search for
	uint8_t DataLength;

	// address of a variable in which we will return the index of the data in the buffer
	uint32_t Index; 
}Parse_Cfg;


/************************************************************************************************************
**************									Function Prototype							  ***************
************************************************************************************************************/

/*
 * This function is used to Parse any buffer and search for data in this buffer
 * Inputs:
 	- ParseParameter: pointer to the structure in which user determine the pointer and length of buffer and data
 * Outputs:
 	- PARSE_OK : in case we find the data that we searching for it in the buffer
 	- PARSE_NOK : in case we didnot find the data
*/
Parse_CheckType Parse (Parse_Cfg* ParseParameter);
#endif
