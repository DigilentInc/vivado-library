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
/*	This file contains a template that you can use to create a library	*/
/*	for the PmodENC (insert name, duh). 								*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 										*/
/*	06/10/2016(ArtVVB): Adapted for PmodENC								*/
/*																		*/
/************************************************************************/


#ifndef PmodENC_H
#define PmodENC_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */

#define ENC_GPIO_PIN_A   0x1
#define ENC_GPIO_PIN_B   0x2
#define ENC_GPIO_PIN_BTN 0x4
#define ENC_GPIO_PIN_SWT 0x8

typedef struct PmodENC{
	u32 GPIO_addr;
}PmodENC;

void ENC_begin(PmodENC* InstancePtr, u32 GPIO_Address);

u32 ENC_getState(PmodENC* InstancePtr);
int ENC_getRotation(u32 state, u32 laststate);
int ENC_switchOn(u32 state);
int ENC_buttonPressed(u32 state);

#endif // PmodENC_H
