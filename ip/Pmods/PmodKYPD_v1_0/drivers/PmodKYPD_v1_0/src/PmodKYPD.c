/************************************************************************/
/*                                                                      */
/* PmodKYPD.c --    Template driver for a Pmod which uses GPIO          */
/*                                                                      */
/************************************************************************/
/*  Author:   Mikel Skreen                                              */
/*  Copyright 2016, Digilent Inc.                                       */
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*  This library contains basic functions to read keypresses from       */
/*  Digilent's PmodKYPD                                                 */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  06/08/2016(MikelS): Created                                         */
/*                                                                      */
/************************************************************************/

/***************************** Include Files ****************************/
#include "PmodKYPD.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/************************** Function Definitions ************************/

/* -------------------------------------------------------------------- */
/***  void KYPB_begin(PmodKYPD* InstancePtr, u32 GPIO_Address)
**
**  Parameters:
**    InstancePtr: A PmodKYPB object to start
**    GPIO_Address: The Base address of the PmodKYPB GPIO
**
**  Return Value:
**    none
**
**  Errors:
**    none
**
**  Description:
**    Initialize the PmodKYPD.
*/
void KYPD_begin(PmodKYPD* InstancePtr, u32 GPIO_Address)
{
  int i;

  //initialize tables
  u32 copyToKeyTable[16] = {3, 15, 14, 13, 11, 10, 9, 7, 6, 5, 12, 8, 4, 0, 1, 2};
  u32 copyToKeyLookUp[16] = {13, 12, 11, 10, 14, 9, 6, 3, 15, 8, 5, 2, 0, 7, 4, 1};
  for(i=0; i<16; i++){
    InstancePtr->keyTable[i] = copyToKeyTable[i];
    InstancePtr->keyLookUp[i] = copyToKeyLookUp[i];
  }

  InstancePtr->GPIO_addr=GPIO_Address;
  //set tri-state register, first 4 pins are col, outputs ,last 4 are rows, inputs
  Xil_Out32(InstancePtr->GPIO_addr+4, 0xF0);//0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
}

/* -------------------------------------------------------------------- */
/***  bool KYPD_getKey(PmodKYPB* InstancePtr, u32 button)
**
**  Parameters:
**    InstancePtr: A PmodKYPD object
**    button: the button to be checked
**
**  Return Value:
**      bool: key pressed
**
**  Errors:
**    none
**
**  Description:
**    This function checks if the given button is pressed. This is accomplished
**    by setting all columns low except for the button's column and reading
**    the rows
*/
bool KYPD_getKey(PmodKYPD* InstancePtr, u32 button)
{
  u32 row = 1 << (InstancePtr->keyTable[button] / 4);
  u32 col = 1 << (InstancePtr->keyTable[button] % 4);
  u32 data;

  data = Xil_In32(InstancePtr->GPIO_addr);
  data = data & ~0xF;
  //set the col we want low and the others high
  //don't have to worry about changing initial col data back
  Xil_Out32(InstancePtr->GPIO_addr, (~col & 0xF)|data);
  data = Xil_In32(InstancePtr->GPIO_addr);

  if((~(data >> 4) & row) != 0)
    return true;

    return false;
}

/* ------------------------------------------------------------------- */
/***  u16 KYPD_getStatus(PmodKYPB* InstancePtr)
**
**  Parameters:
**    InstancePtr: A PmodKYPB object
**
**  Return Value:
**    u16: each key is mapped to a single bit, 0 is pressed, 1 depressed
**
**  Errors:
**    none
**
**  Description:
**    returns the status of all keys
*/
u16 KYPD_getStatus(PmodKYPD* InstancePtr)
{
  u32 i, preserve, data;
  u16 col, colData = 0x0000;

  //preserve the initial state
  preserve = Xil_In32(InstancePtr->GPIO_addr);

  for(i=0; i<4; i++)
  {
    col = 1 << i;

    Xil_Out32(InstancePtr->GPIO_addr, ~col);

    data = Xil_In32(InstancePtr->GPIO_addr);
    data = data >> 4;
    data = data & 0xF;

    colData = colData | (data << (i*4));
  }
  Xil_Out32(InstancePtr->GPIO_addr, preserve);
  return colData;
}
