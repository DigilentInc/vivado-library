

/***************************** Include Files *******************************/
#include "PmodTMP3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ***************************/
XIic_Config TMP3_Config =
{

	0,
	0,
	0,
	2

};
static void StatusHandler(PmodTMP3 *InstancePtr);
/* ------------------------------------------------------------ */
/***	void TMP3_begin(PmodTMP3* InstancePtr, u32 IIC_Address)
**
**	Parameters:
**		InstancePtr: A PmodTMP3 object to start
**		IIC_Address: The Base address of the PmodTMP3 IIC
**		Chip_Address: 7 bit Pmod device address
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodTMP3.
*/
void TMP3_begin(PmodTMP3* InstancePtr, u32 IIC_Address, u8 Chip_Address)
{
	TMP3_Config.BaseAddress=IIC_Address;
	InstancePtr->chipAddr=Chip_Address;
	TMP3_IICInit(&InstancePtr->TMP3Iic);
	XIic_SetAddress(&InstancePtr->TMP3Iic, XII_ADDR_TO_SEND_TYPE, InstancePtr->chipAddr);
}

/* ------------------------------------------------------------ */
/***	TMP3_end(void)
**
**	Parameters:
**		InstancePtr		- PmodTMP3 object to stop
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
void TMP3_end(PmodTMP3* InstancePtr){
	//XIic_Stop(&InstancePtr->TMP3Iic);
}

/* ------------------------------------------------------------ */
/***	TMP3_IICInit
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
**		Initializes the PmodTMP3 IIC.

*/

