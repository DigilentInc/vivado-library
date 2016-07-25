/************************************************************************/
/*																		*/
/* PmodJSTK2.c	--		driver for the PmodJSTK2							*/
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
/*	This file contains the drivers for the PmodJSTK2 IP from Digilent	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/01/2016(SamL): Created
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodJSTK2.h"



/************************** Function Definitions ***************************/
XSpi_Config JSTK2Config =
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
/***	void JSTK2_begin(PmodJSTK2* InstancePtr, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**		SPI_Address: The Base address of the PmodJSTK2 SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodJSTK2.
*/
void JSTK2_begin(PmodJSTK2* InstancePtr, u32 SPI_Address)
{
	JSTK2Config.BaseAddress=SPI_Address;
	JSTK2_SPIInit(&InstancePtr->JSTK2Spi);
}

/* ------------------------------------------------------------ */
/***	JSTK2_end(void)
**
**	Parameters:
**		InstancePtr		- PmodJSTK2 object to stop
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
void JSTK2_end(PmodJSTK2* InstancePtr){
	XSpi_Stop(&InstancePtr->JSTK2Spi);
}

/* ------------------------------------------------------------ */
/***	JSTK2_SPIInit
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
**		Initializes the PmodJSTK2 SPI.

*/

int JSTK2_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &JSTK2Config, JSTK2Config.BaseAddress);
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
/***	void JSTK2_setLeds(PmodJSTK2* InstancePtr, u8 ledst)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**		Red: pwm for red led (0-255)
**		Green: pwm for green led (0-255)
**		Blue:  pwm for blue led (0-255)
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets the RGB Led on the board
*/
void JSTK2_setLed(PmodJSTK2* InstancePtr, u8 red, u8 green, u8 blue)
{
	u8 recv[5];
	//load buffer to send led data
	recv[0] = cmdSetLedRGB;
	recv[1] = red;
	recv[2] = green;
	recv[3] = blue;
	JSTK2_getData(InstancePtr, recv, 5);
	delay(100);
}

/* ------------------------------------------------------------ */
/***	void JSTK2_getYRaw(PmodJSTK2* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**
**	Return Value:
**		u16 xPos: The raw 16 bit representation of the x-axis
**
**	Errors:
**		none
**
**	Description:
**		Gets the raw y-axis position from the PmodJSTK2.
*/
u16 JSTK2_getYRaw(PmodJSTK2* InstancePtr)
{
	u16 yPos;
	u8 recv[5] = {0};
	JSTK2_getData(InstancePtr, recv, 5);

	yPos = recv[2];
	yPos |= (recv[3] << 8);

	return yPos;
}

/* ------------------------------------------------------------ */
/***	void JSTK2_getXRaw(PmodJSTK2* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**
**	Return Value:
**		u16 yPos: The raw 16 bit representation of the x-axis
**
**	Errors:
**		none
**
**	Description:
**		Gets the raw x-axis position from the PmodJSTK2.
*/
u16 JSTK2_getXRaw(PmodJSTK2* InstancePtr)
{
	u16 xPos;
	u8 recv[5] = {0};

	JSTK2_getData(InstancePtr, recv, 5);

	xPos = recv[0];
	xPos |= (recv[1] << 8);

	return xPos;
}

/* ------------------------------------------------------------ */
/***	void JSTK2_getBtn(PmodJSTK2* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**
**	Return Value:
**		u8 btn: The 8 bit representation of the buttons which are in the
**		0th and 1st positions
**
**	Errors:
**		none
**
**	Description:
**		Gets the push button states of the PmodJSTK22
*/
u8 JSTK2_getBtns(PmodJSTK2* InstancePtr)
{
	u8 recv[5] = {0};
	JSTK2_getData(InstancePtr, recv, 5);

	return (recv[4] & 0x03);
}


u8 JSTK2_getX(PmodJSTK2* InstancePtr){
	u8 recv[7] = {0};
	recv[0] = cmdGetPosition;
	JSTK2_getData(InstancePtr, recv, 7);

	return recv[5];

}

u8 JSTK2_getY(PmodJSTK2* InstancePtr){
	u8 recv[7] = {0};
	recv[0] = cmdGetPosition;
	JSTK2_getData(InstancePtr, recv, 7);

	return recv[6];

}

u8 JSTK2_getStatus(PmodJSTK2* InstancePtr){\
	u8 recv[6] = {0};
	recv[0] = cmdGetStatus;
	JSTK2_getData(InstancePtr, recv, 6);

	return recv[5];
}


void JSTK2_setInversion(PmodJSTK2* InstancePtr, u8 invX, u8 invY){
	u8 recv[5] = {0};

	recv[0] = cmdSetInversion | ((invX << 1 | invY) & (0x03));

	JSTK2_getData(InstancePtr, recv, 5);

}


u8 JSTK2_rldFromFlash(PmodJSTK2* InstancePtr){
	u8 recv[5] = {0};

	recv[0] = cmdRldFromFlash;

	JSTK2_getData(InstancePtr, recv, 5);

	//strongly recommended that the user waits 100 microseconds
	//after the chip select line goes high in the setCoand
	//function before performing another SPI operation with this
	//device.

	delay(100);

	//check if read was successful
	if(JSTK2_getStatus(InstancePtr) & 0x10) //success
		return 1;
	else 									//failure
		return 0;

}

u8 JSTK2_writeFlash(PmodJSTK2* InstancePtr){
	u8 buffer[5] = {0};
	buffer[0] = cmdWriteFlash;

	JSTK2_getData(InstancePtr, buffer, 5);

	//strongly recommended that the user waits 5 milliseconds
	//after the chip select line goes high in the setCommand
	//function before performing another SPI operation with this
	//device.
	delay(5000);

	//check if write was successful
	if(JSTK2_getStatus(InstancePtr) & 0x20) //success
		return 1;
	else 									//failure
		return 0;


}


