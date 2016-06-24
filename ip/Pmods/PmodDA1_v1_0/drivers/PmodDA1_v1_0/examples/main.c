
#include "PmodDA1.h"
#include "xparameters.h"
#include "xil_cache.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif
  u8 status;
  int whole; // used to get the whole number for the float
  int not_whole;// used to get the non whole number for the float
  float dMaxValue = 3;
  float dMinValue = 0;
  float dStep = 0.05;
  float dValue;
void DemoInitialize(); // initializes
void DemoRun();// reads the data and prints the integer and the analog floating point number

PmodDA1 myDevice;

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

	DA1_begin(&myDevice, XPAR_PMODDA1_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{


	xil_printf("starting...\n\r");

    while(1)
    {
    	 // increase physical value from minimum to maximum value
    	  for(dValue = dMinValue; dValue <= dMaxValue; dValue += dStep)
    	  {
    		  whole = dValue; // getting whole number for the physical value
    		  not_whole= (dValue - whole) * 1000; // getting the non whole number of the physical value
    	    // send value to the DA converter
    	    status = DA1_WritePhysicalValue(dValue,&myDevice);
    	
    	  }

    	  // decrease physical value from maximum to minimum value
    	   for(dValue = dMaxValue; dValue >= dMinValue; dValue -= dStep)
    	   {
    		   whole = dValue; // getting whole number for the physical value
    		   not_whole= (dValue - whole) * 1000; // getting the non whole number of the physical value
    	     // send value to the DA converter
    		   status= DA1_WritePhysicalValue(dValue,&myDevice);
    		
    	   }

    }

    DA1_end(&myDevice);
    cleanup_platform();
}
