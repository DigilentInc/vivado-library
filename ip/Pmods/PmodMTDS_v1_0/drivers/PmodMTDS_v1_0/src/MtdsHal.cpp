/************************************************************************/
/*																		*/
/*	MtdsHalSim.cpp	--	MTDS HAL Layer for MPIDE/Arduino				*/
/*																		*/
/************************************************************************/
/*	Author: 	Gene Apperson											*/
/*	Copyright 2016, Digilent, Inc. All rights reserved.					*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This module contains the hardware abstraction layer implementation	*/
/*	for the MTDS library for use on the MPIDE and Arduino platforms.	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	2016-09-26(GeneA): Created by moving low level communications and	*/
/*		control code from mtds.cpp and MtdsCore.cpp here as part of the	*/
/*		definition of a hardware abstraction layer to make the library	*/
/*		more portable across platforms.									*/
/*																		*/
/************************************************************************/


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#if defined(MPIDE)
#include	<WProgram.h>
#elif defined(__MICROBLAZE__)
#include "xparameters.h"
#include "xspi.h"
#include "xtmrctr.h"
XSpi MTDS_Spi;
XTmrCtr Timer;
XTmrCtr_Config Timer_Config =
{
	0,
	0,
#ifdef XPAR_CPU_CORE_CLOCK_FREQ_HZ
	XPAR_CPU_CORE_CLOCK_FREQ_HZ
#else
	100000000
#endif
};
XSpi_Config MTDSSPIConfig =
{
	0,
	XPAR_PMODMTDS_0_AXI_LITE_SPI_BASEADDR,
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
#else
#include	<Arduino.h>
p32_spi *	pspiMtds;			// pointer to the SPI port used
#endif

#include	<stdint.h>

#include	"ProtoDefs.h"
#include	"mtds.h"
#include	"MtdsHal.h"

/* ------------------------------------------------------------ */
/*				Local Type Definitions							*/
/* ------------------------------------------------------------ */

/* Symbols used for initializing and operating the SPI controller.
*/
#define SPI_MODE0 ((0 << _SPICON_CKP)|(1 << _SPICON_CKE))		// CKP = 0 CKE = 1
#define SPI_MODE1 ((0 << _SPICON_CKP)|(0 << _SPICON_CKE))		// CKP = 0 CKE = 0
#define SPI_MODE2 ((1 << _SPICON_CKP)|(1 << _SPICON_CKE))		// CKP = 1 CKE = 1 
#define SPI_MODE3 ((1 << _SPICON_CKP)|(0 << _SPICON_CKE))		// CKP = 1 CKE = 0

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */



int		pinMtdsSel;				// digital pin number of CS pin

/* ------------------------------------------------------------ */
/*				Local Variables									*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

uint16_t MtdsHalComputeBrg(uint32_t spd);

/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */
/***	MtdsHalInitPins(pinSel)
**
**	Parameters:
**		pinSel		- digital pin number of select pin
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This initializes performs any initialization required for digital
**		pin i/o. The pinSel parameter is specifically used in the Arduini/MPIDE
**		platform and is not used is ignored on other platforms.
*/

void MtdsHalInitPins(int pinSel) {

	/* Set up the pin directions for the SPI pins. The hardware automatically sets
	** up the pin directions when the SPI controller is enabled, but there are some
	** ancillary things (such as ensuring that the pins are digital pins) that occur
	** by calling pinMode()
	*/
#if defined(MPIDE)
	pinMode(MOSI, OUTPUT);
	pinMode(MISO, INPUT);
	pinMode(SCK, OUTPUT);

	pinMtdsSel = pinSel;

	digitalWrite(pinMtdsSel, HIGH);
	pinMode(pinMtdsSel, OUTPUT);
#elif defined(__MICROBLAZE__) || defined(__arm__)
	XTmrCtr_CfgInitialize(&Timer, &Timer_Config, XPAR_PMODMTDS_0_AXI_LITE_TIMER_BASEADDR);
	XTmrCtr_SetOptions(&Timer, 0, XTC_AUTO_RELOAD_OPTION| XTC_CASCADE_MODE_OPTION);
	XTmrCtr_SetResetValue(&Timer, 0, 0);
	XTmrCtr_Reset(&Timer,0);
	XTmrCtr_Reset(&Timer,1);
	XTmrCtr_Start(&Timer, 0);
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+4, 0b11);//Set IntA and IntB to inputs
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, 0b111);
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+12, 0b000);//Set chip selects(ch2) to outputs
#endif
}

