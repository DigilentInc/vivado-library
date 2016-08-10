
#include "PmodDPG1.h"
#include "xparameters.h"
#include "xil_cache.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif

void DemoInitialize(); // initializes
void DemoRun();// reads the data and prints the integer and the analog floating point number

PmodDPG1 myDevice;

// pressure unit types
#define KPA		0
#define ATM		1
#define PSI		2
#define INH20	3
#define CMH20	4
#define MMHG	5

int whole; // used to get the whole number for the float
int not_whole;// used to get the non whole number for the float

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

    DPG1_begin(&myDevice, XPAR_PMODDPG1_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{


	xil_printf("starting...\n\r");

    while(1)
    {

		int rawData;
		double pressureValue;
		DPG1_readData(&myDevice);

		// read raw data from PmodDPG1 and print it
		rawData = (int)myDevice.data ;
		xil_printf("Raw data: %d \n\r", rawData); // format text to be displayed

		// convert raw data into units of kPa and print it
		pressureValue = DPG1_GetPressure(KPA,&myDevice);
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("kPa: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		// convert raw data into units of atm and print it
		pressureValue = DPG1_GetPressure(ATM, &myDevice);
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("atm: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		// convert raw data into units of and print it
		pressureValue = DPG1_GetPressure(PSI, &myDevice);
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("psi: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		// convert raw data into units of and print it
		pressureValue = DPG1_GetPressure(INH20,&myDevice);
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("inH20: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		// convert raw data into units of and print it
		pressureValue = DPG1_GetPressure(CMH20,&myDevice );
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("cmH20: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		// convert raw data into units of and print it
		pressureValue = DPG1_GetPressure(MMHG, &myDevice);
		whole = pressureValue; // getting whole number for the physical value
		not_whole= (pressureValue - whole) * 1000000; // getting the non whole number of the physical value
		xil_printf("mmHg: %d.%06d \n\r", whole, not_whole); // format text to be displayed

		xil_printf("\n\r\n\r");

#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(1200);
#else
		sleep(2);
#endif
    }

    DPG1_end(&myDevice);
    cleanup_platform();
}
