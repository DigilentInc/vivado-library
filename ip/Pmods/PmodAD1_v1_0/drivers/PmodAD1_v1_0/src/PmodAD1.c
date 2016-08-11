/************************************************************************/
/*																		*/
/* PmodAD1.c	--		Template driver for a Pmod which uses SPI		*/
/*																		*/
/************************************************************************/
/*	Author:		Jon Peyron										*/
/*	Copyright 2015, Digilent Inc.										*/
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
/*	This file contains a template that you can use to create a library
/*	for the PmodDA1 (Jon Peyron).
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*  06/20/2016(JonP): Created PmodAD1 IP for microblaze and Zynq    	*/
/*	Only uses ADC2 use pin P3 																	*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodAD1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/
XSpi_Config AD1Config =
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
/* ------------------------------------------------------------ */
/***	void AD1_begin(PmodAD1* InstancePtr, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodAD1 object to start
**		SPI_Address: The Base address of the PmodAD1 SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodAD1.
*/
void AD1_begin(PmodAD1* InstancePtr, u32 SPI_Address)
{
	AD1Config.BaseAddress=SPI_Address;
	AD1_SPIInit(&InstancePtr->AD1Spi);
}

/* ------------------------------------------------------------ */
/***	AD1_end(void)
**
**	Parameters:
**		InstancePtr		- PmodAD1 object to stop
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Stops the device
*/
void AD1_end(PmodAD1* InstancePtr){
	XSpi_Stop(&InstancePtr->AD1Spi);
}

/* ------------------------------------------------------------ */
/***	AD1_SPIInit
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
**		Initializes the PmodDA1 SPI.

*/

int AD1_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &AD1Config, AD1Config.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}



	Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION | XSP_CLK_ACTIVE_LOW_OPTION | XSP_CLK_PHASE_1_OPTION) | XSP_MANUAL_SSELECT_OPTION);
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
/***	void AD1_readData(PmodAD1* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodAD1 object to start
**
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		this function reads data into PmodAD1* InstancePtr->data using SPI
*/
void AD1_readData(PmodAD1* InstancePtr){
	u8 byte[4];
	InstancePtr->data = 0;
	XSpi_Transfer(&InstancePtr->AD1Spi, byte, byte, 4);

	// convert the byte array to an int
	InstancePtr->data |= byte[0];
	InstancePtr->data = InstancePtr->data << 8;
	InstancePtr->data |= byte[1];


}
/* ------------------------------------------------------------ */
/***	int AD1_GetIntegerValue(PmodAD1* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodAD1 object to start
**
**
**	Return Value:
**		integer- the 12 bits value read from the AD converter
**
**	Errors:
**		none
**
**	Description:
**		This function returns the 12 bits value read from the AD converter, obtained by reading 16 bits through the SPI interface.
*/




int AD1_GetIntegerValue(PmodAD1* InstancePtr)
{

	int buffer = 0;
	AD1_readData(InstancePtr);
	buffer = InstancePtr->data;

	return buffer/2 + 50;

}

/* ------------------------------------------------------------ */
/***	float AD1_GetPhysicalValue(float dReference, PmodAD1* InstancePtr )
**
**	Parameters:
**		InstancePtr: A PmodAD1 object to start
**		dReference: the value corresponding to the maximum converter value.
**
**	Return Value:
**		 float	- the value corresponding to the value read from the AD converter and to the reference value
**
**	Errors:
**		none
**
**	Description:
**		This function returns the value corresponding to the value read from the AD converter and to the selected reference value
*/


#ifdef AD1_FLOATING_POINT

float AD1_GetPhysicalValue(float dReference, PmodAD1* InstancePtr )
{
	int wIntegerValue = AD1_GetIntegerValue(InstancePtr);
	float dValue = ((float)wIntegerValue) * (dReference /((1<<ADCSPI_NO_BITS) - 1));
	return dValue;
}

#endif
