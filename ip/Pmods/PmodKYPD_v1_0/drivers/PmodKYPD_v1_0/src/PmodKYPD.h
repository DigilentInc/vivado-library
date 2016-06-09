/************************************************************************/
/*                                    */
/* PmodKYPD.c --    Template driver for a Pmod which uses GPIO    */
/*                                    */
/************************************************************************/
/*  Author:   Mikel Skreen                    */
/*  Copyright 2016, Digilent Inc.                   */
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
/*  File Description:                         */
/*                                    */
/*  This file contains a template that you can use to create a library  */
/*  for the PmodKYPD_H                               */
/*                                    */
/************************************************************************/
/*  Revision History:                         */
/*                                    */
/*  06/08/2016(MikelS): Created                     */
/*                                    */
/************************************************************************/


#ifndef PmodKYPD_H
#define PmodKYPD_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

/* ------------------------------------------------------------ */
/*          Definitions                 */
/* ------------------------------------------------------------ */
#define bool u8
#define true 1
#define false 0


typedef struct PmodKYPD{
  u32 GPIO_addr;
  u32 keyTable[16];
  u32 keyLookUp[16];
}PmodKYPD;

#define LOW

  void KYPD_begin(PmodKYPD* InstancePtr, u32 GPIO_Address);
  bool KYPD_getKey(PmodKYPD* InstancePtr, u32 button);
  u16 KYPD_getStatus(PmodKYPD* InstancePtr);


#endif // PmodKYPD_H
