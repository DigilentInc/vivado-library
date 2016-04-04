

/***************************** Include Files *******************************/
#include "PmodBT2.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/
XUartLite_Config BT2_Config =
{
	0,
	0,
	115200,
	0,
	0,
	8
};
/* ------------------------------------------------------------ */
/***	void BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address)
**
**	Parameters:
**		InstancePtr: A PmodBT2 object to start
**		GPIO_Address: The Base address of the PmodBT2 GPIO
**		UART_Address: The Base address of the PmodBT2 UART
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodBT2.
*/
void BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	BT2_Config.RegBaseAddr=UART_Address;
	Xil_Out32(InstancePtr->GPIO_addr+4, 0b11);//Set RTS and CTS to inputs
	BT2_UARTInit(&InstancePtr->BT2Uart);
}

/* ------------------------------------------------------------ */
/***	BT2_UARTInit
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initializes the PmodBT2 UART.

*/

int BT2_UARTInit(XUartLite *UartInstancePtr){
	int Status;
	XUartLite_Config *ConfigPtr; /* Pointer to Configuration data */

	ConfigPtr = &BT2_Config;
	if (ConfigPtr == NULL) {
		return XST_DEVICE_NOT_FOUND;
	}

	Status = XUartLite_CfgInitialize(UartInstancePtr, ConfigPtr, ConfigPtr->RegBaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*int BT2_readPPSpin(PmodBT2* InstancePtr){
	return Xil_In32(InstancePtr->GPIO_addr)&0b10;
}

int BT2_read3DFpin(PmodBT2* InstancePtr){
	return Xil_In32(InstancePtr->GPIO_addr)&0b01;
}*/

/* ------------------------------------------------------------ */
/*  getData()
**
**  Parameters:
**	  serPort: The HardwareSerial port (Serial1, Serial2, etc) from
**				MPIDE that will be used to communicate with the PmodBT2
**
**  Return Value:
**    The type of sentence that was recieved.
**
**  Errors:
**    none
**
**  Description:
**    Does a read of data. finishes when a value of decimal
**	  10 (ASCII <LF>) is detected. Reads the first three characters to
**	  verify the packet starts with $GP, then checks the next three
**	  to decide which struct to parse the data into.
*/
bool BT2_getData(PmodBT2* InstancePtr, char* output)
{
	//char recv[500]={0};
	int i=0;
	int count = 0;
	unsigned int ReceivedCount = 0;
	u8 StatusRegister = XUartLite_GetStatusReg(InstancePtr->BT2Uart.RegBaseAddress);
	if (StatusRegister & XUL_SR_RX_FIFO_VALID_DATA){	//If there is a sentence
		ReceivedCount += XUartLite_Recv(&InstancePtr->BT2Uart, output+ReceivedCount, 16);
	}
	else{
		return 0;
	}
	return ReceivedCount;//Return the type of sentence that was sent
}

/* ------------------------------------------------------------ */
/*  getData()
**
**  Parameters:
**	  serPort: The HardwareSerial port (Serial1, Serial2, etc) from
**				MPIDE that will be used to communicate with the PmodBT2
**
**  Return Value:
**    The type of sentence that was recieved.
**
**  Errors:
**    none
**
**  Description:
**    Does a read of data. finishes when a value of decimal
**	  10 (ASCII <LF>) is detected. Reads the first three characters to
**	  verify the packet starts with $GP, then checks the next three
**	  to decide which struct to parse the data into.
*/
bool BT2_sendData(PmodBT2* InstancePtr, char* sendData, int size)
{
	char recv[500]={0};
	int i=0;
	int count = 0;
	unsigned int ReceivedCount = 0;
	while(i<size){
		i+= XUartLite_Send(&InstancePtr->BT2Uart, sendData, size);
		while(XUartLite_IsSending(&InstancePtr->BT2Uart));
	}


	/*u8 StatusRegister = XUartLite_GetStatusReg(InstancePtr->BT2Uart.RegBaseAddress);

	if (StatusRegister & XUL_SR_RX_FIFO_VALID_DATA){	//If there is a sentence
			while(recv[ReceivedCount-1] != '\n'){
				ReceivedCount += XUartLite_Recv(&InstancePtr->BT2Uart, recv+ReceivedCount, 500);
			}
			//mode=chooseMode(recv+count);
			count=ReceivedCount;
			ReceivedCount += XUartLite_Recv(&InstancePtr->BT2Uart, recv+ReceivedCount, 500);
	}
	else{
		return 0;
	}*/
	//Format the sentence into structs
	//XUartLite_ResetFifos(&InstancePtr->BT2Uart);


	return i;//Return the type of sentence that was sent
}

