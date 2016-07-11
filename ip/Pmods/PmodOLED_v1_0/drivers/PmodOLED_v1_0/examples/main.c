/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "xil_printf.h"

#include "PmodOLED.h"
//#include "OLED.h"

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
**		Handles opening and closing UART connection and demo function calls.
*/
int main()
{
    init_platform();

	DemoInitialize();
	DemoRun();
	DemoCleanup();

    cleanup_platform();
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
	OLED_Begin(&myDevice, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR);
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
