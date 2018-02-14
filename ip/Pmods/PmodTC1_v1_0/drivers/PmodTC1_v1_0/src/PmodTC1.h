/************************************************************************/
/*																		*/
/*	PmodR2R.h		--		Declaration for Pmod TC1 library		 	*/
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*	This file declares the TC1 library functions.						*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	03/01/2017(ArtVVB): Validated for Vivado 2015.4						*/
/*																		*/
/************************************************************************/

#ifndef PMODTC1_H
#define PMODTC1_H

/****************** Include Files ********************/

#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodTC1{
	XSpi TC1Spi;
	u32 data;
}PmodTC1;

void TC1_begin(PmodTC1* InstancePtr, u32 SPI_Address);
void TC1_end(PmodTC1* InstancePtr);
int TC1_SPIInit(XSpi *SpiInstancePtr);
void TC1_readData(PmodTC1* InstancePtr);
double TC1_getTemp(PmodTC1* InstancePtr);
double TC1_getAMBTemp(PmodTC1* InstancePtr);
double TC1_tempC2F(double celsius);

#endif // PMODTC1_H
