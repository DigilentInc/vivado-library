
#ifndef PMODDPG1_H
#define PMODDPG1_H


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

#define KPA_PRESSURE_TYPE		0
#define ATM_PRESSURE_TYPE		1
#define PSI_PRESSURE_TYPE		2
#define INH20_PRESSURE_TYPE		3
#define CMH20_PRESSURE_TYPE		4
#define MMHG_PRESSURE_TYPE		5

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

typedef struct PmodDPG1{
	XSpi DPG1Spi;
	u32 data;
}PmodDPG1;

void   DPG1_begin(PmodDPG1* InstancePtr, u32 SPI_Address); //Initialize the PmodDPG1.
void   DPG1_end(PmodDPG1* InstancePtr); //Stops the device
int    DPG1_SPIInit(XSpi *SpiInstancePtr); // spi initialization
void   DPG1_readData(PmodDPG1* InstancePtr); // uses spi and reads the data into InstancePtr->data
double DPG1_GetPressure(int pType, PmodDPG1* InstancePtr); // returns the physical value either KPA, ATM,PSI, INH2O, CMH2O or MMHG



#endif // PMODDPG1_H
