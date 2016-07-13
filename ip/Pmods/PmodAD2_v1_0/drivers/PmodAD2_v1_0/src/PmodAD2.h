
#ifndef PMODAD2_H
#define PMODAD2_H


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

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */

#define AD2_IIC_ADDR 0x28

#define AD2_CONFIG_CH3 				0x80
#define AD2_CONFIG_CH2 				0x40
#define AD2_CONFIG_CH1 				0x20
#define AD2_CONFIG_CH0 				0x10
#define AD2_CONFIG_REF_SEL 			0x08
#define AD2_CONFIG_FLTR				0x04
#define AD2_CONFIG_BIT_TRIAL_DELAY 	0x02
#define AD2_CONFIG_SAMPLE_DELAY 	0x01

#define AD2_DEFAULT_CONFIG (AD2_CONFIG_CH0 | AD2_CONFIG_CH1 | AD2_CONFIG_CH2 | AD2_CONFIG_CH3)

#define AD2_CONV_CHANNEL(val) (val >> 12)
#define AD2_CONV_VALUE(val) (val & 0xFFF)

/* ------------------------------------------------------------ */
/*				Parameters Definitions							*/
/* ------------------------------------------------------------ */

//RECALL, 1 microsecond conversion time!

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodAD2{
	XIic AD2Iic;
	u8 chipAddr;
	u8 currentRegister;
	u8 recvbytes;
	u8* recv;
#ifndef NO_IRPT
	INTC intc;
#endif
}PmodAD2;

void AD2_begin(PmodAD2* InstancePtr, u32 IIC_Address, u8 Chip_Address);
void AD2_end(PmodAD2* InstancePtr);
int AD2_IICInit(XIic *IicInstancePtr);
void AD2_ReadConv(PmodAD2* InstancePtr, u8 buf[], int nbytes);
void AD2_WriteConfig(PmodAD2* InstancePtr, u8 configuration);
int AD2_SetupInterruptSystem(PmodAD2* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* SendHandler,  void* ReceiveHandler);


#endif // PMODAD2_H
