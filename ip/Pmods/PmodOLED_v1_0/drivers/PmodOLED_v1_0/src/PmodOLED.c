/************************************************************************/
/*                                                                      */
/* PmodOLED.c   --      Basic functions for PmodOLED IP Driver          */
/*                                                                      */
/************************************************************************/
/*  Author:     Thomas Kappenman                                        */
/*  Copyright 2015, Digilent Inc.                                       */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  Contains drivers for use with a PmodOLED                            */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  04/19/2016(TommyK): Created                                         */
/*  06/30/2016(ArtVVB): Updated for PmodOLED IP Drivers                 */
/* 12/15/2016(jPeyron) edited for better use for OnboardOLED in         */
/* as well as inverting the white and black                             */
/*  08/25/2017(ArtVVB):  Validated for Vivado 2015.4                    */
/*                                                                      */
/************************************************************************/

/***************************** Include Files *******************************/

#include "PmodOLED.h"

#ifdef __MICROBLAZE__
    #include "microblaze_sleep.h"
#else
    #include "sleep.h"
#endif

XSpi_Config OLEDConfig =
{
    0,
    0,
    1,
    0,
    1,
    8,
    0,
    0,
    0,
    0,
    0
};

/************************** Function Definitions ***************************/

/* ------------------------------------------------------------ */
/***    void OLED_Begin(PmodOLED* InstancePtr, u32 GPIO_Address, u32 SPI_Address)
**
**  Parameters:
**      InstancePtr: A PmodOLED device to start
**      GPIO_Address: The Base address of the PmodOLED GPIO
**      SPI_Address: The Base address of the PmodOLED SPI
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Initialize the PmodOLED.
*/
void OLED_Begin(PmodOLED* InstancePtr, u32 GPIO_Address, u32 SPI_Address, u8 orientation, u8 invert)
{
    OLED_Init(InstancePtr, GPIO_Address, SPI_Address, orientation, invert);
    /*
    InstancePtr->GPIO_addr=GPIO_Address;
    OLEDConfig.BaseAddress=SPI_Address;
    Xil_Out32(InstancePtr->GPIO_addr+4, 0b1111);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc. 
    OLED_SPIInit(&InstancePtr->OLEDSpi);
    //*/
}
/* ------------------------------------------------------------ */
/***    OLED_End(void)
**
**  Parameters:
**      InstancePtr     - PmodOLED device to stop
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Stops the device
*/
void OLED_End(PmodOLED* InstancePtr) {
    OLED_Term(InstancePtr);
    XSpi_Stop(&InstancePtr->OLEDSpi);
}

/* ------------------------------------------------------------ */
/***    OLED_SPIInit
**
**  Parameters:
**      none
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Initializes the PmodOLED SPI.

*/

int OLED_SPIInit(XSpi *SpiInstancePtr){
    int Status;

    Status = XSpi_CfgInitialize(SpiInstancePtr, &OLEDConfig, OLEDConfig.BaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }



    Status = XSpi_SetOptions(SpiInstancePtr, (XSP_MASTER_OPTION | XSP_CLK_ACTIVE_LOW_OPTION | XSP_CLK_PHASE_1_OPTION) | XSP_MANUAL_SSELECT_OPTION); //Change these based on your SPI device
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    Status = XSpi_SetSlaveSelect(SpiInstancePtr, 1);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Start the SPI driver so that the device is enabled.
     */
    XSpi_Start(SpiInstancePtr);

    /*
     * Disable Global interrupt to use polled mode operation
     */
    XSpi_IntrGlobalDisable(SpiInstancePtr);

    return XST_SUCCESS;
}

/* ------------------------------------------------------------ */
/***    OLED_Readbyte
**
**  Synopsis:
**      byte = OLED_Readbyte(&OLED)
**
**  Parameters:
**      PmodOLED *InstancePtr   - the PmodOLED device to communicate with
**
**  Return Value:
**      u8 byte - Byte read from XSpi
**
**  Errors:
**      none
**
**  Description:
**      Reads SPI
*/

u8 OLED_ReadByte(PmodOLED* InstancePtr){
    u8 byte;
    XSpi_Transfer(&InstancePtr->OLEDSpi, &byte, &byte, 1);
    return byte;
}

/* ------------------------------------------------------------ */
/***    OLED_WriteSPICommand
**
**  Parameters:
**      InstancePtr - PmodOLED device to send to
**      cmd         - Command to send
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Writes a single byte over SPI
**
**
*/
void OLED_WriteByte(PmodOLED* InstancePtr, u8 cmd)
{
    XSpi_Transfer(&InstancePtr->OLEDSpi, &cmd, NULL, 1);
}

/* ------------------------------------------------------------ */
/***    OLED_WriteSpi
**
**  Synopsis:
**      OLED_WriteSpi(&OLEDobj, 0x3A, &bytearray, 5);
**
**  Parameters:
**      PmodOLED *InstancePtr   - the PmodOLED device to communicate with
**      u8 reg          - the starting register to write to
**      u8* wData       - the data to write
**      int nData       - the number of data bytes to write
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Writes the byte array to the chip via SPI. It will write the first byte into the specified register, then the next into the following register until all of the data has been sent.

*/
void OLED_WriteSpi(PmodOLED* InstancePtr, u8 reg, u8 *wData, int nData)
{
    // As requested by documentation, first byte contains:
    //  bit 7 = 0 because is a write operation
    //  bit 6 = 1 if more than one bytes is written, 0 if a single byte is written
    //  bits 5-0 - the address
    u8 bytearray[nData+1];
    bytearray[0] = ((nData>1) ? 0x40: 0) | (reg&0x3F);
    memcpy(&bytearray[1],wData, nData);//Copy write commands over to bytearray
    XSpi_Transfer(&InstancePtr->OLEDSpi, bytearray, 0, nData+1);

}


