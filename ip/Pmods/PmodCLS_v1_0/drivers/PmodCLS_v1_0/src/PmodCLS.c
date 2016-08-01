/* PmodCLS.c	--		Template driver for a Pmod which uses SPI		*/
/*																		*/
/************************************************************************/
/*	Author:		Mikel Skreen											*/
/*	Copyright 2016, Digilent Inc.										*/
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains a basic library for the Pmod CLS.				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/15/2016(MikelSkreen): Created									*/
/*																		*/
/************************************************************************/

/***************************** Include Files ****************************/
#include "PmodCLS.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ************************/
XSpi_Config CLSConfig =
{
	0,
	0,
	1,
	0,
	1,
	8,
	0,
	0,
	0,
	0,
	0
};

/* --------------------------------------------------------------------*/
/** CLS_DefineUserChar
**
**	Parameters:
**		strUserDef - characters array containing the numerical value of each row in the char
**		charPos - the position of the character saved in the memory
**
**
**	Return Value:
**		uint8_t
**					- LCDS_ERR_SUCCESS - The action completed successfully
**					- LCDS_ERR_ARG_POS_RANGE - The argument is not within 0, 7 range
**
**	Errors:
**		none
**
**	Description:
**		This function saves a user defined char in the RAM memory
**
-----------------------------------------------------------------------*/
u8 CLS_DefineUserChar(PmodCLS *InstancePtr, uint8_t* strUserDef, uint8_t charPos) {
	char rgcCmd[MAX];
	uint8_t bResult;
	if (charPos >= 0 && charPos <= 7){
		rgcCmd[0] = ESC;
		rgcCmd[1] = BRACKET;
		rgcCmd[2] = 0;
		//build the values to be sent for defining the custom character
		CLS_BuildUserDefChar(strUserDef, rgcCmd + 2);
		u8 bLength = strlen(rgcCmd);
		rgcCmd[bLength++] = (char)charPos + '0';
		rgcCmd[bLength++] = DEF_CHAR_CMD;
		//save the defined character in the RAM
		rgcCmd[bLength++] = ESC;
		rgcCmd[bLength++] = BRACKET;
		rgcCmd[bLength++] = '3';
		rgcCmd[bLength++] = PRG_CHAR_CMD;
		CLS_WriteSpi(InstancePtr, (unsigned char*)(rgcCmd), bLength);
		bResult = LCDS_ERR_SUCCESS;
	}
	else {
		bResult = LCDS_ERR_ARG_POS_RANGE;
	}
	return bResult;
}

/* --------------------------------------------------------------------*/
/** CLS_BuildUserDefChar
**
**	Parameters:
**		strUserDef - bytes array containing the values to be converted in values that are recognized by the firmware
**		cmdStr	   - characters array containing the values converted
**
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This function builds the string to be converted in an interpretable array of chars for the LCD
-----------------------------------------------------------------------*/
void CLS_BuildUserDefChar(uint8_t* strUserDef, char* cmdStr) {
	uint8_t len = 8;
	int i;
	char elStr[4];
	//print the bytes from the input array as hex values
	for(i = 0; i < len; i++){
		sprintf(elStr, "%2.2X", strUserDef[i]);
		//concatenate the result with the 0x chars to be able to send it to the LCD
		strcat(cmdStr, "0x");

		if (strUserDef[i] > 15) {
			elStr[3] = 0;
			strcat(cmdStr, elStr + 1);
		}
		else {
			elStr[2] = 0;
			strcat(cmdStr, elStr);
		}
		strcat(cmdStr, ";");
	}
}

/* --------------------------------------------------------------------*/
/** CLS_DisplayMode
**
**	Parameters:
**		charNumber - parameter for selecting the wrapping type of the line: to 16 or 40 characters
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This function wraps the line at 16 or 40 characters
-----------------------------------------------------------------------*/
void CLS_DisplayMode(PmodCLS *InstancePtr, bool charNumber){
	uint8_t dispMode16[] = {ESC, BRACKET, '0', DISP_MODE_CMD};
	uint8_t dispMode40[] = {ESC, BRACKET, '1', DISP_MODE_CMD};
	if (charNumber){
		//wrap line at 16 characters
		CLS_WriteSpi(InstancePtr, dispMode16, 4);
	}
	else{
		//wrap line at 40 characters
		CLS_WriteSpi(InstancePtr, dispMode40, 4);
	}
}

