/************************************************************************/
/*																		*/
/* PmodGPIO.c	--		Basic GPIO IP Driver*/
/************************************************************************/
/*                                    									*/
/* PmodKYPD.c --    Basic GPIO Pmod Driver   							*/
/*                                    									*/
/************************************************************************/
/*  Author:   Samuel Lowe                   							*/
/*  Copyright 2016, Digilent Inc.                   					*/
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
/*  File Description:                         							*/
/*                                    									*/
/*  Header file for the PmodGPIO IP	driver								*/
/*  																	*/
/*                                    									*/
/************************************************************************/
/*  Revision History:                         							*/
/*                                   									*/
/*  06/08/2016 SLowe: Created                     						*/
/*                                    									*/
/************************************************************************/


#ifndef PmodGPIO_H
#define PmodGPIO_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xil_io.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0

#ifdef XPAR_MICROBLAZE_ID
	#define ITERS_PER_USEC   (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
	#define ITERS_PER_USEC     (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif



typedef struct PmodGPIO{
	u32 GPIO_addr;
}PmodGPIO;

void GPIO_begin(PmodGPIO* InstancePtr, u32 GPIO_Address, u8 bitmap);
u8 GPIO_getPins(PmodGPIO* InstancePtr);
void GPIO_setPins(PmodGPIO* InstancePtr, u8 value);
u8 GPIO_getPin(PmodGPIO* InstancePtr, u8 pinNumber);
void GPIO_setPin(PmodGPIO* InstancePtr, u8 pinNumber, u8 value);
void delay(int micros);


#endif // PmodGPIO_H
