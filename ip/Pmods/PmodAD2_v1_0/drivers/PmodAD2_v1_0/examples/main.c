/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodAD2.h"

#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

void DemoInitialize();
void DemoRun();
//static void SendHandler(PmodAD2 *InstancePtr);

//static void ReceiveHandler(PmodAD2 *InstancePtr);

PmodAD2 myDevice;

//void print(char *str);

int main()
{

    Xil_ICacheEnable();
	Xil_DCacheEnable();


	DemoInitialize();
	DemoRun();

    return 0;
}



void DemoInitialize()
{
	xil_printf("UART Established\n\r");
	AD2_begin(&myDevice, XPAR_PMODAD2_0_AXI_LITE_IIC_BASEADDR, AD2_IIC_ADDR);
	//AD2_SetupInterruptSystem(&myDevice, XPAR_PS7_SCUGIC_0_DEVICE_ID, XPAR_FABRIC_AXI_IIC_0_IIC2INTC_IRPT_INTR, SendHandler, ReceiveHandler);
	//XIic_Start(&myDevice.AD2Iic);
}


void DemoRun()
{
	u16 an[4], conv;
	u8 buf[2] = {0,0};
	double val;
	int nan = 4, i = 0;
	xil_printf("Demo Started\n\r");
	AD2_WriteConfig(&myDevice, AD2_DEFAULT_CONFIG);
	//Default config is 0xF0, all channels on
	xil_printf("AD2 Configured\n\r");
	while(1) {
		AD2_ReadConv(&myDevice, buf, 2);
		//xil_printf("%x%x\n\r", buf[0], buf[1]);

		conv = (buf[0] << 8) | buf[1];
		val = (double) AD2_CONV_VALUE(conv);
		//xil_printf("conv=%x val=%d\n\r", conv, (int)val);
		val = (val * 3.3)/4096.0;
		xil_printf("CHANNEL %d = %d.%d V\n\r", AD2_CONV_CHANNEL(conv), (int)val, (int)(val*100)%100);
		//an[AD2_CONV_CHANNEL(conv)] = AD2_CONV_VALUE(conv);
	#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(1000);
	#else
		usleep(1000000);
	#endif
	}

}
/*static void SendHandler(PmodAD2 *InstancePtr)
{
	xil_printf("Sent\r\n");
	if (InstancePtr->recvbytes){
		XIic_MasterRecv(&InstancePtr->AD2Iic, InstancePtr->recv, InstancePtr->recvbytes);
	}
}


static void ReceiveHandler(PmodAD2 *InstancePtr)
{
	xil_printf("Recv\r\n");
}*/

