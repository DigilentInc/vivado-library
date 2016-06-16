#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "platform.h"
#include "PmodCLS.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif

void DemoInitialize();
void DemoRun();

PmodCLS myDevice;
char    szInfo1[0x20];
char    szInfo2[0x20];

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	DemoInitialize();
	DemoRun();

	cleanup_platform();

	return 0;
}

void DemoInitialize()
{
	init_platform();
	CLS_begin(&myDevice, XPAR_PMODCLS_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "  PmodCLS Demo");
    strcpy(szInfo2, "  Hello World!");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);
#ifdef XPAR_MICROBLAZE_ID
    MB_Sleep(1000);
#else
    usleep(500000);
#endif

    while(1){
    CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "->PmodCLS Demo<- ");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);

#ifdef XPAR_MICROBLAZE_ID
    MB_Sleep(1000);
#else
    usleep(500000);
#endif

    CLS_DisplayClear(&myDevice);
    strcpy(szInfo1, "  PmodCLS Demo   ");
    CLS_WriteStringAtPos(&myDevice, 0, 0, szInfo1);
    CLS_WriteStringAtPos(&myDevice, 1, 0, szInfo2);

#ifdef XPAR_MICROBLAZE_ID
    MB_Sleep(1000);
#else
    usleep(500000);
#endif
    }
}