int TMP3_IICInit(XIic *IicInstancePtr){
	int Status;

	Status = XIic_CfgInitialize(IicInstancePtr, &TMP3_Config, TMP3_Config.BaseAddress);
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
/***	TMP3_ReadIIC
**
**	Parameters:
**		InstancePtr - PmodTMP3 object to initialize
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
**		Reads nData data bytes from register reg
**
*/
void TMP3_ReadIIC(PmodTMP3* InstancePtr, u8 reg, u8 *Data, int nData)
{
	int Status;
	Status = XIic_Start(&InstancePtr->TMP3Iic);
	if (Status != XST_SUCCESS) {
		return;
	}
	if (InstancePtr->currentRegister!=reg){
		//XIic_MasterSend(&InstancePtr->TMP3Iic, &reg, 1);
		XIic_Send(InstancePtr->TMP3Iic.BaseAddress, InstancePtr->chipAddr, &reg, 1, XII_REPEATED_START_OPTION);
		InstancePtr->currentRegister=reg;
	}//else{
		//XIic_MasterRecv(&InstancePtr->TMP3Iic, Data, nData);
	//}
	XIic_Recv(InstancePtr->TMP3Iic.BaseAddress, InstancePtr->chipAddr, Data, nData, XIIC_STOP);


	//XIic_MasterSend(&InstancePtr->TMP3Iic, &reg, 2);
	//InstancePtr->recvbytes=nData+1;
	//InstancePtr->recv=Data;
	//XIic_MasterRecv(&InstancePtr->TMP3Iic, Data, nData);

	Status = XIic_Stop(&InstancePtr->TMP3Iic);
	if (Status != XST_SUCCESS) {
		return;
	}
}

/* ------------------------------------------------------------ */
/***	TMP3_WriteIIC
**
**	Parameters:
**		InstancePtr - PmodTMP3 object to initialize
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
void TMP3_WriteIIC(PmodTMP3* InstancePtr, u8 reg, u8 *Data, int nData)
{
	u8 out[10];
	out[0]=reg;
	out[1]=*Data;
	int Status;

	if (InstancePtr->currentRegister!=reg){
			InstancePtr->currentRegister=reg;
	}
	Status = XIic_Start(&InstancePtr->TMP3Iic);
	if (Status != XST_SUCCESS) {
		return;
	}
	XIic_Send(InstancePtr->TMP3Iic.BaseAddress, InstancePtr->chipAddr, out, nData+1, XIIC_STOP);

	//XIic_MasterSend(&InstancePtr->TMP3Iic, out, nData+1);

	//XIic_DynMasterSend(&InstancePtr->TMP3Iic, out, nData+1);

	//while(XIic_IsIicBusy(&InstancePtr->TMP3Iic));

	Status = XIic_Stop(&InstancePtr->TMP3Iic);
	if (Status != XST_SUCCESS) {
		return;
	}

}

/* ------------------------------------------------------------ */
/*  TMP3_SetupInterruptSystem()
**
**  Parameters:
**	  InstancePtr: PmodTMP3 object to get data from
**	  interruptDeviceID: Device ID of the interrupt controller
**	  interruptID: The vector ID of the TMP3 I2C interrupt
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
int TMP3_SetupInterruptSystem(PmodTMP3* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* SendHandler,  void* ReceiveHandler)
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
		      (Xil_ExceptionHandler)XIic_InterruptHandler, &InstancePtr->TMP3Iic);

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
	XIic_SetSendHandler(&InstancePtr->TMP3Iic, InstancePtr, (XIic_Handler)SendHandler);
	XIic_SetRecvHandler(&InstancePtr->TMP3Iic, InstancePtr, (XIic_Handler)ReceiveHandler);

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
				 (Xil_ExceptionHandler)XIic_InterruptHandler, &InstancePtr->TMP3Iic);
	if (Result != XST_SUCCESS) {
		return Result;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, interruptID);


	XIic_SetSendHandler(&InstancePtr->TMP3Iic, InstancePtr, (XIic_Handler)SendHandler);
	XIic_SetRecvHandler(&InstancePtr->TMP3Iic, InstancePtr, (XIic_Handler)ReceiveHandler);
	XIic_SetStatusHandler(&InstancePtr->TMP3Iic, InstancePtr, (XIic_Handler)StatusHandler);

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

static void StatusHandler(PmodTMP3 *InstancePtr){
	xil_printf("status\r\n");
}

/* ------------------------------------------------------------ */
/***	void TMP3_config(PmodTMP3 *InstancePtr, u8 configuration)
**
**	Parameters:
**		InstancePtr: A PmodTMP3 object to start
**		configuration: 8 bit TMP3 configuration code, refer to "Configuration settings" under Bit Masks in header for details
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Configures the PmodTMP3's Microchip TCN75A. Does not need to be called on startup.
*/
void TMP3_config(PmodTMP3 *InstancePtr, u8 configuration) {
	u8 buf[1];
	buf[0] = configuration;
	TMP3_WriteIIC(InstancePtr, TMP3_REG_CONFIG, buf, 1);
}

/* ------------------------------------------------------------ */
/***	double TMP3_getTemp(PmodTMP3 *InstancePtr)
**
**	Parameters:
**		InstancePtr: A PmodTMP3 object to start
**
**	Return Value:
**		tempResult: Current temperature seen by the TMP3 in degrees Celsius.
**
**	Errors:
**		none
**
**	Description:
**		Requests and returns the temperature off of the Pmod TMP3
*/
double TMP3_getTemp(PmodTMP3 *InstancePtr) {
	double tempResult;
	u16 uResult = 0;
	int wResult = 0;
	u8 buf[2] = {0};

	//Read temperature register into buf.
	TMP3_ReadIIC(InstancePtr, TMP3_REG_TEMP, buf, 2);

	uResult = buf[1] | (buf[0] << 8);
	if (buf[0] & 0x80 == 0) {
		//result is positive
		wResult = (int)uResult;
	}
	else {
		//result negative -- two's complement
		wResult = -1 * (int)(~uResult + 1);
	}
	
	//9 bit resolution has 0.5 *C resolution, stored in the uppermost 9 bits of the buffer.
	tempResult = (double)wResult / 256.0;

	return tempResult;
}

/* ------------------------------------------------------------ */
/***	double TMP3_FtoC(double tempF)
**
**	Parameters:
**		tempF: a temperature value in degrees Fahrenheit
**
**	Return Value:
**		tempC: a temperature value in degrees Celsius
**
**	Errors:
**		none
**
**	Description:
**		Converts temperature in Fahrenheit to Celsius
*/
double TMP3_FtoC(double tempF) {
	double tempC;

	tempC = (tempF - 32.0) / 1.8;

	return tempC;
}

/* ------------------------------------------------------------ */
/***	double TMP3_CtoF(double tempC)
**
**	Parameters:
**		tempC: a temperature value in degrees Celsius
**
**	Return Value:
**		tempF: a temperature value in degrees Fahrenheit
**
**	Errors:
**		none
**
**	Description:
**		Converts temperature in Celsius to Fahrenheit
*/
double TMP3_CtoF(double tempC) {
	double tempF;

	tempF = (tempC * 1.8) + 32.0;

	return tempF;
}

/* ------------------------------------------------------------ */
/***	void TMP3_delay(int micros)
**
**	Parameters:
**		micros: the number of microseconds to delay for
**
**	Return Value:
**		none
**
**	Errors:
**		Slower than expected on Microblaze
**
**	Description:
**		Delays the processor for a number of microseconds
*/
void TMP3_delay(int micros) {
	int i;

#ifdef XPAR_MICROBLAZE_ID
	for(i = 0; i < (ITERS_PER_USEC*micros); i++){
			asm("");
	}
#else
	usleep(micros);
#endif
}
