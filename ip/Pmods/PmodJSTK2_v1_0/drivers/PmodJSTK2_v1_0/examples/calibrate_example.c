/************************************************************************/
/*																		*/
/* calibrate_example.c		calibration project for the PmodJSTK IP 					*/
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
/*	This demo runs through the calibration proccess for the PmodJSTK2	*/
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

    init_platform();

    //init joystick
    JSTK2_begin(&joystick, XPAR_PMODJSTK2_0_AXI_LITE_SPI_BASEADDR);

    //reload calibration values from flash in case users run this sketch
    //multiple times without power-cycling the system board in order to
    //make sure that the original collected values are correct and not
    //the RAM values from the previous run-through.
    xil_printf("\n\rStarting up the sketch with a reloading of calibration values from Flash memory");
    JSTK2_rldFromFlash(&joystick);

    xil_printf("\n\rValues stored in flash:");

    xil_printf("\n\rpower-up smpXMin: %d", JSTK2_getCalXMin(&joystick));
    xil_printf("\n\rpower-up smpXMax: %d", JSTK2_getCalXMax(&joystick));
    xil_printf("\n\rpower-up smpYMin: %d", JSTK2_getCalYMin(&joystick));
    xil_printf("\n\rpower-up smpYMax: %d", JSTK2_getCalYMax(&joystick));
    xil_printf("\n\rpower-up smpXCenterMin: %d", JSTK2_getCalXCenMin(&joystick));
    xil_printf("\n\rpower-up smpXCenterMax: %d", JSTK2_getCalXCenMax(&joystick));
    xil_printf("\n\rpower-up smpYCenterMin: %d", JSTK2_getCalYCenMin(&joystick));
    xil_printf("\n\rpower-up smpYCenterMax: %d", JSTK2_getCalYCenMax(&joystick));



    xil_printf("\n\rcalibrating, rotate the Joystick around in all directions then let it rest for 1 second");

    JSTK2_calibrate(&joystick);

    xil_printf("\n\rcalibration complete!");

    JSTK2_setInversion(&joystick, 0, 1);

    xil_printf("\n\rSaving to flash memory....\n");

    JSTK2_writeFlash(&joystick);

    xil_printf("\n\rSave complete\n\n\r");

    xil_printf("\n\rsaved smpXMin: %d", JSTK2_getCalXMin(&joystick));
	xil_printf("\n\rsaved smpXMax: %d", JSTK2_getCalXMax(&joystick));
	xil_printf("\n\rsaved smpYMin: %d", JSTK2_getCalYMin(&joystick));
	xil_printf("\n\rsaved smpYMax: %d", JSTK2_getCalYMax(&joystick));
	xil_printf("\n\rsaved smpXCenterMin: %d", JSTK2_getCalXCenMin(&joystick));
	xil_printf("\n\rsaved smpXCenterMax: %d", JSTK2_getCalXCenMax(&joystick));
	xil_printf("\n\rsaved smpYCenterMin: %d", JSTK2_getCalYCenMin(&joystick));
	xil_printf("\n\rsaved smpYCenterMax: %d", JSTK2_getCalYCenMax(&joystick));



    cleanup_platform();
    return 0;
}
