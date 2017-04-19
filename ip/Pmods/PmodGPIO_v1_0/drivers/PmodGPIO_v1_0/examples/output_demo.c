/************************************************************************/
/*																		*/
/* output_demo.c	demo project for the PmodGPIO IP 					*/
/*																		*/
/************************************************************************/
/*	Author:		Samuel Lowe`											*/
/*	Copyright 2016-2017, Digilent Inc.									*/
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
/*	04/19/2019(ArtVVB): Validated for 2015.4							*/
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
#include "PmodGPIO.h"
#include "xil_cache.h"

PmodGPIO myDevice;

#ifdef __MICROBLAZE__
	#define CPU_CLK_FREQ_HZ   (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
	#define CPU_CLK_FREQ_HZ     (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif

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

void DemoRun()
{
    int count = 0;
    int i = 0;
    print("GPIO Output Demo\n\r");
	
    while(1)
	{
    	GPIO_setPin(&myDevice, count, 1);
    	if(count >= 8)
		{
    		count = 0;
    		for(i = 0; i < 4; i++)
			{
				GPIO_setPins(&myDevice, 0xFF);
				GPIO_delay(&myDevice, 300000);
				GPIO_setPins(&myDevice, 0x00);
				GPIO_delay(&myDevice, 300000);
    		}
    	}
    	else
    		count ++;
    	GPIO_delay(&myDevice, 200000);
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