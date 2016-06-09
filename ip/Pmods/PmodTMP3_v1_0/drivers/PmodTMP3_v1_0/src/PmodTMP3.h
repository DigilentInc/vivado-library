
#ifndef PMODTMP3_H
#define PMODTMP3_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xiic_l.h"
#include "xiic.h"
#include "xparameters.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0

#ifdef XPAR_XINTC_NUM_INSTANCES
 #include "xintc.h"
 #define INTC		XIntc
 #define INTC_HANDLER	XIntc_InterruptHandler
#else
#ifdef XPAR_SCUGIC_0_DEVICE_ID
 #include "xscugic.h"
 #define INTC		XScuGic
 #define INTC_HANDLER	XScuGic_InterruptHandler
#else
#define NO_IRPT 1
#endif
#endif

#ifdef XPAR_MICROBLAZE_ID
	#define ITERS_PER_USEC   (XPAR_CPU_CORE_CLOCK_FREQ_HZ / 1000000)
#else
	#define ITERS_PER_USEC     (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 1000000)
#endif
/* ------------------------------------------------------------ */
/*		Register addresses Definitions							*/
/* ------------------------------------------------------------ */

#define TMP3_REG_TEMP     0x0
#define TMP3_REG_CONFIG   0x1
#define TMP3_REG_HYST     0x2
#define TMP3_REG_LIMITSET 0x3

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */

//Configuration settings
#define TMP3_ONESHOT	0x80 //One Shot mode
#define TMP3_RES9		0x00 //9-bit resolution
#define TMP3_RES10		0x20 //10-bit resolution
#define TMP3_RES11		0x40 //11-bit resolution
#define TMP3_RES12		0x60 //12-bit resolution
#define TMP3_FAULT1		0x00 //1 fault queue bits
#define TMP3_FAULT2		0x08 //2 fault queue bits
#define TMP3_FAULT4		0x10 //4 fault queue bits
#define TMP3_FAULT6		0x18 //6 fault queue bits
#define TMP3_ALERTLOW	0x00 //Alert bit active-low
#define TMP3_ALERTHIGH	0x04 //Alert bit active-high
#define TMP3_CMPMODE	0x00 //comparator mode
#define TMP3_INTMODE	0x02 //interrupt mode
#define TMP3_SHUTDOWN	0x01 //Shutdown enabled
#define	TMP3_STARTUP	0x00 //Shutdown Disabled

/* ------------------------------------------------------------ */
/*				Parameters Definitions							*/
/* ------------------------------------------------------------ */

//Device Constants
#define TMP3_ADDR	0x48	//based on jumpers JP1, JP2, and JP3
							//a table explaining the various
							//address configurations is available
							//in the PmodTMP3 Reference Manual

#define TMP3_CONF_DEFAULT	(TMP3_RES9 | TMP3_FAULT1 | TMP3_ALERTLOW | TMP3_CMPMODE)
	//Default Startup Configuration Used, this is just so the device can be
	//reset to startup configurations at a later time, it doesn't need to be
	//called anywhere.

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */


typedef struct PmodTMP3{
	XIic TMP3Iic;
	u8 chipAddr;
	u8 currentRegister;
	u8 recvbytes;
	u8* recv;
#ifndef NO_IRPT
	INTC intc;
#endif
}PmodTMP3;

void TMP3_begin(PmodTMP3* InstancePtr, u32 IIC_Address, u8 Chip_Address);
void TMP3_end(PmodTMP3* InstancePtr);
int TMP3_IICInit(XIic *IicInstancePtr);
void TMP3_ReadIIC(PmodTMP3* InstancePtr, u8 reg, u8 *Data, int nData);
void TMP3_WriteIIC(PmodTMP3* InstancePtr, u8 reg, u8 *Data, int nData);
int TMP3_SetupInterruptSystem(PmodTMP3* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* SendHandler,  void* ReceiveHandler);

void TMP3_config(PmodTMP3 *InstancePtr, u8 configuration);
double TMP3_getTemp(PmodTMP3 *InstancePtr);

double TMP3_FtoC(double tempF);
double TMP3_CtoF(double tempC);

void TMP3_delay(int micros);

#endif // PMODTMP3_H
