/********************************************************************************/
/*                                                                              */
/*  MyDispTest1.cc  --  MyDisp Library Test Function                           */
/*                                                                              */
/********************************************************************************/
/*  Author: 	Gene Apperson	                                                */
/*  Copyright 2016, Digilent Inc. All rights reserved.                          */
/********************************************************************************/
/*  Module Description:                                                         */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
/*  Revision History:                                                           */
/*                                                                              */
/*  2016-03-28(GeneA): created                                                  */
/*                                                                              */
/********************************************************************************/


/* ------------------------------------------------------------ */
/*                Include File Definitions                      */
/* ------------------------------------------------------------ */

#include <MyDisp.h>
#include <mtds.h>
#include <string.h>
#include <stdint.h>
#include "xil_cache.h"

/* ------------------------------------------------------------ */
/*                Local Type Definitions                        */
/* ------------------------------------------------------------ */

#define TEST_CLIP
//#define TEST_STAT_PIN

/* ------------------------------------------------------------ */
/*                Global Variables                              */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*                Local Variables                               */
/* ------------------------------------------------------------ */

int  itstMax = 4;

int  itstCur;

uint32_t  msCur;
uint32_t  msDisp;

/* ------------------------------------------------------------ */
/*                Forward Declarations                          */
/* ------------------------------------------------------------ */

extern int millis();
extern void delay(int milliseconds);

void MyDispTest1();
void MyDispTest2();
void MyDispTest3();
void MyDispTest4();

/* ------------------------------------------------------------ */
/*                Procedure Definitions                         */
/* ------------------------------------------------------------ */
/***  setup()
**
**  Parameters:
**	none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Arduino/MPIDE sketch initialization function.
*/

int main() {
  bool    fStat;
  Xil_DCacheEnable();
  Xil_ICacheEnable();
  //Serial.begin(115200);

  itstCur = 1;
  msDisp = millis() + 100000;

  while (true) {
    fStat = mydisp.begin();
    if (fStat) {
      //Serial.println("mydisp.begin() succeeded");
      break;
    }
    else {
      //Serial.println("mydisp.begin() failed");
      delay(1000);
    }
  }


/* ------------------------------------------------------------ */
/***	loop()
**
**	Parameters:
**		none
**
**	Return Values:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Arduino/MPIDE main sketch function
*/

while(1) {

  msCur = millis();
  if ((msCur - msDisp) > 3000) {
    msDisp = msCur;

    mydisp.clearDisplay(clrBlack);

    switch(itstCur) {
      case 1:
        MyDispTest1();
        break;

       case 2:
         MyDispTest2();
         break;

       case 3:
         MyDispTest3();
         break;

       case 4:
         MyDispTest4();
         break;
    }
    itstCur += 1;
    if (itstCur > itstMax) {
      itstCur = 1;
    }
  }
}
  return 1;
}

/* ------------------------------------------------------------ */
/*			Test Functions				*/
/* ------------------------------------------------------------ */
/***  MyDispTest1()
**
**  Parameters:
**
**  Return Values:
**
**  Errors:
**
**  Description:
**    Test all of the graphics drawing primitives in the MyDisp library.
*/

