/************************************************************************/
/*																		*/
/*	PmodR2R.h		--		Declaration for PmodR2R library		 	    */
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*	This file declares Pmod R2R library functions.						*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created Template								*/
/*	06/13/2016(ArtVVB): Edited for PmodR2R								*/
/*	02/21/2017(ArtVVB): Validated for 2015.4							*/
/*	08/25/2017(ArtVVB): Validated for 2016.4							*/
/*																		*/
/************************************************************************/

#ifndef PmodR2R_H
#define PmodR2R_H

/****************** Include Files ********************/

#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"

/******************* Definitions *********************/

typedef struct PmodR2R {
	u32 GPIO_addr;
} PmodR2R;

void R2R_begin(PmodR2R* InstancePtr, u32 GPIO_Address);
void R2R_write(PmodR2R *InstancePtr, u32 data);
	
#endif // PmodR2R_H
