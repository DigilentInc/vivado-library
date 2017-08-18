/************************************************************************/
/*                                                                      */
/* PmodKYPD.c --    Demo for the use of the Pmod Keypad IP core         */
/*                                                                      */
/************************************************************************/
/*  Author:   Mikel Skreen                                              */
/*  Copyright 2016, Digilent Inc.                                       */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This demo continuously captures keypad data and prints a message	*/
/*  to an attached serial terminal whenever a positive edge is detected	*/
/*  on any of the sixteen keys. In order to receive messages, a serial  */
/*  terminal application on your PC should be connected to the 			*/
/*  appropriate COM port for the micro-USB cable connection to your 	*/
/*  board's USBUART port. The terminal should be configured with 8-bit 	*/
/*  data, no parity bit, 1 stop bit, and the appropriate baud rate for 	*/
/* 	your application. If you are using a Zynq board, use a baud rate of */
/*  115200, if you are using a Microblaze system, use the baud rate 	*/
/*  specified in the AXI UARTLITE IP, typically 115200 or 9600 baud.	*/
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/08/2016(MikelS): Created                                         */
/*  08/17/2017(artvvb): Validated for Vivado 2015.4						*/
/*                                                                      */
/************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodKYPD.h"

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void DisableCaches();
void EnableCaches();
void DemoSleep(u32 millis);

PmodKYPD myDevice;

//lookup table for the labels of the keys on the keypad
const char KeyIndexToLabel[16] = {'D', 'C', 'B', 'A', 'E', '9', '6', '3', 'F', '8', '5', '2', '0', '7', '4', '1'};

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
    KYPD_begin(&myDevice, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
}

void DemoRun()
{
    u32 count = 0;
    u16 kypdStateNew, kypdStateOld, kypdIndex;

    xil_printf("Button edges will be detected...\n\r");

    kypdStateOld = KYPD_getAllKeys(&myDevice);

    while (1)
    {
        kypdStateNew = KYPD_getAllKeys(&myDevice);

        for(count = 0; count < 16; count++)
        {
            kypdIndex = 1 << count;
            if((kypdStateNew & kypdIndex) != 0) // check if rising edge
            {
				if ((kypdStateOld & kypdIndex) == 0)
					xil_printf("button %c pressed\n\r", KeyIndexToLabel[count]);

            	DemoSleep(5); // ignore close-together edges
            }
        }

        kypdStateOld = kypdStateNew;
    }
}

void DemoSleep(u32 millis)
{
#ifdef __MICROBLAZE__
	MB_Sleep(millis);
#else
	usleep(millis*1000);
#endif
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
