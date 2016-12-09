/*
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "PmodRTCC.h"
#include "xil_types.h"
#include "xparameters.h"

#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif


#define SET_RTCC; 	//if RTCC is already set comment this line out.

PmodRTCC myDevice;
int year, mon, date, day, hour, minute, sec, ampm = 0;



void printTime(u8 src);
void system_init();

int main()
{
    init_platform();

    //xil_printf("Testing123\r\n");

    system_init();

	while(1)
	{
		//print current time
		xil_printf("Current time is : ");
		printTime(RTCC_RTCC);
		xil_printf("\r\n");

		//Check if alarm 0 is triggered
		if(RTCCI2C_checkFlag(RTCC_ALM0, &myDevice))
		{
			//alarm 0 has been triggered
			xil_printf("ALM0!!!");
			//disable alarm 0
			RTCCI2C_disableAlarm(RTCC_ALM0, &myDevice);
			xil_printf("\r\n");
		}

		//check if alarm 1 is triggered
		if(RTCCI2C_checkFlag(RTCC_ALM1, &myDevice))
		{
			//alarm 1 has been triggered
			xil_printf("ALM1!!!");
			//disable alarm
			RTCCI2C_disableAlarm(RTCC_ALM1, &myDevice);
			xil_printf("\r\n");
		}

	}

		cleanup_platform();
		return 0;
}


void system_init()
{
	RTCC_begin(&myDevice, XPAR_PMODRTCC_0_AXI_LITE_IIC_BASEADDR, 0x6F);
	//print the power-fail time-stamp
	  xil_printf("Lost Power at: ");
	  printTime(RTCC_PWRD);
	  xil_printf("\r\n");
	  xil_printf("Power was back at: ");
	  printTime(RTCC_PWRU);
	  xil_printf("\r\n");

if(!RTCCI2C_checkVbat(&myDevice))
{
	//set the real time clock
	  RTCCI2C_stopClock(&myDevice);
	  RTCCI2C_setSec(RTCC_RTCC, 0x00, &myDevice);
	  RTCCI2C_setMin(RTCC_RTCC, 0x06, &myDevice);
	  RTCCI2C_setHour12(RTCC_RTCC, 0x09, RTCC_AM, &myDevice);
	  RTCCI2C_setDay(RTCC_RTCC, 0x05, &myDevice);
	  RTCCI2C_setDate(RTCC_RTCC, 0x22, &myDevice);
	  RTCCI2C_setMonth(RTCC_RTCC, 0x07, &myDevice);
	  RTCCI2C_setYear(0x16, &myDevice);
	  RTCCI2C_startClock(&myDevice);
	  xil_printf("The time has been set \r\n");
	  //set vbat high
	  RTCCI2C_enableVbat(&myDevice);

}
	  //set alarm 0
	  RTCCI2C_setSec(RTCC_ALM0, 0x35, &myDevice);
	  RTCCI2C_setMin(RTCC_ALM0, 0x59, &myDevice);
	  RTCCI2C_setHour12(RTCC_ALM0, 0x09, RTCC_AM, &myDevice);
	  RTCCI2C_setDay(RTCC_ALM0, 0x05, &myDevice);
	  RTCCI2C_setDate(RTCC_ALM0, 0x22, &myDevice);
	  RTCCI2C_setMonth(RTCC_ALM0, 0x07, &myDevice);

	  //set alarm 1
	  RTCCI2C_setSec(RTCC_ALM1, 0x45, &myDevice);
	  RTCCI2C_setMin(RTCC_ALM1, 0x33, &myDevice);
	  RTCCI2C_setHour12(RTCC_ALM1, 0x10, RTCC_AM, &myDevice);
	  RTCCI2C_setDay(RTCC_ALM1, 0x05, &myDevice);
	  RTCCI2C_setDate(RTCC_ALM1, 0x22, &myDevice);
	  RTCCI2C_setMonth(RTCC_ALM1, 0x07, &myDevice);

	  //print current time
	  xil_printf("Current time is: ");
	  printTime(RTCC_RTCC);
	  xil_printf("\r\n");
	  //print alarm 0
	  xil_printf("Alarm 0 is set to : ");
	  printTime(RTCC_ALM0);
	  xil_printf("\r\n");
	  //print alarm 1
	  xil_printf("Alarm 1 is set to : ");
	  printTime(RTCC_ALM1);
	  xil_printf("\r\n");

	  //enables alarm 0
	  //set configuration bits to RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0
	  //this will drive the MPF pin high when the alarm triggered
	  //it also sets the alarm to be triggered when the alarm matches
	  //Seconds, Minutes, Hour, Day, Date, Month of the RTCC
	  RTCCI2C_enableAlarm(RTCC_ALM0, RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0, &myDevice);

	  //enable alarm 1
	  //set configuration bits to RTCC_ALM_POL
	  //this will drive the MPF pin high when the alarm triggered
	  //it also sets the alarm to be triggered when the alarm matches
	  //seconds of the RTCC
	  RTCCI2C_enableAlarm(RTCC_ALM1,  RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0, &myDevice);

	  //Enable back up battery
	  RTCCI2C_enableVbat(&myDevice);

	  RTCCI2C_clearPWRFAIL(&myDevice);
}



void printTime(u8 src)
{
	sec = RTCCI2C_getSec(src, &myDevice);
	minute = RTCCI2C_getMin(src, &myDevice);
	hour = RTCCI2C_getHour(src, &myDevice);
	ampm = RTCCI2C_getAmPm(src, &myDevice);
	day = RTCCI2C_getDay(src, &myDevice);
	date = RTCCI2C_getDate(src, &myDevice);
	mon = RTCCI2C_getMonth(src, &myDevice);
	year = RTCCI2C_getYear(&myDevice);

	 // print all parameter of the src
	xil_printf("%02x", mon);
	xil_printf("/");
	xil_printf("%02x", date);
	//year is only available for the RTCC
	if( src == RTCC_RTCC)
	{
		xil_printf("/");
		xil_printf("%02x", year);
	}
	xil_printf(" day");
	xil_printf("%02x", day);
	xil_printf(" ");
	xil_printf("%02x", hour);
	xil_printf(":");
	xil_printf("%02x", minute);

//second is not supported by the power fail registers
    if( src != RTCC_PWRD && src != RTCC_PWRU)
    {
    	xil_printf(":");
    	xil_printf("%02x", sec);
    }

    if(ampm)
    {
    	xil_printf(" PM");
    }
    else
    {
    	xil_printf(" AM");
    }
#ifdef XPAR_MICROBLAZE_ID
		MB_Sleep(1500);
#else
		usleep(1000000);
#endif
}



















