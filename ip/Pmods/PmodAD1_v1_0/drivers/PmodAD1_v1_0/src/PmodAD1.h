
#ifndef PMODAD1_H
#define PMODAD1_H
#define AD1_FLOATING_POINT

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0
#define ADCSPI_NO_BITS		12  //used in the process to get the physical value

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

typedef struct PmodAD1{
	XSpi AD1Spi;
	u32 data;
}PmodAD1;

void  AD1_begin(PmodAD1* InstancePtr, u32 SPI_Address); //Initialize the PmodAD1.
void  AD1_end(PmodAD1* InstancePtr); //Stops the device
int   AD1_SPIInit(XSpi *SpiInstancePtr); // spi initialization
void  AD1_readData(PmodAD1* InstancePtr);// uses spi and reads the data into InstancePtr->data
int   AD1_GetIntegerValue(PmodAD1* InstancePtr);// returns the integer value 
float AD1_GetPhysicalValue(float dReference, PmodAD1* InstancePtr );// returns the physical value





#endif // PMODAD1_H
