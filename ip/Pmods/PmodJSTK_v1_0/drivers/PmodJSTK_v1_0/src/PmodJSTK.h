/************************************************************************/
/*																		*/
/* PmodJSTK.h	--		driver definitions for the PmodJSTK				*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe												*/
/*	Copyright 2015, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains the driver definitions for the PmodJSTK IP       */
/*    from Digilent	                                                    */
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/01/2016(SamL): Created											*/
/*	04/07/2017(ArtVVB): Validated										*/
/*																		*/
/************************************************************************/

#ifndef PMODJSTK_H
#define PMODJSTK_H

/* ------------------------------------------------------------ */
/*					Include Files							    */
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*					Type Definitions							*/
/* ------------------------------------------------------------ */

#define bool u8
#define true 1
#define false 0


typedef struct PmodJSTK{
	XSpi JSTKSpi;
	u8 btn, led;
	u16 X, Y;
	u32 ItersPerUSec;
}PmodJSTK;

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address, u32 CpuClkFreqHz);
void JSTK_end(PmodJSTK* InstancePtr);
int JSTK_SPIInit(XSpi *SpiInstancePtr);

void JSTK_transfer(PmodJSTK* InstancePtr);
void JSTK_delay(PmodJSTK *InstancePtr, int micros);

#endif // PMODJSTK_H
