/*************************************************************************/
/*                                                                       */
/*     RX.c --     PmodCAN Example Projects                              */
/*                                                                       */
/*************************************************************************/
/*     Author: Arthur Browm                                              */
/*                                                                       */
/*************************************************************************/
/*  File Description:                                                    */
/*                                                                       */
/*  This demo receive data through the PmodCAN and send it through uart  */
/*  normal mode function                                                 */
/*                                                                       */
/*************************************************************************/
/*  Revision History:                                                    */
/*                                                                       */
/*            8/30/2017(ArtVVB): Created                                 */
/* 			  9/1/2017(jPeyron): formatted Validated                     */
/*            9/6/2017(jPeyron): Added RX and TX demos                   */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/
/*  Baud Rates:                                                          */
/*                                                                       */
/*  Microblaze: 9600 or what was specified in UARTlite core              */
/*  Zynq: 115200                                                         */
/*                                                                       */
/*************************************************************************/



#include "xparameters.h"
#include "xil_cache.h"

#include "PmodCAN.h"
#ifdef __MICROBLAZE__
#include "microblaze_sleep.h"
#else
#include "sleep.h"
#endif




void DemoInitialize();
void DemoRun();
void DemoCleanup();
void DemoPrintMessage(CAN_Message message);
CAN_Message DemoComposeMessage();
void EnableCaches();
void DisableCaches();

PmodCAN myDevice;

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
    CAN_begin(&myDevice, XPAR_PMODCAN_0_AXI_LITE_GPIO_BASEADDR, XPAR_PMODCAN_0_AXI_LITE_SPI_BASEADDR);
    CAN_Configure(&myDevice, CAN_ModeNormalOperation);
}

void DemoPrintMessage(CAN_Message message)
{
    u8 i;

    xil_printf("message:\r\n");

    xil_printf("    %s Frame\r\n", (message.ide) ? "Extended" : "Standard");
    xil_printf("    ID: %03x\r\n", message.id);

    if (message.ide)
    xil_printf("    EID: %05x\r\n", message.eid);

    if (message.rtr)
    xil_printf("    Remote Transmit Request\r\n");

    else
    xil_printf("    Standard Data Frame\r\n");

    xil_printf("    dlc: %01x\r\n", message.dlc);
    xil_printf("    data:\r\n");

    for (i=0; i<message.dlc; i++)
    xil_printf("        %02x\r\n", message.data[i]);
}

CAN_Message DemoComposeMessage()
{
    CAN_Message message;

    message.id = 0x100;
    message.dlc = 6;
    message.eid = 0x15a;
    message.rtr = 0;
    message.ide = 0;
    message.data[0] = 0x01;
    message.data[1] = 0x02;
    message.data[2] = 0x04;
    message.data[3] = 0x08;
    message.data[4] = 0x10;
    message.data[5] = 0x20;
    message.data[6] = 0x40;
    message.data[7] = 0x80;

    return message;
}

void DemoRun()
{
    CAN_Message RxMessage;
    CAN_RxBuffer target;
    u8 status;
    u8 rx_int_mask;

    xil_printf("Welcome to the Pmod CAN IP Core Loopback Demo\r\n");

    while (1)
    {

        do {
            status = CAN_ReadStatus(&myDevice);
            xil_printf("Waiting to receive\r\n");
        } while ((status & CAN_STATUS_RX0IF_MASK) != 0 && (status & CAN_STATUS_RX1IF_MASK) != 0);

        switch(status & 0x03)
        {
        case 0b01:
        case 0b11:
            xil_printf("fetching message from receive buffer 0\r\n");
            target = CAN_Rx0;
            rx_int_mask = CAN_CANINTF_RX0IF_MASK;
            break;
        case 0b10:
            xil_printf("fetching message from receive buffer 1\r\n");
            target = CAN_Rx1;
            rx_int_mask = CAN_CANINTF_RX1IF_MASK;
            break;
        default:
            xil_printf("Error, message not received\r\n");
            continue;
        }

        CAN_ReceiveMessage(&myDevice, &RxMessage, target);

        CAN_ModifyReg(&myDevice, CAN_CANINTF_REG_ADDR, rx_int_mask, 0);

        xil_printf("received ");
        DemoPrintMessage(RxMessage);

        #ifdef __MICROBLAZE__
            MB_Sleep(1000);
        #else
            sleep(1);
        #endif
    }
}

void DemoCleanup()
{
    CAN_end(&myDevice);
    DisableCaches();
}

void EnableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#endif
}
