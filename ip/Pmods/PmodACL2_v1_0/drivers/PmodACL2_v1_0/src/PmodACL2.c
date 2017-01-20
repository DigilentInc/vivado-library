/************************************************************************/
/*																		*/
/* PmodACL2.c	--		Template driver for a Pmod which uses SPI		*/
/*																		*/
/************************************************************************/
/*	Author:		Mikel Skreen											*/
/*	Copyright 2016, Digilent Inc.										*/
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  File Description:													*/
/*																		*/
/*	This file contains a basic library in order to interface with the 	*/
/*	PmodACL2.															*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/19/2016(MikelS): Created											*/
/*																		*/
/************************************************************************/

/***************************** Include Files *******************************/
#include "PmodACL2.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/************************** Function Definitions ***************************/
XSpi_Config ACL2Config =
{
	0,
	0,
	1,
	0,
	1,
	8,
	0,
	0,
	0,
	0,
	0
};
/* ------------------------------------------------------------ */
/***	void ACL2_begin(PmodACL2* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**	Parameters:
**		InstancePtr: A PmodACL2 object to start
**		GPIO_Address: The Base address of the PmodACL2 GPIO
**		SPI_Address: The Base address of the PmodACL2 SPI
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initialize the PmodACL2.
*/
void ACL2_begin(PmodACL2* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
{
	u8 value;
	InstancePtr->GPIO_addr=GPIO_Address;
	ACL2Config.BaseAddress=SPI_Address;
	ACL2_SPIInit(&InstancePtr->ACL2Spi);

	//if we know the ACL2 will be at rest during start up run setZero() instead.
	InstancePtr->xZero = -120;
	InstancePtr->yZero = -175;
	InstancePtr->zZero = -330;
//	ACL2_setZero(InstancePtr);

	ACL2_reset(InstancePtr);
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(50);
#else
	usleep(500);
#endif

	value = 0x1;
	ACL2_WriteSpi(InstancePtr, ACT_INACT_CTL, &value, 1);	//Enables the activity functionality
	print("ACT_INACT_CTL set\n\r");
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(15);
#else
	usleep(500);
#endif
//
	value = 0xA;
	ACL2_WriteSpi(InstancePtr, FIFO_CONTROL, &value, 1);	//Allows for 512 samples and sets the FIFO in stream mode.
	print("FIFO_CONTROL set\n\r");							//Tells FIFO not to store temperature samples.
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(15);
#else
	usleep(500);
#endif

	value = 0xFF;
	ACL2_WriteSpi(InstancePtr, FIFO_SAMPLES, &value, 1);	//Set for 255 samples
	print("FIFO_SAMPLES set\n\r");
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(15);
#else
	usleep(500);
#endif

	value = SENSOR_RANGE_8;
	ACL2_WriteSpi(InstancePtr, FILTER_CTL, &value, 1);		//Sets sensor range to 8g with 100Hz ODR
	print("FILTER_CTL set\n\r");
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(15);
#else
	usleep(500);
#endif

	value = BEGIN_MEASURE;
	ACL2_WriteSpi(InstancePtr, POWER_CTL, &value, 1);		//Begins measurement
	print("POWER_CTL set\n\r");
#ifdef XPAR_MICROBLAZE_ID
	MB_Sleep(15);
#else
	usleep(500);
#endif

	ACL2_updateRange(InstancePtr);							//Sets class range value

	Xil_Out32(InstancePtr->GPIO_addr+4, 0b0011);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
}

/* ------------------------------------------------------------ */
/***	ACL2_end(void)
**
**	Parameters:
**		InstancePtr:	PmodACL2 object to stop
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Stops the device
*/
void ACL2_end(PmodACL2* InstancePtr){
	XSpi_Stop(&InstancePtr->ACL2Spi);
}

/* ------------------------------------------------------------ */
/***	ACL2_SPIInit(XSpi *SpiInstancePtr)
**
**	Parameters:
**		InstancePtr:	XSpi object to initialize
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Initializes the PmodACL2 SPI.
*/
int ACL2_SPIInit(XSpi *SpiInstancePtr){
	int Status;

	Status = XSpi_CfgInitialize(SpiInstancePtr, &ACL2Config, ACL2Config.BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION | XSP_CLK_ACTIVE_LOW_OPTION
							| XSP_CLK_PHASE_1_OPTION) | XSP_MANUAL_SSELECT_OPTION); //Change these based on your SPI device
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(SpiInstancePtr, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the SPI driver so that the device is enabled.
	 */
	XSpi_Start(SpiInstancePtr);

	/*
	 * Disable Global interrupt to use polled mode operation
	 */
	XSpi_IntrGlobalDisable(SpiInstancePtr);

	return XST_SUCCESS;

}

/* ------------------------------------------------------------ */
/***	ACL2_Readbyte(PmodACL2* InstancePtr)
**
**	Synopsis:
**		byte = ACL2_Readbyte(&ACL2)
**
**	Parameters:
**		InstancePtr:	the PmodACL2 object to communicate with
**
**	Return Value:
**		u8 byte:		Byte read from XSpi
**
**	Errors:
**		none
**
**	Description:
**		Reads a byte from SPI
*/
u8 ACL2_ReadByte(PmodACL2* InstancePtr){
	u8 byte;
	XSpi_Transfer(&InstancePtr->ACL2Spi, &byte, &byte, 1);
	return byte;
}

/* ------------------------------------------------------------ */
/***	ACL2_WriteByte(PmodACL2* InstancePtr, u8 cmd)
**
**	Parameters:
**		InstancePtr:	PmodACL2 object to send to
**		cmd:			Command to send
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Writes a single byte over SPI
*/
void ACL2_WriteByte(PmodACL2* InstancePtr, u8 cmd)
{
	XSpi_Transfer(&InstancePtr->ACL2Spi, &cmd, NULL, 1);
}

/* ------------------------------------------------------------ */
/***	ACL2_WriteSpi(PmodACL2* InstancePtr, u8 reg, u8 *wData, int nData)
**
**	Synopsis:
**		ACL2_WriteSpi(&ACL2obj, 0x3A, &bytearray, 5);
**
**	Parameters:
**		InstancePtr:	the PmodACL2 object to communicate with
**		reg:			the starting register to write to
**		wData:			the data to write
**		nData:			the number of data bytes to write
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Writes the byte array to the chip via SPI. It will write the first
**		byte into the specified register, then the next into the following
**		register until all of the data has been sent.
*/
void ACL2_WriteSpi(PmodACL2* InstancePtr, u8 reg, u8 *wData, int nData)
{
	u8 bytearray[nData+2];
	bytearray[0] = 0xA; // write command
	bytearray[1] = reg;
	bytearray[2] = *wData;
	XSpi_Transfer(&InstancePtr->ACL2Spi, bytearray, bytearray, nData+2);
}

/* ------------------------------------------------------------ */
/***	ACL2_ReadSpi(PmodACL2* InstancePtr, u8 reg, u8 *rData, int nData)
**
**	Synopsis:
**		ACL2_ReadSpi(&ACL2obj, 0x3A, &bytearray, 5);
**
**	Parameters:
**		InstancePtr:	the PmodACL2 object to communicate with
**		reg:			the starting register to read from
**		Data:			the byte array to read into
**		nData:			the number of data bytes to read
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Reads data in through SPI. It will read the first byte from the
**		starting register, then the next from the following register.
**		Data is stored into rData.
*/
void ACL2_ReadSpi(PmodACL2* InstancePtr, u8 reg, u8 *rData, int nData)
{
	u8 bytearray[nData+2];
	bytearray[0] = 0x0B; // read command
	bytearray[1] = reg;
	bytearray[2] = 0;
	XSpi_Transfer(&InstancePtr->ACL2Spi, bytearray, bytearray, nData+2);
	memcpy(rData,&bytearray[2], nData);

}

/* ------------------------------------------------------------ */
/***	getX(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object to take measurement from
**
**  Return Value:
**		result:		an integer value of the read X data fromACL2_getData
**
**  Errors:
**		none
**
**  Description:
**   	calls ACL2_getData with the XDATA_H and XDATA_L registers and returns the value
*/
int ACL2_getX(PmodACL2 *InstancePtr){
	int result = 0;
	//get data from xdata register
	result = ACL2_getData(InstancePtr, XDATA_H, XDATA_L);
	//process to achieve desired reading
	result = result * 1000;
	result = result / (2000 / InstancePtr->range);
	result = result + InstancePtr->xZero;

	return result;
}

/* ------------------------------------------------------------ */
/***	getY(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object to take measurement from
**
**  Return Value:
**		result:			an integer value of the read Y data fromACL2_getData
**
**  Errors:
**		none
**
**  Description:
**   	callACL2_getData with the YDATA_H and YDATA_L registers and returns that value
*/
int ACL2_getY(PmodACL2 *InstancePtr){

	int result = 0;

	//get data from ydata register
	result =ACL2_getData(InstancePtr, YDATA_H, YDATA_L);
	//process to achieve desired reading
	result = result * 1000;
	result = result /(2000 / InstancePtr->range);
	result = result + InstancePtr->yZero;

	return result;
}

/* ------------------------------------------------------------ */
/***	getZ(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object to take measurement from
**
**  Return Value:
**		result:			an integer value of the read Z data fromACL2_getData
**
**  Errors:
**		none
**
**  Description:
**   	calls ACL2_getData with the ZDATA_H and ZDATA_L registers and returns that value
*/
int ACL2_getZ(PmodACL2 *InstancePtr){

	int result = 0;

	//get data from zdata register
	result =ACL2_getData(InstancePtr, ZDATA_H, ZDATA_L);
	//process to achieve desired reading
	result = result * 1000;
	result = result / (2000 / InstancePtr->range);
	result = result + InstancePtr->zZero;

	return result;

}

/* ------------------------------------------------------------ */
/***	ACL2_getTemp(PmodACL2 *InstancePtr)
**
**  Parameters:
**		none
**
**  Return Value:
**		result - an integer value of the read temp data fromACL2_getData
**
**  Errors:
**		Is not accurate. Needs calibration.
**
**  Description:
**   	calls ACL2_getData with the TEMP_H and TEMP_L registers and returns that value
*/
int16_t ACL2_getTemp(PmodACL2 *InstancePtr){
	int16_t temp = ACL2_getData(InstancePtr, TEMP_H, TEMP_L);
//	temp = 44 - temp; // for better accuracy
//	temp = 32 + (1.8 * temp); // converts to farenheit

	return temp;
}

/* ------------------------------------------------------------ */
/***	ACL2_getStatus(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object to take measurement from
**
**  Return Value:
**   	result:			8 bit binary from status register
**
**  Errors:
**		none
**
**  Description:
**   	read from the STATUS register and return its value
*/
u8 ACL2_getStatus(PmodACL2 *InstancePtr){
	u8 regRead = 0;
	//get the data from the STATUS register
	ACL2_ReadSpi(InstancePtr, STATUS, &regRead, 1);
	return regRead;
}

/* ------------------------------------------------------------ */
/***	ACL2_getData(PmodACL2 *InstancePtr, uint8_t reg1, uint8_t reg2)
**
**  Parameters:
**  	InstancePtr:	the PmodACL2 object
**    	reg1:	first register to read from. the high data value which contains the 3 MSBs
**		reg2:	second register to read from. the low data value which contains the 8 LSBs
**
**  Return Value:
**		result:	an integer value of the combined data in the two registers
**
**  Errors:
**		none
**
**  Description:
**   	get Data is used to get the 11 bit x,y,z and temp values that come from 2 registers
**		from the accelerometer. The function outputs a signed integer value
*/
int ACL2_getData(PmodACL2 *InstancePtr, uint8_t reg1, uint8_t reg2){

	u8 buffer[2] = {0,0};
	u16 bufferComb = 0;
	int result = 0;
	int sign = 0;

	//store XData high into result
	ACL2_ReadSpi(InstancePtr, reg1, &buffer[1], 1);
	//store XData low into 8 LSB
	ACL2_ReadSpi(InstancePtr, reg2, buffer, 1);

	bufferComb |= buffer[1];
	bufferComb <<= 8;
	bufferComb |= buffer[0];

	if(bufferComb >= 0x8000){// If negative
		sign = 1;
		bufferComb = ACL2_twosToBin(bufferComb);
	}

	//Get rid of first 4 bits
	result = bufferComb & 0x07ff;

	if(sign == 1){
		result = 0 - result;
	}

	return result;
}

/* ------------------------------------------------------------ */
/***	ACL2_reset(PmodACL2 *InstancePtr)
**
**	Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	writes the byte 'R' to the reset register to initiate a soft reset.
*/
void ACL2_reset(PmodACL2 *InstancePtr){
	u8 write = 'R';
	//write 'R' to soft reset register
	ACL2_WriteSpi(InstancePtr, SOFT_RESET, &write, 1);
}

/* ------------------------------------------------------------ */
/***	ACL2_updateRange(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	Reads the filter control register and stores the sensitivity range into
**		the range variable inside of the PmodACL2 object
*/
void ACL2_updateRange(PmodACL2 *InstancePtr){

	uint8_t value;
	ACL2_ReadSpi(InstancePtr, FILTER_CTL, &value, 1);

	//only looking at first two bits. 192 = 0b11000000 = 0xC0
	value = value & 0xC0;

	//set range based on register value
	switch(value){
		case 0xC0:
			InstancePtr->range = 8;
			break;

		case 0x80:
			InstancePtr->range = 8;
			break;

		case 0x40:
			InstancePtr->range = 4;
			break;

		case 0:
			InstancePtr->range = 2;
			break;
	}
}

/* ------------------------------------------------------------ */
/***	ACL2_setRange(PmodACL2 *InstancePtr, int newRange)
**
**  Parameters:
**  	InstancePtr:	the PmodACL2 object
**		newRange:		The new sensitivity range for the ACL to use (2,4,8)
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	Takes in the user choice newRange and if it is valid, writes to the filter
**		control to change the sensitivity range while keeping other filter preferences
*/
void ACL2_setRange(PmodACL2 *InstancePtr, int newRange){

	uint8_t temp = 0;

	//get range control data
	ACL2_ReadSpi(InstancePtr, FILTER_CTL, &temp, 1);

	//modify the temp data to change to desired range
	switch(newRange){
		case 2:
			temp = temp & 0x3F;
			break;
		case 4:
			temp = temp & 0x3F;
			temp = temp | 0x40;
			break;
		case 8:
			temp = temp | 0x80;
			break;
		default:
			break;				//if not a valid range, will just set the FILTER_CTL register to what it was
	}

	//write modified temp back to FILTER_CTL
	ACL2_WriteSpi(InstancePtr, FILTER_CTL, &temp, 1);

	//set class range value
	ACL2_updateRange(InstancePtr);

}

/* ------------------------------------------------------------ */
/***	ACL2_setZero(PmodACL2 *InstancePtr)
**
**	Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	Sets the zeroing variables so that the ACL puts out x= 0, y = 0, z = 1000
**		has a 100 point sample
*/
void ACL2_setZero(PmodACL2 *InstancePtr){
	//math sets the current acceleration to read zero

	int xAvg = 0;
	int yAvg = 0;
	int zAvg = 0;
	int i;

	for(i = 0; i < 100; i ++){
			xAvg = xAvg + ACL2_getX(InstancePtr);
			yAvg = yAvg + ACL2_getY(InstancePtr);
			zAvg = zAvg + ACL2_getZ(InstancePtr);
	}
	xAvg = xAvg / 100;
	yAvg = yAvg / 100;
	zAvg = zAvg / 100;

	InstancePtr->xZero = 0 - (xAvg - InstancePtr->xZero);
	InstancePtr->yZero = 0 - (yAvg - InstancePtr->yZero);
	InstancePtr->zZero = 0 - (zAvg - InstancePtr->zZero);
}



/* ------------------------------------------------------------ */
/***	ACL2_getFIFOentries(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		int:			the number of FIFO entries stored in the buffer
**
**  Errors:
**		none
**
**  Description:
**   	This function reads the FIFO_ENTRIES registers and returns the value inside
*/
int ACL2_getFIFOentries(PmodACL2 *InstancePtr){

	return ACL2_getData(InstancePtr, FIFO_ENTRIES_H, FIFO_ENTRIES_L);
}

/* ------------------------------------------------------------ */
/***	ACL2_initFIFO(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	This function manipulates the FIFO and control registers to start the FIFO buffer in a streaming mode
*/
//void ACL2_initFIFO(PmodACL2 *InstancePtr){
//	u8 writeReg = 1;
//	//set interupt1 pin to data ready
//	ACL2_WriteSpi(InstancePtr, INTMAP1, &writeReg, 1);
//
//	//turn on FIFO
//	writeReg = 10;
//	ACL2_WriteSpi(InstancePtr, FIFO_CONTROL, &writeReg, 1);
//	writeReg = 255;
//	ACL2_WriteSpi(InstancePtr, FIFO_SAMPLES, &writeReg, 1);		//set to 512 values for each
//
//}

/* ------------------------------------------------------------ */
/***	ACL2_fillFIFO(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	the PmodACL2 object
**
**  Return Value:
**		none
**
**  Errors:
**		none
**
**  Description:
**   	This function initiates the FIFO read then reads and processes the data
**		coming out of the FIFO buffer and places them into their respective myQueue
*/
void ACL2_fillFIFO(PmodACL2 *InstancePtr){
	u8 buffer[2] = {0,0};
	u8 LSB[2] = {0,0};
	u16 bufferComb = 0;
	int result = 0;
	int sign = 0;
	char dir = '\0';
	int samples = 0;
	int i = 0;
	u8 read = FIFO_READ;
	uint8_t nothing = 0; //unsigned value to send over SPI

	//get the number of samples
	samples = ACL2_getFIFOentries(InstancePtr);

	if(samples > 0){

		//lower chip select and send FIFO_READ byte.
		//chipSelect needs to stay low throughout the transfer
		XSpi_SetSlaveSelectReg(&InstancePtr->ACL2Spi, InstancePtr->ACL2Spi.SlaveSelectReg);
		XSpi_CustomTransfer(&InstancePtr->ACL2Spi, &read, &nothing, 1);

		nothing = 0;
		while(i < (samples)){
			bufferComb = 0;
			//read the 8 LSBs
			XSpi_CustomTransfer(&InstancePtr->ACL2Spi, &nothing, &buffer[0], 1);
			//read the 8 MSBs
			XSpi_CustomTransfer(&InstancePtr->ACL2Spi, &nothing, &buffer[1], 1);
			//shift MSBs to correct position then OR with LSB
			bufferComb |= buffer[1];
			bufferComb = (bufferComb << 8) | buffer[0];
			//receive axis of data
			dir = ACL2_getDIR(bufferComb);

			//get rid of first two directional bits
			bufferComb = bufferComb & 0b0011111111111111;

			//check to see if value is negative and if so run through twosToBin
			if(bufferComb > 8192){
				sign = 1;
				bufferComb = ACL2_twosToBin(bufferComb);
			}

			//copy and mask buffer data into a signed int for storing
			result = bufferComb & 0x07ff;

			//account for negative
			if(sign == 1){
				result = 0 - result;
			}
			//scales data
			result *= (1000 / (2000 / InstancePtr->range));
			//processes the data and puts it into the correct myQueue according to DIR
			if(dir == 'x'){
				result = result + InstancePtr->xZero;
				ACL2_myQueuePushBack(&InstancePtr->xFIFO, result);
			}
			else if(dir == 'y'){
				result = result + InstancePtr->yZero;
				ACL2_myQueuePushBack(&InstancePtr->yFIFO, result);
			}
			else if(dir == 'z'){
				result = result + InstancePtr->zZero;
				ACL2_myQueuePushBack(&InstancePtr->zFIFO, result);
			}
			else{
			}

			//increment counter
			i = i + 1;

			sign = 0;
			dir = '\0';
		}
		//set chip select high again once FIFO transfer is over
		XSpi_SetSlaveSelectReg(&InstancePtr->ACL2Spi, InstancePtr->ACL2Spi.SlaveSelectMask);
	}

	return;
}

/* ------------------------------------------------------------ */
/***	ACL2_twosToBin(uint16_t input)
**
**  Parameters:
**    	input:	an 11 bit twos complement value to be converted to a binary number
**
**  Return Value:
**		returns a 16 bit unsigned integer with the positive value of the negative twos compliment
**
**  Errors:
**    	none
**
**  Description:
**   	This function converts a negative twos compliment value and preforms a bitwise flip and subtracts
**		one to return the positive int value.
*/
int16_t ACL2_twosToBin(uint16_t input){

	//flip all 11 bits
	input = input ^ 0x07ff;

	//subtract 1 to get binary
	input = input - 1;

	return input;
}

/* ------------------------------------------------------------ */
/***	ACL2_getDIR(uint16_t value)
**
**  Parameters:
**    	value:		FIFO raw data to parse direction from
**
**  Return Value:
**    	result:		axis that the FIFO data represents
**
**  Errors:
**    	does not handle temperature FIFO data
**
**  Description:
**   	This function takes the raw FIFO data and analyzes the 2 MSBs to determine
**		the axis the data represents
*/
char ACL2_getDIR(uint16_t value){
	char result = '\0';

	//only care about first 2 bits
	value = value >> 14;

	if(value  == 0)
		result = 'x';
	if(value == 1)
		result = 'y';
	if(value ==  2)
		result = 'z';
	if(value == 3)
		result = 't';

	return result;

}

/* ------------------------------------------------------------ */
/***	ACL2_myQueueEmpty(myQueue *QueuePtr)
**
**  Parameters:
**		QueuePtr:	FIFO object to be cleared
**
**  Return Value:
**    	none
**
**  Errors:
**   	none
**
**  Description:
**    sets all members of the queue to -9999 and resets the pointers to the beginning
*/
void ACL2_myQueueEmpty(myQueue *QueuePtr){
	int i;
	//traverse through queue and set to -1
	for(i = 0; i < 170; i ++){
		QueuePtr->dataQueue[i] = -9999;
	}

	//set ptrs to the beginning
	QueuePtr->tail_ptr = 0;
	QueuePtr->head_ptr = 0;
	return;
}

/* ------------------------------------------------------------ */
/***	ACL2_myQueueSize(myQueue *QueuePtr)
**
**  Parameters:
**		QueuePtr:	FIFO object
**
**  Return Value:
**		number of items in the queue
**
**  Errors:
**		none
**
**  Description:
**		returns the size of the queue which is dependent on where the
**		tail and head are indexed at
*/
int ACL2_myQueueSize(myQueue *QueuePtr){
	if(QueuePtr->tail_ptr > QueuePtr->head_ptr)
		return (QueuePtr->tail_ptr - QueuePtr->head_ptr);
	else if(QueuePtr->tail_ptr < QueuePtr->head_ptr)
		return (170 - (QueuePtr->head_ptr - QueuePtr->tail_ptr));
	return 0;
}

/* ------------------------------------------------------------ */
/***	ACL2_myQueuePushBack(myQueue *QueuePtr, int value)
**
**  Parameters:
**  	QueuePtr:	FIFO object
**		value:	item to be pushed onto the back of the queue
**
**  Return Value:
**    	bool:	whether the push was successful or not
**
**  Errors:
**		none
**
**  Description:
**		accepts a value then pushes it onto the back of the queue.
**		If the Queue is full then "false" will be returned.
*/
bool ACL2_myQueuePushBack(myQueue *QueuePtr, int value){
	if((((QueuePtr->tail_ptr - QueuePtr->head_ptr) < 170) && (QueuePtr->tail_ptr - QueuePtr->head_ptr) > 0) ||
		(QueuePtr->tail_ptr == QueuePtr->tail_ptr)){
		QueuePtr->dataQueue[QueuePtr->tail_ptr] = value;
		QueuePtr->tail_ptr++;
		return true;
	}
	else if ((QueuePtr->tail_ptr == 170) && (QueuePtr->head_ptr > 0))
	{
		QueuePtr->tail_ptr = 0;
		QueuePtr->dataQueue[QueuePtr->tail_ptr] = value;
		return true;
	}
	else if((QueuePtr->tail_ptr < QueuePtr->head_ptr) && ((QueuePtr->head_ptr - QueuePtr->tail_ptr) > 1)){
		QueuePtr->dataQueue[QueuePtr->tail_ptr] = value;
		QueuePtr->tail_ptr++;
		return true;
	}
		return false;
}

/* ------------------------------------------------------------ */
/***	ACL2_myQueuePopFront(myQueue *QueuePtr)
**
**  Parameters:
**		QueuePtr:	FIFO object
**
**  Return Value:
**    	front:		item that is popped off front of the queue
**
**  Errors:
**    	none
**
**  Description:
**    	Takes the item first in the queue and returns it then
**    	moves the head_ptr to the next item in the queue
*/
int ACL2_myQueuePopFront(myQueue *QueuePtr){

	//store front value to return
	int result = QueuePtr->dataQueue[QueuePtr->head_ptr];
	QueuePtr->dataQueue[QueuePtr->head_ptr] = -9999;

	//if queue not empty, move head ptr back one over
	//new first item
	if((QueuePtr->tail_ptr > QueuePtr->head_ptr) || (QueuePtr->tail_ptr < QueuePtr->head_ptr)){
		QueuePtr->head_ptr = QueuePtr->head_ptr + 1;
	}
	//if the queue is empty reset pointers to start again
	else if((QueuePtr->head_ptr == 170) && (QueuePtr->head_ptr != QueuePtr->head_ptr))
		QueuePtr->head_ptr = 0;
	else{
		ACL2_myQueueRst(QueuePtr);
		return 0;
	}

	return result;
}

/* ------------------------------------------------------------ */
/***	ACL2_myQueueRst(myQueue *QueuePtr)
**
**  Parameters:
**		QueuePtr:	FIFO object
**
**  Return Value:
**    	none
**
**  Errors:
**   	none
**
**  Description:
**    	sets pointer values to start over queue then empties it by calling empty()
**
*/
void ACL2_myQueueRst(myQueue *QueuePtr)
{
	//initialize pointers
	QueuePtr->tail_ptr = 0;
	QueuePtr->head_ptr = 0;

	//clear queue
	ACL2_myQueueEmpty(QueuePtr);
	return;
}

void ACL2_myQueueEmptyAll(PmodACL2 *InstancePtr){
	ACL2_myQueueRst(&InstancePtr->xFIFO);
	ACL2_myQueueRst(&InstancePtr->yFIFO);
	ACL2_myQueueRst(&InstancePtr->zFIFO);
}

/* ------------------------------------------------------------ */
/* 	getQueue()
**
**  Parameters:
**		Queue:	an integer array to copy data to
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    accepts an integer array pointer then empties the dataQueue into the array.
**
*/
//void  ACL2_getQueue(myQueue *QueuePtr, int* outqueue){
//	int i;
//	//traverse queue and print and pop
//	for(i = 0; i < (ACL2_myQueueSize(QueuePtr)); i ++){
//		outqueue[i]=ACL2_myQueuePopFront(QueuePtr);
//	}
//	ACL2_myQueueRst(Queue);
//
//	return;
//}

/* ------------------------------------------------------------ */
/***	ACL2_printQueue(PmodACL2 *InstancePtr)
**
**  Parameters:
**		InstancePtr:	PmodACL2 object
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    accepts a PmodACL2 object and prints the contents of its
**    x, y, and z objects by passing them into ACL2_printQueueHelp.
*/
void ACL2_printQueue(PmodACL2 *InstancePtr){
	xil_printf("xFIFO:\n\r");
	ACL2_printQueueHelp(InstancePtr->xFIFO);
	xil_printf("yFIFO:\n\r");
	ACL2_printQueueHelp(InstancePtr->yFIFO);
	xil_printf("zFIFO:\n\r");
	ACL2_printQueueHelp(InstancePtr->zFIFO);
}

/* ------------------------------------------------------------ */
/***	ACL2_printQueueHelp(myQueue Queue)
**
**  Parameters:
**		QueuePtr:	an integer array to copy data to
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    accepts a myQueue object and prints the contents of its FIFO (dataQueue)
*/
void ACL2_printQueueHelp(myQueue QueuePtr){
	int i;
	for(i=0; i<170; i++)
		xil_printf("%d: %d\n\r", i+1, QueuePtr.dataQueue[i]);
	xil_printf("\n\r");
}
