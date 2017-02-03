#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "PmodACL2.h"
#include "xparameters.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif


void DemoInitialize();
void DemoRun();
void SamplesExample();
void FIFOexample();

PmodACL2 myDevice;

int main(void)
{
	DemoInitialize();
	DemoRun();

	return 0;
}

void DemoInitialize()
{
	init_platform();
	ACL2_begin(&myDevice, XPAR_PMODACL2_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODACL2_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	print("Starting...\n\r");

//	SamplesExample();
	FIFOexample();

	ACL2_end(&myDevice);
	cleanup_platform();
}

void SamplesExample()
{
	int x, y, z;
	int status;

	while(1)
	{
		xil_printf("\x1B[2J");		// clear screen
		xil_printf("\x1B[H");		// reset cursor to 0,0

		status = ACL2_getStatus(&myDevice);
		xil_printf("Status: %x\n\r", status);

		x = ACL2_getX(&myDevice);
		y = ACL2_getY(&myDevice);
		z = ACL2_getZ(&myDevice);

		xil_printf("X= %d, Y= %d, Z= %d\n\r", x, y, z);

#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(500);
#else
		usleep(100000);
#endif
	}
}

void FIFOexample()
{
	int fifoEnt, status;

	while(1)
	{
		xil_printf("\x1B[2J");		// clear screen
		xil_printf("\x1B[H");		// reset cursor to 0,0

		status = ACL2_getStatus(&myDevice);
		xil_printf("Status: %x\n\r", status);

		fifoEnt = ACL2_getFIFOentries(&myDevice);
		xil_printf("FIFO: %d\n\r", fifoEnt);
		if(fifoEnt == 512)
		{
			xil_printf("Threshold triggered.\n\r");
			ACL2_fillFIFO(&myDevice);

#ifdef XPAR_MICROBLAZE_ID
			MB_Sleep(10);
#else
			usleep(1000000);
#endif

			xil_printf("FIFO: %d\n\r", ACL2_getFIFOentries(&myDevice));
			ACL2_printQueue(&myDevice);
			ACL2_myQueueEmptyAll(&myDevice);

#ifdef XPAR_MICROBLAZE_ID
			MB_Sleep(5000);
#else
			usleep(1000000);
#endif
		}
	}
}
