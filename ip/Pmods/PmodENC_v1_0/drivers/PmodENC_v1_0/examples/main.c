/*************************************************************************/
/*                                                                       */
/*     main.c --     PmodENC Example Project                             */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Brown                                              */
/*     Copyright 2017, Digilent Inc.                                     */
/*************************************************************************/
/*  Module Description:                                                  */
/*                                                                       */
/*            This file contains code for running a demonstration of the */
/*            Pmod Encoder when used with the PmodENC IP core.           */
/*                                                                       */
/*            Rotating the Pmod ENC's rotary shaft increments or 	 */
/*	      	decrements a counter, depending on direction of 	 */
/*		rotation. 						 */
/*            The Pmod ENC's switch controls whether the counter is 	 */
/*		enabled. 						 */
/*            Pressing the Pmod ENC's shaft in prints the current count  */
/*                                                                       */
/*            Requires a Serial Terminal (such as Tera Term) set to 9600 */
/* 			  baud for Arty, 112500 for Zynq   		 */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            2/13/2017(ArtVVB): Validated                               */
/*                                                                       */
/*************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "PmodENC.h"

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

PmodENC myDevice;

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
	ENC_begin(&myDevice, XPAR_PMODENC_0_AXI_LITE_GPIO_BASEADDR);
}

void DemoCleanup()
{
	DisableCaches();
}

void DemoRun()
{
	u32 state, laststate;//comparing current and previous state to detect edges on GPIO pins.
	int ticks = 0;
	xil_printf("Running PmodENC Demo\n\r");
	laststate = ENC_getState(&myDevice);
	while (1) {
		state = ENC_getState(&myDevice);
		if (ENC_switchOn(state))//using switch as an enable for counter
		{
			ticks += ENC_getRotation(state, laststate);
		}
		if (ENC_buttonPressed(state) && !ENC_buttonPressed(laststate))//only print on button posedge
		{
			xil_printf("ticks = %d\n\r", ticks);
		}
		laststate = state;
		DemoSleep(1);
	}
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

