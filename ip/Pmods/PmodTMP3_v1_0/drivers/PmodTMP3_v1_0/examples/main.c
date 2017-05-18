/*************************************************************************/
/*                                                                       */
/*     main.c --     PmodTMP3 Example Projects                           */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Brown                                              */
/*     Copyright 2016, Digilent Inc.                                     */
/*************************************************************************/
/*  Module Description:                                                  */
/*                                                                       */
/*            This file contains code for running a demonstration of the */
/*            PmodTMP3 when used with the PmodTMP3 IP core.              */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            6/9/2016(ABrown): Created                                  */
/* 			  5/8/2017(jPeyron): updated                                 */
/*                                                                       */
/*************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodTMP3.h"
#include "xil_printf.h"
#include <stdio.h>

#ifdef __MICROBLAZE__
    #include "microblaze_sleep.h"
    //(deprecated in Vivado 2016.4)
#else
    #include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();

PmodTMP3 myDevice;

#ifdef __MICROBLAZE__
    #define CPU_CLOCK_FREQ_HZ (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
    #define CPU_CLOCK_FREQ_HZ (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif




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
	xil_printf("\x1B[H");//Move Terminal Cursor to top left
	xil_printf("\x1B[1K");//Clear Terminal
	xil_printf("Connected to PmodTMP3 Demo over UART\n\r");

	TMP3_begin(&myDevice, XPAR_PMODTMP3_0_AXI_LITE_IIC_BASEADDR, TMP3_ADDR, CPU_CLOCK_FREQ_HZ );
	xil_printf("Connected to PmodTMP3 over IIC on JB\n\r\n\r");
}

void DemoRun()
{
	double temp = 0.0;
	double temp2 = 0.0;
	double temp3 = 0.0;

    while(1) {
    	temp = TMP3_getTemp(&myDevice);
    	temp2 = TMP3_CtoF(temp);
        temp3 = TMP3_FtoC(temp2);
       	printf("Temperature:  %.02lf  in Fahrenheit\n\r", temp2);//print double to 2 decimal places
    	printf("Temperature:  %.02lf  in Celcius\n\r\n\r", temp3 );//print double to 2 decimal places
    	TMP3_delay(&myDevice, 1000000);//delay
    }
}

void DemoCleanup()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#endif
	TMP3_end(&myDevice);
	xil_printf("PmodTMP3 Disconnected\n\r");
	xil_printf("Closing UART Connection\n\r");

}
