/*************************************************************************/
/*                                                                       */
/*     main.c --     PmodAD1 Example Project                             */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Brown                                              */
/*     Copyright 2017, Digilent Inc.                                     */
/*************************************************************************/
/*  Module Description:                                                  */
/*                                                                       */
/*            This file contains code for running a demonstration of the */
/*            PmodAD1 when used with the PmodAD1 IP core.              	 */
/*																		 */
/*			  This demo initializes the PmodAD1 IP core and then polls   */
/*			  it's sample register, printing the analog voltage last     */
/*			  sampled by each of the AD1's two channels over UART.   	 */
/*																		 */
/*            Messages printed by this demo can be received by using a   */
/*			  serial terminal configured with the appropriate baud rate. */
/*		      115200 for Zynq systems, and whatever the AXI UARTLITE IP  */
/*			  is configured with for Microblaze systems - typically 9600 */
/*			  or 115200 baud.											 */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            08/15/2017(ArtVVB): Created                                */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xil_cache.h"
#include "PmodAD1.h"

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

PmodAD1 myDevice;
const float ReferenceVoltage = 3.3;

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

int main ()
{
	DemoInitialize();
	DemoRun();
	DemoCleanup();
	return 0;
}

void DemoInitialize()
{
	EnableCaches();

	AD1_begin(&myDevice, XPAR_PMODAD1_0_AXI_LITE_SAMPLE_BASEADDR);

	//wait for AD1 to finish powering on
#ifdef __MICROBLAZE__
	MB_Sleep(1); // 1 ms
#else
	usleep(1); // 1 us (minimum)
#endif
}

void DemoRun()
{
	AD1_RawData RawData;
	AD1_PhysicalData PhysicalData;

	while (1) {
		AD1_GetSample (&myDevice, &RawData); // capture raw samples
		AD1_RawToPhysical(ReferenceVoltage, RawData, &PhysicalData); // convert raw samples into floats scaled to 0 - VDD
		printf("Input Data 1: %02f\t\t", PhysicalData[0]);
		printf("Input Data 2: %02f\r\n", PhysicalData[1]);

		// do this 10x per second
		#ifdef __MICROBLAZE__
			MB_Sleep(100);
		#else
			usleep(100000);
		#endif
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
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheDisable();
#endif
#endif
}
