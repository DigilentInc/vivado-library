/************************************************************************/
/*																		*/
/* basic_example.c		demo project for the PmodJSTK IP 					*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe`											*/
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
/*	This demo reads the calibrated data from the PmodJSTK2 and prints	*/
/*	the X and Y values while mapping the axis to the LED. 				*/
/*	If the buttons on the JSTK are pressed, the LED will turn green 	*/	
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/27/2016(SamL): Created 											*/
/*																		*/
/************************************************************************/
/*  Baud Rates:															*/
/*																		*/
/*	Microblaze: 9600 or what was specified in UARTlite core				*/
/*	Zynq: 115200														*/
/*																		*/
/************************************************************************/

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "PmodJSTK2.h"

PmodJSTK2 joystick;

int main()
{
	u16 Ydata;
	u16 Xdata;

    init_platform();

    //init joystick
    JSTK2_begin(&joystick, XPAR_PMODJSTK2_0_AXI_LITE_SPI_BASEADDR);

	//set no inversion
    JSTK2_setInversion(&joystick, 0, 1);

    xil_printf("\n\rJoystick Demo\n");


    while(1){
		//get coord values
    	Ydata = JSTK2_getY(&joystick);
    	Xdata = JSTK2_getX(&joystick);
    	xil_printf("\n\rX: %d     Y: %d	   BTN: %x",Xdata, Ydata, JSTK2_getBtns(&joystick));
    	delay(50000);
		
		//set led from btns and axis
    	if(JSTK2_getBtns(&joystick))
    		JSTK2_setLed(&joystick, 0, 255, 0);
    	else
    		JSTK2_setLed(&joystick, Xdata, 0, Ydata);

    }

    cleanup_platform();
    return 0;
}
