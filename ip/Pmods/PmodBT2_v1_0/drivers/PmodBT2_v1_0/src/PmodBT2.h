/************************************************************************/
/*  File Description:                                       			*/
/*                                                    					*/
/*  This file contains a basic library for the PmodBT2.           		*/
/*                                                      				*/
/************************************************************************/
/*  Revision History:                                       			*/
/*                                                      				*/
/*  07/12/2016(TommyK, MikelS): Created                           		*/
/*                                                      				*/
/************************************************************************/

#ifndef PMODBT2_H
#define PMODBT2_H

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xuartns550_l.h"
#include "xuartns550.h"
#include "xparameters.h"

#ifdef XPAR_XINTC_NUM_INSTANCES
 #include "xintc.h"
 #define INTC   XIntc
 #define INTC_HANDLER XIntc_InterruptHandler
#elif XPAR_FABRIC_PMODBT2_0_BT2_UART_INTERRUPT_INTR
 #include "xscugic.h"
 #define INTC   XScuGic
 #define INTC_HANDLER XScuGic_InterruptHandler
#else
#define NO_IRPT 1
#endif

#ifdef XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ
#define PERIPHERAL_CLK 100000000 //FCLK0 frequency is not in Xparameters.h for some reason
#else
#define PERIPHERAL_CLK XPAR_CPU_M_AXI_DP_FREQ_HZ
#endif

#define bool u8
#define true 1
#define false 0


/**************************** Type Definitions *****************************/
/**
 * Write a value to a PMODBT2 register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PMODBT2device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 *  void PMODBT2_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 */
#define PMODBT2_mWriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 * Read a value from a PMODBT2 register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PMODBT2 device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 *  u32 PMODBT2_mReadReg(u32 BaseAddress, unsigned RegOffset)
 */
#define PMODBT2_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))


/************************** Function Prototypes ****************************/
XStatus PMODBT2_Reg_SelfTest(void * baseaddr_p);

typedef struct PmodBT2{
  u32 GPIO_addr;
#ifndef NO_IRPT
  INTC intc;
  void (* recvHandler)(int bytesRecv);
  void (* sendHandler)(int bytesSent);
  void (* timeoutHandler)(int bytesSent);
#endif
  XUartNs550 BT2Uart;
  char recv[600];
}PmodBT2;

void BT2_begin(PmodBT2* InstancePtr, u32 GPIO_Address, u32 UART_Address);
int BT2_getData(PmodBT2* InstancePtr, int buffersize);
int BT2_sendData(PmodBT2* InstancePtr, char* sendData, int size);
int BT2_changeBaud(PmodBT2* InstancePtr, int baud);
int BT2_UARTInit(XUartNs550 *UartInstancePtr);
int BT2_ReadRTS(PmodBT2* InstancePtr);
int BT2_ReadCTS(PmodBT2* InstancePtr);
void BT2_WriteRTS(PmodBT2* InstancePtr, bool enable);
void BT2_WriteCTS(PmodBT2* InstancePtr, bool enable);
bool BT2_getLine(PmodBT2* InstancePtr, char* output);
int BT2_SetupInterruptSystem(PmodBT2* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* recvHandler, void* sendHandler, void* timeoutHandler);
void BT2_intHandler(PmodBT2* InstancePtr, u32 Event, unsigned int EventData);


#endif // PMODBT2_H
