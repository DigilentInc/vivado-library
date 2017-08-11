/************************************************************************/
/*                                                                      */
/*     main.c --     PmodACL Example Project                            */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*     Author: Thomas Kappenman                                         */
/*     Copyright 2016-2017, Digilent Inc.                               */
/*                                                                      */
/************************************************************************/
/*  Module Description:                                                 */
/*          This file contains code for running a demonstration of the  */
/*          PmodACL2 when used with the PmodACL2 IP core.               */
/*                                                                      */
/*          X, Y, and Z acceleration data in units of g are printed to  */
/*          a serial terminal application ten times per second.         */
/*          Baud rates to set up this connection should be 115200 for   */
/*          a Zynq device and whatever the AXI UART LITE IP is          */
/*          configured for for a Microblaze device - typically 9600 or  */
/*          115200 baud.                                                */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*      03/23/2016(TKappenman): Created                                 */
/*      08/15/2016(jpeyron): Sleep and Zynq include fixes               */
/*      08/11/2017(artvvb): Validated for Vivado 2015.4                 */
/*                                                                      */
/************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodACL.h"

#include <stdio.h>

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include <sleep.h>
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void DemoSleep(u32 millis);
void EnableCaches();
void DisableCaches();

PmodACL acl;

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
    ACL_begin(&acl, XPAR_PMODACL_0_AXI_LITE_GPIO_BASEADDR,XPAR_PMODACL_0_AXI_LITE_SPI_BASEADDR);
    ACL_SetMeasure(&acl, 0);
    ACL_SetGRange(&acl, ACL_PAR_GRANGE_PM4G);
    ACL_SetMeasure(&acl, 1);
    ACL_CalibrateOneAxisGravitational(&acl, ACL_PAR_AXIS_ZP);
    DemoSleep(1000); // After calibration, some delay is required for the new settings to take effect.
}

void DemoRun()
{
    float x, y, z;

    while (1)
    {
        ACL_ReadAccelG(&acl, &x, &y, &z);
        printf("X=%f\tY=%f\tZ=%f\n\r", x, y, z);
        DemoSleep(100);
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
    usleep(1000 * millis);
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
