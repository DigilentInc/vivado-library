

/***************************** Include Files *******************************/
#include "PmodOLEDrgb.h"
#include "xspi_l.h"
#include "xspi.h"
#include <stdlib.h>
#include <string.h>
/************************** Function Definitions ***************************/

int SPIInit(XSpi *SpiInstancePtr, u16 SpiDeviceId, u32 mode);

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

XSpi *OLEDSpi=NULL;
u8 num_devices=0;

uint16_t BuildRGB(uint8_t R,uint8_t G,uint8_t B){return ((R>>3)<<11) | ((G>>2)<<5) | (B>>3);};
uint8_t ExtractRFromRGB(uint16_t wRGB){return (uint8_t)((wRGB>>11)&0x1F);};
uint8_t ExtractGFromRGB(uint16_t wRGB){return (uint8_t)((wRGB>>5)&0x3F);};
uint8_t ExtractBFromRGB(uint16_t wRGB){return (uint8_t)(wRGB&0x1F);};


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
void Oledrgb_begin(OLEDrgb* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
{
	XSpi* newarray = malloc(sizeof(XSpi)*(num_devices+1));
	if (num_devices!=0){
		memcpy(newarray, OLEDSpi,sizeof(XSpi)*num_devices);
		free(OLEDSpi);
	}
	OLEDSpi=newarray;
	InstancePtr->GPIO_addr=GPIO_Address;
	InstancePtr->SPI_addr=SPI_Address;
	InstancePtr->device_num=num_devices;
	num_devices++;
	XSpi_OLEDrgb.BaseAddress=InstancePtr->SPI_addr;

	Oledrgb_HostInit(InstancePtr);
	Oledrgb_DevInit(InstancePtr);
}

void DrawPixel(OLEDrgb* InstancePtr, uint8_t c, uint8_t r, uint16_t pixelColor)
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

	WriteSPI2(InstancePtr, cmds, 6, data, 2);
	Oledrgb_delay(1);
}

void DrawLine(OLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor)
{
	uint8_t cmds[8];
	cmds[0] = CMD_DRAWLINE; 		//draw line
	cmds[1] = c1;					// start column
	cmds[2] = r1;					// start row
	cmds[3] = c2;					// end column
	cmds[4] = r2;					//end row
	cmds[5] = ExtractRFromRGB(lineColor);	//R
	cmds[6] = ExtractGFromRGB(lineColor);	//G
	cmds[7] = ExtractBFromRGB(lineColor);	//R

	WriteSPI2(InstancePtr, cmds, 8, NULL, 0);
	Oledrgb_delay(1);
}

