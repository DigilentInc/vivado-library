/***************************************************************************/
/*                                                                         */
/*	main.c		--		Definition for Pmod TC1 demo	 	               */
/*                                                                         */
/***************************************************************************/
/*	Author:		Arthur Brown											   */
/*	Copyright 2017, Digilent Inc.										   */
/***************************************************************************/
/*  File Description:													   */
/*	This file defines a demonstration for the use of the Pmod Thermocouple */
/*  IP core. Temperature data in Fahrenheit is streamed out over UART to a */
/*  serial terminal such as TeraTerm twice per second.                     */
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


#include "PmodTC1.h"
#include "xparameters.h"
#include "xil_cache.h"
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

PmodTC1 myDevice;

int main(void)
{
	DemoInitialize();
	DemoRun();
	DemoCleanup();
	return 0;
}

void DemoInitialize()
{
	EnableCaches();
    TC1_begin(&myDevice, XPAR_PMODTC1_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	double celsius, fahrenheit;
	xil_printf("Starting Pmod TC1 Demo...\n\r");

    while(1)
    {
    	celsius = TC1_getTemp(&myDevice);
    	fahrenheit = TC1_tempC2F(celsius);
		xil_printf("Temperature: %d.%d deg F   %d.%d deg C\n\r",
			(int)(fahrenheit),
			(int)(fahrenheit*100)%100,
			(int)(celsius),
			(int)(celsius*100)%100
		);
		DemoSleep(500);
    }
}


void DemoCleanup()
{
    TC1_end(&myDevice);
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
