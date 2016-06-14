/************************************************************************/
/*																		*/
/* PmodJSTK.c	--		driver for the PmodJSTK							*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe										*/
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
/*	This file contains the drivers for the PmodJSTK IP from Digilent	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/01/2016(SamL): Created
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodJSTK.h"

 


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/************************** Function Definitions ***************************/
XSpi_Config JSTKConfig =
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
/***	void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**		SPI_Address: The Base address of the PmodJSTK SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodJSTK.
*/
void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address)
{
	JSTKConfig.BaseAddress=SPI_Address;
	JSTK_SPIInit(&InstancePtr->JSTKSpi);
}

/* ------------------------------------------------------------ */
/***	JSTK_end(void)
**
**	Parameters:
**		InstancePtr		- PmodJSTK object to stop
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
void JSTK_end(PmodJSTK* InstancePtr){
	XSpi_Stop(&InstancePtr->JSTKSpi);
}

/* ------------------------------------------------------------ */
/***	JSTK_SPIInit
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
**		Initializes the PmodJSTK SPI.

*/

int JSTK_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &JSTKConfig, JSTKConfig.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}



	Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION) | XSP_MANUAL_SSELECT_OPTION);
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
/***	void JSTK_setLeds(PmodJSTK* InstancePtr, u8 ledst)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**		ledst: the desired state of the LEDs
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets the 2 user LEDs on the board
*/
void JSTK_setLeds(PmodJSTK* InstancePtr, u8 ledSt)
{
	u8 recv[5];
	//load buffer to send led data
	recv[0] |= 0x80;
	recv[0] |= ledSt;
	JSTK_getData(InstancePtr, recv);
}

/* ------------------------------------------------------------ */
/***	void JSTK_getX(PmodJSTK* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**
**	Return Value:
**		u16 xPos: The 16 bit representation of the x-axis
**
**	Errors:
**		none
**
**	Description:
**		Gets the x-axis position from the PmodJSTK.
*/
u16 JSTK_getX(PmodJSTK* InstancePtr)
{
	u16 xPos;
	u8 recv[5] = {0};
	JSTK_getData(InstancePtr, recv);

	xPos = recv[2];
	xPos |= (recv[3] << 8);

	return xPos;
}

/* ------------------------------------------------------------ */
/***	void JSTK_getY(PmodJSTK* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**
**	Return Value:
**		u16 yPos: The 16 bit representation of the x-axis
**
**	Errors:
**		none
**
**	Description:
**		Gets the y-axis position from the PmodJSTK.
*/
u16 JSTK_getY(PmodJSTK* InstancePtr)
{
	u16 yPos;
	u8 recv[5] = {0};

	JSTK_getData(InstancePtr, recv);

	yPos = recv[0];
	yPos |= (recv[1] << 8);

	return yPos;
}

/* ------------------------------------------------------------ */
/***	void JSTK_getBtn(PmodJSTK* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**
**	Return Value:
**		u8 btn: The 8 bit representation of the buttons which are in the
**		0th and 1st positions
**
**	Errors:
**		none
**
**	Description:
**		Gets the push button states of the PmodJSTK
*/
u8 JSTK_getBtn(PmodJSTK* InstancePtr)
{
	u8 recv[5] = {0};
	JSTK_getData(InstancePtr, recv);

	return recv[4];
}

/* ------------------------------------------------------------ */
/***	void JSTK_getData(PmodJSTK* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**		recv: A byte array to act as the receive and send buffer
**
**	Return Value:
**		none
**
**	Errors:
**		need to implement microsecond delay in both microblaze and zynq
**
**	Description:
**		Gets the push button states of the PmodJSTK
**
**	Problems:
**		Setting the chip select low through SetSlaveSelectReg isnt working so
**		the function now uses XilOut
*/
void JSTK_getData(PmodJSTK* InstancePtr, u8* recv){

	//In order to correctly communicate with the Pmod, there needs to be a small delay between
	//each spi read

	XSpi_SetSlaveSelectReg(&InstancePtr->JSTKSpi,	&InstancePtr->JSTKSpi.SlaveSelectReg);
	delay(20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[0], &recv[0], 1);
	delay(20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[1], &recv[1], 1);
	delay(20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[2], &recv[2], 1);
	delay(20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[3], &recv[3], 1);
	delay(20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[4], &recv[4], 1);

//	XSpi_SetSlaveSelectReg(&InstancePtr->JSTKSpi,	&InstancePtr->JSTKSpi.SlaveSelectMask);
	//above doesn't seem to work so manually set it in memory
	Xil_Out32(InstancePtr->JSTKSpi.BaseAddr+0x70, 1);

}

/* ------------------------------------------------------------ */
/***	void delaya(int micros)
**
**	Parameters:
**		micros: amount of microseconds to delay
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		delays for a given amount of microseconds. Adapted from sleep and MB_Sleep
*/
void delay(int micros){
	int i;

#ifdef XPAR_MICROBLAZE_ID
	for(i = 0; i < (ITERS_PER_USEC*micros); i++){
			asm("");
	}
#else
	usleep(micros);
#endif
}






