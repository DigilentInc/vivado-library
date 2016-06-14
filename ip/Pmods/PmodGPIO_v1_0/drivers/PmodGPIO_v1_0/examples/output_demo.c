/************************************************************************/
/*																		*/
/* output_demo.c	demo project for the PmodGPIO IP 					*/
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
/*	This demo_project assigns each output high one at a time and then 	*/
/*	toggles all outputs on and off 3 times								*/
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

    int count = 0;
    int i = 0;

    print("GPIO Output Demo\n\r");

    GPIO_begin(&myDevice, XPAR_PMODGPIO_0_AXI_LITE_GPIO_BASEADDR, 0x00);

    while(1){

    	GPIO_setPin(&myDevice, count, 1);
    	if(count >= 8){
    		count = 0;
    		for(i = 0;i < 4; i++){
				GPIO_setPins(&myDevice, 0xFF);
				delay(300000);
				GPIO_setPins(&myDevice, 0x00);
				delay(300000);
    		}
    	}
    	else
    		count ++;

    	delay(200000);

    }

    cleanup_platform();
    return 0;
}
