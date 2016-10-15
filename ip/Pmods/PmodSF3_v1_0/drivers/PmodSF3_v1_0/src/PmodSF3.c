/***************************** Include Files *********************************/
#include "PmodSF3.h"

/************************** Variable Definitions *****************************/
unsigned char verbose = 1;

volatile static int TransferInProgress;

// This variable tracks any errors that occur during interrupt processing
static int ErrorCount;

/*
 * Byte offset value written to Flash. This need to be redefined for writing different
 * patterns of data to the Flash device.
 */

static u8 TestByte = 0x20;

XSpi_Config XSpi_SF3Config = {
		0,
		0,
		1,
		0,
		1,
		8, 
		2,
		0,
		0,
		0,
		0
};

XStatus SF3_begin(PmodSF3* InstancePtr, const ivt_t ivt[], u32 SPI_Address) {
	XStatus Status;

	XSpi_SF3Config.BaseAddress=SPI_Address;

	 // Initialize the interrupt controller
	Status = InitInterruptController(&InstancePtr->sIntc);
	if (Status != XST_SUCCESS) {
		xil_printf("\r\nError initializing interrupts");
		return XST_FAILURE;
	}
	// Initialize SPI
	Status = SF3_SpiInit(&InstancePtr->SF3Spi);
	if (Status != XST_SUCCESS) {
		xil_printf("\r\nError initializing SPI");
		return XST_FAILURE;
	}
	// Enable all interrupts in our interrupt vector table
	// Make sure all driver instances using interrupts are initialized first
	EnableInterrupts(&InstancePtr->sIntc, &ivt[0], 1); // sizeof(ivt)/sizeof(ivt[0]));


	XSpi_IntrGlobalEnable(&InstancePtr->SF3Spi);

	return XST_SUCCESS;
}

