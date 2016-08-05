#include "xparameters.h"
#include "xil_cache.h"
#include "PmodBT2.h"

void DemoInitialize();
void DemoRun();
void PolledMode();
void IntMode();
void sent(int bytes);
void recvd(int bytes);
void timeout(int bytes);


PmodBT2 myDevice;


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
	xil_printf("Starting...\r\n");
	BT2_begin(&myDevice, XPAR_PMODBT2_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODBT2_0_AXI_LITE_UART_BASEADDR);
}


void DemoRun()
{
#ifndef NO_IRPT
	IntMode();
#else
	PolledMode();
#endif
}

void PolledMode(){
	int len;
	while(1){
		//Polled mode, if no interrupts
		if((len=BT2_getData(&myDevice, 600))){
			//echo back to sender
			BT2_sendData(&myDevice, myDevice.recv, len);
		}
	}
}

void IntMode(){

#ifndef NO_IRPT //If there are interrupts
#ifdef XPAR_MICROBLAZE_ID
		BT2_SetupInterruptSystem(&myDevice, XPAR_PMODBT2_0_DEVICE_ID, XPAR_AXI_INTC_0_PMODBT2_0_BT2_UART_INTERRUPT_INTR, recvd, sent, timeout);
#else
		BT2_SetupInterruptSystem(&myDevice, XPAR_PS7_SCUGIC_0_DEVICE_ID, XPAR_FABRIC_PMODBT2_0_BT2_UART_INTERRUPT_INTR, recvd, sent, timeout);//Setup interrupts, Zynq
#endif
#endif
		XUartNs550_Recv(&myDevice.BT2Uart, (u8*)myDevice.recv, 600);//Wait for new sentence

	while(1){}
}


void sent(int bytes){
	xil_printf("%d bytes sent\r\n", bytes);
}

void recvd(int bytes){
	XUartNs550_Recv(&myDevice.BT2Uart, (u8*)myDevice.recv, 600);//Wait for new sentence
	xil_printf("RDATA: %s \r\n", myDevice.recv);
}

void timeout(int bytes){
	xil_printf("Timeout: %d bytes received so far\r\n", bytes);
	if(myDevice.recv[bytes-1]=='\n'){
		myDevice.recv[bytes]='\0';
		XUartNs550_Send(&myDevice.BT2Uart, (u8*)&myDevice.recv, bytes);
		XUartNs550_Recv(&myDevice.BT2Uart, (u8*)&myDevice.recv, 600);//Wait for new sentence
	}
}
