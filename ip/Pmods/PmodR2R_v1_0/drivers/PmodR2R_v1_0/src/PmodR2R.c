/************************************************************************/
/*																		*/
/*	PmodR2R.c		--		Definitions for the PmodR2R library		 	*/
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*	This file defines the R2R library functions to initialize the IP 	*/
/* 	core and support writing real voltages.								*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 										*/
/*	06/13/2016(ArtVVB): Edited for PmodR2R								*/
/*	02/28/2017(ArtVVB): Validated										*/
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/

#include "PmodR2R.h"

/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***	void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodR2R object to start
**		GPIO_Address: The Base address of the PmodR2R GPIO
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodR2R.
*/
void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	Xil_Out32(InstancePtr->GPIO_addr+4, 0x00);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
}

/* ------------------------------------------------------------ */
/***	void R2R_writeVoltage(PmodR2R* InstancePtr, double voltage)
**
**	Parameters:
**		InstancePtr: A PmodR2R object to start
**		voltage: the voltage level to output, accepts 0 to 3.3 Volts
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Set output voltage of R2R.
*/
void R2R_writeVoltage(PmodR2R* InstancePtr, double voltage)
{
	u32 uv;
	if (voltage > 3.3)
	{
		uv = 0xFF;
	}
	else if (voltage < 0.0)
	{
		uv = 0x00;
	}
	else
	{
		uv = (int)(255.0 * voltage / 3.3);
	}
	Xil_Out32(InstancePtr->GPIO_addr, uv);
}
