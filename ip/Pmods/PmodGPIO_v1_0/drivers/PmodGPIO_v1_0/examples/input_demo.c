/************************************************************************/
/*																		*/
/* input_demo.c		demo project for the PmodGPIO IP 					*/
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
/*	This demo_project reads 4 inputs individually then all 4 together	*/
/*	and then prints the results out over UART							*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/9/2016(SamL): Created 											*/
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
#include "PmodGPIO.h"

PmodGPIO myDevice;


int main()
{
    init_platform();

    int pin1 = 0;
    int pin2 = 0;
    int pin3 = 0;
    int pin4 = 0;
    int all_pins = 0;

    print("GPIO Demo\n\r");

    GPIO_begin(&myDevice, XPAR_PMODGPIO_0_AXI_LITE_GPIO_BASEADDR, 0xFF);

    while(1){
		//read individually
    	pin1 = GPIO_getPin(&myDevice, 1);
    	pin2 = GPIO_getPin(&myDevice, 2);
    	pin3 = GPIO_getPin(&myDevice, 3);
    	pin4 = GPIO_getPin(&myDevice, 4);
		//read all together
    	all_pins = GPIO_getPins(&myDevice);
		//print individual reads and the total read masked to the first four bits
    	xil_printf("switch 1: %d   switch 2: %d   switch 3: %d   switch 4: %d   all pins: %d\n\r", pin1, pin2, pin3, pin4, all_pins & 0x0f);
    }

    cleanup_platform();
    return 0;
}
