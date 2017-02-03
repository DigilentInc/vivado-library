
/************************************************************************/
/*																		*/
/* input_demo.c		demo project for the PmodGPIO IP 					*/
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2016, Digilent Inc.										*/
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
/*	This demo_project initializes and uses the PmodOLED to display strings and show different available fill patterns.							*/
/*																		*/
/*  Revision History:													*/
/*																		*/
/*	06/20/2016(ArtVVB): Created 										*/
/*                                                                      */
/* 12/15/2016(jPeyron) edited for better use for OnboardOLED in         */
/* as well as inverting the white and black                             */
/*                                                                      */
/************************************************************************/
/*  Baud Rates:															*/
/*																		*/
/*	Microblaze: 9600 or what was specified in UARTlite core				*/
/*	Zynq: 115200														*/
/*																		*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include "xparameters.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "PmodOLED.h"

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

PmodOLED myDevice;

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */



void DemoInitialize();
void DemoRun();
void DemoCleanup();

//to change betweeen PmodOLED and OnBoardOLED is to change Orientation 
bool orientation = false;  //set up for Normal PmodOLED(false) vs normal Onboard OLED(true)
bool invert = false;       //true = whitebackground/black letters      false = black background /white letters

/* ------------------------------------------------------------ */
/*				Function Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/***	main()
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Handles opening caches and wraps demo function calls.
*/
int main()
{
    Xil_ICacheEnable();
    Xil_DCacheEnable();

    DemoInitialize();
    DemoRun();
    DemoCleanup();

    return 0;
}

/* ------------------------------------------------------------ */
/***	DemoInitialize()
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initializes SPI and GPIO connections for PmodOLED instance.
*/
void DemoInitialize()
{
	OLED_Begin(&myDevice, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR, orientation, invert);
}

/* ------------------------------------------------------------ */
/***	DemoRun()
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		If demo shut down without proper exit, could damage PmodOLED.
**
**	Description:
**		Displays Demo message and each available Fill Pattern.
**		Pauses between runs to check if user wants to continue, if not, exits.
**		To be safe, exit through prompt before closing demo.
**		Requires UART connection to terminal program on PC.
*/
void DemoRun()
{

	int irow, ib, i;
	u8 *pat;
	char c;

	xil_printf("UART and SPI opened for PmodOLED Demo\n\r");

	while (1)
	{
		//Choosing Fill pattern 0
		pat = OLED_GetStdPattern(0);
		OLED_SetFillPattern(&myDevice, pat);
		//Turn automatic updating off
		OLED_SetCharUpdate(&myDevice, 0);

		//Draw a rectangle over writing then slide the rectangle
		//down slowly displaying all writing
		for (irow = 0; irow < OledRowMax; irow++)
		{
			OLED_ClearBuffer(&myDevice);
			OLED_SetCursor(&myDevice, 0, 0);
			OLED_PutString(&myDevice, "PmodOLED");
			OLED_SetCursor(&myDevice, 0, 1);
			OLED_PutString(&myDevice, "by Digilent");
			OLED_SetCursor(&myDevice, 0, 2);
			OLED_PutString(&myDevice, "Simple Demo");

			OLED_MoveTo(&myDevice, 0, irow);
			OLED_FillRect(&myDevice, 127, 31);
			OLED_MoveTo(&myDevice, 0, irow);
			OLED_LineTo(&myDevice, 127, irow);
			OLED_Update(&myDevice);
			OLED_Delay(100);
		}

		OLED_Delay(1000);
		// Blink the display three times.
		for (i = 0; i < 3; i++)
		{
			OLED_DisplayOff	(&myDevice);
			OLED_Delay		(500);
			OLED_DisplayOn	(&myDevice);
			OLED_Delay		(500);
		}
		OLED_Delay(2000);

		// Now erase the characters from the display
		for (irow = OledRowMax-1; irow >= 0; irow--) {
			OLED_SetDrawColor(&myDevice, 1);
			OLED_SetDrawMode(&myDevice, OledModeSet);
			OLED_MoveTo(&myDevice, 0, irow);
			OLED_LineTo(&myDevice, 127, irow);
			OLED_Update(&myDevice);
			OLED_Delay(25);
			OLED_SetDrawMode(&myDevice, OledModeXor);
			OLED_MoveTo(&myDevice, 0, irow);
			OLED_LineTo(&myDevice, 127, irow);
			OLED_Update(&myDevice);
		}

		OLED_Delay(1000);

		// Draw a rectangle in center of screen
		// Display the 8 different patterns available
		OLED_SetDrawMode(&myDevice, OledModeSet);

		for(ib = 1; ib < 8; ib++)
		{
			OLED_ClearBuffer(&myDevice);
			pat = OLED_GetStdPattern(ib);
			OLED_SetFillPattern(&myDevice, pat);
			OLED_MoveTo(&myDevice, 55, 1);
			OLED_FillRect(&myDevice, 75, 27);
			OLED_DrawRect(&myDevice, 75, 27);
			OLED_Update(&myDevice);

			OLED_Delay(1000);
		}
		xil_printf("(q)uit or any key to continue:\n\r");
		c = inbyte();
		if (c == 'q' || c == 'Q')
			break;
	}
	xil_printf("Exiting PmodOLED Demo\n\r");
}

/* ------------------------------------------------------------ */
/***	DemoCleanup()
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Tells PmodOLED to turn the display off.
*/
void DemoCleanup()
{
	OLED_End(&myDevice);
}
