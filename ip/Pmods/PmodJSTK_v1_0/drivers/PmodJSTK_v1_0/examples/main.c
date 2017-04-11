/************************************************************************/
/*                                                                      */
/* PmodJSTK.h   --      demonstration main for the PmodJSTK             */
/*                                                                      */
/************************************************************************/
/*  Author:     Samuel Lowe                                             */
/*  Copyright 2015-2017, Digilent Inc.                                  */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*    This file contains code for running a demonstration of the        */
/*    Pmod Joystick when used with the PmodJSTK IP core.                */
/*                                                                      */
/*    The joystick X and Y data is sent over UART to a serial terminal  */
/*    - such as TeraTerm - at 115200 baud 10 times per second.          */
/*    The status of the two push buttons on the Pmod joystick are       */
/*    displayed on the two leds.                                        */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/01/2016(SamL): Created                                           */
/*  04/07/2017(ArtVVB): Validated                                       */
/*                                                                      */
/************************************************************************/

#include "PmodJSTK.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "xil_cache.h"
#ifdef __MICROBLAZE__
    #include "microblaze_sleep.h"
    //(deprecated in Vivado 2016.4)
#else
    #include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void DemoEnableCaches();
void DemoDisableCaches();
void DemoSleep();

PmodJSTK joystick;

#ifdef __MICROBLAZE__
    #define CPU_CLOCK_FREQ_HZ (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
    #define CPU_CLOCK_FREQ_HZ (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif

int main()
{
    DemoInitialize();
    DemoRun();
    DemoCleanup();
    return 0;
}

void DemoInitialize()
{
    DemoEnableCaches();
    JSTK_begin(&joystick, XPAR_PMODJSTK_0_AXI_LITE_SPI_BASEADDR, CPU_CLOCK_FREQ_HZ);
}

void DemoRun()
{
    xil_printf("Joystick Demo\n\r");

    joystick.led = 0;
    while (1) {
        JSTK_transfer(&joystick); // capture and send all data
        joystick.led = joystick.btn; // set the leds equal to the buttons, button 1 will turn on led 1
        xil_printf("X:%d Y:%d\n\r", joystick.X, joystick.Y); // print the status of the joystick
        DemoSleep(100); // delay for 100ms
    }
}

void DemoCleanup()
{
    DemoDisableCaches();
}

void DemoSleep(int millis)
{
#ifdef __MICROBLAZE__
    MB_Sleep(millis);
#else
    usleep(1000 * millis);//delay for param microseconds
#endif
}

void DemoEnableCaches()
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

void DemoDisableCaches()
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