void DrawRectangle(OLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor, bool bFill, uint16_t fillColor)
{
	uint8_t cmds[13];
    cmds[0] = CMD_FILLWINDOW;		//fill window
    cmds[1] = (bFill ? ENABLE_FILL: DISABLE_FILL);
    cmds[2] = CMD_DRAWRECTANGLE;	//draw rectangle
	cmds[3] = c1;					// start column
	cmds[4] = r1;					// start row
	cmds[5] = c2;					// end column
	cmds[6] = r2;					//end row

	cmds[7] = ExtractRFromRGB(lineColor);	//R
	cmds[8] = ExtractGFromRGB(lineColor);	//G
	cmds[9] = ExtractBFromRGB(lineColor);	//R


	if(bFill)
	{
		cmds[10] = ExtractRFromRGB(fillColor);	//R
		cmds[11] = ExtractGFromRGB(fillColor);	//G
		cmds[12] = ExtractBFromRGB(fillColor);	//R
	}
	WriteSPI2(InstancePtr, cmds, bFill ? 13: 10, NULL, 0);
	Oledrgb_delay(1);
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
void Oledrgb_Clear(OLEDrgb* InstancePtr)
{
	uint8_t cmds[5];
	cmds[0] = CMD_CLEARWINDOW; 		// Enter the “clear mode”
	cmds[1] = 0x00;					// Set the starting column coordinates
	cmds[2] = 0x00;					// Set the starting row coordinates
	cmds[3] = OLEDRGB_WIDTH - 1;	// Set the finishing column coordinates;
	cmds[4] = OLEDRGB_HEIGHT - 1;	// Set the finishing row coordinates;
	WriteSPI2(InstancePtr, cmds, 5, NULL, 0);
	Oledrgb_delay(1);
}

void DrawBitmap(OLEDrgb* InstancePtr, uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t *pBmp)
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

	WriteSPI2(InstancePtr, cmds, 6, pBmp, (((c2 - c1 + 1)  * (r2 - r1 + 1)) << 1));
	Oledrgb_delay(1);
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
void Oledrgb_HostInit(OLEDrgb* InstancePtr)
	{
	Xil_Out32(InstancePtr->GPIO_addr+4, 0x0000);
	Xil_Out32(InstancePtr->GPIO_addr, 0xA);
	SPIInit(&OLEDSpi[InstancePtr->device_num], 0, 0);
}

void Oledrgb_DevInit(OLEDrgb* InstancePtr)
	{
	/*Bring PmodEn HIGH*/
	Xil_Out32(InstancePtr->GPIO_addr,0b1010);

	/* command un-lock*/
	WriteSPI(InstancePtr->device_num,0xFD);
    WriteSPI(InstancePtr->device_num,0x12);

	/* 5. Univision Initialization Steps*/

	// 5a) Set Display Off
	WriteSPI(InstancePtr->device_num, CMD_DISPLAYOFF);

    // 5b) Set Remap and Data Format
	WriteSPI1(InstancePtr->device_num, CMD_SETREMAP, 0x72);
	// 5c) Set Display Start Line
	WriteSPI1(InstancePtr->device_num, CMD_SETDISPLAYSTARTLINE, 0x00); //start line is set at upper left corner

	// 5d) Set Display Offset
	WriteSPI1(InstancePtr->device_num, CMD_SETDISPLAYOFFSET, 0x00); //no offset

    // 5e)
    WriteSPI(InstancePtr->device_num, CMD_NORMALDISPLAY);

	// 5f) Set Multiplex Ratio
	WriteSPI1(InstancePtr->device_num, CMD_SETMULTIPLEXRATIO, 0x3F); //64MUX

    // 5g)Set Master Configuration
    WriteSPI1(InstancePtr->device_num, CMD_SETMASTERCONFIGURE, 0x8E);

    // 5h)Set Power Saving Mode
    WriteSPI1(InstancePtr->device_num, CMD_POWERSAVEMODE, 0x0B);

	// 5i) Set Phase Length
	WriteSPI1(InstancePtr->device_num, CMD_PHASEPERIODADJUSTMENT, 0x31); //phase 2 = 14 DCLKs, phase 1 = 15 DCLKS


	// 5j) Send Clock Divide Ratio and Oscillator Frequency
	WriteSPI1(InstancePtr->device_num, CMD_DISPLAYCLOCKDIV, 0xF0); //mid high oscillator frequency, DCLK = FpbCllk/2

	// 5k) Set Second Pre-charge Speed of Color A
	WriteSPI1(InstancePtr->device_num, CMD_SETPRECHARGESPEEDA, 0x64); //Set Second Pre-change Speed For ColorA

	// 5l) Set Set Second Pre-charge Speed of Color B
	WriteSPI1(InstancePtr->device_num, CMD_SETPRECHARGESPEEDB, 0x78); //Set Second Pre-change Speed For ColorB

	// 5m) Set Second Pre-charge Speed of Color C
	WriteSPI1(InstancePtr->device_num, CMD_SETPRECHARGESPEEDC, 0x64); //Set Second Pre-change Speed For ColorC

	// 5n) Set Pre-Charge Voltage
	WriteSPI1(InstancePtr->device_num, CMD_SETPRECHARGEVOLTAGE, 0x3A); // Pre-charge voltage =...Vcc

	// 50) Set VCOMH Deselect Level
	WriteSPI1(InstancePtr->device_num, CMD_SETVVOLTAGE, 0x3E); // Vcomh = ...*Vcc

	// 5p) Set Master Current
	WriteSPI1(InstancePtr->device_num, CMD_MASTERCURRENTCONTROL, 0x06);

	// 5q) Set Contrast for Color A
	WriteSPI1(InstancePtr->device_num, CMD_SETCONTRASTA, 0x91); //Set contrast for color A

	// 5r) Set Contrast for Color B
	WriteSPI1(InstancePtr->device_num, CMD_SETCONTRASTB, 0x50); //Set contrast for color B

	// 5s) Set Contrast for Color C
	WriteSPI1(InstancePtr->device_num, CMD_SETCONTRASTC, 0x7D); //Set contrast for color C

	WriteSPI(InstancePtr->device_num, CMD_DEACTIVESCROLLING);   //disable scrolling

	// 5u) Clear Screen
	Oledrgb_Clear(InstancePtr);

	/* Turn on VCC and wait 100ms*/
	Xil_Out32(InstancePtr->GPIO_addr,0b1110);

	Oledrgb_delay(100);

	/* Send Display On command*/
	WriteSPI(InstancePtr->device_num, CMD_DISPLAYON);

}

int SPIInit(XSpi *SpiInstancePtr, u16 SpiDeviceId, u32 mode){
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


uint8_t WriteSPI(uint8_t dev_num, uint8_t bVal)
{
	uint8_t	bRx;
	uint8_t txBuf=bVal;

	XSpi_Transfer(&OLEDSpi[dev_num], &txBuf, &bRx, 1);

	return bRx;
}

uint8_t WriteSPI1(uint8_t dev_num, uint8_t bVal1, uint8_t bVal2)
{
	uint8_t	bRx[2];
	uint8_t txBuf[2] = {bVal1, bVal2};

	XSpi_Transfer(&OLEDSpi[dev_num], txBuf, bRx, 2);

	return bRx[0];
}

uint8_t WriteSPI2(OLEDrgb* InstancePtr, uint8_t *pCmd, int nCmd, uint8_t *pData, int nData)
{
	XSpi_Transfer(&OLEDSpi[InstancePtr->device_num], pCmd, 0, nCmd);
	if (nData!= 0){
		Xil_Out32(InstancePtr->GPIO_addr,0b1111);
		XSpi_Transfer(&OLEDSpi[InstancePtr->device_num], pData, 0, nData);
		Xil_Out32(InstancePtr->GPIO_addr,0b1110);
	}

	return 1;
}
void Oledrgb_delay(int ms){
	int i;
	for(i=0;i<1000*ms;i++);
}

