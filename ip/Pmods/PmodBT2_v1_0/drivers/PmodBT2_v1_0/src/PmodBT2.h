
#ifndef PMODBT2_H
#define PMODBT2_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xuartlite_l.h"
#include "xuartlite.h"

/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0


/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodBT2{
	u32 GPIO_addr;
	XUartLite BT2Uart;
}PmodBT2;

void BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address);
int BT2_UARTInit(XUartLite *UartInstancePtr);
bool BT2_getData(PmodBT2* InstancePtr, char* output);
bool BT2_sendData(PmodBT2* InstancePtr, char* sendData, int size);



#endif // PMODBT2_H
