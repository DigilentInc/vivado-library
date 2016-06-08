#include "xparameters.h"
#include "xil_cache.h"
#include "PmodTC1.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif

void DemoInitialize();
void DemoRun();

PmodTC1 myDevice;

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{
	init_platform();

    TC1_begin(&myDevice, XPAR_PMODTC1_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	double celsius, fahrenheit;
	xil_printf("starting...\n\r");

    while(1)
    {
    	celsius = TC1_getTemp(&myDevice);
    	fahrenheit = TC1_celToFar(celsius);
		xil_printf("\n\r\n\r");
		xil_printf("Temperature: %d.%d deg C\n\r", (int)celsius, (int)(celsius*100)%100);
		xil_printf("Temperature: %d.%d deg F\n\r", (int)fahrenheit, (int)(fahrenheit*100)%100);
#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(500);
#else
		sleep(1);
#endif
    }

    TC1_end(&myDevice);
    cleanup_platform();
}