/* ------------------------------------------------------------ */
/***	MtdsHalEnableStatusPin(idPin
**
**	Parameters:
**		idPin		- status pin id number of pin to enable
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Enable the specified status pin. This means make it be an input.
**
**		When building for the Multi-Touch Display Shield, the pin numbers for
**		the status pins are defined in the MtdsHal.h header file. When building
**		for the PmodMtds, we have to compute the pin numbers based on the Pmod
**		connector the board is plugged into. We can get this as a constant offset
**		from the pin number of ch SPI CS pin, pinMtdsSel. For now, we assume that
**		if we are being built for a board with Pmod connectors, we are using the
**		PmodMtds.
*/

void MtdsHalEnableStatusPin(int idPin) {

#if defined(_BOARD_CEREBOT_MX3CK_) || \
	defined(_BOARD_CEREBOT_MX4CK_) || \
	defined(_BOARD_CEREBOT_MX7CK_)

	if (idPin == idPinStatusA) {
		pinMode(pinMtdsSel+6, INPUT);
	}
	else if (idPin == idPinStatusB) {
		pinMode(pinMtdsSel+7, INPUT);
	}
#elif defined(__MICROBLAZE__) || defined(__arm__)
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+4,0b11);//Set interrupts (ch1) as inputs
#else

	if (idPin == idPinStatusA) {
		pinMode(pinMtdsIntA, INPUT);
	}
	else if (idPin == idPinStatusB) {
		pinMode(pinMtdsIntB, INPUT);
	}

#endif

}

/* ------------------------------------------------------------ */
/***	MtdsHalGetStatusPin(idPin)
**
**	Parameters:
**		idPin		- id of status pin to read
**
**	Return Values:
**		Returns true if pin is high, false if pin is low
**
**	Errors:
**		none
**
**	Description:
**		Return the state of the specified status pin.
*/

bool MtdsHalGetStatusPin(int idPin) {

#if defined(_BOARD_CEREBOT_MX3CK_) || \
	defined(_BOARD_CEREBOT_MX4CK_) || \
	defined(_BOARD_CEREBOT_MX7CK_)

	if (idPin == idPinStatusA) {
		return (digitalRead(pinMtdsSel+6) != LOW);
	}
	else if (idPin == idPinStatusB) {
		return (digitalRead(pinMtdsSel+7) != LOW);
	}
	else {
		return false;
	}
#elif defined (__MICROBLAZE__)

#else

	if (idPin == idPinStatusA) {
		return (digitalRead(pinMtdsIntA) != LOW);
	}
	else if (idPin == idPinStatusB) {
		return (digitalRead(pinMtdsIntB) != LOW);
	}
	else {
		return false;
	}

#endif

}

/* ------------------------------------------------------------ */
/***	MtdsHalResetDisplay(pinSel)
**
**	Parameters:
**		pinSel		- digital pin number for SPI CS pin
**
**	Return Values:
**		Returns true if reset actually performed, false if not
**
**	Errors:
**		none
**
**	Description:
**		This function is used to reset the display board on hardware
**		and platforms where the display board can be reset. 
**		The Multi-Toucn	Display Shield doesn't support software controlled
**		reset and is automatically reset whenever the host board is reset.
**		The PmodMTDS doesn't get reset automatically and needs to be reset
**		under software control when the system initializes.
**		The pinSel parameter can be used to determine which pin needs to
**		be toggled to reset the display.
*/

bool MtdsHalResetDisplay(int pinSel) {
	int pinRst = pinSel + 5;

#if defined(_BOARD_CEREBOT_MX3CK_) || \
	defined(_BOARD_CEREBOT_MX4CK_) || \
	defined(_BOARD_CEREBOT_MX7CK_)

	/* If we are building for a Cerebot Board, we are most likely talking to a
	** PmodMTDS, so we need to do a hardware reset on it before we try to establish
	** communications with it.
	*/
	pinMode(pinRst, OUTPUT);
	digitalWrite(pinRst, LOW);
	MtdsHalDelayMs(1);
	digitalWrite(pinRst, HIGH);
	MtdsHalDelayMs(1);
	return true;
#else
	return false;
#endif


}

/* ------------------------------------------------------------ */
/***	MtdsHalInitSpi(pspiInit, frq)
**
**	Parameters:
**		pspiInit		- indicator for the SPI port number
**		frq				- spi clock frequency to use
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This function initializes the SPI port being used by the platform
**		for communications between the host and the display interface. Depending
**		on the platform, one of both of the paramters may be ignored.
*/

