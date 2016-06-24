
#include "PmodAD1.h"
#include "xparameters.h"
#include "xil_cache.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif
unsigned int wValue; // unsigned 16 bit variable to store integer value
    	float fValue;  // float variable to store physical value
    	float dReference = 3.32; //used in getting the physical value
    	int whole; // used to get the whole number for the float
    	int not_whole;// used to get the non whole number for the float
void DemoInitialize(); // initializes
void DemoRun();// reads the data and prints the integer and the analog floating point number

PmodAD1 myDevice;

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

    AD1_begin(&myDevice, XPAR_PMODAD1_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{


	xil_printf("starting...\n\r");

    while(1)
    {
    	wValue =AD1_GetIntegerValue(&myDevice);  // read integer value
    	fValue = AD1_GetPhysicalValue(dReference,&myDevice);  // read physical value
    	whole = fValue; // getting whole number for the physical value
    	not_whole= (fValue - whole) * 1000000; // getting the non whole number of the physical value
    	xil_printf("Integer value: %d, Physical Value = %d.%06d", wValue, whole, not_whole); // format text to be displayed
    	xil_printf("\n\r\n\r");

#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(500);
#else
		sleep(1);
#endif
    }

    AD1_end(&myDevice);
    cleanup_platform();
}
