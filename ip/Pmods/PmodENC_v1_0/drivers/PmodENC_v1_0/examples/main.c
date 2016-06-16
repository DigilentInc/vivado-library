#include "xparameters.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "PmodENC.h"

#ifdef XPAR_MICROBLAZE_ID
	#include "microblaze_sleep.h"
#else // Going on the basis that if it isn't a Microblaze system then it is a Zynq system
	#include "sleep.h"
#endif


void DemoInitialize();
void DemoRun();

PmodENC myDevice;

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
	ENC_begin(&myDevice, XPAR_PMODENC_0_AXI_LITE_GPIO_BASEADDR);
}


void DemoRun()
{
	u32 state, laststate;//comparing current and previous state to detect edges on GPIO pins.
	int ticks = 0;
	printf("Running PmodENC Demo\n\r");
	laststate = ENC_getState(&myDevice);
	while (1) {
		state = ENC_getState(&myDevice);
		if (ENC_switchOn(state))//using switch as an enable for counter
		{
			ticks += ENC_getRotation(state, laststate);
		}
		if (ENC_buttonPressed(state) && !ENC_buttonPressed(laststate))//only print on button posedge
		{
			xil_printf("ticks = %d\n\r", ticks);
		}
		laststate = state;
		
		#ifdef XPAR_MICROBLAZE_ID
			MB_Sleep(1);//1ms
		#else
			usleep(1000);//delay makes the program ignore noisy parts of the incoming signal (possibly including fast intended presses)
		#endif
	}
}


