/************************************************************************/
/*																		*/
/*	MtdsCore.c	--	Low Level Support Functions For mtds.cpp			*/
/*																		*/
/************************************************************************/
/*	Author: 	Gene Apperson											*/
/*	Copyright 2016, Digilent, Inc. All rights reserved.					*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This module contains utility support functions for the Digilent		*/
/*	Multi-Touch Display Shield library defined in mtds.cpp.				*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/* 2015/08/03(GeneApperson): Created									*/
/* 2015/10/05(GeneApperson): Renamed from MtdsUtil.cpp					*/
/*																		*/
/************************************************************************/

#define	OPT_BOARD_INTERNAL

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#if !defined(__SIM__)
#if defined(MPIDE)
#include	<WProgram.h>
#elif defined(__MICROBLAZE__) || defined(__arm__)
#include "xparameters.h"
#include "xil_io.h"
#include "XSpi.h"
#include "XTmrCtr.h"
XSpi MTDS_Spi;
XTmrCtr Timer;
XTmrCtr_Config Timer_Config =
{
	0,
	0,
#ifdef XPAR_CPU_CORE_CLOCK_FREQ_HZ
	XPAR_CPU_CORE_CLOCK_FREQ_HZ
#else
	100000000
#endif
};
XSpi_Config MTDSSPIConfig =
{
	0,
	XPAR_PMODMTDS_0_AXI_LITE_SPI_BASEADDR,
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
#else
#include	<Arduino.h>
#endif
#endif

#include	<stdint.h>

#include	"ProtoDefs.h"
#include	"mtds.h"
#include	"MtdsCore.h"

/* ------------------------------------------------------------ */
/*				Local Type Definitions							*/
/* ------------------------------------------------------------ */

//#define __MTDSDEBUG__
#define __MTDSTRACE__

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

uint8_t	rgbMtdsRetVal[cbRetValInit];

CHDR	chdrMtdsCmd;
DHDR	dhdrMtdsData;

RHDR *	prhdrMtdsRet = (RHDR *)rgbMtdsRetVal;
DHDR *	pdhdrMtdsData = &dhdrMtdsData;

#if !defined(__SIM__)

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
p32_spi *	pspiMtds;			// pointer to the SPI port used
#endif

#endif
int		pinMtdsSel;				// digital pin number of CS pin

/* ------------------------------------------------------------ */
/*				Local Variables									*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */
/***	MtdsSendCmdPacket(cls, cmd, cbParam, pbParam)
**
**	Parameters:
**		cls		- command class
**		cmd		- command code
**		cbParam	- number of bytes of parameter data in command packet
**		pbParam	- pointer to parameter data to send
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Send the specified command packet to the shield.
*/
void MtdsSendCmdPacket(uint8_t cls, uint8_t cmd, uint16_t cbParam, uint8_t * pbParam) {

	/* Set up the packet header.
	*/
	chdrMtdsCmd.cls = cls;
	chdrMtdsCmd.cmd = cmd;
	chdrMtdsCmd.cb = cbParam;

	/* Send the packet.
	*/
	MtdsEnableSlave(true);

	MtdsSendData(sizeof(CHDR), (uint8_t *)&chdrMtdsCmd);

	if (cbParam > 0) {
		MtdsSendData(cbParam, pbParam);
	}

	MtdsEnableSlave(false);

}

/* ------------------------------------------------------------ */
/***	MtdsReadStatusPacket()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		Returns status code
**
**	Description:
**		Read a status packet into the status packet buffer.
*/

bool MtdsReadStatusPacket() {
	bool	fStat;

	fStat = true;
	MtdsEnableSlave(true);

	if (MtdsReadRhdr(rgbMtdsRetVal)) {
		/* We got a good status packet header. Read the rest of the packet.
		*/
		if (prhdrMtdsRet->cb != 0) {
			MtdsReadData(prhdrMtdsRet->cb, &rgbMtdsRetVal[sizeof(RHDR)]);
		}
	}
	else {
		/* We got an error reading the header. This means that the first real
		** byte that we got back wasn't the class byte for a status packet header,
		** we timed out, or there was an internal inconsistency in the packet header.
		** The cls byte of the RHDR structure contains the channel status byte
		** indicating what happened. Return failure and let the higher level code
		** figure out how to recover.
		*/
		fStat = false;
	}

	MtdsEnableSlave(false);

	return fStat;
}

/* ------------------------------------------------------------ */
/***	MtdsSendDataPacket(cmd, cb, pb)
**
**	Parameters:
**		cmd		- command value to put in packet header
**		cb		- number of bytes for packet payload
**		pb		- pointer to packet payload to send
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Send the specified data packet to the shield.
*/
void MtdsSendDataPacket(uint8_t cmd, uint16_t cb, uint8_t * pb) {

	/* Set up the packet header.
	*/
	dhdrMtdsData.cls = clsDataOut;
	dhdrMtdsData.cmd = cmd;
	dhdrMtdsData.cb = cb;

	/* Send the packet payload.
	*/
	MtdsEnableSlave(true);
	MtdsSendData(sizeof(DHDR), (uint8_t *)&dhdrMtdsData);
	MtdsSendData(cb, pb);
	MtdsEnableSlave(false);

}

/* ------------------------------------------------------------ */
/***	MtdsReadDataPacket(cbMax, pb, pcbRes)
**
**	Parameters:
**		cbMax		- maximum number of bytes to read
**		pb			- pointer to buffer to receive the data
**		pcbRes		- pointer to variable to receive actual number of bytes read
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Read a data packet header and then read the packet payload
**		into the specified buffer.
*/

bool MtdsReadDataPacket(uint16_t cbMax, uint8_t * pb, uint16_t * pcbRes) {
	uint16_t	cbCur;
	uint16_t	cbRem;
	bool		fStat;

	fStat = true;
	*pcbRes = 0;
	MtdsEnableSlave(true);

	if (MtdsReadDhdr((uint8_t *)&dhdrMtdsData)) {
		/* We have the data in packet header. We need to read up to the requested amount
		** of data into the specified buffer.
		*/
		cbCur = dhdrMtdsData.cb;
		if (cbCur > cbMax) {
			cbCur = cbMax;
		}
		if (cbCur > 0) {
			MtdsReadData(cbCur, pb);
			*pcbRes = cbCur;
		}

		/* Discard any data from the data-in packet that won't fit in the return buffer.
		*/
		cbRem = dhdrMtdsData.cb - cbCur;
		if (cbRem > 0) {
			MtdsReadData(cbRem, 0);
		}
	}
	else {
		/* We got an error reading the header. This means that the first real
		** byte that we got back wasn't the class byte for a data packet header,
		** we timed out, or there was an internal inconsistency in the packet header.
		** The cls byte of the DHDR structure contains the channel status byte
		** indicating what happened. Return failure and let the higher level code
		** figure out how to recover.
		*/
		fStat = false;
	}

	MtdsEnableSlave(false);

	return fStat;

}

/* ------------------------------------------------------------ */
/***	MtdsReadRhdr(pb)
**
**	Parameters:
**		pb		- buffer to receive the data
**
**	Return Values:
**		none
**
**	Errors:
**		Returns true if successful, false if we get an error.
**
**	Description:
**		Read a status packet header from the shield.
*/

bool MtdsReadRhdr(uint8_t * pb) {
	uint8_t		bRcv;
	int			cbCur;
	RHDR *		prhdr;
#if !defined(__SIM__)
	uint32_t	tmsStart;
#endif

	cbCur = 0;
	prhdr = (RHDR *)pb;
	
	/* It may take a while for the shield to process the read status command
	** and to begin sending the status packet. While it is processing the command
	** we will get back "chnStaBusy" bytes. We may also get some "chnStaIdle" bytes
	** before we get the actual status packet. We need to skip past those until
	** we get the actual status packet header.
	*/
#if !defined(__SIM__)
	tmsStart = millis();
#endif
	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}
		bRcv = MtdsPutSpiByte(chnCmdRead);
		if ((bRcv & mskPacketCls) == clsPacketSta) {
			break;
		}
#if !defined(__SIM__)
		if ((millis() - tmsStart) > tmsRhdrTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!RdRhdr timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!RdRhdr timeout\r\n");
#endif
#endif
			prhdr->cls = chnStaTimeout;
			return false;
		}
#endif
	}

	/* Stick the first byte in the buffer.
	*/
	*pb++ = bRcv;
	cbCur += 1;

	/* Check that we have the class byte for a status packet.
	*/
	if ((bRcv & mskPacketCls) != clsPacketSta) {
		/* We don't have the correct initial byte for a status packet.
		*/
#if !defined(__SIM__)
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
		Serial.print("!RdRhdr: ");
		Serial.println(bRcv, HEX);
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!RdRhdr: %X\r\n", bRcv);
#endif
#endif
#endif
		return false;
	}

	/* Read the rest of the header.
	*/
	while (cbCur < sizeof(RHDR)) {
		while (!FMtdsSpiReady()) {
		}
		bRcv = MtdsPutSpiByte(chnCmdRead);
		*pb++ = bRcv;
		cbCur +=1;
	}

	if (prhdr->cb > cbRhdrDataMax) {
		/* The cb field is too large for this to be a valid header. Either the host
		** sent us a bad packet, or it got screwed up by a channel error.
		*/
		prhdr->cls = chnStaDataSize;
#if !defined(__SIM__)
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
		Serial.println("!RdRhdr data size");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!RdRhdr data size\r\n");
#endif
#endif
#endif
		return false;
	}

	return true;
		
}

