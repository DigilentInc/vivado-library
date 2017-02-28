/************************************************************************/
/*																		*/
/*	HYGROI2C.h		--		Declaration for HYGROI2C library		 	*/
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*	This file declares the R2R library functions and the constants		*/
/*	involved.															*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 										*/
/*	06/13/2016(ArtVVB): Edited for PmodR2R								*/
/*	02/21/2017(ArtVVB): Validated										*/
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

typedef struct PmodR2R{
	u32 GPIO_addr;
}PmodR2R;

	void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address);
	void R2R_writeVoltage(PmodR2R* InstancePtr, double voltage);
	void R2R_delay(int millis);
#endif // PmodR2R_H