/* --------------------------------------------------------------------*/
/** CLS_CursorModeSet
**
**	Parameters:
**		setCursor - boolean parameter through which the cursor is set on or off
**		setBlink - boolean parameter through which the blink option is set on or off
**
**	Return Value:
**		None
**
**	Errors:
**		none
**
**	Description:
**		This function turns the cursor and the blinking option on or off,
**    according to the user's selection.
-----------------------------------------------------------------------*/
void CLS_CursorModeSet(PmodCLS *InstancePtr, bool setCursor, bool setBlink) {
	uint8_t cursorOff[]			  = {ESC, BRACKET, '0', CURSOR_MODE_CMD};
	uint8_t cursorOnBlinkOff[]    = {ESC, BRACKET, '1', CURSOR_MODE_CMD};
	uint8_t cursorBlinkOn[]       = {ESC, BRACKET, '2', CURSOR_MODE_CMD};
	if (!setCursor)	{
		//send the command for both display and blink off
		CLS_WriteSpi(InstancePtr, cursorOff, 4);
	}
	else if ((setCursor)&&(!setBlink)) {
		//send the command for display on and blink off
		CLS_WriteSpi(InstancePtr, cursorOnBlinkOff, 4);
	}
		else {
			//send the command for display and blink on
			CLS_WriteSpi(InstancePtr, cursorBlinkOn, 4);
		}
}

/* ------------------------------------------------------------------- */
/** CLS_DisplayClear
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This function clears the display and returns the cursor home
-----------------------------------------------------------------------*/
void CLS_DisplayClear(PmodCLS *InstancePtr) {
	u8 dispClr[] = {ESC, BRACKET, '0', DISP_CLR_CMD};
	//clear the display and returns the cursor home
	CLS_WriteSpi(InstancePtr, dispClr, 4);
}

/*---------------------------------------------------------------------*/
/***	void CLS_begin(PmodCLS* InstancePtr, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodCLS object to start
**		SPI_Address: The Base address of the PmodCLS SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodCLS.
-----------------------------------------------------------------------*/
void CLS_begin(PmodCLS* InstancePtr, u32 SPI_Address)
{
	CLSConfig.BaseAddress=SPI_Address;
	CLS_SPIInit(&InstancePtr->CLSSpi);
}

/*---------------------------------------------------------------------*/
/***	CLS_end(void)
**
**	Parameters:
**		InstancePtr		- PmodCLS object to stop
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Stops the device
-----------------------------------------------------------------------*/
void CLS_end(PmodCLS* InstancePtr){
	XSpi_Stop(&InstancePtr->CLSSpi);
}

/*---------------------------------------------------------------------*/
/***	CLS_SPIInit
**
**	Parameters:
**		InstancePtr	- the PmodCLS object to communicate with
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initializes the PmodCLS SPI.
-----------------------------------------------------------------------*/
int CLS_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &CLSConfig, CLSConfig.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION /*| XSP_CLK_ACTIVE_LOW_OPTION | XSP_CLK_PHASE_1_OPTION*/) | XSP_MANUAL_SSELECT_OPTION);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(SpiInstancePtr, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the SPI driver so that the device is enabled.
	 */
	XSpi_Start(SpiInstancePtr);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XSpi_IntrGlobalDisable(SpiInstancePtr);

	return XST_SUCCESS;

}

/* ------------------------------------------------------------ */
/***	CLS_Readbyte
**
**	Parameters:
**		InstancePtr	- the PmodCLS object to communicate with
**
**	Return Value:
**		byte	- Byte read from XSpi
**
**	Errors:
**		none
**
**	Description:
**		Reads a single byte over SPI
-----------------------------------------------------------------------*/
u8 CLS_ReadByte(PmodCLS* InstancePtr){
	u8 byte;
	XSpi_Transfer(&InstancePtr->CLSSpi, &byte, &byte, 1);
	return byte;
}

