/************************************************************************/
/*																		*/
/* PmodDA1.c	--		Template driver for a Pmod which uses SPI		*/
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
/*	This file contains a template that you can use to create a library  */
/*	for the PmodDA1 (JonP).                                             */
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*									                                    */
/*  06/20/2016(JonP): Created PmodDA1 IP for microblaze and Zynq    	*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodDA1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/
XSpi_Config DA1Config =
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
/***	void DA1_begin(PmodDA1* InstancePtr, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodDA1 object to start
**		SPI_Address: The Base address of the PmodDA1 SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodDA1.
*/
void DA1_begin(PmodDA1* InstancePtr, u32 SPI_Address)
{
	DA1Config.BaseAddress=SPI_Address;
	DA1_SPIInit(&InstancePtr->DA1Spi);
}

/* ------------------------------------------------------------ */
/***	DA1_end(void)
**
**	Parameters:
**		InstancePtr		- PmodDA1 object to stop
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
void DA1_end(PmodDA1* InstancePtr){
	XSpi_Stop(&InstancePtr->DA1Spi);
}

/* ------------------------------------------------------------ */
/***	DA1_SPIInit
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

int DA1_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &DA1Config, DA1Config.BaseAddress);
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
/*        DACSPI1::WriteIntegerValue
**
**        Synopsis:
**				WriteIntegerValue(0x00FF);
**        Parameters:
**				- uint8_t bIntegerValue - the 8 bits value to be written to DA converter
**              - InstancePtr: A PmodDA1 object to start
**        Return Value:
**                uint8_t
**					- DACSPI1_ERR_SUCCESS (0) 				- The action completed successfully
**					- DACSPI1_ERR_VAL_OUT_OF_RANGE	(1) 	- The value is not within the 0 - 0xFF range
**
**        Errors:
**			If module is not initialized (using begin), the function does nothing. Also, see return value.
**
**        Description:
**			If the value is inside the allowed range (0 - 0xFF), this function writes the 12 bits value to the DA converter, by writing 16 bits to SPI and returns the DACSPI1_ERR_SUCCESS status message.
**			If the value is outside the allowed range, the function does nothing and returns the DACSPI1_ERR_VAL_OUT_OF_RANGE message.
**
**
*/
u8 DA1_WriteIntegerValue(u8 bIntegerValue, PmodDA1* InstancePtr )
{
	u8 recv[2];
     recv[0]= DACSPI1_CTRL_BYTE;
     recv[1]= bIntegerValue;

	u8 bResult = 0;
	if(bIntegerValue < 0 || bIntegerValue >= (1 << DACSPI1_NO_BITS))
		{
			bResult = DACSPI1_ERR_VAL_OUT_OF_RANGE;
		}
		else
		{
			//xil_printf(" made it here, %d , yes i did %x   " , recv[1], recv[1]);

			XSpi_Transfer(&InstancePtr->DA1Spi, recv, recv, 2);
		}
	return bResult;
}

/* ------------------------------------------------------------ */
/*        DACSPI1::WritePhysicalValue
**
**        Synopsis:
**
**        Parameters:
**				- float dPhysicalValue - the physical value that must be reconstructed at the output of the DA converter
**				- InstancePtr: A PmodDA1 object to start
**
**        Return Values:
**                uint8_t
**					- DACSPI2_ERR_SUCCESS (0) 				- The action completed successfully
**					- DACSPI2_ERR_VAL_OUT_OF_RANGE	(1) 	- The physical value is not inside accepted range
**
**        Errors:
**			If module is not initialized (using begin), the function does nothing. Also, see return value.
**
**        Description:
**			The function computes the integer value corresponding to the physical value by considering the reference value as the one corresponding to the maximum integer value (0xFF).
**			If the integer value is within the accepted range (0 - 0xFF), this function writes the 12- bit value to the DA converter, by writing 8 bits to SPI, and returns the DACSPI1_ERR_SUCCESS message.
**			If the integer value is outside the allowed range, the function does nothing and returns the DACSPI1_ERR_VAL_OUT_OF_RANGE message.
**			If the dReference function argument is missing, 3.3 value is used as reference value.
**
*/


u8 DA1_WritePhysicalValue(float dPhysicalValue,PmodDA1* InstancePtr)
{
	u8 status;
	float dReference = 3.3; //the value corresponding to the maximum converter value (reference voltage). If this parameter is not provided, it has a default value of 3.3.

	u8 wIntegerValue = dPhysicalValue * (float)(1<<DACSPI1_NO_BITS) / dReference;
	status = DA1_WriteIntegerValue(wIntegerValue, InstancePtr);
	return status;
}






