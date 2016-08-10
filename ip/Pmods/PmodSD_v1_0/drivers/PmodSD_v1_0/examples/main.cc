/************************************************************************/
/*                                                                      */
/*      PmodSD Demo	                                                    */
/*                                                                      */
/*      This demo creates a file in your SD card and writes to it		*/
/*                                                                      */
/************************************************************************/
/*       Author:        Thomas Kappenman                                */
/*       Copyright 2016, Digilent Inc.                                  */
/************************************************************************/
/*
*
* Copyright (c) 2013-2016, Digilent <www.digilentinc.com>
* Contact Digilent for the latest version.
*
* This program is free software; distributed under the terms of
* BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*		 8/10/2016 (TommyK): Created									*/
/*                                                                      */
/************************************************************************/

#include "PmodSD.h"
#include "xil_cache.h"

DXSPISDVOL disk(XPAR_PMODSD_0_AXI_LITE_SPI_BASEADDR, XPAR_PMODSD_0_AXI_LITE_SDCS_BASEADDR);
DFILE file;

// the drive to mount the SD volume to.
// options are: "0:", "1:", "2:", "3:", "4:"
static const char szDriveNbr[] = "0:";

void DemoInitialize();
void DemoRun();


int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{

}


void DemoRun()
{
	FRESULT fr;
	u32 bytesWritten=0;

	//Mount the disk
	DFATFS::fsmount(disk, szDriveNbr, 1);

	if ((fr = file.fsopen("newfile.txt", FA_WRITE | FA_CREATE_ALWAYS)) == FR_OK){
		file.fswrite("It works!\r\n", 11, &bytesWritten);
		file.fsclose();
	}


	while(1){
	}
}