/* ------------------------------------------------------------ */
/***	CLS_WriteByte
**
**	Parameters:
**		InstancePtr - PmodCLS object to send to
**		cmd					- Command to send
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Writes a single byte over SPI
-----------------------------------------------------------------------*/
void CLS_WriteByte(PmodCLS* InstancePtr, u8 cmd)
{
	XSpi_Transfer(&InstancePtr->CLSSpi, &cmd, NULL, 1);
}

/* ------------------------------------------------------------ */
/***	CLS_WriteSpi
**
**	Parameters:
**		PmodCLS *InstancePtr	- the PmodCLS object to communicate with
**		u8 reg			- the starting register to write to
**		u8* wData		- the data to write
**		int nData		- the number of data bytes to write
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Writes the byte array to the chip via SPI.
-----------------------------------------------------------------------*/
void CLS_WriteSpi(PmodCLS *InstancePtr, u8 *wData, int nData)
{
	XSpi_Transfer(&InstancePtr->CLSSpi, wData, 0, nData);
}

/* --------------------------------------------------------------------*/
/*** uint8_t  CLS_WriteStringAtPos
**
**	Parameters:
**		idxLine - the line where the string is written: 0 or 1
**		idxPos  - the start column for the string to be written:0 to 39
**		strLn   - the string to be written
**
**	Return Value:
**		u8
**				- LCDS_ERR_SUCCESS - The action completed successfully
**				- a combination of the following errors(OR-ed):
**				- LCDS_ERR_ARG_COL_RANGE - The argument is not within 0, 39 range
**				- LCDS_ERR_ARG_ROW_RANGE - The argument is not within 0, 2 range
**
**	Errors:
**		see returned values
**
**	Description:
**		This function writes a string at a specified position
**
-----------------------------------------------------------------------*/
u8 CLS_WriteStringAtPos(PmodCLS *InstancePtr, uint8_t idxRow, uint8_t idxCol, char* strLn) {

	uint8_t bResult = LCDS_ERR_SUCCESS;
	if (idxRow < 0 || idxRow > 2){
		bResult |= LCDS_ERR_ARG_ROW_RANGE;
	}
	if (idxCol < 0 || idxCol > 39){
		bResult |= LCDS_ERR_ARG_COL_RANGE;
	}
	if (bResult == LCDS_ERR_SUCCESS){
		//separate the position digits in order to send them, useful when the position is greater than 10
		uint8_t firstDigit 		= idxCol % 10;
		uint8_t secondDigit 	= idxCol / 10;
		uint8_t length 			= strlen(strLn);
		uint8_t lengthToPrint   = length + idxCol;
		uint8_t stringToSend[]  = {ESC, BRACKET, idxRow + '0', ';', secondDigit + '0', firstDigit + '0', CURSOR_POS_CMD};
		if (lengthToPrint > 40) {
			//truncate the length of the string
			//if it's greater than the positions number of a line
			length = 40 - idxCol;
		}
		CLS_WriteSpi(InstancePtr, stringToSend, 7);
		CLS_WriteSpi(InstancePtr, strLn, length);
	}
	return bResult;
}


/*---------------------------------------------------------------------*/
/***	CLS_ReadSpi
**
**	Synopsis:
**		CLS_ReadSpi(&CLSobj, 0x3A, &bytearray, 5);
**
**	Parameters:
**		PmodCLS *InstancePtr	- the PmodCLS object to communicate with
**		u8 reg			- the starting register to read from
**		u8* rData		- the byte array to read into
**		int nData		- the number of data bytes to read
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Reads data in through SPI. Data is stored into rData.
-----------------------------------------------------------------------*/
void CLS_ReadSpi(PmodCLS* InstancePtr, u8 *rData, int nData)
{
	u8 bytearray[nData];

	XSpi_Transfer(&InstancePtr->CLSSpi, bytearray, bytearray, nData);
	memcpy(rData,&bytearray[0], nData);
}
