
#ifndef PmodNAV_H
#define PmodNAV_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"
w
'rhs
f'h
d';
/* ------------------------------------------------------------ */
/*					Definitions									*/
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0



typedef struct PmodNAV{
	u32 GPIO_addr;
	XSpi NAVSpi;
}PmodNAV;


	void NAV_begin(PmodNAV* InstancePtr, u32 GPIO_Address, u32 SPI_Address);
	void NAV_end(PmodNAV* InstancePtr);
	int NAV_SPIInit(XSpi *SPIInstancePtr);
	u8 NAV_ReadByte(PmodNAV* InstancePtr);
	void NAV_WriteByte(PmodNAV* InstancePtr, u8 cmd);
	void NAV_WriteSPI(PmodNAV* InstancePtr, u8 reg, u8 *wData, int nData);
	void NAV_ReadSPI(PmodNAV* InstancePtr, u8 reg, u8 *rData, int nData);
	void NAV_SetRegisterBits(PmodNAV* InstancePtr, u8 reg, u8 mask, bool fValue);
	u8 NAV_GetRegisterBits(PmodNAV* InstancePtr, u8 bRegisterAddress, u8 bMask);






#endif // PmodNAV_H