/* ------------------------------------------------------------ */
/***	MtdsReadDhdr(pb)
**
**	Parameters:
**		pb		- buffer to receive the data
**
**	Return Values:
**		none
**
**	Errors:
**		Returns true if successful, false if we get an error.
**
**	Description:
**		Read a data packet header from the shield.
*/

bool MtdsReadDhdr(uint8_t * pb) {
	uint8_t		bRcv;
	int			cbCur;
	DHDR *		pdhdr;
#if !defined(__SIM__)
	uint32_t	tmsStart;
#endif

	cbCur = 0;
	pdhdr = (DHDR *)pb;
	
	/* It may take a while for the shield to process the command and to begin
	** sending the data packet.
	** While it is processing the command we will get back "chnStaBusy" bytes. 
	** We need to skip past these. If the shield generated a data in packet, we
	** will get clsDataIn. If the shield terminated the command and didn't generate
	** a data packet, we will get chnStaDone.
	*/
#if !defined(__SIM__)
	tmsStart = millis();
#endif
	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}
		bRcv = MtdsPutSpiByte(chnCmdRead);
		if ((bRcv == clsDataIn) || (bRcv == chnStaDone)) {
			break;
		}
#if !defined(__SIM__)
		if ((millis() - tmsStart) > tmsDhdrTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!RdDhdr timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!RdDhdr timeout\r\n");
#endif
#endif
			pdhdr->cls = chnStaTimeout;
			return false;
		}
