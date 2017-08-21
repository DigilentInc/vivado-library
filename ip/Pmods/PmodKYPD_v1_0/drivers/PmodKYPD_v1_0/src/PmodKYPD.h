/************************************************************************/
/*                                    .                                 */
/* PmodKYPD.h --    Driver definitions for the Pmod Keypad		        */
/*                                    .                                 */
/************************************************************************/
/*  Author:   Mikel Skreen       .                                      */
/*  Copyright 2016, Digilent Inc.                                       */
/************************************************************************/
/*  File Description:                         .                         */
/*                                    .                                 */
/*  This library contains the definitions of the Pmod Keypad device and */
/*	the functions used to control it.									*/
/*                                    .                                 */
/************************************************************************/
/*  Revision History:                         .                         */
/*                                    .                                 */
/*  06/08/2016(MikelS): Created                     .                   */
/*  08/17/2017(artvvb): Validated for Vivado 2015.4						*/
/*                                    .                                 */
/************************************************************************/

#ifndef PMODKYPD_H
#define PMODKYPD_H

/****************************** Include Files ***************************/

#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

/**************************** Type Definitions **************************/

typedef struct PmodKYPD {
    u32 GPIO_addr;
} PmodKYPD;

/************************** Function Definitions ************************/

void KYPD_begin(PmodKYPD* InstancePtr, u32 GPIO_Address);
u8 KYPD_getKey(PmodKYPD* InstancePtr, u8 button);
u16 KYPD_getAllKeys(PmodKYPD* InstancePtr);

#endif // PmodKYPD_H
