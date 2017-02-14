/*************************************************************************/
/*                                                                       */
/* PmodENC.c	--		Source file for Pmod Encoder driver library		 */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Brown                                              */
/*     Copyright 2016, Digilent Inc.                                     */
/*************************************************************************/
/*  Module Description:                                                  */
/*                                                                       */
/*            This file contains functions to capture and evaluate the   */
/*            state of the pmod's rotary shaft encoder                   */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*	04/19/2016(TommyK): Created 								 	     */
/*	06/10/2016(ArtVVB): Adapted for PmodENC								 */
/*	02/14/2017(ArtVVB): Validated										 */
/*																		 */
/*************************************************************************/

/***************************** Include Files *****************************/

#include "PmodENC.h"

/************************** Function Definitions *************************/

/* ------------------------------------------------------------ */
/***	void ENC_begin(PmodENC* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodENC object to start
**		GPIO_Address: The Base address of the PmodENC GPIO
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
