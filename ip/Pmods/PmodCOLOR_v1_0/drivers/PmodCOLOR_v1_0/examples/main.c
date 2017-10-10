/******************************************************************************/
/*                                                                            */
/* main.c -- Example program using the PmodCOLOR IP                           */
/*                                                                            */
/******************************************************************************/
/* Author: Arthur Brown                                                       */
/*                                                                            */
/******************************************************************************/
/* File Description:                                                          */
/*                                                                            */
/* This demo continuously prints RGB color information collected from the     */
/* PmodCOLOR                                                                  */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    10/10/2017(artvvb): Created                                             */
/*                                                                            */
/******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodCOLOR.h"
#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

typedef struct {
	COLOR_Data min, max;//, mid;
} CalibrationData;

void DemoInitialize();
void DemoRun();
CalibrationData DemoInitCalibrationData(COLOR_Data firstSample);
void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib);
COLOR_Data DemoNormalizeToCalibration(COLOR_Data sample, CalibrationData calib);
void Demo_usleep(u32 micros);
void EnableCaches();
void DisableCaches();

PmodCOLOR myDevice;

int main(void)
{
	DemoInitialize();
	DemoRun();
	DisableCaches();
	return 0;
}

void DemoInitialize()
{
	EnableCaches();
	COLOR_Begin(&myDevice, XPAR_PMODCOLOR_0_AXI_LITE_IIC_BASEADDR, XPAR_PMODCOLOR_0_AXI_LITE_GPIO_BASEADDR, 0x29);

	COLOR_SetENABLE(&myDevice, COLOR_REG_ENABLE_PON_MASK);
	Demo_usleep(2400);
	COLOR_SetENABLE(&myDevice, COLOR_REG_ENABLE_PON_MASK | COLOR_REG_ENABLE_RGBC_INIT_MASK);
	Demo_usleep(2400);
}

void DemoRun()
{
	u8 ID;
	COLOR_Data data;
	CalibrationData calib;

	xil_printf("Pmod COLOR Demo launched\r\n");
	ID = COLOR_GetID(&myDevice);
	xil_printf("Device ID = %02x\r\n", ID);

	data = COLOR_GetData(&myDevice);
	calib = DemoInitCalibrationData(data);
	Demo_usleep(2400);

	while (1) {
		data = COLOR_GetData(&myDevice);
		DemoCalibrate(data, &calib);
		data = DemoNormalizeToCalibration(data, calib);
		xil_printf("r=%04x g=%04x b=%04x\n\r",data.r,data.g,data.b);
		Demo_usleep(2400);
	}
}

CalibrationData DemoInitCalibrationData(COLOR_Data firstSample)
{
	CalibrationData calib;
	calib.min = firstSample;
	calib.max = firstSample;
	return calib;
}

void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib)
{
	if (newSample.c < calib->min.c) calib->min.c = newSample.c;
	if (newSample.r < calib->min.r) calib->min.r = newSample.r;
	if (newSample.g < calib->min.g) calib->min.g = newSample.g;
	if (newSample.b < calib->min.b) calib->min.b = newSample.b;

	if (newSample.c > calib->max.c) calib->max.c = newSample.c;
	if (newSample.r > calib->max.r) calib->max.r = newSample.r;
	if (newSample.g > calib->max.g) calib->max.g = newSample.g;
	if (newSample.b > calib->max.b) calib->max.b = newSample.b;
}

COLOR_Data DemoNormalizeToCalibration(COLOR_Data sample, CalibrationData calib)
{
	COLOR_Data norm;
	norm.c = (sample.c - calib.min.c) * (0xFFFF / (calib.max.c - calib.min.c));
	norm.r = (sample.r - calib.min.r) * (0xFFFF / (calib.max.r - calib.min.r));
	norm.g = (sample.g - calib.min.g) * (0xFFFF / (calib.max.g - calib.min.g));
	norm.b = (sample.b - calib.min.b) * (0xFFFF / (calib.max.b - calib.min.b));
	return norm;
}

void Demo_usleep(u32 micros)
{
#ifdef __MICROBLAZE__
	//round up to the nearest millisecond
	if (micros % 1000 != 0)
		MB_Sleep(micros/1000+1);
	else
		MB_Sleep(micros/1000);
#else
	usleep(micros);
#endif
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}
