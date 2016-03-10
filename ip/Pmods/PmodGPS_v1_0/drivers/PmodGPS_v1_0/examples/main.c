#include "xparameters.h"
#include "xil_types.h"
#include "PmodGPS.h"
#include "xil_io.h"


void DemoInitialize();
void DemoRun();

typedef enum{
  RESTART,
  NOTFIXED,
  FIXED
}STATE;

NMEA mode;
STATE state=RESTART;


int main(void)
{
	//Xil_ICacheEnable();
	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{
	GPS_begin(XPAR_PMODGPS_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODGPS_0_AXI_LITE_UART_BASEADDR);
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
		      mode = GPS_getData();//Receive data from GPS
		      if (mode == GGA){//If GGAdata was received
		        xil_printf("Number of Satellites: %d\n\r", getNumSats());
		        if (isFixed()){//When it is fixed, continue
		          state=FIXED;
		        }
		      }
		        break;
		    case(FIXED):
		        if(isFixed()){//Update data while there is a position fix
		          mode = GPS_getData();
		          if (mode == GGA)//If GGAdata was received
		          {
		        	  xil_printf("Latitude: %s\n\r",getLatitude());
		        	  xil_printf("Longitude: %s\n\r",getLongitude());
		        	  xil_printf("Altitude: %s\n\r",getAltitudeString());
		        	  xil_printf("Number of Satellites: %d\n\n\r",getNumSats());
		          }
		        }
		        else {
		          state=RESTART;//If PFI = 0, re-enter connecting state
		        }
		      break;
		  }

	}
}
