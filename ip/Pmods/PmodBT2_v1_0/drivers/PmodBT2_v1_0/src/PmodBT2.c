/************************************************************************/
/*	Author:		Mikel Skreen											*/
/*	Copyright 2016, Digilent Inc.										*/
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains the necessary functions in order to operate the 	*/
/*	Pmod BT2 using simple reads and writes. 							*/
/*																		*/
/************************************************************************/
/*  Revision History:																										*/
/*																																			*/
/*	07/19/2016(MikelS): Created 																				*/
/*																																			*/
/************************************************************************/

/***************************** Include Files *****************************/
#include "PmodBT2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions *************************/
XUartNs550_Config BT2_Config =
{
		0,
		0,
		PERIPHERAL_CLK,
		115200
};

/* ------------------------------------------------------------ */
/*  BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address)
**
**	Parameters:
**		InstancePtr:	A PmodBT2 object to start
**		GPIO_Address:	The Base address of the PmodBT2 GPIO
**		UART_Address:	The Base address of the PmodBT2 UART
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodBT2.
**
******************************************************************/
void BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	BT2_Config.BaseAddress=UART_Address;
	Xil_Out32(InstancePtr->GPIO_addr+4, 0b11);//Set pins to inputs
	BT2_UARTInit(&InstancePtr->BT2Uart);
}

/* ------------------------------------------------------------ */
/*  BT2_getData(PmodBT2* InstancePtr, int buffersize)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to configure
**	  buffersize:	Size of buffer
**
**  Return Value:
**    Number of bytes received
**
**  Errors:
**    none
**
**  Description:
**    Reads data from the BT2
**
******************************************************************/
int BT2_getData(PmodBT2* InstancePtr, int buffersize)
{
	unsigned int ReceivedCount = 0;
	ReceivedCount += XUartNs550_Recv(&InstancePtr->BT2Uart, InstancePtr->recv, buffersize);

	return ReceivedCount;
}

/* ------------------------------------------------------------ */
/*  BT2_sendData(PmodBT2* InstancePtr, char* sendData, int size)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to configure
**	  sendData:		string to send through bluetooth
**	  size:			size of string
**
**  Return Value:
**    Return the type of sentence that was sent
**
**  Errors:
**    none
**
**  Description:
**    Changes the baud rate of the PmodBT2
**
******************************************************************/
int BT2_sendData(PmodBT2* InstancePtr, char* sendData, int size)
{
	char recv[500]={0};
	int i=0;
	int count = 0;
	unsigned int ReceivedCount = 0;
	while(i<size){
		i+= XUartNs550_Send(&InstancePtr->BT2Uart, sendData+i, size-i);
	}

	return i;
}

