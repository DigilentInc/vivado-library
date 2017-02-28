/***************************************************************************/
/*																		   */
/*	main.c		--		Definition for PmodR2R Resistor Ladder DAC demo	   */
/*																		   */
/***************************************************************************/
/*	Author:		Arthur Brown											   */
/*	Copyright 2017, Digilent Inc.										   */
/***************************************************************************/
/*  File Description:													   */
/*	This file defines a demonstration for the use of the Pmod R2R ip       */
/*  core. A 3.3V Sin wave with period 360ms is created on the Vout pin.	   */
/*																		   */
/*	A UART connection is used - but is not required for demo functionality */
/*	to connect, use a serial terminal such as TeraTerm attached to the 	   */
/*	programmed FPGA's port at the proper baud rate						   */
/*																		   */
/*   UART TYPE   BAUD RATE                        						   */
/*   uartns550   9600													   */
/*   uartlite    Configurable only in HW design							   */
/*   ps7_uart    115200 (configured by bootrom/bsp)						   */
/*																		   */
/***************************************************************************/
/*  Revision History:													   */
/*	02/28/2017(ArtVVB): validated										   */
/***************************************************************************/


#include "xparameters.h"
#include "xil_cache.h"
#include "PmodR2R.h"
#include "math.h"
#include "xil_printf.h"
#ifdef __MICROBLAZE__
	#include "microblaze_sleep.h"
	//(deprecated in Vivado 2016.4)
#else
	#include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();
void DemoSleep(int millis);

PmodR2R myDevice;

int main(void)
{
	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoRun()
{
	int i=0;
    xil_printf("Demo Initialized, producing 3.3 V sin wave, T=360ms\n\r");

    while(1)
    {
    	R2R_writeVoltage(&myDevice, 3.3*(sin(6.28*(double)i/360.0)+1.0)/2.0);//convert degrees to radians, output 0->3.3V sin wave
    	i = (i >= 359) ? (0) : (i+1);
		DemoSleep(1);
    }
}

void DemoInitialize()
{
	EnableCaches();
	R2R_begin(&myDevice, XPAR_PMODR2R_0_AXI_LITE_GPIO_BASEADDR);
}

void DemoCleanup()
{
	DisableCaches();
}

void DemoSleep(int millis)
{
#ifdef __MICROBLAZE__
	MB_Sleep(millis);
#else
	usleep(1000 * millis);//delay for param microseconds
#endif
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
