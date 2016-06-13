/************************************************************************/
/*																		*/
/* PmodR2R.c	--		Template driver for a Pmod which uses GPIO		*/
/*																		*/
/************************************************************************/
/*	Author:		Thomas Kappenman, Arthur Brown									*/
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
/*	for the PmodR2R (insert name, duh). 
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 
/*	06/13/2016(ArtVVB): Edited for PmodR2R
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodR2R.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***	void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodR2R object to start
**		GPIO_Address: The Base address of the PmodR2R GPIO
**		SPI_Address: The Base address of the PmodR2R SPI
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

/* ------------------------------------------------------------ */
/***	void R2R_delay(int millis)
**
**	Parameters:
**		millis: number of milliseconds to delay for
**
**	Return Value:
**		none
**
**	Errors:
**		massively imprecise on microblaze, measured output period of demo sin wave at 800ms instead of 360ms.
**
**	Description:
**		Delay program for a number of milliseconds.
*/
void R2R_delay(int millis)
{	
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(millis);
#else
	usleep(1000*millis);
#endif
}