/* ------------------------------------------------------------ */
/*  BT2_changeBaud(PmodBT2* InstancePtr, int baud)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to configure
**	  baud:			Desired baud rate to set the PmodBT2 to
**
**  Return Value:
**    1 if successful, 0 if invalid baud
**
**  Errors:
**    none
**
**  Description:
**    Changes the baud rate of the PmodBT2
**
******************************************************************/
int BT2_changeBaud(PmodBT2* InstancePtr, int baud){
	XUartNs550_SetBaud(InstancePtr->BT2Uart.BaseAddress, PERIPHERAL_CLK,baud);

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */
/*	BT2_UARTInit(XUartNs550 *UartInstancePtr)
**
**	Parameters:
**		UartInstancePtr:	The XUartNs550 object to initialize
**
**	Return Value:
**		0 if success, 1 if failure
**
**	Errors:
**		none
**
**	Description:
**		Initializes the PmodBT2 UART.
**
******************************************************************/
int BT2_UARTInit(XUartNs550 *UartInstancePtr){
	int Status;
	u16 Options;
	Status = XUartNs550_CfgInitialize(UartInstancePtr, &BT2_Config, BT2_Config.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Options = XUN_OPTION_FIFOS_ENABLE;
	XUartNs550_SetOptions(UartInstancePtr, Options);
	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */
/*  BT2_ReadRTS(PmodBT2* InstancePtr)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to read from
**
**  Return Value:
**	  pin value if read
**
**  Errors:
**    none
**
**  Description:
**    Reads from the RTS pin
**
******************************************************************/
int BT2_ReadRTS(PmodBT2* InstancePtr){
	Xil_Out32(InstancePtr->GPIO_addr+4,Xil_In32(InstancePtr->GPIO_addr+4)|0b01);
	return Xil_In32(InstancePtr->GPIO_addr)&0b01;
}

/* ------------------------------------------------------------ */
/*  BT2_WriteRTS(PmodBT2* InstancePtr, bool enable)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to read from
**	  bool:			Enable
**
**  Return Value:
**	  void
**
**  Errors:
**    none
**
**  Description:
**    Enables or disables the RTS pin
**
******************************************************************/
void BT2_WriteRTS(PmodBT2* InstancePtr, bool enable){
	Xil_Out32(InstancePtr->GPIO_addr+4,Xil_In32(InstancePtr->GPIO_addr+4)&0b10);
	Xil_Out32(InstancePtr->GPIO_addr, enable);
}

/* ------------------------------------------------------------ */
/*  BT2_ReadCTS(PmodBT2* InstancePtr)
**
**  Parameters:
**	  InstancePtr: PmodBT2 object to read from
**
**  Return Value:
**	  pin value if read
**
**  Errors:
**    none
**
**  Description:
**    Reads from the CTS pin
**
******************************************************************/
int BT2_ReadCTS(PmodBT2* InstancePtr){
	Xil_Out32(InstancePtr->GPIO_addr+4,Xil_In32(InstancePtr->GPIO_addr+4)|0b10);
	return Xil_In32(InstancePtr->GPIO_addr)&0b10;
}

/* ------------------------------------------------------------ */
/*  BT2_WriteCTS(PmodBT2* InstancePtr, bool enable)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to read from
**	  bool:			Enable
**
**  Return Value:
**	  void
**
**  Errors:
**    none
**
**  Description:
**    Enables or disables the CTS pin
**
******************************************************************/
void BT2_WriteCTS(PmodBT2* InstancePtr, bool enable){
	Xil_Out32(InstancePtr->GPIO_addr+4,Xil_In32(InstancePtr->GPIO_addr+4)&0b01);
	Xil_Out32(InstancePtr->GPIO_addr, enable<<1);
}

/* ------------------------------------------------------------ */
/*  BT2_getLine(PmodBT2* InstancePtr, char* output)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to read from
**	  output:		A pointer to where the string will be stored
**
**  Return Value:
**    True if sentence received, false if not
**
**  Errors:
**    none
**
**  Description:
**    Attempts to read a single sentence through UART, BLOCKING until newline char received
**
******************************************************************/
bool BT2_getLine(PmodBT2* InstancePtr, char* output){
	int ReceivedCount=0;
	if (ReceivedCount += XUartNs550_Recv(&InstancePtr->BT2Uart, (u8*)output+ReceivedCount, 1)){	//If there is a sentence
		while(output[ReceivedCount-1] != '\n'){
			ReceivedCount += XUartNs550_Recv(&InstancePtr->BT2Uart, (u8*)output+ReceivedCount, 1);
		}
		return 1;
	}
	else return 0;
}


/* ------------------------------------------------------------ */
/*  BT2_SetupInterruptSystem(PmodBT2* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* recvHandler, void* sendHandler, void* timeoutHandler)
**
**  Parameters:
**	  InstancePtr:			PmodBT2 object to get data from
**	  interruptDeviceID:	Device ID of the interrupt controller
**	  interruptID:			The vector ID of the BT2 UART interrupt
**
**  Return Value:
**    A status indicating XST_SUCCESS or a value that is contained in
**		xstatus.h.
**
**  Errors:
**    none
**
**  Description:
**    This function sets up the interrupt system for the example.  The processing
** 	  contained in this function assumes the hardware system was built with
** 	  and interrupt controller.
**
*******************************************************************/
int BT2_SetupInterruptSystem(PmodBT2* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* recvHandler, void* sendHandler, void* timeoutHandler)
{
	int Result;
	u16 Options;

#ifdef XPAR_XINTC_NUM_INSTANCES
	INTC *IntcInstancePtr = &InstancePtr->intc;
	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	 */
	Result = XIntc_Initialize(IntcInstancePtr, interruptDeviceID);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	/* Hook up interrupt service routine */
	XIntc_Connect(IntcInstancePtr, interruptID,
		      (Xil_ExceptionHandler)XUartNs550_InterruptHandler, &InstancePtr->BT2Uart);

	/* Enable the interrupt vector at the interrupt controller */

	XIntc_Enable(IntcInstancePtr, interruptID);

	/*
	 * Start the interrupt controller such that interrupts are recognized
	 * and handled by the processor
	 */
	Result = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Result != XST_SUCCESS) {
		return Result;
	}
	XUartNs550_SetHandler(&InstancePtr->BT2Uart, (void*)BT2_intHandler, InstancePtr);
	InstancePtr->recvHandler= recvHandler;
	InstancePtr->sendHandler= sendHandler;
	InstancePtr->timeoutHandler= timeoutHandler;
	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, IntcInstancePtr);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();
	Options = XUN_OPTION_DATA_INTR |
				XUN_OPTION_FIFOS_ENABLE;
	XUartNs550_SetOptions(&InstancePtr->BT2Uart, Options);

#endif
#ifdef XPAR_FABRIC_PMODBT2_0_BT2_UART_INTERRUPT_INTR
	INTC *IntcInstancePtr = &InstancePtr->intc;
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(interruptDeviceID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Result = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, interruptID,
					0xA0, 0x3);

	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Result = XScuGic_Connect(IntcInstancePtr, interruptID,
				 (Xil_ExceptionHandler)XUartNs550_InterruptHandler, &InstancePtr->BT2Uart);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, interruptID);


	XUartNs550_SetHandler(&InstancePtr->BT2Uart, (void*)BT2_intHandler, InstancePtr);
	InstancePtr->recvHandler= recvHandler;
	InstancePtr->sendHandler= sendHandler;
	InstancePtr->timeoutHandler= timeoutHandler;
	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, IntcInstancePtr);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

	Options = XUN_OPTION_DATA_INTR | XUN_OPTION_FIFOS_ENABLE;
	XUartNs550_SetOptions(&InstancePtr->BT2Uart, Options);
#endif

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */
/*  BT2_intHandler(PmodBT2* InstancePtr, u32 Event, unsigned int EventData)
**
**  Parameters:
**	  InstancePtr:	PmodBT2 object to get data from
**	  Event:		Describes the UART interrupt type
**	  EventData:	Describes how many bytes were sent/received
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    XUartNs550 Interrupt that receives data until the last one is received
**
******************************************************************/
void BT2_intHandler(PmodBT2* InstancePtr, u32 Event, unsigned int EventData){
#ifndef NO_IRPT
	if (Event == XUN_EVENT_SENT_DATA) {
		InstancePtr->sendHandler(EventData);
	}
	if (Event == XUN_EVENT_RECV_DATA) {
		InstancePtr->recvHandler(EventData);
	}
	if (Event == XUN_EVENT_RECV_TIMEOUT) {//Triggers every 8 bytes received
		InstancePtr->timeoutHandler(EventData);
	}
	if (Event == XUN_EVENT_RECV_ERROR) {
		xil_printf("ERROR: %X error\r\n", EventData);
	}
#endif
}
