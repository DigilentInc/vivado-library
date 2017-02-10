/***************************************************************************/
/*																		   */
/*	main.c		--		Definition for HYGROI2C demo	 	    		   */
/*																		   */
/***************************************************************************/
/*	Author:		Arthur Brown											   */
/*	Copyright 2017, Digilent Inc.										   */
/***************************************************************************/
/*  File Description:													   */
/*	This file defines a demonstration for the use of the Pmod HYGRO ip     */
/*  core. Temperature and Humidity data is captured from the I2C  		   */
/*  connection and displayed twice per second over the Serial connection.  */
/*  This application configures UART 16550 to baud rate 9600.			   */
/*  PS7 UART (Zynq) is not initialized by this application, since		   */
/*  bootrom/bsp configures it to baud rate 115200						   */
/*																		   */
/*   UART TYPE   BAUD RATE                        						   */
/*   uartns550   9600													   */
/*   uartlite    Configurable only in HW design							   */
/*   ps7_uart    115200 (configured by bootrom/bsp)						   */
/*																		   */
/***************************************************************************/
/*  Revision History:													   */
/*	01/30/2017(ArtVVB): created											   */
/***************************************************************************/

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "PmodHYGRO.h"
#include "sleep.h"
PmodHYGRO myDevice;

int main()
{
	float temp_degc, hum_perrh, temp_degf;
    init_platform();

    xil_printf("Init Started\n\r");
    HYGRO_begin(&myDevice, XPAR_PMODHYGRO_0_AXI_LITE_IIC_BASEADDR, 0x40);
    usleep(15000);
    xil_printf("Init Done\n\r");

    while (1) {
		temp_degc = HYGRO_getTemperature(&myDevice);
		temp_degf = HYGRO_tempC2F(temp_degc);
		hum_perrh = HYGRO_getHumidity(&myDevice);
		xil_printf("Temperature: %d.%d deg F  Humidity: %d.%d RH\n\r",
			(int)temp_degf, ((int)(temp_degf*100))%100,
			(int)hum_perrh, ((int)(hum_perrh*100))%100
		); // %f does not work with xil_printf, instead converting float to a pair of ints to display %.2f.
		usleep(1000000); // 1 sample per second maximum, as per 9.2.1 in HDC1080 reference manual
    }

    cleanup_platform();
    return 0;
}