/*****************************************************************************
* This function initializes the SPI and performs a self test to ensure
* that the HW was set correctly
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_SpiInit(XSpi* SpiPtr) {
	XStatus Status;
	XSpi_Config *ConfigPtr;

	Xil_AssertNonvoid(SpiPtr != NULL);

	ConfigPtr = &XSpi_SF3Config;
	if (ConfigPtr == NULL) {
		return XST_DEVICE_NOT_FOUND;
	}

	Status = XSpi_CfgInitialize(SpiPtr, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Self-test to ensure the hardware was built properly
	Status = XSpi_SelfTest(SpiPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Set the handler for the SPI that is called from the interrupt context
	XSpi_SetStatusHandler(SpiPtr, SpiPtr, (XSpi_StatusHandler) SpiHandler);

	Status = XSpi_SetOptions(SpiPtr, (XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(SpiPtr, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Start the SPI driver, enabling the device
	Status = XSpi_Start(SpiPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************
* This function enables writes to the Winbond Serial Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_FlashWriteEnable(PmodSF3* InstancePtr) {
	XStatus Status;

	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	InstancePtr->WriteBuffer[0] = COMMAND_WRITE_ENABLE;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, NULL, WRITE_ENABLE_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while(TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function writes the data to the specified locations in the Serial
* Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the address in the Buffer, where to write the data.
* @param	ByteCount is the number of bytes to be written.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_FlashWrite(PmodSF3* InstancePtr, u32 Addr, u32 ByteCount, u8 WriteCmd) {
	XStatus Status;
	u32 Index;

	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	InstancePtr->WriteBuffer[0] = WriteCmd;
	InstancePtr->WriteBuffer[1] = (u8) (Addr >> 16);
	InstancePtr->WriteBuffer[2] = (u8) (Addr >> 8);
	InstancePtr->WriteBuffer[3] = (u8) Addr;

	//Fill in the data that is to be written into the Micron Serial Flash
	for (Index = 4; Index < ByteCount + READ_WRITE_EXTRA_BYTES; Index++) {
		InstancePtr->WriteBuffer[Index] = (u8) (Index - 4) + TestByte;
	}

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, NULL, (ByteCount + READ_WRITE_EXTRA_BYTES));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function reads the data from the Serial Flash Memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the starting address in the Flash Memory from which the
*			data is to be read.
* @param	ByteCount is the number of bytes to be read.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_FlashRead(PmodSF3* InstancePtr, u32 Addr, u32 ByteCount, u8 ReadCmd) {
	XStatus Status;

	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Prepare the write buffer
	InstancePtr->WriteBuffer[0] = ReadCmd;
	InstancePtr->WriteBuffer[1] = (u8) (Addr >> 16);
	InstancePtr->WriteBuffer[2] = (u8) (Addr >> 8);
	InstancePtr->WriteBuffer[3] = (u8) Addr;

	switch (ReadCmd) {
	case COMMAND_DUAL_READ:
		ByteCount += DUAL_READ_DUMMY_BYTES;
		break;
	case COMMAND_DUAL_IO_READ:
		ByteCount += DUAL_READ_DUMMY_BYTES;
		break;
	case COMMAND_QUAD_IO_READ:
		ByteCount += QUAD_IO_READ_DUMMY_BYTES;
		break;
	case COMMAND_QUAD_READ:
		ByteCount += QUAD_READ_DUMMY_BYTES;
		break;
	default:
		break;
	}

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, InstancePtr->ReadBuffer, (ByteCount + READ_WRITE_EXTRA_BYTES));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function erases the entire contents of the Micron Serial Flash device.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The erased bytes will read as 0xFF.
*
******************************************************************************/
XStatus SF3_BulkErase(PmodSF3* InstancePtr) {
	XStatus Status;

	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	InstancePtr->WriteBuffer[0] = COMMAND_BULK_ERASE;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, NULL, BULK_ERASE_BYTES);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while(TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

XStatus SF3_SectorErase(PmodSF3* InstancePtr, u32 Addr) {
	XStatus Status;

	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	InstancePtr->WriteBuffer[0] = COMMAND_SECTOR_ERASE;
	InstancePtr->WriteBuffer[1] = (u8) (Addr >> 16);
	InstancePtr->WriteBuffer[2] = (u8) (Addr >> 8);
	InstancePtr->WriteBuffer[3] = (u8) (Addr);

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, NULL, SECTOR_ERASE_BYTES);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while(TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

XStatus SF3_GetStatus(PmodSF3* InstancePtr) {
	XStatus Status;

	InstancePtr->WriteBuffer[0] = COMMAND_READ_STATUSREG;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, InstancePtr->ReadBuffer, STATUS_READ_BYTES);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while(TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************
* This function waits until the serial Flash is ready to accept next command.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		This function reads the status register of the Buffer and waits
*			until the WIP bit of the status register becomes 0.
*
******************************************************************************/
XStatus SF3_WaitForFlashReady(PmodSF3* InstancePtr) {
	XStatus Status;
	u8 StatusReg;

	while(1) {
		/*
		 * Get the Status REgister. The status register content is
		 * stored at the second byte pointed by the ReadBuffer.
		 */
		Status = SF3_GetStatus(InstancePtr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Check to see if the flash is ready to accept the next command.
		 * If so, break.
		 */
		StatusReg = InstancePtr->ReadBuffer[1];
		if ((StatusReg & 0x01) == 0) {
			break;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function writes the data to the specified locations in the Serial
* Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_QuadSpiTest(PmodSF3* InstancePtr) {
	XStatus Status;
	u32 Index;
	u32 Address;

	Address = FLASH_TEST_ADDRESS;

	Status = SF3_FlashWriteEnable(InstancePtr);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash wait ready error");
		}
		return XST_FAILURE;
	}

	// Perform the sector Erase operation.
	Status = SF3_SectorErase(InstancePtr, Address);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash sector erase error");
		}
		return XST_FAILURE;
	}

	// Perform write enable operation
	Status = SF3_FlashWriteEnable(InstancePtr);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash write enable error");
		}
		return XST_FAILURE;
	}

	// Write the data to the Page using Page Program command.
	Status = SF3_FlashWrite(InstancePtr, Address, PAGE_SIZE, COMMAND_PAGE_PROGRAM);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash page write error");
		}
	}

	// Clear the read buffer
	for (Index = 0; Index < PAGE_SIZE + READ_WRITE_EXTRA_BYTES; Index++) {
		InstancePtr->ReadBuffer[Index] = 0x0;
	}

	// Read the data from the Page using Random Read command
	Status = SF3_FlashRead(InstancePtr, Address, PAGE_SIZE, COMMAND_RANDOM_READ);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash random read error");
		}
		return XST_FAILURE;
	}
	xil_printf("\r\ntest: %d", InstancePtr->ReadBuffer[Index + READ_WRITE_EXTRA_BYTES]);
	// Compare the data read against data written
	for (Index = 0; Index < PAGE_SIZE; Index++) {

		if (InstancePtr->ReadBuffer[Index + READ_WRITE_EXTRA_BYTES] != (u8) (Index + TestByte)) {
			if (verbose) {
				xil_printf("\r\nQuad-SPI Flash data read/write comparison error");
			}

			return XST_FAILURE;
		}
	}
	// Clear the read buffer
	for (Index = 0; Index < PAGE_SIZE + READ_WRITE_EXTRA_BYTES + DUAL_READ_DUMMY_BYTES; Index++) {
		InstancePtr->ReadBuffer[Index] = 0x0;
	}

	// Read the data from the Page using Dual Output Fast Read command.
	Status = SF3_FlashRead(InstancePtr, Address, PAGE_SIZE, COMMAND_DUAL_READ);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash dual read error");
		}
		return XST_FAILURE;
	}

	// Compare data read against the data written
	for (Index = 0; Index < PAGE_SIZE; Index++) {
		if (InstancePtr->ReadBuffer[Index + READ_WRITE_EXTRA_BYTES + DUAL_READ_DUMMY_BYTES] != (u8) (Index + TestByte)){
			if (verbose) {
				xil_printf("\r\nQuad-SPI Flash data dual read/write comparison error");
			}
			return XST_FAILURE;
		}
	}

	// Clear the read buffer
	for (Index = 0; Index < PAGE_SIZE + READ_WRITE_EXTRA_BYTES + QUAD_READ_DUMMY_BYTES; Index++) {
		InstancePtr->ReadBuffer[Index] = 0x0;
	}

	// Read the data from the Page using Quad Output Fast Read command
	Status = SF3_FlashRead(InstancePtr, Address, PAGE_SIZE, COMMAND_QUAD_READ);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash quad read error");
		}
		return XST_FAILURE;
	}

	// Compare data read against the data written
	for (Index = 0; Index < PAGE_SIZE; Index++) {
		if (InstancePtr->ReadBuffer[Index + READ_WRITE_EXTRA_BYTES + QUAD_READ_DUMMY_BYTES] != (u8) (Index + TestByte)) {
			if (verbose) {
				xil_printf("\r\nQuad-SPI Flash data quad read/write comparison error");
			}
			return XST_FAILURE;
		}
	}

	// Perform the Write Enable operation
	Status = SF3_FlashWriteEnable(InstancePtr);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash write enable error");
		}
		return XST_FAILURE;
	}

	/*
	 * Write the data to the next Page using Quad Fast Write command. Erase
	 * is not required since we are writing to the next page within the same
	 * erased sector
	 */
	TestByte = 0x09;
	Address += PAGE_SIZE;

	Status = SF3_FlashWrite(InstancePtr, Address, PAGE_SIZE, COMMAND_QUAD_WRITE);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash quad write error");
		}
		return XST_SUCCESS;
	}

	// Wait while the flash is busy
	Status = SF3_WaitForFlashReady(InstancePtr);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash wait ready error");
		}
		return XST_FAILURE;
	}

	// Clear the read buffer
	for (Index = 0; Index < PAGE_SIZE + READ_WRITE_EXTRA_BYTES; Index++) {
		InstancePtr->ReadBuffer[Index] = 0x0;
	}

	// Read the data from the Page using Normal Read command
	Status = SF3_FlashRead(InstancePtr, Address, PAGE_SIZE, COMMAND_QUAD_IO_READ);
	if (Status != XST_SUCCESS) {
		if (verbose) {
			xil_printf("\r\nQuad-SPI Flash read error");
		}
		return XST_FAILURE;
	}

	// Compare the data read against the data written
	for (Index = 0; Index < PAGE_SIZE; Index++) {
		if (InstancePtr->ReadBuffer[Index + READ_WRITE_EXTRA_BYTES +
		               QUAD_IO_READ_DUMMY_BYTES] != (u8) (Index + TestByte)) {
			if (verbose) {
				xil_printf("\r\nQuad-SPI Flash data read/write comparison error");
			}
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function reads the ID register of the serial Flash
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
XStatus SF3_ReadID(PmodSF3* InstancePtr){
	XStatus Status;

	InstancePtr->WriteBuffer[0] = COMMAND_READ_ID;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(&InstancePtr->SF3Spi, InstancePtr->WriteBuffer, InstancePtr->ReadBuffer, 20);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while(TransferInProgress);
	if (ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************
* This function is the handler which performs processing for the SPI driver.
* It is called from an interrupt context such that the amount of processing
* performed should be minimized. It is called when a transfer of SPI data
* completes or an error occurs.
*
* This handler provides an example of how to handle SPI interrupts and
* is application specific.
*
* @param	CallBackRef is the upper layer callback reference passed back
*			when the callback function is invoked.
* @param	StatusEvent is the event that just occurred.
* @param	ByteCount is the number of bytes transferred up until the event
*			occurred.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount) {
	TransferInProgress = FALSE;

	if (StatusEvent != XST_SPI_TRANSFER_DONE) {
		ErrorCount++;
	}
}