#endif
	}

	/* Stick the first byte in the buffer.
	*/
	*pb++ = bRcv;
	cbCur += 1;

	/* Check that we have the class byte for a data packet.
	*/
	if (bRcv != clsDataIn) {
		return false;
	}

	/* Read the rest of the header.
	*/
	while (cbCur < sizeof(DHDR)) {
		while (!FMtdsSpiReady()) {
		}
		bRcv = MtdsPutSpiByte(chnCmdRead);
		*pb++ = bRcv;
		cbCur +=1;
	}

	if (pdhdr->cb > cbDhdrDataInMax) {
		/* The cb field is too large for this to be a valid header. Either the host
		** sent us a bad packet, or it got screwed up by a channel error.
		*/
		pdhdr->cls = chnStaDataSize;
#if !defined(__SIM__)
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
		Serial.println("!RdDhdr data size");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!RdDhdr data size\r\n");
#endif
#endif
#endif
		return false;
	}

	return true;
		
}

/* ------------------------------------------------------------ */
/***	MtdsSendData(cb, pb)
**
**	Parameters:
**		cb		- number of bytes to send
**		pb		- pointer to the bytes to send
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		This functions writes the specified number of bytes into the
**		transmit buffer to the shield. Because the shield uses the SPI
**		port in enhanced mode with FIFO, we can burst the data quickly.
**		However, we need to be careful to not overrun the FIFO, so we
**		burst it in bursts of a maximum of 8 bytes and then wait a bit.
*/

