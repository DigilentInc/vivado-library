
#ifndef PMODOLEDRGB_H
#define PMODOLEDRGB_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

#define bool u8
#define true 1
#define false 0

#define PMODOLEDRGB_AXI_LITE_GPIO_SLV_REG0_OFFSET 0
#define PMODOLEDRGB_AXI_LITE_GPIO_SLV_REG1_OFFSET 4
#define PMODOLEDRGB_AXI_LITE_GPIO_SLV_REG2_OFFSET 8
#define PMODOLEDRGB_AXI_LITE_GPIO_SLV_REG3_OFFSET 12

#define PMODOLEDRGB_AXI_LITE_SPI_SLV_REG0_OFFSET 0
#define PMODOLEDRGB_AXI_LITE_SPI_SLV_REG1_OFFSET 4
#define PMODOLEDRGB_AXI_LITE_SPI_SLV_REG2_OFFSET 8
#define PMODOLEDRGB_AXI_LITE_SPI_SLV_REG3_OFFSET 12

#define OLEDRGB_WIDTH                      96
#define OLEDRGB_HEIGHT                     64

#define CMD_DRAWLINE                       0x21
#define CMD_DRAWRECTANGLE                  0x22
#define CMD_COPYWINDOW                     0x23
#define CMD_DIMWINDOW                      0x24
#define CMD_CLEARWINDOW                    0x25
#define CMD_FILLWINDOW                     0x26
#define DISABLE_FILL    0x00
#define ENABLE_FILL     0x01
#define CMD_CONTINUOUSSCROLLINGSETUP      0x27
#define CMD_DEACTIVESCROLLING              0x2E
#define CMD_ACTIVESCROLLING                0x2F

#define CMD_SETCOLUMNADDRESS              	0x15
#define CMD_SETROWADDRESS                 	0x75
#define CMD_SETCONTRASTA                  	0x81
#define CMD_SETCONTRASTB                  	0x82
#define CMD_SETCONTRASTC                  	0x83
#define CMD_MASTERCURRENTCONTROL          	0x87
#define CMD_SETPRECHARGESPEEDA           	0x8A
#define CMD_SETPRECHARGESPEEDB           	0x8B
#define CMD_SETPRECHARGESPEEDC           	0x8C
#define CMD_SETREMAP                       	0xA0
#define CMD_SETDISPLAYSTARTLINE          	0xA1
#define CMD_SETDISPLAYOFFSET              	0xA2
#define CMD_NORMALDISPLAY                  	0xA4
#define CMD_ENTIREDISPLAYON              	0xA5
#define CMD_ENTIREDISPLAYOFF              	0xA6
#define CMD_INVERSEDISPLAY                 	0xA7
#define CMD_SETMULTIPLEXRATIO             	0xA8
#define CMD_DIMMODESETTING                	0xAB
#define CMD_SETMASTERCONFIGURE            	0xAD
#define CMD_DIMMODEDISPLAYON             	0xAC
#define CMD_DISPLAYOFF                     	0xAE
#define CMD_DISPLAYON    					0xAF
#define CMD_POWERSAVEMODE                 	0xB0
#define CMD_PHASEPERIODADJUSTMENT         	0xB1
#define CMD_DISPLAYCLOCKDIV               	0xB3
#define CMD_SETGRAySCALETABLE            	0xB8
#define CMD_ENABLELINEARGRAYSCALETABLE  	0xB9
#define CMD_SETPRECHARGEVOLTAGE           	0xBB
#define CMD_SETVVOLTAGE                   	0xBE

/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a PMODOLEDRGB register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PMODOLEDRGBdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void PMODOLEDRGB_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define PMODOLEDRGB_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a PMODOLEDRGB register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PMODOLEDRGB device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 PMODOLEDRGB_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define PMODOLEDRGB_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the PMODOLEDRGB instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus PMODOLEDRGB_Reg_SelfTest(void * baseaddr_p);


typedef struct{
	u32 GPIO_addr;
	u32 SPI_addr;
	XSpi OLEDSpi;
}PmodOLEDrgb;

int OLEDrgb_SPIInit(XSpi *SpiInstancePtr, u16 SpiDeviceId, u32 mode);
uint16_t OLEDrgb_BuildRGB(uint8_t R,uint8_t G,uint8_t B);
uint16_t OLEDrgb_BuildHSV(uint8_t hue, uint8_t sat, uint8_t val);
uint8_t OLEDrgb_ExtractRFromRGB(uint16_t wRGB);
uint8_t OLEDrgb_ExtractGFromRGB(uint16_t wRGB);
uint8_t OLEDrgb_ExtractBFromRGB(uint16_t wRGB);


void OLEDrgb_begin(PmodOLEDrgb* InstancePtr, u32 GPIO_Address, u32 SPI_Address);

void OLEDrgb_DrawPixel(PmodOLEDrgb* InstancePtr, uint8_t c, uint8_t r, uint16_t pixelColor);
void OLEDrgb_DrawLine(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor);
void OLEDrgb_DrawRectangle(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor, bool bFill, uint16_t fillColor);
void OLEDrgb_Clear(PmodOLEDrgb* InstancePtr);
void OLEDrgb_DrawBitmap(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t *pBmp);

void OLEDrgb_HostInit();
void OLEDrgb_DevInit();
uint8_t OLEDrgb_WriteSPI(XSpi* OLEDSpi, uint8_t bVal);
uint8_t OLEDrgb_WriteSPI1(XSpi* OLEDSpi, uint8_t bVal1, uint8_t bVal2);
uint8_t OLEDrgb_WriteSPI2(PmodOLEDrgb* InstancePtr, uint8_t *pCmd, int nCmd, uint8_t *pData, int nData);


#endif // PMODOLEDRGB_H
