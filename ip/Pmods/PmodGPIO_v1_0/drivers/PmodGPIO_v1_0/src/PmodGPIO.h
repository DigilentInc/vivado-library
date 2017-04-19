/************************************************************************/
/*																		*/
/* PmodGPIO.h	--		Basic GPIO IP Driver							*/
/************************************************************************/
/*  Author:   Samuel Lowe                   							*/
/*  Copyright 2016-2017, Digilent Inc.                   				*/
/************************************************************************/
/*  File Description:                         							*/
/*                                    									*/
/*  Header file for the PmodGPIO IP	driver								*/
/*                                    									*/
/************************************************************************/
/*  Revision History:                         							*/
/*                                   									*/
/*  06/08/2016(SLowe): Created                     						*/
/*	04/19/2019(ArtVVB): Validated for 2015.4							*/
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

typedef struct PmodGPIO{
	u32 GPIO_addr;
	u32 cpuClockFreqHz;
} PmodGPIO;

void GPIO_begin(PmodGPIO* InstancePtr, u32 GPIO_Address, u8 bitmap, u32 cpuClockFreqHz);
u8 GPIO_getPins(PmodGPIO* InstancePtr);
void GPIO_setPins(PmodGPIO* InstancePtr, u8 value);
u8 GPIO_getPin(PmodGPIO* InstancePtr, u8 pinNumber);
void GPIO_setPin(PmodGPIO* InstancePtr, u8 pinNumber, u8 value);
void GPIO_delay(PmodGPIO* InstancePtr, int micros);

#endif // PmodGPIO_H