void MtdsSendData(uint16_t cb, uint8_t * pb) {
	int		cbCur;

	for (cbCur = 0; cbCur < cb; cbCur++) {
		while (!FMtdsSpiReady()) {
		}
		MtdsPutSpiByte(*pb++);
	}

}

/* ------------------------------------------------------------ */
/***	MtdsReadData(cb, pb)
**
**	Parameters:
**		cb		- number of bytes to receive from shield
**		pb		- buffer to receive the data, pb==0 means to discard the data
**
**	Return Values:
**		none
**
**	Errors:
**		Returns true if successful, false if a time-out occurs.
**
**	Description:
**		Read the specified number of bytes from the shield.
*/

void MtdsReadData(uint16_t cb, uint8_t * pb) {
	int		cbCur;
	uint8_t	bRcv;

	cbCur = 0;

	while(cbCur < cb) {
		while (!FMtdsSpiReady()) {
		}
		bRcv = MtdsPutSpiByte(chnCmdRead);
		if (pb != 0) {
			*pb++ = bRcv;
		}
		cbCur += 1;
	}

}

/* ------------------------------------------------------------ */
/***	MtdsWaitUntilShieldIdle()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Wait until the shield indicates that it had finished executing the
**		last command. 
*/

bool MtdsWaitUntilShieldIdle() {
	uint8_t		bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif
	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdRead);
		MtdsEnableSlave(false);
		if (bTmp == chnStaIdle) {
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsIdleTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitIdle timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitIdle timeout\r\n");
#endif
#endif
			return false;
		}
#endif
	}

	return true;
}

/* ------------------------------------------------------------ */
/***	MtdsWaitUntilShieldBusy()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Wait until we get a chnStaBusy from the shield. This indicates that the
**		SPI FIFOs have been flushed of any previous characters that may have been
**		there prior to the transmission of the command packet. 
*/

bool MtdsWaitUntilShieldBusy() {
	uint8_t		bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif
	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdRead);
		MtdsEnableSlave(false);
		if (bTmp == chnStaBusy) {
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsBusyTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitBusy timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitBusy timeout\r\n");
#endif
#endif
			return false;
		}
#endif
	}

	return true;
}

/* ------------------------------------------------------------ */
/***	MtdsWaitUntilShieldDone()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Wait until the shield indicates that it had finished executing the
**		last command. 
*/

bool MtdsWaitUntilShieldDone() {
	uint8_t		bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif
	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdRead);
		MtdsEnableSlave(false);
		if (bTmp == chnStaDone) {
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsIdleTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitDone timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitDone timeout\r\n");
#endif
#endif
			return false;
		}
#endif
	}

	return true;
}

/* ------------------------------------------------------------ */
/***	MtdsWaitUntilShieldReady()
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns the ending condition.
**
**	Errors:
**		none
**
**	Description:
**		Wait until the shield indicates that it is ready to accept a
**		data out packet. The shield will return chnStaReady when it is ready
**		to accept a data packet.
**		Normally this function returns when the shield has sent chnStaReady. However,
**		it can terminate because the shield aborted the command by returning
**		chnStaAbort, or we timed out waiting for the shield to become ready or
**		abort.
**		This function returns the character that caused the end condition, or 0 if
**		it timed out.
*/

