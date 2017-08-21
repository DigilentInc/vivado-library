/************************************************************************/
/*                                                                      */
/* PmodKYPD.c --    Driver for the Pmod Keypad				            */
/*                                                                      */
/************************************************************************/
/*  Author:   Mikel Skreen                                              */
/*  Copyright 2016, Digilent Inc.                                       */
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This library contains basic functions to read the states of the		*/
/*  keys of the Digilent's PmodKYPD                                     */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/08/2016(MikelS): Created                                         */
/*  08/17/2017(artvvb): Validated for Vivado 2015.4						*/
/*                                                                      */
/************************************************************************/

/***************************** Include Files ****************************/

#include "PmodKYPD.h"

/************************** Function Definitions ************************/

/* -------------------------------------------------------------------- */
/***  void KYPD_begin(PmodKYPD* InstancePtr, u32 GPIO_Address)
**
**  Parameters:
**    InstancePtr: A PmodKYPD object to start
**    GPIO_Address: The Base address of the PmodKYPD GPIO
**
**  Return Value:
**    none
**
**  Description:
**    Initialize the PmodKYPD driver device
*/
void KYPD_begin(PmodKYPD* InstancePtr, u32 GPIO_Address)
{
    InstancePtr->GPIO_addr=GPIO_Address;
    //set tri-state register, first 4 pins are column outputs, last 4 are row inputs
    Xil_Out32(InstancePtr->GPIO_addr+4, 0xF0);
}

/* -------------------------------------------------------------------- */
/***  u8 KYPD_getKey(PmodKYPD* InstancePtr, u32 button)
**
**  Parameters:
**    InstancePtr: A PmodKYPD object
**    button: the index of the button to be checked
**
**  Return Value:
**      1 if the key is pressed, 0 otherwise
**
**  Errors:
**    none
**
**  Description:
**    This function checks if the given button is pressed. This is accomplished
**    by setting all columns low except for the button's column and reading
**    the rows
*/
u8 KYPD_getKey(PmodKYPD* InstancePtr, u8 button)
{
    u32 row_mask = 1 << (button / 4);
    u32 col_mask = 1 << (button % 4);
    u32 data;

    //set the column we want to check low and the others high
    Xil_Out32(InstancePtr->GPIO_addr, (~col_mask) & 0xF);

    // retrieve row data
    data = ~(Xil_In32(InstancePtr->GPIO_addr) >> 4);

    if((data & row_mask) != 0)
        return 0;
    else
        return 1;
}

/* ------------------------------------------------------------------- */
/***  u16 KYPD_getAllKeys(PmodKYPD* InstancePtr)
**
**  Parameters:
**    InstancePtr: A PmodKYPD object
**
**  Return Value:
**    each key is mapped to a single bit, 0 is pressed, 1 depressed
**
**  Description:
**    retrieves the status of all keys
*/
u16 KYPD_getAllKeys(PmodKYPD* InstancePtr)
{
    u32 i, row, col;
    u16 state = 0;

    for(i=0; i<4; i++)
    {
	    col = 1 << i;

	    //set the column we want to check low and the others high
	    Xil_Out32(InstancePtr->GPIO_addr, ~col);

	    // capture row data and place in the first 4 bits of the row variable
	    row = (Xil_In32(InstancePtr->GPIO_addr) >> 4) & 0xF;

	    // copy row data into its slot in the state
	    state = state | (row << (i*4));
    }

    return ~state;
}
