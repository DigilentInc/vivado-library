#include "platform.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "PmodTMP3.h"

void DemoInitialize();
void DemoRun();
void DemoCleanup();


PmodTMP3 myDevice;

int main(void)
{
	DemoInitialize();
	DemoRun();
	DemoCleanup();
	return 0;
}

void DemoInitialize()
{
	init_platform();//initialize UART connection.
	xil_printf("\x1B[H");//Move Terminal Cursor to top left
	xil_printf("\x1B[1K");//Clear Terminal
	xil_printf("Connected to PmodTMP3 Demo over UART\n\r");

	TMP3_begin(&myDevice, XPAR_PMODTMP3_0_AXI_LITE_IIC_BASEADDR, TMP3_ADDR);
	xil_printf("Connected to PmodTMP3 over IIC on JB\n\r");
}

void DemoRun()
{
	double temp = 0.0;

    while(1) {
    	temp = TMP3_getTemp(&myDevice);
    	temp = TMP3_CtoF(temp);
    	xil_printf("\x1B[3;0H");//Move Terminal Cursor to beginning of current row(3)
    	xil_printf("\x1B[1K");//Clear Terminal
    	xil_printf("Temperature: %d.%d\n\r", (int)temp, (int)(temp*100.0)%100);//print float as in %.2f
    	TMP3_delay(500000);//wait for a half second
    }
}

void DemoCleanup()
{
	TMP3_end(&myDevice);
	xil_printf("PmodTMP3 Disconnected\n\r");

	xil_printf("Closing UART Connection\n\r");
	cleanup_platform();
}