uint8_t MtdsWaitUntilShieldReady() {
	uint8_t	bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif

	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}		
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdRead);
		MtdsEnableSlave(false);
		if ((bTmp == chnStaReady) || (bTmp == chnStaAbort)) {
			/* We are waiting for the shield to indicate that it is ready for a
			** data packet, or that it wants to abort the command.
			*/
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsReadyTimeout) {
			/* We timed out waiting for the expected response.
			*/
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitReady timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitReady timeout\r\n");
#endif
#endif
			return chnStaTimeout;
		}
#endif
	}

	return bTmp;
}

/* ------------------------------------------------------------ */
/***	MtdsWaitUntilShieldSync()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Wait until the shield indicates that it is ready to accept a
**		data out packet. It will return chnStaReady when it is ready
**		to accept a data packet.
*/

bool MtdsWaitUntilShieldSync() {
	uint8_t	bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif

	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}		
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdRead);
		MtdsEnableSlave(false);
		if (bTmp == chnStaSync) {
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsSyncTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitSync timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitSync timeout\r\n");
#endif
#endif
			return false;
		}
#endif
	}

	return true;
}

/* ------------------------------------------------------------ */
/***	MtdsResumeChannel()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		Returns true if successful, false if it times out waiting for shield
**
**	Description:
**		Following an abort of the command by the shield, we need to send
**		chnCmdResume until it goes to the idle state.
*/

bool MtdsResumeChannel() {
	uint8_t	bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif

	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}		
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdResume);
		MtdsEnableSlave(false);
		if (bTmp == chnStaDone) {
			break;
		}
#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsResumeTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitResume timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitResume timeout\r\n");
#endif
#endif
			return false;
		}
#endif
	}

	return true;
}

/* ------------------------------------------------------------ */
/***	MtdsBeginUpdate()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		Returns chnStaSuccess if successful, other channel status code if not
**
**	Description:
**		This issues the channel command to the shield firmware to
**		launch the firmware update process.
*/

uint8_t MtdsBeginUpdate() {
	uint8_t		bTmp;

#if !defined(__SIM__)
	uint32_t	tmsStart;
	tmsStart = millis();
#endif

	while (true) {
#if !defined(__SIM__)
		delayMicroseconds(tusPacketDelay);
#endif
		while (!FMtdsSpiReady()) {
		}		
		MtdsEnableSlave(true);
		bTmp = MtdsPutSpiByte(chnCmdUpdate);
		MtdsEnableSlave(false);
		if (bTmp == chnStaUpdate) {
			break;
		}
		if ((bTmp == chnStaNak) || (bTmp == chnStaAbort) ) {
			return bTmp;
		}

#if !defined(__SIM__) && !defined(__MTDSDEBUG__)
		if ((millis() - tmsStart) > tmsUpdateTimeout) {
#if defined(__MTDSTRACE__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
			Serial.println("!WaitUpdate timeout");
#elif defined(__MICROBLAZE__)||defined(__arm__)
		xil_printf("!WaitSync timeout\r\n");
#endif
#endif
			return chnStaTimeout;
		}
#endif
	}

	return chnStaSuccess;

}

/* ------------------------------------------------------------ */
/***	MtdsQueryUpdate()
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns update state code returned by the display device
**
**	Errors:
**		Errors are indicated by the update status code returned
**
**	Description:
**		Send chnCmdRead to the display device and get the returned
**		character.
*/

