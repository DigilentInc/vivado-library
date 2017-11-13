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
/*	08/25/2017(ArtVVB): Validated for 2016.4							*/
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

	//set tristate direction to all output
	Xil_Out32(InstancePtr->GPIO_addr+4, 0x00);
}

/* ------------------------------------------------------------ */
/***	void R2R_write(PmodR2R* InstancePtr, u32 data)
**
**	Parameters:
**		InstancePtr: A PmodR2R object to write to
**		data: 8 bits of data to write to the R2R gpio pins. 0x00:0xFF at gpio -> 0:Vdd at Vout
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
void R2R_write(PmodR2R *InstancePtr, u32 data)
{
	Xil_Out32(InstancePtr->GPIO_addr, data);
}
