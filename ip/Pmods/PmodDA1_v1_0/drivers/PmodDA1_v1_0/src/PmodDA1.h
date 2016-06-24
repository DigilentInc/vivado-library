
#ifndef PMODDA1_H
#define PMODDA1_H


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
#define DACSPI1_NO_BITS		8
#define DACSPI1_CTRL_BYTE	0x00	// selecting convertor DAC A to be written
#define DACSPI1_ERR_SUCCESS				0	// The action completed successfully
#define DACSPI1_ERR_VAL_OUT_OF_RANGE	1	// The value is out of range

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

typedef struct PmodDA1{
	XSpi DA1Spi;
}PmodDA1;

void DA1_begin(PmodDA1* InstancePtr, u32 SPI_Address);//Initialize the PmodDA1.
void DA1_end(PmodDA1* InstancePtr);//Stops the device
int  DA1_SPIInit(XSpi *SpiInstancePtr);//Initializes the PmodDA1 SPI
u8  DA1_WriteIntegerValue(u8 bIntegerValue,PmodDA1* InstancePtr); //If the value is inside the allowed range (0 - 0xFF), this function writes the 12 bits value to the DA converter,
																  //by writing 16 bits to SPI and returns the DACSPI1_ERR_SUCCESS status message.
																  // If the value is outside the allowed range, the function does nothing and returns the DACSPI1_ERR_VAL_OUT_OF_RANGE message.
u8  DA1_WritePhysicalValue(float dPhysicalValue,PmodDA1* InstancePtr);//If the integer value is within the accepted range (0 - 0xFF), this function writes the 12- bit value to the
																	  //DA converter, by writing 8 bits to SPI, and returns the DACSPI1_ERR_SUCCESS message.
																	  //If the integer value is outside the allowed range, the function does nothing and returns the
																	  //DACSPI1_ERR_VAL_OUT_OF_RANGE message.
																	  //If the dReference function argument is missing, 3.3 value is used as reference value.





#endif // PMODDA1_H
