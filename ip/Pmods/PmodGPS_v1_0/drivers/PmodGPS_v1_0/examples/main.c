#include "xparameters.h"
#include "xil_types.h"
#include "PmodGPS.h"
#include "xil_io.h"
#include "bitmap.h"
#include "xil_cache.h"

void DemoInitialize();
void DemoRun();

typedef enum{
  RESTART,
  NOTFIXED,
  FIXED
}STATE;

NMEA mode;
STATE state=RESTART;
PmodGPS GPS;


int main(void)
{
	Xil_ICacheEnable();

	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{
	GPS_begin(&GPS, XPAR_PMODGPS_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODGPS_0_AXI_LITE_UART_BASEADDR);
}


void DemoRun()
{
	while(1){
		switch (state)
		  {
		    case(RESTART):
		        state=NOTFIXED;
		        break;
		    case(NOTFIXED)://Look for satellites, display how many the GPS is connected to
		      mode = GPS_getData(&GPS);//Receive data from GPS
		      if (mode == GGA){//If GGAdata was received
		        xil_printf("Number of Satellites: %d\n\r", getNumSats(&GPS));
		        if (isFixed(&GPS)){//When it is fixed, continue
		          state=FIXED;
		        }
		      }
		        break;
		    case(FIXED):
		        if(isFixed(&GPS)){//Update data while there is a position fix
		          mode = GPS_getData(&GPS);
		          if (mode == GGA)//If GGAdata was received
		          {
		        	  xil_printf("Latitude: %s\n\r",getLatitude(&GPS));
		        	  xil_printf("Longitude: %s\n\r",getLongitude(&GPS));
		        	  xil_printf("Altitude: %s\n\r",getAltitudeString(&GPS));
		        	  xil_printf("Number of Satellites: %d\n\n\r",getNumSats(&GPS));
		          }
		        }
		        else {
		          state=RESTART;//If PFI = 0, re-enter connecting state
		        }
		      break;
		  }

	}
}
