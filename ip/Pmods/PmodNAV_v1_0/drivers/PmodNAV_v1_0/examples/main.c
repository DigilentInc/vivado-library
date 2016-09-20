#include "xparameters.h"
#include "xil_cache.h"
#include "PmodNAV.h"

void DemoInitialize();
void DemoRun();

PmodNAV myDevice;

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
	NAV_begin(&myDevice, XPAR_PMODNAV_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODNAV_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{

}


