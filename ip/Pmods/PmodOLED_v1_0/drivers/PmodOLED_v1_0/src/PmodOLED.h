/************************************************************************/
/*																		*/
/* PmodOLED.h	--		Header for PmodOLED IP Drivers and Library		*/
/*																		*/
/************************************************************************/
/*	Author:		Thomas Kappenman										*/
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
/*	Contains drivers and library functions for use with a PmodOLED
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(TommyK): Created 										*/
/*	06/30/2016(ArtVVB): Updated for PmodOLED IP Drivers					*/
/* 12/15/2016(jPeyron) edited for better use for OnboardOLED in         */
/* as well as inverting the white and black                             */
/************************************************************************/

#ifndef PmodOLED_H
#define PmodOLED_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"
#include "xparameters.h"

#ifdef XPAR_MICROBLAZE_ID
	#include "microblaze_sleep.h"
#else
	#include "sleep.h"
#endif

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0

#define OledColMax 	128	//number of columns in the display
#define OledRowMax 	32	//number of rows in the display
#define OledPageMax	4	//number of display pages
#define OledModeSet	0	//set pixel drawing mode
#define OledModeOr 	1	//or pixels drawing mode
#define OledModeAnd	2	//and pixels drawing mode
#define OledModeXor	3	//xor pixels drawing mode

#define	cbOledDispMax	512		//max number of bytes in display buffer

#define	ccolOledMax		128		//number of display columns
#define	crowOledMax		32		//number of display rows
#define	cpagOledMax		4		//number of display memory pages

#define	cbOledChar		8		//font glyph definitions is 8 bytes long
#define	chOledUserMax	0x20	//number of character defs in user font table
#define	cbOledFontUser	(chOledUserMax*cbOledChar)

/* Graphics drawing modes.
*/
#define	modOledSet		0
#define	modOledOr		1
#define	modOledAnd		2
#define	modOledXor		3

typedef struct OLED {
	uint8_t		rgbOledBmp[cbOledDispMax];
	/* Coordinates of current pixel location on the display. The origin
	** is at the upper left of the display. X increases to the right
	** and y increases going down.
	*/
	int			xcoOledCur;
	int			ycoOledCur;

	uint8_t *	pbOledCur;			//address of byte corresponding to current location
	int			bnOledCur;			//bit number of bit corresponding to current location
	uint8_t		clrOledCur;			//drawing color to use
	uint8_t *	pbOledPatCur;		//current fill pattern
	int			fOledCharUpdate;

	int			dxcoOledFontCur;
	int			dycoOledFontCur;

	uint8_t    *pbOledFontCur;
	uint8_t    *pbOledFontUser;

	uint8_t	  (*pfnDoRop)(uint8_t bPix, uint8_t bDsp, uint8_t mskPix);
	int			modOledCur;

	int			xchOledCur;
	int			ychOledCur;

	int			xchOledMax;
	int			ychOledMax;

	uint8_t    *pbOledFontExt;

	uint8_t		rgbOledFontUser[cbOledFontUser];
} OLED;

typedef struct PmodOLED {
	u32 GPIO_addr;
	XSpi OLEDSpi;
	OLED OLEDState;
} PmodOLED;

extern XSpi_Config OLEDConfig;

void OLED_Begin					(PmodOLED* InstancePtr, u32 GPIO_Address, u32 SPI_Address, bool orientation, bool invert);
void OLED_End					(PmodOLED* InstancePtr);
int  OLED_SPIInit				(XSpi *SPIInstancePtr);
u8 	 OLED_ReadByte				(PmodOLED* InstancePtr);
void OLED_WriteByte				(PmodOLED* InstancePtr, u8 cmd);
void OLED_WriteSPI				(PmodOLED* InstancePtr, u8 reg, u8 *wData, int nData);
void OLED_ReadSPI				(PmodOLED* InstancePtr, u8 reg, u8 *rData, int nData);
void OLED_SetRegisterBits		(PmodOLED* InstancePtr, u8 reg, u8 mask, bool fValue);
u8 	 OLED_GetRegisterBits		(PmodOLED* InstancePtr, u8 bRegisterAddress, u8 bMask);
void OLED_SetGPIOBits			(PmodOLED* InstancePtr, u8 mask, bool fValue);
void OLED_SetGPIOTristateBits	(PmodOLED* InstancePtr, u8 mask, bool fValue);
void OLED_Delay					(int millis);

/* ------------------------------------------------------------ */
/*					OLED Driver Procedure Declarations			*/
/* ------------------------------------------------------------ */

void OLED_ClearBuffer			(PmodOLED *InstancePtr);
void OLED_Init					(PmodOLED *InstancePtr, u32 GPIO_Address, u32 SPI_Address, bool orientation, bool invert);
void OLED_Term					(PmodOLED *InstancePtr);
void OLED_DisplayOn				(PmodOLED *InstancePtr);
void OLED_DisplayOff			(PmodOLED *InstancePtr);
void OLED_Clear					(PmodOLED *InstancePtr);
void OLED_Update				(PmodOLED *InstancePtr);

/* ------------------------------------------------------------ */
/*					OLED Graph Procedure Declarations			*/
/* ------------------------------------------------------------ */

void OLED_SetDrawColor			(PmodOLED *InstancePtr, uint8_t clr);
void OLED_SetDrawMode			(PmodOLED *InstancePtr, int mod);
int	 OLED_GetDrawMode			(PmodOLED *InstancePtr);
u8 	*OLED_GetStdPattern			(int ipat);
void OLED_SetFillPattern		(PmodOLED *InstancePtr, uint8_t *pbPat);

void OLED_MoveTo				(PmodOLED *InstancePtr, int xco, int yco);
void OLED_GetPos				(PmodOLED *InstancePtr, int *pxco, int *pyco);
void OLED_DrawPixel				(PmodOLED *InstancePtr);
u8	 OLED_GetPixel				(PmodOLED *InstancePtr);
void OLED_LineTo				(PmodOLED *InstancePtr, int xco, int yco);
void OLED_DrawRect				(PmodOLED *InstancePtr, int xco, int yco);
void OLED_FillRect				(PmodOLED *InstancePtr, int xco, int yco);
void OLED_GetBmp				(PmodOLED *InstancePtr, int dxco, int dyco, uint8_t *pbBmp);
void OLED_PutBmp				(PmodOLED *InstancePtr, int dxco, int dyco, uint8_t *pbBmp);
void OLED_DrawChar				(PmodOLED *InstancePtr, char ch);
void OLED_DrawString			(PmodOLED *InstancePtr, char *sz);

/* ------------------------------------------------------------ */
/*					OLED Char Procedure Declarations			*/
/* ------------------------------------------------------------ */

void OLED_SetCursor				(PmodOLED *InstancePtr, int xch, int ych);
void OLED_GetCursor				(PmodOLED *InstancePtr, int *pxcy, int *pych);
int	 OLED_DefUserChar			(PmodOLED *InstancePtr, char ch, uint8_t *pbDef);
void OLED_SetCharUpdate			(PmodOLED *InstancePtr, int f);
int	 OLED_GetCharUpdate			(PmodOLED *InstancePtr);
void OLED_PutChar				(PmodOLED *InstancePtr, char ch);
void OLED_PutString				(PmodOLED *InstancePtr, char *sz);

#endif // PmodOLED_H
