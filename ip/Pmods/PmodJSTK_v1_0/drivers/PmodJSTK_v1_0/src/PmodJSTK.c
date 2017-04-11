/************************************************************************/
/*																		*/
/* PmodJSTK.c	--		driver for the PmodJSTK							*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe										        */
/*	Copyright 2015, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains the drivers for the PmodJSTK IP from Digilent	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/01/2016(SamL): Created											*/
/*	04/10/2017(ArtVVB): Validated										*/
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodJSTK.h"

/**************************** Global Variables *****************************/

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

/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***	void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address, u32 CpuClkFreqHz)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**		SPI_Address: The base address of the PmodJSTK SPI
**		CpuClkFreqHz: The speed of the cpu, used to calculate delays
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodJSTK.
*/
void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address, u32 CpuClkFreqHz)
{
	JSTKConfig.BaseAddress=SPI_Address;
	JSTK_SPIInit(&InstancePtr->JSTKSpi);
	InstancePtr->ItersPerUSec = CpuClkFreqHz / 1000000;
}

/* ------------------------------------------------------------ */
/***	void JSTK_end(PmodJSTK* InstancePtr)
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
/***	int JSTK_SPIInit(XSpi *SpiInstancePtr)
**
**	Parameters:
**		XSpi *SpiInstancePtr: a pointer to the joystick spi device
**
**	Return Value:
**		int Status: success or failure
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
/***	void JSTK_transfer(PmodJSTK* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodJSTK object to start
**		InstancePtr->led: the state to set the leds to
**
**	Return Values:
**		InstancePtr->X: the y axis position of the joystick
**		InstancePtr->Y: the x axis position of the joystick
**		InstancePtr->btn: the state of the push buttons
**
**	Errors:
**		none
**
**	Description:
**		Sets the leds
**		Gets the x and y positions of the joystick
**		Gets the push button states of the PmodJSTK
**
**	Problems:
**		Setting the chip select low through SetSlaveSelectReg isnt working so
**		the function now uses XilOut
*/
void JSTK_transfer(PmodJSTK* InstancePtr){

	//In order to correctly communicate with the Pmod, there needs to be a small delay between
	//each spi read
	u8 recv[5] = {0};
	recv[0] = 0x80 | InstancePtr->led;

//	XSpi_SetSlaveSelect(&InstancePtr->JSTKSpi, 1);
	XSpi_SetSlaveSelectReg(&InstancePtr->JSTKSpi,	&InstancePtr->JSTKSpi.SlaveSelectReg);
	JSTK_delay(InstancePtr, 20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[0], &recv[0], 1);
	JSTK_delay(InstancePtr, 20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[1], &recv[1], 1);
	JSTK_delay(InstancePtr, 20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[2], &recv[2], 1);
	JSTK_delay(InstancePtr, 20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[3], &recv[3], 1);
	JSTK_delay(InstancePtr, 20);
	XSpi_Transfer(&InstancePtr->JSTKSpi, &recv[4], &recv[4], 1);


//	XSpi_SetSlaveSelectReg(&InstancePtr->JSTKSpi,	&InstancePtr->JSTKSpi.SlaveSelectMask);
//	XSpi_SetSlaveSelect(&InstancePtr->JSTKSpi, 0);
	//above doesn't seem to work so manually set it in memory
	Xil_Out32(InstancePtr->JSTKSpi.BaseAddr+0x70, 1);

	InstancePtr->btn = (recv[4] >> 1) & 0x3;
	InstancePtr->Y = (recv[3] << 8) | recv[2];
	InstancePtr->X = (recv[1] << 8) | recv[0];
}

/* ------------------------------------------------------------ */
/***	void JSTK_delay(int micros)
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
**		Delays for a given amount of microseconds. Adapted from sleep and MB_Sleep
**
**	Problems:
**		none
*/
void JSTK_delay(PmodJSTK *InstancePtr, int micros){
	int i;
#ifdef __MICROBLAZE__
	for(i = 0; i < (InstancePtr->ItersPerUSec*micros); i++){
		asm("");
	}
#else
	usleep(micros);
#endif
}






