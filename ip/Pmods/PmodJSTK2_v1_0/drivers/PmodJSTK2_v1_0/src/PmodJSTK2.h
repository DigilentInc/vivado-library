
#ifndef PMODJSTK2_H
#define PMODJSTK2_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

#include "xparameters.h"

#ifdef XPAR_MICROBLAZE_ID
	#define ITERS_PER_USEC   (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
	#define ITERS_PER_USEC     (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0

/* Define PmodJSTK2 SPI commands.
*/
#define mskCmd				0xFC

#define cmdSetLed			0x80
#define cmdSetLedOn			cmdSetLed | 0x01
#define cmdSetLedOff		cmdSetLed

#define cmdSetLedRGB		0x84

#define cmdSetInversion		0x88
#define cmdSetInversionX	0x88 | 0x02
#define cmdSetInversionY	0x88 | 0x01
#define cmdSetInversionOff	cmdSetInversion

#define cmdGetPosition		0xC0

#define cmdGetStatus		0xF0
#define cmdGetFirmwareVer	0xF1

#define cmdGetCalXMin		0xE0
#define cmdGetCalXMax		0xE1
#define cmdGetCalYMin		0xE2
#define cmdGetCalYMax		0xE3
#define cmdGetCalXCenMin	0xE4
#define cmdGetCalXCenMax	0xE5
#define cmdGetCalYCenMin	0xE6
#define cmdGetCalYCenMax	0xE7

#define cmdCalibrate		0xA4

#define cmdSetCalXMin		0xA8
#define cmdSetCalXMax		0xA9
#define cmdSetCalYMin		0xAA
#define cmdSetCalYMax		0xAB
#define cmdSetCalXCenMin	0xAC
#define cmdSetCalXCenMax	0xAD
#define cmdSetCalYCenMin	0xAE
#define cmdSetCalYCenMax	0xAF

#define cmdSetCalXMinMax	0xB0
#define cmdSetCalYMinMax	0xB1
#define cmdSetCalXCenMinMax	0xB2
#define cmdSetCalYCenMinMax	0xB3

#define cmdWriteFlash		0xB8
#define cmdRldFromFlash		0xBC

#define cbSndMax			16

/* Define the bit positions used in a status packet.
*/
#define bnCalibrating           7
#define bnLastCal               6
#define bnLastFWS               5
#define bnLastFRS               4
#define bnInvertX               1
#define bnInvertY               0

#define bitCalibrating          (1 << bnCalibrating)
#define bitLastCal              (1 << bnLastCal)
#define bitLastFWS              (1 << bnLastFWS)
#define bitLastFRS              (1 << bnLastFRS)
#define bitInvertX              (1 << bnInvertX)
#define bitInvertY              (1 << bnInvertY)

/* Define bit postiions used in fsButtons.
*/
#define bnExtpkt                7
#define bnTrigger               1
#define bnJstk                  0

#define bitExtpkt               (1 << bnExtpkt)
#define bitTrigger              (1 << bnTrigger)
#define bitJstk                 (1 << bnJstk)


/* ------------------------------------------------------------ */
/*		Register addresses Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Parameters Definitions							*/
/* ------------------------------------------------------------ */



/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodJSTK2{
	XSpi JSTK2Spi;
}PmodJSTK2;

void JSTK2_begin(PmodJSTK2* InstancePtr, u32 SPI_Address);
void JSTK2_end(PmodJSTK2* InstancePtr);
int JSTK2_SPIInit(XSpi *SpiInstancePtr);


u16 JSTK2_getYRaw(PmodJSTK2* InstancePtr);
u16 JSTK2_getXRaw(PmodJSTK2* InstancePtr);
void JSTK2_setLed(PmodJSTK2* InstancePtr, u8 red, u8 blue, u8 green);

u8 JSTK2_getBtns(PmodJSTK2* InstancePtr);

u8 JSTK2_getX(PmodJSTK2* InstancePtr);
u8 JSTK2_getY(PmodJSTK2* InstancePtr);

u8 JSTK2_getStatus(PmodJSTK2* InstancePtr);

void JSTK2_setInversion(PmodJSTK2* InstancePtr, u8 invX, u8 invY);

u8 JSTK2_rldFromFlash(PmodJSTK2* InstancePtr);
u8 JSTK2_writeFlash(PmodJSTK2* InstancePtr);
//u8 getStatus(PmodJSTK2* InstancePtr);
//

//Calibration functions
void JSTK2_calibrate();	//done

u16 JSTK2_getCalXMin(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalXMax(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalYMin(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalYMax(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalXCenMin(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalXCenMax(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalYCenMin(PmodJSTK2* InstancePtr);
u16 JSTK2_getCalYCenMax(PmodJSTK2* InstancePtr);

void JSTK2_setCalXMin(PmodJSTK2* InstancePtr, u16 XMinCal);
void JSTK2_setCalXMax(PmodJSTK2* InstancePtr, u16 XMaxCal);
void JSTK2_setCalYMin(PmodJSTK2* InstancePtr, u16 YMinCal);
void JSTK2_setCalYMax(PmodJSTK2* InstancePtr, u16 YMaxCal);
void JSTK2_setCalXCenMin(PmodJSTK2* InstancePtr, u16 XCenMinCal);
void JSTK2_setCalXCenMax(PmodJSTK2* InstancePtr, u16 XCenMaxCal);
void JSTK2_setCalYCenMin(PmodJSTK2* InstancePtr, u16 YCenMinCal);
void JSTK2_setCalYCenMax(PmodJSTK2* InstancePtr, u16 YCenMaxCal);
void JSTK2_setCalXMinMax(PmodJSTK2* InstancePtr, u16 XMinCal, u16 XMaxCal);
void JSTK2_setCalYMinMax(PmodJSTK2* InstancePtr, u16 YMinCal, u16 YMaxCal);
void JSTK2_setCalXCenMinMax(PmodJSTK2* InstancePtr, u16 XCenMinCal, u16 XCenMaxCal);
void JSTK2_setCalYCenMinMax(PmodJSTK2* InstancePtr, u16 YCenMinCal, u16 YCenMaxCal);

//utility functions
void JSTK2_getData(PmodJSTK2* InstancePtr, u8* recv, u8 nData);
void delay(int micros);



#endif // PMODJSTK2_H
