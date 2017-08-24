/************************************************************************/
/*                                                                      */
/* main.c      demo project for the PmodAD2 IP                         */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  Author:     Arthur Brown                                            */
/*  Copyright 2016, Digilent Inc.                                       */
/*                                                                      */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This demo reads conversion data captured from each of the four		*/
/*  channels of the AD2. This data is printed to the terminal.          */
/*                                                                      */
/*  Data should be viewed with a serial terminal application          	*/
/*  connected to your board and configured to use the appropriate baud  */
/*  rate below.                                                         */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/13/2016(artvvb): Created                                         */
/*  08/24/2017(artvvb): Validated for Vivado 2015.4                     */
/*                                                                      */
/************************************************************************/
/*  Baud Rates:                                                         */
/*                                                                      */
/*  Microblaze: 9600 or other as specified in UARTlite core             */
/*  Zynq: 115200                                                        */
/*                                                                      */
/************************************************************************/

#include <stdio.h>

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodAD2.h"

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void DemoSleep(u32 millis);
void EnableCaches();
void DisableCaches();

PmodAD2 myDevice;

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
	AD2_begin(&myDevice, XPAR_PMODAD2_0_AXI_LITE_IIC_BASEADDR, AD2_IIC_ADDR);
}


void DemoRun()
{
	u16 conv;
	u8 channel;
	double voltage;

	xil_printf("Demo Started\n\r");

	//turn on all channels
	AD2_WriteConfig(&myDevice, AD2_DEFAULT_CONFIG);

	xil_printf("AD2 Configured\n\r");

	while(1)
	{
		AD2_ReadConv(&myDevice, &conv);

		//scale captured data such that 0x000:0xFFF => 0.0:3.3
		voltage = (double)(conv & AD2_DATA_MASK) * 3.3 / (AD2_DATA_MASK + 1);

		//pull channel read information out of conv
		channel = (conv & AD2_CHANNEL_MASK) >> AD2_CHANNEL_BIT;

		printf("CHANNEL %d = %02f V%s",
				channel,
				voltage,
				(channel == 3) ? "\r\n" : "    "
		);

		DemoSleep(10);
	}
}

void DemoCleanup()
{
	DisableCaches();
}

void DemoSleep(u32 millis)
{
#ifdef __MICROBLAZE__
	MB_Sleep(millis);
#else
	usleep(1000*millis);
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
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#endif
}