uint8_t	MtdsQueryUpdate() {
	uint8_t		bTmp;
	uint8_t		sta;

#if !defined(__SIM__)
	delayMicroseconds(tusPacketDelay);
#endif
	while (!FMtdsSpiReady()) {
	}		
	MtdsEnableSlave(true);
	bTmp = MtdsPutSpiByte(chnCmdRead);
	MtdsEnableSlave(false);

	if (bTmp == chnStaNak) {
		/* The firmware updater got a fatal error. The update failed and we
		** can't recover.
		*/
		sta = staUpdateError;
	}
	else if (bTmp == chnStaAbort) {
		/* The firmware updater was unable to start the update process, but
		** the current firmware image is still intact. Tell the updater to
		** resume the current firmware image. The updater will transfer control
		** to the reset entry point for the firmware image. We send chnCmdResume
		** until we get back chnStaStartup. This indicates that the device firmware
		** is doing its reset initialization.
		*/
		while (true) {
#if !defined(__SIM__)
			delayMicroseconds(tusPacketDelay);
#endif
			while (!FMtdsSpiReady()) {
			}		
			MtdsEnableSlave(true);
			bTmp = MtdsPutSpiByte(chnCmdResume);
			MtdsEnableSlave(false);
			if (bTmp == chnStaStartup) {
				break;
			}
		}
		sta = staUpdateAbort;
	}
	else if (bTmp == chnStaStartup) {
		/* The firmware updater has completed the update and transferred control
		** to the beginning of the new firmware image. It is in the process of
		** initializing itself.
		*/
		sta = staUpdateSuccess;
	}
	else {
		/* The updater is still working.
		*/
		sta = staUpdateBusy;
	}

	return sta;

}

/* ------------------------------------------------------------ */
/***	ProcName
**
**	Parameters:
**
**	Return Values:
**
**	Errors:
**
**	Description:
**
*/

/* ------------------------------------------------------------ */
/*			Low Level Communications Functions					*/
/* ------------------------------------------------------------ */
/***	MtdsInitPins(pinSel)
**
**	Parameters:
**		pinSel		- digital pin number of CS pin for SPI interface
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Set the pin directions and other low level hardware features
**		being used.
*/

void MtdsInitPins(int pinSel) {

#if !defined(__SIM__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	/* Set up the pin directions for the SPI pins. The hardware automatically sets
	** up the pin directions when the SPI controller is enabled, but there are some
	** ancillary things (such as ensuring that the pins are digital pins) that occur
	** by calling pinMode()
	*/
	pinMode(MOSI, OUTPUT);
	pinMode(MISO, INPUT);
	pinMode(SCK, OUTPUT);
#endif
#endif

	pinMtdsSel = pinSel;

#if !defined(__SIM__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	digitalWrite(pinMtdsSel, HIGH);
	pinMode(pinMtdsSel, OUTPUT);
#elif defined(__MICROBLAZE__) || defined(__arm__)
	XTmrCtr_CfgInitialize(&Timer, &Timer_Config, XPAR_PMODMTDS_0_AXI_LITE_TIMER_BASEADDR);
	XTmrCtr_SetOptions(&Timer, 0, XTC_AUTO_RELOAD_OPTION| XTC_CASCADE_MODE_OPTION);
	XTmrCtr_SetResetValue(&Timer, 0, 0);
	XTmrCtr_Reset(&Timer,0);
	XTmrCtr_Reset(&Timer,1);
	XTmrCtr_Start(&Timer, 0);
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, 0b111);
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+12, 0b000);//Set chip selects(ch2) to outputs
#endif
#endif

}

/* ------------------------------------------------------------ */
/***	MtdsEnableStatusPin(idPin)
**
**	Parameters:
**		idPin	- identifier for the status pin being enabled
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Ensure that the specified pin is made to be an input.
*/

void MtdsEnableStatusPin(int idPin) {

#if !defined(__SIM__)
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	if (idPin == idPinStatusA) {
		pinMode(pinMtdsIntA, INPUT);
	}
	else if (idPin == idPinStatusB) {
		pinMode(pinMtdsIntB, INPUT);
	}
#elif defined(__MICROBLAZE__) || defined(__arm__)
	Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+4,0b11);//Set interrupts (ch1) as inputs
