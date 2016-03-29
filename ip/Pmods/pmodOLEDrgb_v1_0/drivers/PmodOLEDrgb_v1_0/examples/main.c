/*
 * main.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: tkappenm
 */
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"
//#include "xspi.h"
#include "bitmap.h"
#include "PmodOLEDrgb.h"
void DemoInitialize();
void DemoRun();

PmodOLEDrgb oled;

int main(void)
{
	Xil_ICacheEnable();
	//Xil_DCacheEnable();
	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{
	Oledrgb_begin(&oled, XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);
}


void DemoRun()
{
	int i=0;
	while(1){
		if (i==1){
			DrawBitmap(&oled, 0,0,95,63, (uint8_t*)tommy);
			break;
		}
		else {
			DrawBitmap(&oled, 0, 0, 95,63, (uint8_t*)snake1);
			DrawBitmap(&oled, 0, 0, 95,63, (uint8_t*)snake2);
			DrawBitmap(&oled, 0, 0, 95,63, (uint8_t*)snake3);
			DrawBitmap(&oled, 0, 0, 95,63, (uint8_t*)snake2);
		}
		/*for (i =0; i<96; i++){
			DrawRectangle(&oled, 0,0,i,63,BuildRGB(i,i*3,i*4),true,BuildRGB(i,i*3,i*4));
		}*/
	}
}
