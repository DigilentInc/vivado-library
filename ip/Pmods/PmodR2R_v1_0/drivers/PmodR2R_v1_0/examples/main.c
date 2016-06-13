#include "xparameters.h"
#include "xil_cache.h"
#include "PmodR2R.h"
#include "platform.h"
#include "math.h"
//for math.h, use Project/Properties/CC++ Build/Settings/Arm Linker/Libraries/Add... the option m

void DemoInitialize();
void DemoRun();

PmodR2R myDevice;

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
	R2R_begin(&myDevice, XPAR_PMODR2R_0_AXI_LITE_GPIO_BASEADDR);
}

void DemoRun()
{
	int i=0;
    xil_printf("Demo Initialized, producing 3.3 V sin wave, T=360ms\n\r");

    while(1)
    {
    	R2R_writeVoltage(&myDevice, 3.3*(sin(6.28*(double)i/360.0)+1.0)/2.0);//convert degrees to radians, output 0->3.3V sin wave
    	i = (i == 359) ? (0) : (i+1);
		R2R_delay(1);
    }
}