void MyDispTest1() {
  int         xco;
  int         yco;
  uint16_t    clr;

  mydisp.setForeground(clrWhite);
  mydisp.setBackground(clrBlack);
  mydisp.setPen(penSolid);
  mydisp.setTransparency(false);

  /* Test setPixel and getPixel
  */
  for (xco = 10; xco < 230; xco++) {
    mydisp.setPixel(xco, 20, clrRed);
  }
  clr = mydisp.getPixel(10, 20);
  for (xco = 10; xco < 230; xco++) {
    mydisp.setPixel(xco, 30, clr);
  }

  /* Test drawLine
  */
  mydisp.drawLine(10, 30, 230, 100);
  mydisp.drawLine(230, 30, 10, 100);
  mydisp.setForeground(clrBlue);
  mydisp.drawLine(10, 30, 10, 100);
  mydisp.drawLine(230, 30, 230, 100);
  mydisp.setForeground(clrGreen);
  mydisp.drawLine(10, 100, 230, 100);

  /* Test drawArc
  */
  mydisp.setPen(penSolid);
  mydisp.setForeground(clrDkYellow);
  mydisp.drawArc(10, 30, 230, 100, 10, 100, 230, 100);

  /* Test drawRectangle
  */
  mydisp.setPen(penDash);
  mydisp.setForeground(clrDkYellow);
  mydisp.setBackground(clrDkCyan);
  mydisp.drawRectangle(false, 20, 120, 220, 190);
  mydisp.setPen(penNull);
  mydisp.setForeground(clrDkGray);
  mydisp.drawRectangle(true, 25, 125, 215, 185);

  /* Test drawEllipse
  */
  mydisp.setPen(penDash);
  mydisp.setForeground(clrDkRed);
  mydisp.setBackground(clrDkYellow);
  mydisp.drawEllipse(false, 20, 200, 220, 300);
  mydisp.setPen(penNull);
  mydisp.setForeground(clrDkGray);
  mydisp.drawEllipse(true, 25, 205, 215, 295);

  /* Test drawText
  */
  mydisp.setPen(penSolid);
  mydisp.setForeground(clrGreen);
  mydisp.setBackground(clrBlack);
  mydisp.drawText("This is some text", 50, 150);

}

/* ------------------------------------------------------------ */
/***  MyDispTest2()
**
**  Parameters:
**
**  Return Values:
**
**  Errors:
**
**  Description:
**    Test the drawImage() function.
*/

void MyDispTest2() {

  mydisp.drawImage("IMG_0100.bmp", 10, 10);
  mydisp.drawImage("IMG_0101.bmp", 10, 110);
  mydisp.drawImage("IMG_0102.bmp", 10, 210);

}

/* ------------------------------------------------------------ */
/***  MyDispTest3()
**
**  Parameters:
**
**  Return Values:
**
**  Errors:
**
**  Description:
**    Another test of the drawImage() function.
*/

void MyDispTest3() {

  mydisp.drawImage("IMG_LOGO.bmp", 10, 10);

}

/* ------------------------------------------------------------ */
/***  MyDispTest4()
**
**  Parameters:
**
**  Return Values:
**
**  Errors:
**
**  Description:
**    Test creating and rendering buttons.
*/

void MyDispTest4() {

  if (mydisp.createButton(0, "IMG_0001.BMP", "IMG_0002.BMP", 10, 10)) {
    //Serial.println("mydisp.CreateButton(0) succeeded");
  }
  else {
    //Serial.println("mydisp.createButton(0) failed");
  }

  if (mydisp.createButton(1, "IMG_0010.BMP", 0, 10, 100)) {
    //Serial.println("mydisp.createButton(1) succeeded");
  }
  else {
    //Serial.println("mydisp.createButton(1) failed");
  }

  mydisp.drawImage("IMG_LOGO.BMP", 0, 0);

  mydisp.enableButton(0, true);
  mydisp.enableButton(1, true);

  mydisp.drawButton(0, BUTTON_UP);
  mydisp.drawButton(1, BUTTON_UP);

  delay(3000);

  mydisp.drawButton(0, BUTTON_DOWN);
  mydisp.drawButton(1, BUTTON_DOWN);

  delay(3000);

  mydisp.drawButton(0, BUTTON_UP);
  mydisp.drawButton(1, BUTTON_UP);

  delay(2000);

  mydisp.drawButton(0, BUTTON_ERASE);
  mydisp.drawButton(1, BUTTON_ERASE);

  delay(2000);

 /* Clean up the buttons before we get out of this function.
  */
  mydisp.enableButton(0, false);
  mydisp.enableButton(1, false);
  mydisp.deleteButton(0);
  mydisp.deleteButton(1);

}

/* ------------------------------------------------------------ */
/***	ProcName
**
**	Parameters:
**
**	Return Values:
**
**	Errors:
**
**	Description:
**
*/

/* ------------------------------------------------------------ */

/********************************************************************************/