void MtdsHalInitSpi(uint32_t pspiInit, uint32_t frq) {

#if defined(__PIC32__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor.
	*/
	uint8_t		tmp;
	uint16_t	brg;

	/* We don't actually use the SPI library, but we use the same port, so
	** we can make use of the same symbols in the board variant file for our
	** host board.
	*/
	pspiMtds = (p32_spi *)pspiInit;

#if defined(__PIC32MX1XX__) || defined(__PIC32MX2XX__) || defined(__PIC32MZXX__)  || defined(__PIC32MX47X__)
	/* Set up PPS pin mapping if we are on a chip with PPS.
	*/
    mapPps(_SPI_MISO_PIN, _SPI_MISO_IN);
    mapPps(_SPI_MOSI_PIN, _SPI_MOSI_OUT);
#endif

	brg = MtdsHalComputeBrg(frq);

	/* Initialize the SPI controller.
	*/
	pspiMtds->sxCon.reg = 0;								// turn SPI controller off
	tmp = pspiMtds->sxBuf.reg;								// clear the receive buffer
	pspiMtds->sxBrg.reg = brg;								// set the clock frequency
	pspiMtds->sxStat.clr = (1 << _SPISTAT_SPIROV);			// clear the receive overflow bit.
	pspiMtds->sxCon.set = (1 << _SPICON_MSTEN) + SPI_MODE0;	// set master, mode 0
	pspiMtds->sxCon.set = (1 << _SPICON_ON);				// turn SPI controller on

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	** Init the SPI controller with the following conditions:
	**		SPIE = 0	: SPI interrupts disabled
	**		SPE = 1		: SPI enabled
	**		DORD = 0	: shift directions MSB first
	**		MSTR = 1	: Master mode
	**		CPOL = 0	: SPI mode 0
	**		CPHA = 0
	**		SPR1 = 0	: SCK frequency = Fosc/4
	**		SPR0 = 0
	**
	**		SPI2X = 0
	*/
	SPCR = _BV(SPE) | _BV(MSTR);
	SPSR = _BV(SPI2X);
#elif defined(__MICROBLAZE__)||defined(__arm__)
XSpi_CfgInitialize(&MTDS_Spi, &MTDSSPIConfig, XPAR_PMODMTDS_0_AXI_LITE_SPI_BASEADDR);
XSpi_SetOptions(&MTDS_Spi, (XSP_MASTER_OPTION) | XSP_MANUAL_SSELECT_OPTION);
XSpi_SetSlaveSelect(&MTDS_Spi, 1);
XSpi_Start(&MTDS_Spi);
XSpi_IntrGlobalDisable(&MTDS_Spi);
#else
#error "No supported processor defined"
#endif

}

/* ------------------------------------------------------------ */
/***	MtdsHalEnableSlave(fEn)
**
**	Parameters:
**		fEn		- true to enable, false to disable
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Enable/Disable the SPI slave (display board).
*/

void MtdsHalEnableSlave(bool fEn) {
#if defined(PIC32)
	if (fEn) {
		digitalWrite(pinMtdsSel, LOW);
	}
	else {
		digitalWrite(pinMtdsSel, HIGH);
	}
#elif defined(__MICROBLAZE__) || defined(__arm__)
	if (fEn){
		Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, Xil_In32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8)&0b011);
	}
	else{
		Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, Xil_In32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8)|0b100);
	}
#endif
}

/* ------------------------------------------------------------ */
/***	MtdsHalSpiReady() 
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns true if the SPI port is ready to accept a byte
**
**	Errors:
**		none
**
**	Description:
**		Determine if the SPI port can accept a byte
*/

bool MtdsHalSpiReady() {

#if defined(__PIC32__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/
	return (pspiMtds->sxStat.reg & (1 << _SPISTAT_SPITBE)) != 0;

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	*/
	return true;

#elif defined(__MICROBLAZE__)||defined(__arm__)
return XSpi_GetStatusReg(&MTDS_Spi)&&0b100;//TX_Empty bit
#else
#error "No supported processor defined"
#endif

}

/* ------------------------------------------------------------ */
/***	MtdsHalPutSpiByte(bSnd)
**
**	Parameters:
**		bSnd		- byte to write to the SPI port
**
**	Return Values:
**		Returns the byte read from the SPI port
**
**	Errors:
**		none
**
**	Description:
**		Exchange a byte with the SPI slave device (display board)
*/

