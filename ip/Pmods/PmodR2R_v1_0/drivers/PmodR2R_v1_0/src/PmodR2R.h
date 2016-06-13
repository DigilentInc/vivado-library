/************************************************************************/
/*																		*/
/* PmodR2R.c	--		Template driver for a Pmod which uses GPIO		*/
/*																		*/
/************************************************************************/
/*	Author:		Thomas Kappenman, Arthur Brown										*/
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
/*	for the PmodR2R (insert name, duh). 								*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 	
/*	06/13/2016(ArtVVB): Edited for PmodR2R									*/
/*																		*/
/************************************************************************/


#ifndef PmodR2R_H
#define PmodR2R_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0



typedef struct PmodR2R{
	u32 GPIO_addr;
}PmodR2R;

	void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address);
	void R2R_writeVoltage(PmodR2R* InstancePtr, double voltage);
	void R2R_delay(int millis);
#endif // PmodR2R_H
