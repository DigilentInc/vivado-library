#include "xparameters.h"
#include "platform.h"
#include "xil_cache.h"
#include "PmodKYPD.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif

void DemoInitialize();
void DemoRun();

PmodKYPD myDevice;

int main(void)
{
    DemoInitialize();
    DemoRun();

    cleanup_platform();
    return 0;
}

void DemoInitialize()
{
    init_platform();
    KYPD_begin(&myDevice, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
}


void DemoRun()
{
    u32 count = 0;
    u16 kypdStateNew, kypdStateOld, kypdIndex = 0;

    xil_printf("starting...\n\r");

    kypdStateOld = KYPD_getStatus(&myDevice);

    while(1)
    {
        kypdStateNew = KYPD_getStatus(&myDevice);

        for(count = 0; count < 16; count++)
        {
            kypdIndex = 1 << count;
            if((kypdStateNew & kypdIndex) == 0)
            {
                    //check if rising edge
                    if(((kypdStateNew & kypdIndex) == 0) && ((kypdStateOld & kypdIndex) != 0)){
                        xil_printf("%x pressed\n\r", myDevice.keyLookUp[count]);
                    }
                    //debounce
#ifdef XPAR_MICROBLAZE_ID
                    MB_Sleep(5);
#else
                    usleep(5000);
#endif
            }
        }

        kypdStateOld = kypdStateNew;
    }
}
