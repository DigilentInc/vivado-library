/*************************************************************************/
/*                                                                       */
/*     main.c --     PmodCLS Example Projects                            */
/*                                                                       */
/*************************************************************************/
/*     Author: Mikel Skreen                                              */
/*                                                                       */
/*************************************************************************/
/*  File Description:                                                    */
/*                                                                       */
/*  This demo sends data through spi to the PmodCLS.                     */
/*  The demo alters the first string slighly so you can see that         */
/*  the data is changing between the different prints to the PmodCLS.    */
/*  Depending on the Rev of the PmodCLS the jumper configuration will    */
/*  be different. Make sure to refer to the Reference Manual for the     */
/*  correct jumper configuration.                                        */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            06/15/2016(MikelSkreen): Created                           */
/* 			  8/17/2017(jPeyron): updated                                */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/
/*  Baud Rates:                                                          */
/*                                                                       */
/*  Microblaze: 9600 or what was specified in UARTlite core              */
/*  Zynq: 115200                                                         */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "PmodCLS.h"


#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif



void DemoInitialize();
void DemoRun();
void DemoCleanup();



PmodCLS myDevice;
char    szInfo1[0x20];
char    szInfo2[0x20];

int main(void)
{

	DemoInitialize();
	DemoRun();
	DemoCleanup();


	return 0;
}

void DemoInitialize()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheEnable();
#endif
#endif

	CLS_begin(&myDevice, XPAR_PMODCLS_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "  PmodCLS Demo");
    strcpy(szInfo2, "  Hello World!");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);
#ifdef  __MICROBLAZE__
    MB_Sleep(1000);
#else
    usleep(500000);
#endif

    while(1){
    CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "->PmodCLS Demo<- ");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);

#ifdef __MICROBLAZE__
    MB_Sleep(1000);
#else
    usleep(500000);
#endif

    CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "  PmodCLS Demo   ");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);

#ifdef __MICROBLAZE__
    MB_Sleep(1000);
#else
    usleep(500000);
#endif
    }
}


void DemoCleanup()
{
	CLS_end(&myDevice);

#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#endif



}





