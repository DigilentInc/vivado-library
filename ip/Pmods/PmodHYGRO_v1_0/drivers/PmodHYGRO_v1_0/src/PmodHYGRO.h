/************************************************************************/
/*																		*/
/*	HYGROI2C.h		--		Declaration for HYGROI2C library		 	*/
/*																		*/
/************************************************************************/
/*	Author:		Arthur Brown											*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  File Description:													*/
/*	This file declares the HYGROI2C library functions and the constants	*/
/*	involved.															*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	01/30/2017(ArtVVB): created											*/
/*																		*/
/************************************************************************/

#ifndef PMODHYGRO_H
#define PMODHYGRO_H

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
/* ------------------------------------------------------------ */
/*		Register addresses Definitions							*/
/* ------------------------------------------------------------ */

#define HYGRO_CONFIG 0x2
#define HYGRO_HUM    0x1
#define HYGRO_TEMP   0x0

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */

#define HYGRO_CONFIG_RST     0x8000
#define HYGRO_CONFIG_HEAT    0x2000
#define HYGRO_CONFIG_MODE    0x1000
#define HYGRO_CONFIG_BTST    0x0800
#define HYGRO_CONFIG_TRES    0x0400
#define HYGRO_CONFIG_HRES8   0x0200
#define HYGRO_CONFIG_HRES11  0x0100

#define HYGRO_CONFIG_DEFAULT 0x1000

/* ------------------------------------------------------------ */
/*				Parameters Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodHYGRO{
	XIic HYGROIic;
	u8 chipAddr;
	u8 currentRegister;
	u8 recvbytes;
	u8* recv;
#ifndef NO_IRPT
	INTC intc;
#endif
}PmodHYGRO;

void HYGRO_begin(PmodHYGRO* InstancePtr, u32 IIC_Address, u8 Chip_Address);
void HYGRO_end(PmodHYGRO* InstancePtr);
int HYGRO_IICInit(XIic *IicInstancePtr);
void HYGRO_ReadIIC(PmodHYGRO* InstancePtr, u8 reg, u8 *Data, int nData, int conversion_delay_us);
void HYGRO_WriteIIC(PmodHYGRO* InstancePtr, u8 reg, u8 *Data, int nData);
int HYGRO_SetupInterruptSystem(PmodHYGRO* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* SendHandler,  void* ReceiveHandler);

float HYGRO_getTemperature(PmodHYGRO *InstancePtr);
float HYGRO_getHumidity(PmodHYGRO *InstancePtr);
float HYGRO_tempF2C(float tempF);
float HYGRO_tempC2F(float tempC);

#endif // PMODHYGRO_H