uint8_t MtdsHalPutSpiByte(uint8_t bSnd) {
	uint8_t	bRcv;

#if defined(__PIC32__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/
	while ((pspiMtds->sxStat.reg & (1 << _SPISTAT_SPITBE)) == 0) {
	}
	pspiMtds->sxBuf.reg = bSnd;

	while ((pspiMtds->sxStat.reg & (1 << _SPISTAT_SPIRBF)) == 0) {
	}
	bRcv = pspiMtds->sxBuf.reg;

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	*/
    SPDR = bSnd;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop from iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while ((SPSR & _BV(SPIF)) == 0) ; // wait
    bRcv = SPDR;
#elif defined(__MICROBLAZE__)||defined(__arm__)
//XSpi_Transfer(&MTDS_Spi, &bSnd,&bRcv, 1);

    XSpi_WriteReg(MTDS_Spi.BaseAddr, XSP_DTR_OFFSET, bSnd);//Fill TX
	XSpi_SetControlReg(&MTDS_Spi, XSpi_GetControlReg(&MTDS_Spi)&~XSP_CR_TRANS_INHIBIT_MASK);//Start Transfer
	while ((XSpi_IntrGetStatus(&MTDS_Spi) & XSP_INTR_TX_EMPTY_MASK) == 0);//Wait until TX is empty
	XSpi_IntrClear(&MTDS_Spi,XSP_INTR_TX_EMPTY_MASK);//Clear TX Empty interrupt
	XSpi_SetControlReg(&MTDS_Spi, XSpi_GetControlReg(&MTDS_Spi) | XSP_CR_TRANS_INHIBIT_MASK);//Stop Transfer
    bRcv = XSpi_ReadReg(MTDS_Spi.BaseAddr, XSP_DRR_OFFSET);//Receive RX

#else
#error "No supported processor defined"
#endif

	return bRcv;

}

/* ------------------------------------------------------------ */
/***	MtdsHalTmsElapsed()
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns the number of milliseconds elapsed since startup
**
**	Errors:
**		none
**
**	Description:
**		Returns the number of milliseconds that have elapsed since the host
**		started running. This is used for determining timeout for failed communications
**		between the host and the display device and for timing how long API functions
**		take to execute.
**		This function is not strictly required, and can always just return 0 if the
**		hardware resources aren't available to implement it. If it isn't implemented,
**		detection of lost communications between the host and the display board won't
**		work correctly and the host may lock up if communications isn't working reliably.
*/

uint32_t MtdsHalTmsElapsed() {

	return millis();

}

/* ------------------------------------------------------------ */
/***	MtdsHalDelayMs(tmsDelay)
**
**	Parameters:
**		tmsDelay	- number of milliseconds to delay
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Delay the specified number of milliseconds and then return
*/

void MtdsHalDelayMs(uint32_t tmsDelay) {

	delay(tmsDelay);

}

/* ------------------------------------------------------------ */
/***	MtdsHalTusElapsed()
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns the number of microseconds elapsed since reset
**
**	Errors:
**		none
**
**	Description:
**		Return the number of microseconds elapsed since reset. This is only used
**		to time how long MTDS commands take to execute, so if it isn't implemented
**		or isn't accurate the only problem will be that the API function that
**		returns the time for a command to execute won't work correctly.
*/

uint32_t MtdsHalTusElapsed() {

	return micros();

}

/* ------------------------------------------------------------ */
/***	MtdsHalDelayUs(usDelay)
**
**	Parameters:
**		usDelay		- number of microseconds to delay
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Delay the specified number of microseconds and then return. The delay time does not
**		need to be very precise, but must be at least as long as the number of microseconds
**		requested.
*/

void MtdsHalDelayUs(uint32_t usDelay) {

	delayMicroseconds(usDelay);

}

/* ------------------------------------------------------------ */
/***	MtdsHalComputeBrg(spd)
**
**	Parameters:
**		spd		- desired SPI clock speed
**
**	Return Values:
**		Returns value to load into SPI baud rate generator register
**
**	Errors:
**		none
**
**	Description:
**		This function computes the SPI baud rate generator value that
**		yields the closest supported SPI clock speed to the requested
**		value, based on the peripheral bus frequency.
*/

uint16_t MtdsHalComputeBrg(uint32_t spd) {
	uint16_t	brg;

	brg = 0;

	/* Compute the baud rate divider for this frequency.
	*/
#if defined(__PIC32__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/
	brg = (uint16_t)((__PIC32_pbClk / (2 * spd)) - 1);

	/* Check that the result is in the correct range.
	*/
	if (brg == 0xFFFF) {
		/* The user tried to set a frequency that is too high to support.
		** Set it to the highest supported frequency.
		*/
		brg = 0;
	}

	if (brg > 0x1FF) {
		/* The user tried to set a frequency that is too low to support.
		** Set it to the lowest supported frequency.
		*/
		brg = 0x1FF;
	}
#endif

	return brg;
}

/* ------------------------------------------------------------ */
/*				XXXX Object Class Implementation				*/
/* ------------------------------------------------------------ */

/***	ProcName
**
**	Parameters:
**
**	Return Values:
**
**	Errors:
**
**	Description:
**
*/

/* ------------------------------------------------------------ */

/************************************************************************/

