

/***************************** Include Files *******************************/
#include "PmodOLEDrgb.h"

#include <stdlib.h>
#include <string.h>
/************************** Function Definitions ***************************/


u8 num_devices=0;
XSpi_Config XSpi_OLEDrgb =
{
	0,
	0,
	1,
	0,
	1,
	8,
	0,
	0,
	0,
	0,
	0
};
uint16_t OLEDrgb_BuildRGB(uint8_t R,uint8_t G,uint8_t B){return ((R>>3)<<11) | ((G>>2)<<5) | (B>>3);};
uint8_t OLEDrgb_ExtractRFromRGB(uint16_t wRGB){return (uint8_t)((wRGB>>11)&0x1F);};
uint8_t OLEDrgb_ExtractGFromRGB(uint16_t wRGB){return (uint8_t)((wRGB>>5)&0x3F);};
uint8_t OLEDrgb_ExtractBFromRGB(uint16_t wRGB){return (uint8_t)(wRGB&0x1F);};


/* ------------------------------------------------------------ */
/***	void Oledrgb::begin(void)
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
**		Initialize the OLED display controller and turn the display on.
*/
void OLEDrgb_begin(PmodOLEDrgb* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
{
	InstancePtr->GPIO_addr=GPIO_Address;
	XSpi_OLEDrgb.BaseAddress=SPI_Address;

	OLEDrgb_HostInit(InstancePtr);
	OLEDrgb_DevInit(InstancePtr);
}

uint16_t OLEDrgb_BuildHSV(uint8_t hue, uint8_t sat, uint8_t val){
   uint8_t region, remain, p, q, t;
   uint8_t R,G,B;
   region = hue/43;
   remain = (hue - (region * 43))*6;
   p = (val * (255-sat))>>8;
   q = (val * (255 - ((sat * remain)>>8)))>>8;
   t = (val * (255 - ((sat * (255 - remain))>>8)))>>8;

   switch(region){
      case 0:
       R = val;
       G = t;
       B = p;
       break;
      case 1:
       R = q;
       G = val;
       B = p;
       break;
      case 2:
       R = p;
       G = val;
       B = t;
       break;
       case 3:
       R = p;
       G = q;
       B = val;
       break;
       case 4:
       R = t;
       G = p;
       B = val;
       break;
       default:
       R = val;
       G = p;
       B = q;
       break;
   }
   return OLEDrgb_BuildRGB(R,G,B);
}

void OLEDrgb_DrawPixel(PmodOLEDrgb* InstancePtr, uint8_t c, uint8_t r, uint16_t pixelColor)
{
	uint8_t cmds[6];
	uint8_t data[2];
	//set column start and end
	cmds[0] = CMD_SETCOLUMNADDRESS;
	cmds[1] = c;					// Set the starting column coordinates
	cmds[2] = OLEDRGB_WIDTH - 1;					// Set the finishing column coordinates

	//set row start and end
	cmds[3] = CMD_SETROWADDRESS;
	cmds[4] = r;					// Set the starting row coordinates
	cmds[5] = OLEDRGB_HEIGHT - 1;					// Set the finishing row coordinates

	data[0] = pixelColor >> 8;
	data[1] = pixelColor;

	OLEDrgb_WriteSPI2(InstancePtr, cmds, 6, data, 2);
}

void OLEDrgb_DrawLine(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor)
{
	uint8_t cmds[8];
	cmds[0] = CMD_DRAWLINE; 		//draw line
	cmds[1] = c1;					// start column
	cmds[2] = r1;					// start row
	cmds[3] = c2;					// end column
	cmds[4] = r2;					//end row
	cmds[5] = OLEDrgb_ExtractRFromRGB(lineColor);	//R
	cmds[6] = OLEDrgb_ExtractGFromRGB(lineColor);	//G
	cmds[7] = OLEDrgb_ExtractBFromRGB(lineColor);	//R

	OLEDrgb_WriteSPI2(InstancePtr, cmds, 8, NULL, 0);
}

void OLEDrgb_DrawRectangle(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor, bool bFill, uint16_t fillColor)
{
	uint8_t cmds[13];
    cmds[0] = CMD_FILLWINDOW;		//fill window
    cmds[1] = (bFill ? ENABLE_FILL: DISABLE_FILL);
    cmds[2] = CMD_DRAWRECTANGLE;	//draw rectangle
	cmds[3] = c1;					// start column
	cmds[4] = r1;					// start row
	cmds[5] = c2;					// end column
	cmds[6] = r2;					//end row

	cmds[7] = OLEDrgb_ExtractRFromRGB(lineColor);	//R
	cmds[8] = OLEDrgb_ExtractGFromRGB(lineColor);	//G
	cmds[9] = OLEDrgb_ExtractBFromRGB(lineColor);	//R


	if(bFill)
	{
		cmds[10] = OLEDrgb_ExtractRFromRGB(fillColor);	//R
		cmds[11] = OLEDrgb_ExtractGFromRGB(fillColor);	//G
		cmds[12] = OLEDrgb_ExtractBFromRGB(fillColor);	//R
	}
	OLEDrgb_WriteSPI2(InstancePtr, cmds, bFill ? 13: 10, NULL, 0);
}

/* ------------------------------------------------------------ */
/***	void Oledrgb::clear(void)
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
**		Clear the display. This clears the memory buffer and then
**		updates the display.
*/
void OLEDrgb_Clear(PmodOLEDrgb* InstancePtr)
{
	uint8_t cmds[5];
	cmds[0] = CMD_CLEARWINDOW; 		// Enter the “clear mode”
	cmds[1] = 0x00;					// Set the starting column coordinates
	cmds[2] = 0x00;					// Set the starting row coordinates
	cmds[3] = OLEDRGB_WIDTH - 1;	// Set the finishing column coordinates;
	cmds[4] = OLEDRGB_HEIGHT - 1;	// Set the finishing row coordinates;
	OLEDrgb_WriteSPI2(InstancePtr, cmds, 5, NULL, 0);
}

void OLEDrgb_DrawBitmap(PmodOLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t *pBmp)
{
	uint8_t cmds[6];
	//set column start and end
	cmds[0] = CMD_SETCOLUMNADDRESS;
	cmds[1] = c1;					// Set the starting column coordinates
	cmds[2] = c2;					// Set the finishing column coordinates

	//set row start and end
	cmds[3] = CMD_SETROWADDRESS;
	cmds[4] = r1;					// Set the starting row coordinates
	cmds[5] = r2;					// Set the finishing row coordinates

	OLEDrgb_WriteSPI2(InstancePtr, cmds, 6, pBmp, (((c2 - c1 + 1)  * (r2 - r1 + 1)) << 1));
}

/* ------------------------------------------------------------ */
/***	OledrgbHostInit
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
**		Perform FPGA device initialization to prepare for use
**		of the OLEDrgb display.

*/
void OLEDrgb_HostInit(PmodOLEDrgb* InstancePtr)
	{
	Xil_Out32(InstancePtr->GPIO_addr+4, 0x0000);
	Xil_Out32(InstancePtr->GPIO_addr, 0xA);
	OLEDrgb_SPIInit(&InstancePtr->OLEDSpi, 0, 0);
}

void OLEDrgb_DevInit(PmodOLEDrgb* InstancePtr)
	{
	/*Bring PmodEn HIGH*/
	Xil_Out32(InstancePtr->GPIO_addr,0b1010);

	usleep(100000);
	/* command un-lock*/
	OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi,0xFD);
	OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi,0x12);

    usleep(100000);
	/* 5. Univision Initialization Steps*/

	// 5a) Set Display Off
    OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi, CMD_DISPLAYOFF);
	usleep(100000);
    // 5b) Set Remap and Data Format
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETREMAP, 0x72);

	// 5c) Set Display Start Line
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETDISPLAYSTARTLINE, 0x00); //start line is set at upper left corner

	// 5d) Set Display Offset
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETDISPLAYOFFSET, 0x00); //no offset

    // 5e)
	OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi, CMD_NORMALDISPLAY);

	// 5f) Set Multiplex Ratio
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETMULTIPLEXRATIO, 0x3F); //64MUX

    // 5g)Set Master Configuration
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETMASTERCONFIGURE, 0x8E);

    // 5h)Set Power Saving Mode
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_POWERSAVEMODE, 0x0B);

	// 5i) Set Phase Length
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_PHASEPERIODADJUSTMENT, 0x31); //phase 2 = 14 DCLKs, phase 1 = 15 DCLKS


	// 5j) Send Clock Divide Ratio and Oscillator Frequency
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_DISPLAYCLOCKDIV, 0xF0); //mid high oscillator frequency, DCLK = FpbCllk/2

	// 5k) Set Second Pre-charge Speed of Color A
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETPRECHARGESPEEDA, 0x64); //Set Second Pre-change Speed For ColorA

	// 5l) Set Set Second Pre-charge Speed of Color B
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETPRECHARGESPEEDB, 0x78); //Set Second Pre-change Speed For ColorB

	// 5m) Set Second Pre-charge Speed of Color C
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETPRECHARGESPEEDC, 0x64); //Set Second Pre-change Speed For ColorC

	// 5n) Set Pre-Charge Voltage
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETPRECHARGEVOLTAGE, 0x3A); // Pre-charge voltage =...Vcc

	// 50) Set VCOMH Deselect Level
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETVVOLTAGE, 0x3E); // Vcomh = ...*Vcc

	// 5p) Set Master Current
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_MASTERCURRENTCONTROL, 0x06);

	// 5q) Set Contrast for Color A
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETCONTRASTA, 0x91); //Set contrast for color A

	// 5r) Set Contrast for Color B
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETCONTRASTB, 0x50); //Set contrast for color B

	// 5s) Set Contrast for Color C
	OLEDrgb_WriteSPI1(&InstancePtr->OLEDSpi, CMD_SETCONTRASTC, 0x7D); //Set contrast for color C

	OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi, CMD_DEACTIVESCROLLING);   //disable scrolling

	// 5u) Clear Screen
	OLEDrgb_Clear(InstancePtr);
	usleep(100000);
	/* Turn on VCC and wait 100ms*/
	Xil_Out32(InstancePtr->GPIO_addr,0b1110);

	usleep(100000);

	/* Send Display On command*/
	OLEDrgb_WriteSPI(&InstancePtr->OLEDSpi, CMD_DISPLAYON);

}