/* ------------------------------------------------------------ */
/***    OLED_ReadSpi
**
**  Synopsis:
**      OLED_ReadSpi(&OLEDobj, 0x3A, &bytearray, 5);
**
**  Parameters:
**      PmodOLED *InstancePtr   - the PmodOLED device to communicate with
**      u8 reg          - the starting register to read from
**      u8* rData       - the byte array to read into
**      int nData       - the number of data bytes to read
**
**  Return Value:
**      none
**
**  Errors:
**      none
**
**  Description:
**      Reads data in through SPI. It will read the first byte from the starting register, then the next from the following register. Data is stored into rData.
*/
void OLED_ReadSpi(PmodOLED* InstancePtr, u8 reg, u8 *rData, int nData)
{
    // As requested by documentation, first byte contains:
    //  bit 7 = 1 because is a read operation
    //  bit 6 = 1 if more than one bytes is written, 0 if a single byte is written
    //  bits 5-0 - the address
    u8 bytearray[nData+1];

    bytearray[0] = ((nData>1) ? 0xC0: 0x80) | (reg&0x3F);
    XSpi_Transfer(&InstancePtr->OLEDSpi, bytearray, bytearray, nData+1);
    memcpy(rData,&bytearray[1], nData);
}

/* ------------------------------------------------------------ */
/*        SetRegisterBits
**
**        Synopsis:
**              SetRegisterBits(&OLEDobj, OLED_ADR_POWER_CTL, bPowerControlMask, fValue);
**
**        Parameters:
**              PmodOLED *InstancePtr       - the PmodOLED device to communicate with
**              u8 bRegisterAddress     - the address of the register whose bits are set
**              u8 bMask                - the mask indicating which bits are affected
**              u8 fValue                   - 1 if the bits are set or 0 if their bits are reset
**
**        Return Values:
**                void
**
**        Description:
**              This function sets the value of some bits (corresponding to the bMask) of a register
**              (indicated by bRegisterAddress) to 1 or 0 (indicated by fValue).
*/
void OLED_SetRegisterBits(PmodOLED* InstancePtr, u8 reg, u8 mask, u8 fValue)
{
    u8 regval;
    OLED_ReadSpi(InstancePtr, reg, &regval, 1);
    if (fValue)regval |= mask;
    else regval &= ~mask;
    OLED_WriteSpi(InstancePtr, reg, &regval, 1);
}

/* ------------------------------------------------------------ */
/*        GetRegisterBits
**
**        Synopsis:
**              return GetRegisterBits(&OLEDobj, OLED_ADR_BW_RATE, MSK_BW_RATE_RATE);
**
**        Parameters:
**              PmodOLED *InstancePtr   - the PmodOLED device to communicate with
**              u8 bRegisterAddress     - the address of the register whose bits are read
**              u8 bMask                - the mask indicating which bits are read
**
**        Return Values:
**              u8                  - a byte containing only the bits correspoding to the mask.
**
**        Description:
**              Returns a byte containing only the bits from a register (indicated by bRegisterAddress),
**              corresponding to the bMask mask.
*/
u8 OLED_GetRegisterBits(PmodOLED* InstancePtr, u8 bRegisterAddress, u8 bMask)
{
    u8 bRegValue;
    OLED_ReadSpi(InstancePtr, bRegisterAddress, &bRegValue, 1);
    return bRegValue & bMask;
}

/* ------------------------------------------------------------ */
/*  OLED_SetGPIOBits
**
**  Synopsis:
**      return OLED_SetGPIOBits(&OLEDobj, 0xF, 0b1);
**
**  Parameters:
**      InstancePtr       - the PmodOLED device to communicate with
**      bMask             - the mask indicating which bits are affected
**      fValue            - 1 if the bits are set or 0 if their bits are reset
**
**  Return Values:
**      None
**
**  Description:
**      This function sets the values of the Pmod OLED's bottom row pins.
*/
void OLED_SetGPIOBits(PmodOLED* InstancePtr, u8 bMask, u8 fValue)
{
    u8 regval;
    regval = Xil_In32(InstancePtr->GPIO_addr);
    if (fValue)
        regval |= bMask;
    else
        regval &= ~bMask;
    Xil_Out32(InstancePtr->GPIO_addr, regval);
}

/* ------------------------------------------------------------ */
/*  OLED_SetGPIOTristateBits
**
**  Synopsis:
**      return OLED_SetGPIOTristateBits(&OLEDobj, 0xF, 0b1);
**
**  Parameters:
**      InstancePtr       - the PmodOLED device to communicate with
**      bMask             - the mask indicating which bits are affected
**      fValue            - 1 if the bits are set or 0 if their bits are reset
**
**  Return Values:
**      None
**
**  Description:
**      This function sets the direction of the Pmod OLED's bottom row pins
*/
void OLED_SetGPIOTristateBits(PmodOLED* InstancePtr, u8 bMask, u8 fValue)
{
    u8 regval;
    regval = Xil_In32(InstancePtr->GPIO_addr+4);
    if (fValue)
        regval |= bMask;
    else
        regval &= ~bMask;
    Xil_Out32(InstancePtr->GPIO_addr+4, regval);
}

/* ------------------------------------------------------------ */
/*  OLED_Delay
**
**  Synopsis:
**      OLED_Delay(100);
**
**  Parameters:
**      millis: the number of milliseconds to wait
**
**  Return Values:
**      None
**
**  Description:
**      This function delays for the specified number of milliseconds
*/
void OLED_Delay(int millis)
{
#ifdef __MICROBLAZE__
    MB_Sleep(millis);
#else
    usleep(millis*1000);
#endif
}
