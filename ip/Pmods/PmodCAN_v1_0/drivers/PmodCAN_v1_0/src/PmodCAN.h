/*************************************************************************/
/*                                                                       */
/*     PmodCAN.h --     PmodCAN Example Projects                         */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Browm                                              */
/*                                                                       */
/*************************************************************************/
/*  File Description:                                                    */
/*                                                                       */
/*  This demo sends data through GPIO/SPI to the PmodCAN and receives    */
/*  it through the PmodCAN. Loop back function                            */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            8/30/2017(ArtVVB): Created                                 */
/* 			  9/1/2017(jPeyron): formatted Validated                     */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/
/*  Baud Rates:                                                          */
/*                                                                       */
/*  Microblaze: 9600 or what was specified in UARTlite core              */
/*  Zynq: 115200                                                         */
/*                                                                       */
/*************************************************************************/

#ifndef PmodCAN_H
#define PmodCAN_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "xspi_l.h"
#include "xspi.h"

/* ------------------------------------------------------------ */
/*                  Definitions                                 */
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0
/** DRIVER HEADER MATERIALS **/

#define MODIFY_REG_CMD      0x05

#define WRITE_REG_CMD       0x02

#define READ_REG_CMD        0x03

#define RTS_CMD             0x80
#define RTS_TXB0_MASK       0x01
#define RTS_TXB1_MASK       0x02
#define RTS_TXB2_MASK       0x04

#define LOADBUF_CMD         0x40
#define LOADBUF_TXB0SIDH    0x00
#define LOADBUF_TXB0D0      0x01
#define LOADBUF_TXB1SIDH    0x02
#define LOADBUF_TXB1D0      0x03
#define LOADBUF_TXB2SIDH    0x04
#define LOADBUF_TXB2D0      0x05

#define READBUF_CMD         0x90
#define READBUF_RXB0SIDH    0x00
#define READBUF_RXB0D0      0x01
#define READBUF_RXB1SIDH    0x02
#define READBUF_RXB1D0      0x03

#define READSTATUS_CMD      0xA0
#define STATUS_RX0IF_MASK   0x01
#define STATUS_RX1IF_MASK   0x02
#define STATUS_TX0REQ_MASK  0x04
#define STATUS_TX0IF_MASK   0x08
#define STATUS_TX1REQ_MASK  0x10
#define STATUS_TX1IF_MASK   0x20
#define STATUS_TX2REQ_MASK  0x40
#define STATUS_TX2IF_MASK   0x80

#define RXSTATUS_CMD        0xB0
#define RXSTATUS_RX0IF_MASK 0x40
#define RXSTATUS_RX1IF_MASK 0x80

#define CANCTRL_REG_ADDR    0x0F
#define CNF3_REG_ADDR       0x28
#define CNF2_REG_ADDR       0x29
#define CNF1_REG_ADDR       0x2A
#define RXB0CTRL_REG_ADDR   0x60

#define TXB0CTRL_REG_ADDR   0X30
#define TXB0CTRL_TXREQ_MASK 0X08

#define CANINTF_REG_ADDR    0x2C
#define CANINTF_RX0IF_MASK  0x01
#define CANINTF_RX1IF_MASK  0x02
#define CANINTF_TX0IF_MASK  0x04
#define CANINTF_TX1IF_MASK  0x08
#define CANINTF_TX2IF_MASK  0x10
#define CAN_CANCTRL_MODE_MASK   0xE0
#define CAN_CANCTRL_MODE_BIT    5


typedef struct CAN_Message {
    u16 id;     //11 bit id
    u32 eid;    //18 bit extended id
    u8 ide;     //1 to enable sending extended id
    u8 rtr;     //remote transmission request bit
    u8 srr;     //Standard Frame Remote Transmit Request
    u8 dlc;     //data length
    u8 data[8]; //data buffer
    //some additional information has not yet been encapsulated here (ex:priority bits), primarily, no TXBxCTRL bits
} CAN_Message;

typedef enum CAN_RxBuffer {
    CAN_Rx0 = 0,
    CAN_Rx1
} CAN_RxBuffer;

typedef enum CAN_TxBuffer {
    CAN_Tx0 = 0,
    CAN_Tx1,
    CAN_Tx2
} CAN_TxBuffer;
typedef enum CAN_Mode {
    CAN_ModeNormalOperation = 0,
    CAN_ModeSleep,
    CAN_ModeLoopback,
    CAN_ModeListenOnly,
    CAN_ModeConfiguration
} CAN_Mode;


typedef struct PmodCAN {
    u32 GPIO_addr;
    XSpi CANSpi;
} PmodCAN;

typedef struct PmodCAN_Config {

} PmodCAN_Config;

    void CAN_begin(PmodCAN* InstancePtr, u32 GPIO_Address, u32 SPI_Address);
    void CAN_end(PmodCAN* InstancePtr);
    int CAN_SPIInit(XSpi *SPIInstancePtr);
    u8 CAN_ReadByte(PmodCAN* InstancePtr);
    void CAN_WriteByte(PmodCAN* InstancePtr, u8 cmd);
    void CAN_WriteSPI(PmodCAN* InstancePtr, u8 reg, u8 *wData, int nData);
    void CAN_ReadSPI(PmodCAN* InstancePtr, u8 reg, u8 *rData, int nData);
    void CAN_SetRegisterBits(PmodCAN* InstancePtr, u8 reg, u8 mask, bool fValue);
    u8 CAN_GetRegisterBits(PmodCAN* InstancePtr, u8 bRegisterAddress, u8 bMask);
    void CAN_ModifyReg(PmodCAN *InstancePtr, u8 reg, u8 mask, u8 value);
    void CAN_WriteReg(PmodCAN *InstancePtr, u8 reg, u8 *data, u32 nData);
    void CAN_ClearReg(PmodCAN *InstancePtr, u8 reg, u32 nData);
    void CAN_LoadTxBuffer(PmodCAN *InstancePtr, u8 start_addr, u8 *data, u32 nData);
    void CAN_RequestToSend(PmodCAN *InstancePtr, u8 mask);
    void CAN_ReadRxBuffer(PmodCAN *InstancePtr, u8 start_addr, u8 *data, u32 nData);
    void CAN_ReadReg(PmodCAN *InstancePtr, u8 reg, u8 *data, u32 nData);
    u8 CAN_ReadStatus(PmodCAN *InstancePtr);
    u8 CAN_RxStatus(PmodCAN *InstancePtr);
    void CAN_Configure(PmodCAN *InstancePtr, u8 mode); // this function is missing some potential parameters
    XStatus CAN_SendMessage(PmodCAN *InstancePtr, CAN_Message message, CAN_TxBuffer target);
    XStatus CAN_ReceiveMessage(PmodCAN *InstancePtr, CAN_Message *MessagePtr, CAN_RxBuffer target);



#endif // PmodCAN_H
