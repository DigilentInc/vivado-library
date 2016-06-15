#include <stdio.h>
#include "platform.h"
#include "PmodGYRO.h"
#include "xil_printf.h"
#ifdef XPAR_MICROBLAZE_ID
#include "microblaze_sleep.h"
#endif

PmodGYRO myDevice;

int main()
{
    DemoInitialize();
    DemoRun();

    GYRO_end(&myDevice);
    cleanup_platform();
    return 0;
}


void DemoInitialize()
{
    init_platform();
    GYRO_begin(&myDevice, XPAR_PMODGYRO_0_AXI_LITE_SPI_BASEADDR, XPAR_PMODGYRO_0_AXI_LITE_GPIO_BASEADDR);

    //Set Threshold Registers
    GYRO_setThsXH(&myDevice, 0x0F);
//    GYRO_setThsYH(&myDevice, 0x0F);
//    GYRO_setThsZH(&myDevice, 0x0F);

    GYRO_enableInt1(&myDevice, INT1_XHIE);    // Threshold interrupt
//    GYRO_disableInt1(&myDevice);
    GYRO_enableInt2(&myDevice, REG3_I2_DRDY); // Data Rdy/FIFO interrupt
//    GYRO_disableInt2(&myDevice);
}

void DemoRun()
{
    int16_t xAxis=0, yAxis=0, zAxis=0;
    int8_t temp=0;
    int trig = 0;

    print("Starting...\n\r");
    while(1)
    {
#ifdef XPAR_MICROBLAZE_ID
        MB_Sleep(1000);
#else
        usleep(500000);
#endif
        if(GYRO_Int1Status(&myDevice) != 0)
        {
            xil_printf("\x1B[2J");
            xil_printf("\x1B[H");
            xil_printf("Int 1 was triggered\n\r");
            trig = 1;
        }
        if(GYRO_Int2Status(&myDevice) != 0)
        {
            if(trig == 1)
                trig = 0;
            else{
                xil_printf("\x1B[2J"); // clear screen
                xil_printf("\x1B[H");  // reset cursor to 0,0
            }
            xil_printf("Int 2 was triggered\n\r\n\r");

            xAxis = GYRO_getX(&myDevice);
            yAxis = GYRO_getY(&myDevice);
            zAxis = GYRO_getZ(&myDevice);
            temp = GYRO_getTemp(&myDevice);

            xil_printf("X Axis: %d\n\rY Axis: %d\n\rZ Axis: %d\n\r\n\r", xAxis, yAxis, zAxis);
            xil_printf("Temperature: %d deg F\n\r", temp);
        }
    }
}
