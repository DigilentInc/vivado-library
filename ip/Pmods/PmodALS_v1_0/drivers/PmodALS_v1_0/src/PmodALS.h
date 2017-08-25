/************************************************************************/
/*                                                                      */
/* PmodALS.h    --      Driver definitions for the PmodALS              */
/*                                                                      */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This file contains the drivers for the PmodALS IP from Digilent     */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  03/31/2016(tkappenman): Created                                     */
/*  08/24/2017(artvvb): Validated for Vivado 2015.4                     */
/*                                                                      */
/************************************************************************/

#ifndef PMODALS_H
#define PMODALS_H

/****************** Include Files ********************/

#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*                  Definitions                                 */
/* ------------------------------------------------------------ */

typedef struct PmodALS {
    XSpi ALSSpi;
} PmodALS;

/* ------------------------------------------------------------ */
/*                  Procedure Declarations                      */
/* ------------------------------------------------------------ */

XStatus ALS_begin(PmodALS* InstancePtr, u32 SPI_Address);
XStatus ALS_SPIInit(XSpi *SpiInstancePtr);
u8 ALS_read(PmodALS* InstancePtr);

#endif // PMODALS_H