#endif
#endif

}

/* ------------------------------------------------------------ */
/***	MtdsInitSpi(frq)
**
**	Parameters:
**		frq			- SPI clock frequency
**
**	Return Values:
**		none
**
**	Errors:
**		Returns true if successful, false if error.
**
**	Description:
**		Initialize the SPI port used by the MTDS library to communication
**		with the graphics shield.
*/

void MtdsInitSpi(uint32_t pspiInit, uint32_t frq) {
#if !defined(__SIM__)

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor.
	*/
	uint8_t		tmp;
	uint16_t	brg;

	/* We don't actually use the SPI library, but we use the same port, so
	** we can make use of the same symbols in the board variant file for our
	** host board.
	*/
	pspiMtds = (p32_spi *)pspiInit;

#if defined(__PIC32MX1XX__) || defined(__PIC32MX2XX__) || defined(__PIC32MZXX__)
	/* M00TODO - change this to be conditional on processor having PPS, there is a new symbol for this
	/* Set up PPS pin mapping if we are on a chip with PPS.
	*/
    mapPps(_SPI_MISO_PIN, _SPI_MISO_IN);
    mapPps(_SPI_MOSI_PIN, _SPI_MOSI_OUT);
#endif

	brg = MtdsComputeBrg(frq);

	/* Initialize the SPI controller.
	*/
	pspiMtds->sxCon.reg = 0;								// turn SPI controller off
	tmp = pspiMtds->sxBuf.reg;								// clear the receive buffer
	pspiMtds->sxBrg.reg = brg;								// set the clock frequency
	pspiMtds->sxStat.clr = (1 << _SPISTAT_SPIROV);			// clear the receive overflow bit.
	pspiMtds->sxCon.set = (1 << _SPICON_MSTEN) + SPI_MODE0;	// set master, mode 0
	pspiMtds->sxCon.set = (1 << _SPICON_ON);				// turn SPI controller on

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	** Init the SPI controller with the following conditions:
	**		SPIE = 0	: SPI interrupts disabled
	**		SPE = 1		: SPI enabled
	**		DORD = 0	: shift directions MSB first
	**		MSTR = 1	: Master mode
	**		CPOL = 0	: SPI mode 0
	**		CPHA = 0
	**		SPR1 = 0	: SCK frequency = Fosc/4
	**		SPR0 = 0
	**
	**		SPI2X = 0
	*/
	SPCR = _BV(SPE) | _BV(MSTR);
	SPSR = _BV(SPI2X);
#elif defined(__MICROBLAZE__)||defined(__arm__)
XSpi_CfgInitialize(&MTDS_Spi, &MTDSSPIConfig, XPAR_PMODMTDS_0_AXI_LITE_SPI_BASEADDR);
XSpi_SetOptions(&MTDS_Spi, (XSP_MASTER_OPTION) | XSP_MANUAL_SSELECT_OPTION);
XSpi_SetSlaveSelect(&MTDS_Spi, 1);
XSpi_Start(&MTDS_Spi);
XSpi_IntrGlobalDisable(&MTDS_Spi);
#else
#error "No supported processor defined"
#endif

#endif

}

/* ------------------------------------------------------------ */
/***	MtdsEnableSlave(fEn)
**
**	Parameters:
**		fEn		- true to enable slave (CS low), false to disable (CS high)
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Set the CS pin high or low to enable/disable the slave for SPI
**		communications.
*/

void MtdsEnableSlave(bool fEn) {

#if !defined(__SIM__)
#if defined(PIC32)
	if (fEn) {
		digitalWrite(pinMtdsSel, LOW);
	}
	else {
		digitalWrite(pinMtdsSel, HIGH);
	}
#elif defined(__MICROBLAZE__) || defined(__arm__)
	if (fEn){
		Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, Xil_In32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8)&0b011);
	}
	else{
		Xil_Out32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8, Xil_In32(XPAR_PMODMTDS_0_AXI_LITE_GPIO_BASEADDR+8)|0b100);
	}
