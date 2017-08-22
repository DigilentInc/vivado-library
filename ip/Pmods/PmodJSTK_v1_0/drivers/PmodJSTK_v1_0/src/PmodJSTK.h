/************************************************************************/
/*																		*/
/* PmodJSTK.h	--		Driver definitions for the PmodJSTK			*/
/*																		*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains the drivers for the PmodJSTK IP from Digilent	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/01/2016(SamL): Created											*/
/*	08/21/2017(ArtVVB): Validated for Vivado 2015.4						*/
/*																		*/
/************************************************************************/

#ifndef PMODJSTK_H
#define PMODJSTK_H

/* ------------------------------------------------------------ */
/*					Include Files 								*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*					SPI Command Definitions						*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */

// Define bit positions used in fsButtons.
#define JSTK_bnJstk                  0
#define JSTK_bnButton1               1
#define JSTK_bnButton2               2

#define JSTK_bitButton1              (1 << JSTK_bnButton1)
#define JSTK_bitButton2              (1 << JSTK_bnButton2)
#define JSTK_bitJstk                 (1 << JSTK_bnJstk)

/* ------------------------------------------------------------ */
/*					Device Definition							*/
/* ------------------------------------------------------------ */

typedef struct PmodJSTK {
	XSpi SpiDevice;
	u32 GpioAddr;
	u32 ItersPerUSec;
	u8 LedState;
} PmodJSTK;

/* ------------------------------------------------------------ */
/*					Data Type Declarations						*/
/* ------------------------------------------------------------ */

typedef struct JSTK_DataPacket {
	u16 XData;
	u16 YData;
	u8 Jstk;
	u8 Button1;
	u8 Button2;
} JSTK_DataPacket;

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address, u32 GPIO_Address, u32 cpuClockFreqHz);
void JSTK_end(PmodJSTK* InstancePtr);
int JSTK_SPIInit(XSpi *SpiInstancePtr);

void JSTK_setLeds(PmodJSTK* InstancePtr, u8 leds);

JSTK_DataPacket JSTK_getDataPacket(PmodJSTK* InstancePtr);

//utility functions
void JSTK_getData(PmodJSTK* InstancePtr, u8* recv, u8 nData);
void JSTK_delay(PmodJSTK *InstancePtr, int micros);

#endif // PMODJSTK_H
