/****************************************************************************/
/*                                                                          */
/*  main.c      --      Definition for Pmod DA1 demo                        */
/*                                                                          */
/****************************************************************************/
/*  Author:     Jon Peyron                                                  */
/*  Copyright 2017, Digilent Inc.                                           */
/****************************************************************************/
/*  File Description:                                                       */
/*  This file defines a demonstration for the use of the Pmod DA1 ip        */
/*  core. A 3V peak-to-peak triangle wave of an unspecified frequency is    */
/*  output from the first digital to analog converter (pins A1 and B1)      */
/*                                                                          */
/*  to connect to UART, use a serial terminal such as TeraTerm attached to  */
/*  the programmed FPGA's port at the proper baud rate                      */
/*                                                                          */
/*   UART TYPE   BAUD RATE                                                  */
/*   uartns550   9600                                                       */
/*   uartlite    Configurable only in HW design                             */
/*   ps7_uart    115200 (configured by bootrom/bsp)                         */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*  Revision History:                                                       */
/*  06/20/2016(JonP): Created                                               */
/*  04/28/2017(ArtVVB): validated                                           */
/*                                                                          */
/****************************************************************************/

#include "PmodDA1.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_printf.h"

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

PmodDA1 myDevice;

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
    DA1_begin(&myDevice, XPAR_PMODDA1_0_AXI_LITE_SPI_BASEADDR);
}

void DemoRun()
{
    float dMaxValue = 3.0;
    float dMinValue = 0.0;
    float dStep = 0.05;
    float dValue;

    xil_printf("Starting Pmod DA1 demo...\n\r");

    while(1)
    {
        // increase physical value from minimum to maximum value
        for(dValue = dMinValue; dValue <= dMaxValue; dValue += dStep)
        {
            // send value to the DA converter
            DA1_WritePhysicalValue(&myDevice, dValue);
        }

        // decrease physical value from maximum to minimum value
        for(dValue = dMaxValue; dValue >= dMinValue; dValue -= dStep)
        {
            // send value to the DA converter
            DA1_WritePhysicalValue(&myDevice, dValue);
        }
    }
}

void DemoCleanup() {
    DA1_end(&myDevice);
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
