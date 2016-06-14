/************************************************************************/
/*																		*/
/* PmodGPIO.c	--		C file for the PmodGPIO IP driver				*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe`											*/
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
/*	This file defines the available functions associated with the		*/
/*	PmodGPIO IP 														*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/9/2016(SamL): Created 											*/
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodGPIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***	void GPIO_begin(PmodGPIO* InstancePtr, u32 GPIO_Address, u8 bitmap)
**
**	Parameters:
**		InstancePtr: A PmodGPIO object to start
**		GPIO_Address: The Base address of the PmodGPIO GPIO
**		bitmap: The 8 bit representation of the pins to set the tristate
**				0 -> output		1 -> input
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodGPIO and set the tristate
*/
void GPIO_begin(PmodGPIO* InstancePtr, u32 GPIO_Address, u8 bitmap)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	Xil_Out32(InstancePtr->GPIO_addr+4, bitmap);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
}

/* ------------------------------------------------------------ */
/***	void GPIO_getPins(PmodGPIO* InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodGPIO object to use
**
**	Return Value:
**		data: An 8 bit representation of the GPIO pins
**
**	Errors:
**		none
**
**	Description:
**		Reads the 8 states of the GPIO pins and returns them in an 8 bit number
*/
u8 GPIO_getPins(PmodGPIO* InstancePtr){

	u32 data = 0;
	data = Xil_In32(InstancePtr->GPIO_addr);

	return data;
}

/* ------------------------------------------------------------ */
/***	void GPIO_setPins(PmodGPIO* InstancePtr, u8 value)
**
**	Parameters:
**		InstancePtr: A PmodGPIO object to start
**		value: The 8 bit number that will be assigned to the output
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets all 8 outputs at once
*/
void GPIO_setPins(PmodGPIO* InstancePtr, u8 value){

	Xil_Out32(InstancePtr->GPIO_addr, value);

}

/* ------------------------------------------------------------ */
/***	u8 GPIO_getPin(PmodGPIO* InstancePtr, u8 pinNumber)
**
**	Parameters:
**		InstancePtr: A PmodGPIO object to start
**		pinNumber: number of Pmod pin to read from (1-8)
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Sets all 8 outputs at once
*/
u8 GPIO_getPin(PmodGPIO* InstancePtr, u8 pinNumber){

	u32 data = 0;
	data = Xil_In32(InstancePtr->GPIO_addr);
	data = data & (1<<(pinNumber-1));

	if(data)
		return 1;
	else
		return 0;
}

/* ------------------------------------------------------------ */
/***	void GPIO_setPin(PmodGPIO* InstancePtr, u8 pinNumber, u8 value)
**
**	Parameters:
**		InstancePtr: A PmodGPIO object to start
**		pinNumber: number of Pmod pin to write to (1-8)
**		Value: Value to write to the pin
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodGPIO.
*/
void GPIO_setPin(PmodGPIO* InstancePtr, u8 pinNumber, u8 value){

	u32 data = 0;

	data = Xil_In32(InstancePtr->GPIO_addr);

	if(value != 0){
		Xil_Out32(InstancePtr->GPIO_addr, (data | (1 << (pinNumber-1))));
	}
	else{
		Xil_Out32(InstancePtr->GPIO_addr, (data & ~(1 << (pinNumber-1))));
	}
}

/* ------------------------------------------------------------ */
/***	void delay(int micros)
**
**	Parameters:
**		micros: amount of microseconds to delay
**
**	Return Value:
**		none
**
**	Errors:
**		does not delay the same time in microblaze and zynq
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


