/************************************************************************/
/*																		*/
/* PmodENC.c	--		Template driver for a Pmod which uses GPIO		*/
/*																		*/
/************************************************************************/
/*	Author:		Thomas Kappenman, Arthur Brown							*/
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
/*	for the PmodENC (insert name, duh). 
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 
/*	06/10/2016(ArtVVB): Adapted for PmodENC
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodENC.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***	void ENC_begin(PmodENC* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodENC object to start
**		GPIO_Address: The Base address of the PmodENC GPIO
**		SPI_Address: The Base address of the PmodENC SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodENC.
*/
void ENC_begin(PmodENC* InstancePtr, u32 GPIO_Address)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	Xil_Out32(InstancePtr->GPIO_addr+4, 0xFF);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
}

/* ------------------------------------------------------------ */
/***	u32 ENC_getState(PmodENC* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodENC object to interact with
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		returns the current state of the encoder GPIO pins.
*/
u32 ENC_getState(PmodENC* InstancePtr)
{
	u32 rvalue = 0;
	rvalue = Xil_In32(InstancePtr->GPIO_addr) & 0xF;
	return rvalue;
}

/* ------------------------------------------------------------ */
/***	int ENC_getRotation(u32 state, u32 laststate)
**
**	Parameters:
**		state, the most recent GPIO Pin state
**		laststate, the second most recent GPIO Pin state
**
**	Return Value:
**		The direction of motion of the encoder shaft
**
**	Errors:
**		Sign is currently arbitrary, not mapped onto a physical clockwise/counterclockwise format
**
**	Description:
**		checks states to see if there has been a positive edge on pin A, if not, no movement
**		if so, checks pin B to see whether it's waveform is leading or following pin A's
**		
*/
int ENC_getRotation(u32 state, u32 laststate)
{
	//	if posedge(pinA), then
	//		if B is low, return RIGHT
	//		if B is high, return LEFT
	if ((state & ENC_GPIO_PIN_A) != 0 && (laststate & ENC_GPIO_PIN_A) == 0)
	{
		if ((state & ENC_GPIO_PIN_B) != 0)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

/* ------------------------------------------------------------ */
/***	int ENC_buttonPressed(u32 state)
**
**	Parameters:
**		state, the most recent GPIO Pin state
**
**	Return Value:
**		nonzero if the shaft of the PmodENC is pressed in, otherwise zero
**
**	Errors:
**		Assumes button is active high
**
**	Description:
**		simple wrapper function for determining the button gpio state
**		
**		
*/
int ENC_buttonPressed(u32 state)
{
	return state & ENC_GPIO_PIN_BTN;
}

/* ------------------------------------------------------------ */
/***	int ENC_switchOn(u32 state)
**
**	Parameters:
**		state, the most recent GPIO Pin state
**
**	Return Value:
**		nonzero if the switch of the PmodENC is on, otherwise zero
**
**	Errors:
**		none
**
**	Description:
**		simple wrapper function for determining the switch gpio state
**		
**		
*/
int ENC_switchOn(u32 state)
{
	return state & ENC_GPIO_PIN_SWT;
}