int OLEDrgb_SPIInit(XSpi *SpiInstancePtr, u16 SpiDeviceId, u32 mode){
	int Status;
	XSpi_Config *ConfigPtr; /* Pointer to Configuration data */

	/*
	 * Initialize the SPI driver so that it is  ready to use.
	 */
	ConfigPtr = &XSpi_OLEDrgb;
	if (ConfigPtr == NULL) {
		return XST_DEVICE_NOT_FOUND;
	}

	Status = XSpi_CfgInitialize(SpiInstancePtr, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Set the Spi device as a master and in loopback mode.
	 */
	Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION  |  mode) & ~XSP_MANUAL_SSELECT_OPTION); //Might need to be OR'ed with 0x4 for phase also
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(SpiInstancePtr, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the SPI driver so that the device is enabled.
	 */
	XSpi_Start(SpiInstancePtr);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XSpi_IntrGlobalDisable(SpiInstancePtr);

	return XST_SUCCESS;


}


uint8_t OLEDrgb_WriteSPI(XSpi* OLEDSpi, uint8_t bVal)
{
	uint8_t	bRx;
	uint8_t txBuf=bVal;

	XSpi_Transfer(OLEDSpi, &txBuf, &bRx, 1);

	return bRx;
}

uint8_t OLEDrgb_WriteSPI1(XSpi* OLEDSpi, uint8_t bVal1, uint8_t bVal2)
{
	uint8_t	bRx[2];
	uint8_t txBuf[2] = {bVal1, bVal2};

	XSpi_Transfer(OLEDSpi, txBuf, bRx, 2);

	return bRx[0];
}

uint8_t OLEDrgb_WriteSPI2(PmodOLEDrgb* InstancePtr, uint8_t *pCmd, int nCmd, uint8_t *pData, int nData)
{
	XSpi_Transfer(&InstancePtr->OLEDSpi, pCmd, 0, nCmd);
	if (nData!= 0){
		Xil_Out32(InstancePtr->GPIO_addr,0b1111);
		XSpi_Transfer(&InstancePtr->OLEDSpi, pData, 0, nData);
		Xil_Out32(InstancePtr->GPIO_addr,0b1110);
	}

	return 1;
}

