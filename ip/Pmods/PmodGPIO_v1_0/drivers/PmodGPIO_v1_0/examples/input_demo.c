/************************************************************************/
/*																		*/
/* input_demo.c		demo project for the PmodGPIO IP 					*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe`											*/
/*	Copyright 2016-2017, Digilent Inc.									*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This demo_project reads 4 inputs individually then all 4 together	*/
/*	and then prints the results out over UART to a Serial Terminal		*/
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
#include "xil_printf.h"
#include "xil_cache.h"
#include "PmodGPIO.h"

PmodGPIO myDevice;

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

int main()
{
	DemoInitialize();
	DemoRun();
	DemoCleanup();
    return 0;
}

void DemoInitialize()
{
	EnableCaches();
    GPIO_begin(&myDevice, XPAR_PMODGPIO_0_AXI_LITE_GPIO_BASEADDR, 0x00, CPU_CLK_FREQ_HZ);
}

int DemoRun()
{
    int pin1 = 0;
    int pin2 = 0;
    int pin3 = 0;
    int pin4 = 0;
    int all_pins = 0;

    print("GPIO Demo\n\r");

    while(1)
	{
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
}

void DemoCleanup()
{
	DisableCaches();
}

void EnableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#endif
}