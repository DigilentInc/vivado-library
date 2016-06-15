
#ifndef PMODGYRO_H
#define PMODGYRO_H


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
#define WHO_AM_I		0x0F
#define CTRL_REG1		0x20
#define CTRL_REG2		0x21
#define CTRL_REG3		0x22
#define CTRL_REG4		0x23
#define CTRL_REG5		0x24
#define REFERENCE		0x25
#define OUT_TEMP		0x26
#define STATUS_REG		0x27
#define OUT_X_L			0x28
#define OUT_X_H			0x29
#define OUT_Y_L			0x2A
#define OUT_Y_H			0x2B
#define OUT_Z_L			0x2C
#define OUT_Z_H			0x2D
#define FIFO_CTRL_REG	0x2E
#define FIFO_SRC_REG	0x2F
#define INT1_CFG		0x30
#define INT1_SRC		0x31
#define INT1_TSH_XH		0x32
#define INT1_TSH_XL		0x33
#define INT1_TSH_YH		0x34
#define INT1_TSH_YL		0x35
#define INT1_TSH_ZH		0x36
#define INT1_TSH_ZL		0x37
#define INT1_DURATION	0x38

/*	CTRL_REG1
*/
#define REG1_DR1		0x80
#define REG1_DR0		0x40
#define REG1_BW1		0x20
#define REG1_BW0		0x10
#define REG1_PD			0x08
#define REG1_ZEN		0x04
#define REG1_YEN		0x02
#define REG1_XEN		0x01

/*	CTRL_REG3
*/
#define REG3_I1_INT1	0x80
#define REG3_I1_BOOT	0x40
#define REG3_H_LACTIVE	0x20
#define REG3_PP_OD		0x10
#define REG3_I2_DRDY	0x08
#define REG3_I2_WTM		0x04
#define REG3_I2_ORUN	0x02
#define REG3_I2_EMPTY	0x01

/*	INT1_CFG
*/
#define INT1_ANDOR		0x80
#define INT1_LIR		0x40
#define INT1_ZHIE		0x20
#define INT1_ZLIE		0x10
#define INT1_YHIE		0x08
#define INT1_YLIE		0x04
#define INT1_XHIE		0x02
#define INT1_XLIE		0x01

/* ------------------------------------------------------------ */
/*				Bit masks Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*				Parameters Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

typedef struct PmodGYRO{
	XSpi GYROSpi;
	int Base_Addr;
	int GPIO_Base_Addr;
}PmodGYRO;

bool GYRO_Int1Status(PmodGYRO *InstancePtr);
bool GYRO_Int2Status(PmodGYRO *InstancePtr);

int16_t GYRO_getX(PmodGYRO *InstancePtr);
int16_t GYRO_getY(PmodGYRO *InstancePtr);
int16_t GYRO_getZ(PmodGYRO *InstancePtr);
int8_t GYRO_getTemp(PmodGYRO *InstancePtr);
uint8_t GYRO_getInt1Src(PmodGYRO *InstancePtr);
bool GYRO_enableInt1(PmodGYRO *InstancePtr, uint8_t mode);
bool GYRO_disableInt1(PmodGYRO *InstancePtr);
bool GYRO_enableInt2(PmodGYRO *InstancePtr, uint8_t mode);
bool GYRO_setThsXL(PmodGYRO *InstancePtr, uint8_t ths);
bool GYRO_setThsXH(PmodGYRO *InstancePtr, uint8_t ths);
bool GYRO_setThsYL(PmodGYRO *InstancePtr, uint8_t ths);
bool GYRO_setThsYH(PmodGYRO *InstancePtr, uint8_t ths);
bool GYRO_setThsZL(PmodGYRO *InstancePtr, uint8_t ths);
bool GYRO_setThsZH(PmodGYRO *InstancePtr, uint8_t ths);

bool GYRO_begin(PmodGYRO* InstancePtr, u32 SPI_Address, u32 GPIO_Address);
void GYRO_end(PmodGYRO* InstancePtr);
int GYRO_SPIInit(XSpi *SpiInstancePtr);
void GYRO_WriteByte(PmodGYRO* InstancePtr, u8 cmd);
int GYRO_WriteReg(PmodGYRO* InstancePtr, u8 reg, u8 *wData, int nData);
u8 GYRO_ReadByte(PmodGYRO* InstancePtr);
void GYRO_ReadReg(PmodGYRO* InstancePtr, u8 reg, u8 *rData, int nData);
void GYRO_SetRegisterBits(PmodGYRO* InstancePtr, u8 reg, u8 mask, bool fValue);
u8 GYRO_GetRegisterBits(PmodGYRO* InstancePtr, u8 bRegisterAddress, u8 bMask);


#endif // PMODGYRO_H
