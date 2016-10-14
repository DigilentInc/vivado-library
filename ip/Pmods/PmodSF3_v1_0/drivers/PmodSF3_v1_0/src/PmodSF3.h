
#ifndef PMODSF3_H_
#define PMODSF3_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xspi.h"
#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif

#include "intc.h"
#include "xstatus.h"
#include "xparameters.h"


#ifdef XPAR_INTC_0_DEVICE_ID
#define SPI_INTR_ID 		XPAR_INTC_0_SPI_0_VEC_ID
#else
#define SPI_INTR_ID			XPAR_FABRIC_SPI_0_VEC_ID
#endif



#define MANUFACTURER_ID 0x20

#define COMMAND_PAGE_PROGRAM 	0x02 // Page program command
#define COMMAND_QUAD_WRITE 		0x32 // Quad Input Fast Program
#define COMMAND_RANDOM_READ 	0x03 // Random read command
#define COMMAND_DUAL_READ		0x3B // Dual Output Fast Read
#define COMMAND_DUAL_IO_READ	0xBB // Dual IO Fast Read
#define COMMAND_QUAD_READ		0x6B // Quad Output Fast Read
#define COMMAND_QUAD_IO_READ	0xEB // Quad IO Fast Read
#define COMMAND_WRITE_ENABLE 	0x06 // Write Enable command
#define COMMAND_SECTOR_ERASE	0xD8 // Sector Erase command
#define COMMAND_BULK_ERASE		0xC7 // Bulk Erase command
#define COMMAND_READ_STATUSREG 	0x05 // Status read command
#define COMMAND_READ_ID 		0x9E // ID read command
#define COMMAND_WRITE_STATUSREG 0x01 // Status write command

/*
 * These definitions specify the EXTRA bytes in each of the command transactions.
 * This count includes the Command byte, address bytes and any don't care bytes needed.
 */
#define READ_WRITE_EXTRA_BYTES 	4
#define WRITE_ENABLE_BYTES 		1
#define SECTOR_ERASE_BYTES		4
#define BULK_ERASE_BYTES		1
#define STATUS_READ_BYTES		2
#define STATUS_WRITE_BYTES		2

// Flash not busy mask in the status register of the flash device
#define FLASH_SR_IS_READY_MASK 0x01

// Number of bytes per page in the flash device
#define PAGE_SIZE 256

// Address of the page to perform erase, write and read operations.
#define FLASH_TEST_ADDRESS 0xC0000

/*
 * The following definitions specify the number of dummy bytes to ignore in the
 * data read from the flash, through various Read commands. This is apart from
 * the dummy bytes returned in response to the command and address transmitted.
 */
/*
 * After transmitting dual Read command and address on DIO0, the quad spi device
 * configures DIO0 and DIO1 in input mode and receives data on both DIO0 and
 * DIO1 for 8 dummy clock cycles. So we end up with 16 dummy bits in DRR. The
 * same logic applies to Quad read command, so we end up with 4 dummy bytes in
 * that case
 */
#define DUAL_READ_DUMMY_BYTES 2
#define QUAD_READ_DUMMY_BYTES 4

#define DUAL_IO_READ_DUMMY_BYTES 2
#define QUAD_IO_READ_DUMMY_BYTES 5

typedef struct PmodSF3 {
	XSpi SF3Spi;
	INTC sIntc;
	u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 4];
	u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES];
}PmodSF3;

/************************** Function Prototypes ******************************/
XStatus SF3_begin(PmodSF3* InstancePtr, const ivt_t ivt[], u32 SPI_Address);
XStatus SF3_SpiInit(XSpi *SpiPtr);
XStatus SF3_FlashWriteEnable(PmodSF3 *SpiPtr);
XStatus SF3_FlashWrite(PmodSF3 *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd);
XStatus SF3_FlashRead(PmodSF3 *SpiPtr, u32 Addr, u32 ByteCount, u8 ReadCmd);
XStatus SF3_BulkErase(PmodSF3* SpiPtr);
XStatus SF3_SectorErase(PmodSF3 *SpiPtr, u32 Addr);
XStatus SF3_GetStatus(PmodSF3 *SpiPtr);
XStatus SF3_WaitForFlashReady(PmodSF3 *SpiPtr);
XStatus SF3_QuadSpiTest(PmodSF3 *SpiPtr);
XStatus SF3_ReadID(PmodSF3 *SpiPtr);

void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount);

XStatus updateErrorCount();
void SF3_printID(u8 *idData);

#ifdef __cplusplus
}
#endif
#endif /* PMODSF3_H_ */
