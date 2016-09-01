#ifndef PmodACL2_H
#define PmodACL2_H


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

//Sensor's memory register addresses:
#define PART_ID   		0x02 //Device ID
#define X_DATA   		0x08
#define Y_DATA   		0x09
#define Z_DATA   		0x0A
#define STATUS   		0x0B //STATUS register
#define FIFO_ENTRIES_L  0x0C
#define FIFO_ENTRIES_H  0x0D
#define XDATA_L   		0x0E
#define XDATA_H   		0x0F
#define YDATA_L   		0x10
#define YDATA_H   		0x11
#define ZDATA_L   		0x12
#define ZDATA_H   		0x13
#define TEMP_L   		0x14
#define TEMP_H   		0x15
#define SOFT_RESET   	0x1F
#define THRESH_ACT_L	0x20
#define THRESH_INACT_L  0x23
#define THRESH_INACT_H	0x24
#define TIME_INACT_L	0x25
#define TIME_INACT_H	0x26
#define ACT_INACT_CTL   0x27
#define FIFO_CONTROL	0x28
#define FIFO_SAMPLES	0x29
#define INTMAP1    		0x2A
#define INTMAP2   		0x2B
#define FILTER_CTL		0x2C
#define POWER_CTL  		0x2D
#define SELF_TEST		0x2E


//command bytes

#define READ   		0x0B    		//command byte for reading from ACL2
#define WRITE   	0x0A   			//command byte for writing to ACL2
#define FIFO_READ   0x0D 			//command byte for reading from ACL2 FIFO buffer

/*	Initialization Parameters
*/
#define FREE_FALL_THRESH   	0x96   	//Sets free-fall detection threshold to 600mg
#define INACT_STILL_THRESH  0xFA 	//Sets still motion reference threshold to 1g
#define FREE_FALL_TIME		0x03    //Sets free-fall detection time to 30ms
#define INACT_STILL_TIME_L	0x10 	//lower half of 10 second still time
#define INACT_STILL_TIME_H	0x27 	//upper half of 10 second still time
#define REF_INACT_ENABLE	0x0C  	//Enable referenced inactivity detection
#define SET_INACT_INTERUPT	0x20 	//Sets the inactivity interrupt to the interrupt 1 pin
#define SENSOR_RANGE_8		0x83    //Sets sensor range to 8g with 100Hz ODR
#define SENSOR_RANGE_4		0x43    //Sets sensor range to 4g with 100Hz ODR
#define SENSOR_RANGE_2		0x03    //Sets sensor range to 2g with 100Hz ODR
#define BEGIN_MEASURE		0x02    //Begins measurement


/* ------------------------------------------------------------ */
/*					Object Class Declarations					*/
/* ------------------------------------------------------------ */
typedef struct myQueue{
	int dataQueue[170];
	int head_ptr;
	int tail_ptr;
}myQueue;

typedef struct PmodACL2{
	myQueue xFIFO;
	myQueue yFIFO;
	myQueue zFIFO;
	myQueue tempFIFO;

	XSpi ACL2Spi;
	u32 GPIO_addr;

	u8 range;
	int xZero;
	int yZero;
	int zZero;
}PmodACL2;


/* ------------------------------------------------------------ */
/*						Function Prototypes						*/
/* ------------------------------------------------------------ */
void ACL2_myQueueEmpty(myQueue *QueuePtr);
void ACL2_myQueueEmptyAll(PmodACL2 *InstancePtr);
int ACL2_myQueueSize(myQueue *QueuePtr);
int ACL2_myQueueFront(myQueue *QueuePtr);
int ACL2_myQueueBack(myQueue *QueuePtr);
bool ACL2_myQueuePushBack(myQueue *QueuePtr, int value);
int ACL2_myQueuePopFront(myQueue *QueuePtr);
void ACL2_myQueueRst(myQueue *QueuePtr);
void ACL2_getQueue(myQueue *Queue, int* outqueue);

void ACL2_printQueue(PmodACL2 *InstancePtr);
void ACL2_printQueueHelp(myQueue Queue);

int ACL2_getX(PmodACL2 *InstancePtr);
int ACL2_getY(PmodACL2 *InstancePtr);
int ACL2_getZ(PmodACL2 *InstancePtr);

void ACL2_reset(PmodACL2 *InstancePtr);
void ACL2_updateRange(PmodACL2 *InstancePtr);

void ACL2_setRange(PmodACL2 *InstancePtr, int newRange);
void ACL2_setZero(PmodACL2 *InstancePtr);

int ACL2_getFIFOentries(PmodACL2 *InstancePtr);
void ACL2_initFIFO(PmodACL2 *InstancePtr);
void ACL2_fillFIFO(PmodACL2 *InstancePtr);

uint8_t ACL2_getStatus(PmodACL2 *InstancePtr);
int ACL2_getData(PmodACL2 *InstancePtr, uint8_t reg1, uint8_t reg2);

int16_t ACL2_twosToBin(uint16_t input);
char ACL2_getDIR(uint16_t value);

void ACL2_begin(PmodACL2* InstancePtr, u32 GPIO_Address, u32 SPI_Address);
void ACL2_end(PmodACL2* InstancePtr);
int ACL2_SPIInit(XSpi *SPIInstancePtr);
u8 ACL2_ReadByte(PmodACL2* InstancePtr);
void ACL2_WriteByte(PmodACL2* InstancePtr, u8 cmd);
void ACL2_WriteSpi(PmodACL2* InstancePtr, u8 reg, u8 *wData, int nData);

u8 ACL2_ReadReg(PmodACL2* InstancePtr, u8 reg);
void ACL2_ReadSPI(PmodACL2* InstancePtr, u8 reg, u8 *rData, int nData);
void ACL2_SetRegisterBits(PmodACL2* InstancePtr, u8 reg, u8 mask, bool fValue);
u8 ACL2_GetRegisterBits(PmodACL2* InstancePtr, u8 bRegisterAddress, u8 bMask);


#endif // PmodACL2_H