////calibration stuff
void JSTK2_calibrate(PmodJSTK2* InstancePtr){
	u8 buffer[5] = {0};
	buffer[0] = cmdCalibrate;



	JSTK2_getData(InstancePtr, buffer, 5);



	//wait until calibration is completed
	while((JSTK2_getStatus(InstancePtr) >> 7)){
		xil_printf("\n\rcalibrating...");
		delay(100000);
	}

	xil_printf("\n\rCalibration complete");


}

////get calibration settings
u16 JSTK2_getCalXMin(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalXMin;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}

u16 JSTK2_getCalXMax(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalXMax;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}

u16 JSTK2_getCalYMin(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalYMin;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}
u16 JSTK2_getCalYMax(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalYMax;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}
u16 JSTK2_getCalXCenMin(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalXCenMin;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}
u16 JSTK2_getCalXCenMax(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalXCenMax;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}
u16 JSTK2_getCalYCenMin(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalYCenMin;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}
u16 JSTK2_getCalYCenMax(PmodJSTK2* InstancePtr){
	u8 buffer[7] = {0};
	buffer[0] = cmdGetCalYCenMax;

	JSTK2_getData(InstancePtr, buffer, 7);

	return ((u16)((buffer[5]<<8)|buffer[6]));

}


//set commands
void JSTK2_setCalXMin(PmodJSTK2* InstancePtr, u16 XMinCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXMin;
	buffer[1] = XMinCal & 0x00FF;
	buffer[2] = XMinCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalXMax(PmodJSTK2* InstancePtr, u16 XMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXMax;
	buffer[1] = XMaxCal & 0x00FF;
	buffer[2] = XMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYMin(PmodJSTK2* InstancePtr, u16 YMinCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalYMin;
	buffer[1] = YMinCal & 0x00FF;
	buffer[2] = YMinCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYMax(PmodJSTK2* InstancePtr, u16 YMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalYMax;
	buffer[1] = YMaxCal & 0x00FF;
	buffer[2] = YMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalXCenMin(PmodJSTK2* InstancePtr, u16 XCenMinCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXCenMin;
	buffer[1] = XCenMinCal & 0x00FF;
	buffer[2] = XCenMinCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalXCenMax(PmodJSTK2* InstancePtr, u16 XCenMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXCenMax;
	buffer[1] = XCenMaxCal & 0x00FF;
	buffer[2] = XCenMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYCenMin(PmodJSTK2* InstancePtr, u16 YCenMinCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalYCenMin;
	buffer[1] = YCenMinCal & 0x00FF;
	buffer[2] = YCenMinCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYCenMax(PmodJSTK2* InstancePtr, u16 YCenMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalYCenMax;
	buffer[1] = YCenMaxCal & 0x00FF;
	buffer[2] = YCenMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalXMinMax(PmodJSTK2* InstancePtr, u16 XMinCal, u16 XMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXMinMax;
	buffer[1] = XMinCal & 0x00FF;
	buffer[2] = XMinCal >> 8;
	buffer[3] = XMaxCal & 0x00FF;
	buffer[4] = XMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYMinMax(PmodJSTK2* InstancePtr, u16 YMinCal, u16 YMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalYMinMax;
	buffer[1] = YMinCal & 0x00FF;
	buffer[2] = YMinCal >> 8;
	buffer[3] = YMaxCal & 0x00FF;
	buffer[4] = YMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalXCenMinMax(PmodJSTK2* InstancePtr, u16 XCenMinCal, u16 XCenMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXCenMinMax;
	buffer[1] = XCenMinCal & 0x00FF;
	buffer[2] = XCenMinCal >> 8;
	buffer[3] = XCenMaxCal & 0x00FF;
	buffer[4] = XCenMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}
void JSTK2_setCalYCenMinMax(PmodJSTK2* InstancePtr, u16 YCenMinCal, u16 YCenMaxCal){
	u8 buffer[5] = {0};
	buffer[0] = cmdSetCalXCenMinMax;
	buffer[1] = YCenMinCal & 0x00FF;
	buffer[2] = YCenMinCal >> 8;
	buffer[3] = YCenMaxCal & 0x00FF;
	buffer[4] = YCenMaxCal >> 8;

	JSTK2_getData(InstancePtr, buffer, 5);

}





/* ------------------------------------------------------------ */
/***	void JSTK2_getData(PmodJSTK2* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK2 object to start
**		recv: A byte array to act as the receive and send buffer
**
**	Return Value:
**		none
**
**	Errors:
**		need to implement microsecond delay in both microblaze and zynq
**
**	Description:
**		Gets the push button states of the PmodJSTK2
**
**	Problems:
**		Setting the chip select low through SetSlaveSelectReg isnt working so
**		the function now uses XilOut
*/
void JSTK2_getData(PmodJSTK2* InstancePtr, u8* recv, u8 nData){

	//In order to correctly communicate with the Pmod, there needs to be a small delay between
	//each spi read
	int i = 0;

	XSpi_SetSlaveSelectReg(&InstancePtr->JSTK2Spi,	&InstancePtr->JSTK2Spi.SlaveSelectReg);

	delay(100);

	for(i = 0; i < nData; i++){
		delay(40);
		XSpi_Transfer(&InstancePtr->JSTK2Spi, &recv[i], &recv[i], 1);
	}

//	XSpi_SetSlaveSelectReg(&InstancePtr->JSTK2Spi,	&InstancePtr->JSTK2Spi.SlaveSelectMask);
	//above doesn't seem to work so manually set it in memory
	Xil_Out32(InstancePtr->JSTK2Spi.BaseAddr+0x70, 1);
	delay(20);
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






