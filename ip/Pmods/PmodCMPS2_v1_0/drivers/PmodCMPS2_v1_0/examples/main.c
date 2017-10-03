/******************************************************************************/
/*                                                                            */
/* main.c -- Example program using the PmodCMPS2 IP                           */
/*                                                                            */
/******************************************************************************/
/* Author: Arthur Brown                                                       */
/*                                                                            */
/******************************************************************************/
/* File Description:                                                          */
/*                                                                            */
/* This demo continuously polls the CMPS2 data registers and prints the       */
/* y-axis to north angle in degrees as well as the cardinal direction string  */
/* for that angle.                                                            */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    10/02/2017(artvvb):  Created                                            */
/*                                                                            */
/******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "PmodCMPS2.h"

#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif

#include "math.h"

//calibration data struct, track minimum, maximum, and average sample seen for each x/y/z channel
typedef struct {
	CMPS2_DataPacket max, min, mid;
} CMPS2_CalibrationData;

void DemoInitialize();
void DemoRun();
int DemoConvertDegree(PmodCMPS2 *InstancePtr, CMPS2_CalibrationData calib, CMPS2_DataPacket data, int declination);
void DemoClearCalibration(CMPS2_CalibrationData *calib);
void DemoCalibrate (PmodCMPS2 *InstancePtr, CMPS2_CalibrationData *calib, CMPS2_DataPacket data);
char *DemoGetCardinalDirectionString(int deg);
void DemoCleanup();
void DemoSleep(u32 millis);
void EnableCaches();
void DisableCaches();

PmodCMPS2 myDevice;
CMPS2_CalibrationData myCalibrationData;

const int myDeclination = 15; // magnetic declination for Seattle, WA
const u8 chip_address = 0x30; // I2C chip address

int main(void)
{
	DemoInitialize();
	DemoRun();
	DemoCleanup();
	return 0;
}

void DemoInitialize()
{
	EnableCaches();
	CMPS2_begin(&myDevice, XPAR_PMODCMPS2_0_AXI_LITE_IIC_BASEADDR, chip_address);
	DemoSleep(10);
	CMPS2_SetSensor(&myDevice);
	DemoSleep(10);
	CMPS2_SetOutputResolution(&myDevice, 0b00);
}

void DemoRun()
{
	//FIXME: data becomes invalid when the board is not face up and flat.

	int deg;
	CMPS2_DataPacket data;

	DemoClearCalibration(&myCalibrationData);

	while(1)
	{
		data = CMPS2_GetData(&myDevice);

		DemoCalibrate(&myDevice, &myCalibrationData, data);

		deg = DemoConvertDegree(&myDevice, myCalibrationData, data, myDeclination);

		xil_printf("Degree: %d;  Y Direction: %s\r\n", deg, DemoGetCardinalDirectionString(deg));

		DemoSleep(100);
	}
}

int DemoConvertDegree(PmodCMPS2 *InstancePtr, CMPS2_CalibrationData calib, CMPS2_DataPacket data, int declination)
{
	int tx, ty;
	int deg;

	if (data.x < calib.mid.x)
		tx = (calib.mid.x - data.x);
	else
		tx = (data.x - calib.mid.x);

	if (data.y < calib.mid.y)
		ty = (calib.mid.y - data.y);
	else
		ty = (data.y - calib.mid.y);

	if (data.x < calib.mid.x)
	{
		if (data.y > calib.mid.y)
			deg = 90 - atan2f(ty,tx) * 180 / 3.14159;
		else
			deg = 90 + atan2f(ty,tx) * 180 / 3.14159;
	}
	else
	{
		if (data.y < calib.mid.y)
			deg = 270 - atan2f(ty,tx) * 180 / 3.14159;
		else
			deg = 270 + atan2f(ty,tx) * 180 / 3.14159;
	}

	deg += declination;

	while (deg >= 360) deg -= 360;
	while (deg < 0) deg += 360;

	return deg;
}

void DemoClearCalibration(CMPS2_CalibrationData *calib)
{
	calib->max.x = 0x8000; // center point of 0x0000 -> 0xFFFF
	calib->max.y = 0x8000;
	calib->max.z = 0x8000;
	calib->min.x = 0x8000;
	calib->min.y = 0x8000;
	calib->min.z = 0x8000;
	calib->mid.x = 0x8000;
	calib->mid.y = 0x8000;
	calib->mid.z = 0x8000;
}

void DemoCalibrate (PmodCMPS2 *InstancePtr, CMPS2_CalibrationData *calib, CMPS2_DataPacket data)
{
	if (data.x > calib->max.x) calib->max.x = data.x;
	if (data.y > calib->max.y) calib->max.y = data.y;
	if (data.z > calib->max.z) calib->max.z = data.z;
	if (data.x < calib->min.x) calib->min.x = data.x;
	if (data.y < calib->min.y) calib->min.y = data.y;
	if (data.z < calib->min.z) calib->min.z = data.z;
	calib->mid.x = (calib->max.x >> 1) + (calib->min.x >> 1); // find the average
	calib->mid.y = (calib->max.y >> 1) + (calib->min.y >> 1);
	calib->mid.z = (calib->max.z >> 1) + (calib->min.z >> 1);
}

char *cardinal[] = {"North", "North East", "East", "South East", "South", "South West", "West", "North West"};
char *DemoGetCardinalDirectionString(int deg)
{
	float fdeg = deg;
	if (fdeg > 337.5) fdeg -= 337.5;
	else fdeg += 22.5;
	fdeg /= 45.0;
	return cardinal[(int)fdeg];
}

void DemoCleanup()
{
	DisableCaches();
}

void DemoSleep(u32 millis)
{
#ifdef __MICROBLAZE__
	MB_Sleep(millis);
#else
	usleep(1000*millis);
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
