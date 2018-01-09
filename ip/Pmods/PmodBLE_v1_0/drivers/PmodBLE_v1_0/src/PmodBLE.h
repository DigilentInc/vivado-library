/************************************************************************/
/*  File Description:                                       			*/
/*                                                    					*/
/*  This file contains a basic library for the PmodBLE.           		*/
/*                                                      				*/
/************************************************************************/
/*  Revision History:                                       			*/
/*                                                      				*/
/*  07/12/2016(TommyK, MikelS): Created                           		*/
/*                                                      				*/
/************************************************************************/

#ifndef PMODBLE_H
#define PMODBLE_H

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xuartns550_l.h"
#include "xuartns550.h"
#include "xparameters.h"

#ifdef XPAR_INTC_0_PMODBLE_0_VEC_ID
 #include "xintc.h"
 #define INTC   XIntc
 #define INTC_HANDLER XIntc_InterruptHandler
#elif XPAR_FABRIC_PMODBLE_0_BLE_UART_INTERRUPT_INTR
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
 * Write a value to a PMODBLE register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PMODBLEdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 *  void PMODBLE_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 */
#define PMODBLE_mWriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 * Read a value from a PMODBLE register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PMODBLE device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 *  u32 PMODBLE_mReadReg(u32 BaseAddress, unsigned RegOffset)
 */
#define PMODBLE_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))


/************************** Function Prototypes ****************************/
XStatus PMODBLE_Reg_SelfTest(void * baseaddr_p);

typedef struct PmodBLE{
  u32 GPIO_addr;
#ifndef NO_IRPT
  INTC intc;
  void (* recvHandler)(int bytesRecv);
  void (* sendHandler)(int bytesSent);
  void (* timeoutHandler)(int bytesSent);
#endif
  XUartNs550 BLEUart;
  char recv[600];
}PmodBLE;

void BLE_begin(PmodBLE* InstancePtr, u32 GPIO_Address, u32 UART_Address);
int BLE_getData(PmodBLE* InstancePtr, int buffersize);
int BLE_sendData(PmodBLE* InstancePtr, char* sendData, int size);
int BLE_changeBaud(PmodBLE* InstancePtr, int baud);
int BLE_UARTInit(XUartNs550 *UartInstancePtr);
int BLE_ReadRTS(PmodBLE* InstancePtr);
int BLE_ReadCTS(PmodBLE* InstancePtr);
void BLE_WriteRTS(PmodBLE* InstancePtr, bool enable);
void BLE_WriteCTS(PmodBLE* InstancePtr, bool enable);
bool BLE_getLine(PmodBLE* InstancePtr, char* output);
int BLE_SetupInterruptSystem(PmodBLE* InstancePtr, u32 interruptDeviceID, u32 interruptID, void* recvHandler, void* sendHandler, void* timeoutHandler);
void BLE_intHandler(PmodBLE* InstancePtr, u32 Event, unsigned int EventData);


#endif // PMODBLE_H
