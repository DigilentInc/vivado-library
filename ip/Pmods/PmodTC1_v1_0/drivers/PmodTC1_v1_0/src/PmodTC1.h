
#ifndef PMODTC1_H
#define PMODTC1_H


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

typedef struct PmodTC1{
	XSpi TC1Spi;
	u32 data;
}PmodTC1;

void TC1_begin(PmodTC1* InstancePtr, u32 SPI_Address);
void TC1_end(PmodTC1* InstancePtr);
int TC1_SPIInit(XSpi *SpiInstancePtr);
void TC1_readData(PmodTC1* InstancePtr);
double TC1_getTemp(PmodTC1* InstancePtr);
double TC1_getAMBTemp(PmodTC1* InstancePtr);
double TC1_celToFar(double celsius);






#endif // PMODTC1_H
