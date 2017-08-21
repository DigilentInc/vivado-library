/*************************************************************************/
/*                                                                       */
/*     main.c --     PmodCLS Example Projects                            */
/*                                                                       */
/*************************************************************************/
/*     Author: Mikel Skreen                                              */
/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2009 - 2017 Digilent, Inc.  All rights reserved.        */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files            */
/* (the "Software"), to deal in the Software without restriction,        */
/* including without limitation the rights to use, copy, modify, merge,  */ 
/* publish, distribute, sublicense, and/or sell copies of the Software,  */
/* and to permit persons to whom the Software is furnished to do so,     */
/* subject to the following conditions:                                  */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/*included in all copies or substantial portions of the Software.        */
/*                                                                       */
/* Use of the Software is limited solely to applications:                */
/* (a) running on a Digilent device, or                                  */
/* (b) that interact with a Digilent device through a bus or             */
/*     interconnect.                                                     */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/ 
/* IN NO EVENT SHALL Digilent BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,       */
/* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE    */
/* OR OTHER DEALINGS IN THE SOFTWARE.                                    */
/*                                                                       */
/* Except as contained in this notice, the name of the Digilent shall not*/
/* be used in advertising or otherwise to promote the sale, use or other */
/* dealings in this Software without prior written authorization from    */
/* Digient.                                                              */
/*                                                                       */
/*************************************************************************/
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





