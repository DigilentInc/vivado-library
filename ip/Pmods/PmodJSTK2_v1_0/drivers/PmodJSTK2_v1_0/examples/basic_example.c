/************************************************************************/
/*                                                                      */
/* basic_example.c      demo project for the PmodJSTK IP                    */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*  Author:     Samuel Lowe`                                            */
/*  Copyright 2016, Digilent Inc.                                       */
/*                                                                      */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This demo reads the calibrated data from the PmodJSTK2 and prints   */
/*  the X and Y values while mapping the axis to the LED.               */
/*  If the buttons on the JSTK are pressed, the LED will turn green     */
/*                                                                      */
/*  Data can be received with a serial terminal application connected   */
/*  to your board and configured to use the appropriate baud rate below.*/
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/27/2016(SamL): Created                                           */
/*                                                                      */
/************************************************************************/
/*  Baud Rates:                                                         */
/*                                                                      */
/*  Microblaze: 9600 or what was specified in UARTlite core             */
/*  Zynq: 115200                                                        */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include "xil_printf.h"
#include "PmodJSTK2.h"
#include "xil_cache.h"

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#define CPU_CLOCK_FREQ_HZ (XPAR_CPU_CORE_CLOCK_FREQ_HZ)
#else
#include "sleep.h"
#define CPU_CLOCK_FREQ_HZ (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ)
#endif

PmodJSTK2 joystick;

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

    //initialize the joystick device
    JSTK2_begin(
            &joystick,
            XPAR_PMODJSTK2_0_AXI_LITE_SPI_BASEADDR,
            XPAR_PMODJSTK2_0_AXI_LITE_GPIO_BASEADDR,
            CPU_CLOCK_FREQ_HZ
    );

    //set inversion register to invert only the Y axis
    JSTK2_setInversion(&joystick, 0, 1);
}

void DemoRun()
{
    JSTK2_Position position;
    JSTK2_DataPacket rawdata;

    xil_printf("\r\nJoystick Demo\r\n");

    while(1){
        //get joystick x and y coordinate values
        position = JSTK2_getPosition(&joystick);
        //get button states
        rawdata = JSTK2_getDataPacket(&joystick);

        xil_printf("X:%d\tY:%d%s%s\r\n",
                position.XData,
                position.YData,
                (rawdata.Jstk != 0) ? "\tJoystick pressed" : "",
                (rawdata.Trigger != 0) ? "\tTrigger pressed" : ""
        );
        JSTK2_delay(&joystick, 50000);

        //set led from btns and axis
        if(rawdata.Jstk != 0 || rawdata.Trigger != 0)
            JSTK2_setLedRGB(&joystick, 0, 255, 0);
        else
            JSTK2_setLedRGB(&joystick, position.XData, 0, position.YData);
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
