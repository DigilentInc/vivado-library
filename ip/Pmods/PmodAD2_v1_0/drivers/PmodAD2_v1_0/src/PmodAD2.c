

/***************************** Include Files *******************************/
#include "PmodAD2.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/
XIic_Config AD2_Config =
{

	0,
	0,
	0,
	2

};
static void StatusHandler(PmodAD2 *InstancePtr);
/* ------------------------------------------------------------ */
/***	void AD2_begin(PmodAD2* InstancePtr, u32 IIC_Address)
**
**	Parameters:
**		InstancePtr: A PmodAD2 object to start
**		IIC_Address: The Base address of the PmodAD2 IIC
**		Chip_Address: 7 bit Pmod device address
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodAD2.
*/
void AD2_begin(PmodAD2* InstancePtr, u32 IIC_Address, u8 Chip_Address)
{
	AD2_Config.BaseAddress=IIC_Address;
	InstancePtr->chipAddr=Chip_Address;
	AD2_IICInit(&InstancePtr->AD2Iic);
	XIic_SetAddress(&InstancePtr->AD2Iic, XII_ADDR_TO_SEND_TYPE, InstancePtr->chipAddr);
}

/* ------------------------------------------------------------ */
/***	AD2_end(void)
**
**	Parameters:
**		InstancePtr		- PmodAD2 object to stop
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Stops the device
*/
void AD2_end(PmodAD2* InstancePtr){
	//XIic_Stop(&InstancePtr->AD2Iic);
}

/* ------------------------------------------------------------ */
/***	AD2_IICInit
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
**		Initializes the PmodAD2 IIC.

*/

int AD2_IICInit(XIic *IicInstancePtr){
	int Status;

	Status = XIic_CfgInitialize(IicInstancePtr, &AD2_Config, AD2_Config.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}



	/*
	 * Start the IIC driver so that the device is enabled.
	 */
	XIic_Start(IicInstancePtr);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XIic_IntrGlobalDisable(IicInstancePtr);

	return XST_SUCCESS;

}

/* ------------------------------------------------------------ */
/***	AD2_ReadIIC
**
**	Parameters:
**		InstancePtr - PmodAD2 object to initialize
**		reg			- Register to read from
**		Data		- Pointer to recieve buffer
**		nData		- Number of data values to read
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Reads 2 data bytes from conversion register
**
*/
void AD2_ReadConv(PmodAD2* InstancePtr,	u8 buf[], int nbytes)
{

	//u16 rval
	int Status;
	//xil_printf("check\n\r");
	Status = XIic_Start(&InstancePtr->AD2Iic);
	if (Status != XST_SUCCESS) {
		return;
	}

	XIic_Recv(InstancePtr->AD2Iic.BaseAddress, InstancePtr->chipAddr, buf, 2, XIIC_STOP);
	//xil_printf("buf %x-%x\n\r", buf[1], buf[0]);
	Status = XIic_Stop(&InstancePtr->AD2Iic);
	if (Status != XST_SUCCESS) {
		return 0xFF;	//normally conversion register has two leading zeroes, so (val & 0xC000 == 0) will give success.
	}
}

/* ------------------------------------------------------------ */
/***	AD2_WriteIIC
**
**	Parameters:
**		InstancePtr - PmodAD2 object to initialize
**		reg			- Register to send to
**		Data		- Pointer to data buffer to send
**		nData		- Number of data values to send
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Writes nData data bytes to register reg
**
*/
void AD2_WriteConfig(PmodAD2* InstancePtr, u8 configuration)
{
	//u8 buf[1];
	int Status;
	//buf[0]=configuration;

	Status = XIic_Start(&InstancePtr->AD2Iic);
	if (Status != XST_SUCCESS) {
		return;
	}
	XIic_Send(InstancePtr->AD2Iic.BaseAddress, InstancePtr->chipAddr, &configuration, 1, XIIC_STOP);

	Status = XIic_Stop(&InstancePtr->AD2Iic);
	if (Status != XST_SUCCESS) {
		return;
	}

}

/* ------------------------------------------------------------ */
/*  AD2_SetupInterruptSystem()
**
**  Parameters:
**	  InstancePtr: PmodAD2 object to get data from
**	  interruptDeviceID: Device ID of the interrupt controller
**	  interruptID: The vector ID of the AD2 I2C interrupt
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
*****************************************************************************/
int AD2_SetupInterruptSystem(PmodAD2* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* SendHandler,  void* ReceiveHandler)
{
	int Result;

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
		      (Xil_ExceptionHandler)XIic_InterruptHandler, &InstancePtr->AD2Iic);

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
	XIic_SetSendHandler(&InstancePtr->AD2Iic, InstancePtr, (XIic_Handler)SendHandler);
	XIic_SetRecvHandler(&InstancePtr->AD2Iic, InstancePtr, (XIic_Handler)ReceiveHandler);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, IntcInstancePtr);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

#endif
#ifdef XPAR_SCUGIC_0_DEVICE_ID
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
				 (Xil_ExceptionHandler)XIic_InterruptHandler, &InstancePtr->AD2Iic);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, interruptID);


	XIic_SetSendHandler(&InstancePtr->AD2Iic, InstancePtr, (XIic_Handler)SendHandler);
	XIic_SetRecvHandler(&InstancePtr->AD2Iic, InstancePtr, (XIic_Handler)ReceiveHandler);
	XIic_SetStatusHandler(&InstancePtr->AD2Iic, InstancePtr, (XIic_Handler)StatusHandler);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER, IntcInstancePtr);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

#endif

	return XST_SUCCESS;
}

static void StatusHandler(PmodAD2 *InstancePtr){
	xil_printf("status\r\n");
}

