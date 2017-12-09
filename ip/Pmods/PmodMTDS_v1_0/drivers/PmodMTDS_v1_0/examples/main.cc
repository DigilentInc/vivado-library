/******************************************************************************/
/*                                                                            */
/* main.cc -- Main file for running PmodMTDS example programs                 */
/*                                                                            */
/******************************************************************************/
/* Author: Thomas Kappenman                                                   */
/* Copyright 2016, Digilent Inc. All rights reserved.                         */
/******************************************************************************/
/* Module Description:                                                        */
/*                                                                            */
/* Contains main function for running PmodMTDS example programs               */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    09/20/2016(tom21091): Created                                           */
/*    12/09/2017(atangzwj): Validated for Vivado 2016.4                       */
/*                                                                            */
/******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"

extern void setup();
extern void loop();

void EnableCaches();
void DisableCaches();

int main() {
   EnableCaches();
   setup();
   while (1) {
      loop();
   }
   DisableCaches();
   return 0;
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#endif
}
