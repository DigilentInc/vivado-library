
#ifndef PMODJSTK_H
#define PMODJSTK_H


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

typedef struct PmodJSTK{
	XSpi JSTKSpi;
}PmodJSTK;

void JSTK_begin(PmodJSTK* InstancePtr, u32 SPI_Address);
void JSTK_end(PmodJSTK* InstancePtr);
int JSTK_SPIInit(XSpi *SpiInstancePtr);


u16 JSTK_getY(PmodJSTK* InstancePtr);
u16 JSTK_getX(PmodJSTK* InstancePtr);
void JSTK_setLeds(PmodJSTK* InstancePtr, u8 ledSt);

u8 JSTK_getBtn(PmodJSTK* InstancePtr);
void JSTK_getData(PmodJSTK* InstancePtr, u8* recv);
void delay(int micros);



#endif // PMODJSTK_H