#endif
#endif

}

/* ------------------------------------------------------------ */
/***	FMtdsSpiReady()
**
**	Parameters:
**		none
**
**	Return Values:
**		Returns true if SPI port is ready to accept a byte.
**
**	Errors:
**		none
**
**	Description:
**		Check if the SPI port is ready to send a byte.
*/

bool FMtdsSpiReady() {

#if !defined(__SIM__)

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/

	return (pspiMtds->sxStat.reg & (1 << _SPISTAT_SPITBE)) != 0;

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	*/
	return true;
#elif defined(__MICROBLAZE__)||defined(__arm__)
return !MTDS_Spi.IsBusy;
#else
#error "No supported processor defined"
#endif

#else
	extern bool FSimMstTbe();
	return FSimMstTbe();
#endif

}

/* ------------------------------------------------------------ */
/***	MtdsPutSpiByte(bSnd)
**
**	Parameters:
**		bSnd		- byte to send to the display shield
**
**	Return Values:
**		Returns byte read from the display shield
**
**	Errors:
**		none
**
**	Description:
**		This function writes the output byte to the SPI port, waits
**		for the transmission to complete and then reads the received
**		byte from the SPI port.
*/

uint8_t	MtdsPutSpiByte(uint8_t bSnd) {
	uint8_t		bRcv;

#if !defined(__SIM__)

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/
	while ((pspiMtds->sxStat.reg & (1 << _SPISTAT_SPITBE)) == 0) {
	}
	pspiMtds->sxBuf.reg = bSnd;

	while ((pspiMtds->sxStat.reg & (1 << _SPISTAT_SPIRBF)) == 0) {
	}
	bRcv = pspiMtds->sxBuf.reg;

#elif defined(__AVR__)
	/* We are building for an Arduino board using an AVR processor.
	*/
    SPDR = bSnd;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop from iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while ((SPSR & _BV(SPIF)) == 0) ; // wait
    bRcv = SPDR;
#elif defined(__MICROBLAZE__)||defined(__arm__)
XSpi_Transfer(&MTDS_Spi, &bSnd,&bRcv, 1);
#else
#error "No supported processor defined"
#endif

#else

extern	uint8_t SimSpiTransfer(uint8_t b);

	bRcv = SimSpiTransfer(bSnd);

#endif

	return bRcv;

}

/* ------------------------------------------------------------ */
/***	MtdsComputeBrg(spd)
**
**	Parameters:
**		spd		- desired SPI clock speed
**
**	Return Values:
**		Returns value to load into SPI baud rate generator register
**
**	Errors:
**		none
**
**	Description:
**		This function computes the SPI baud rate generator value that
**		yields the closest supported SPI clock speed to the requested
**		value, based on the peripheral bus frequency.
*/

uint16_t MtdsComputeBrg(uint32_t spd) {
	uint16_t	brg;

#if defined(__SIM__)

	brg = 0;

#else !defined(__SIM__)

	/* Compute the baud rate divider for this frequency.
	*/
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
	/* We are building for chipKIT/MPIDE using a PIC32 processor
	*/
	brg = (uint16_t)((__PIC32_pbClk / (2 * spd)) - 1);

	/* Check that the result is in the correct range.
	*/
	if (brg == 0xFFFF) {
		/* The user tried to set a frequency that is too high to support.
		** Set it to the highest supported frequency.
		*/
		brg = 0;
	}

	if (brg > 0x1FF) {
		/* The user tried to set a frequency that is too low to support.
		** Set it to the lowest supported frequency.
		*/
		brg = 0x1FF;
	}
#endif

#endif

	return brg;
}

/* ------------------------------------------------------------ */
/***	ProcName
**
**	Parameters:
**
**	Return Values:
**
**	Errors:
**
**	Description:
**
*/

/* ------------------------------------------------------------ */

/************************************************************************/

