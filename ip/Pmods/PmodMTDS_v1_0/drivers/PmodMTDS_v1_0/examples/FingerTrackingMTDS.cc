/*
 * Empty C++ Application
 */

#include<stdint.h>
#include "mtds.h"
#include "MtdsCore.h"
#include "xil_cache.h"
//#define  PRINT_MSG

bool fRun = true;
uint32_t  msDisp;
RCT      rctDisp;
int      itstCur = 1;

uint32_t  msStart;
uint32_t  msCur;
bool      fLedState;

int      pinBtnSnd;
bool     fSndByte;
uint8_t  bSnd;
uint8_t  bRcv;

int    pinBtn;
int    pinLedBlink;
int    pinLedBtn;
int    pinLedDisp;

int  itstMax = 17;
HDS   hdsDisp;
HDS   hdsFng1;
HBMP  hbmpFng1;
HDS   hdsFng2;
HBMP  hbmpFng2;

int16_t xcoPrv1;
int16_t ycoPrv1;
int16_t xcoPrv2;
int16_t ycoPrv2;

void WaitBtn();

void MtdsTest5();

int main()
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	bool  fStat;


	#if defined(DEAD)
	  pinBtn = PIN_BTN1;
	  pinMode(pinBtn, INPUT);
	  pinLedBtn = PIN_LED4;
	  pinMode(pinLedBtn, OUTPUT);
	#endif


	  //msStart = millis();

	  #if defined(DEAD)
	  pinBtnSnd = PIN_BTN2;
	  pinMode(pinBtnSnd, INPUT);
	  fSndByte = false;
	  #endif

	 // msDisp = millis() + 100000;

	  rctDisp.xcoLeft = 0;
	  rctDisp.ycoTop = 0;
	  rctDisp.xcoRight = 239;
	  rctDisp.ycoBottom = 319;

	  while(true) {
	    fStat = mtds.begin(pinMtdsSelStd, frqMtdsSpiDefault);
	    if (fStat) {
	      //xil_printf("mtds.begin() succeeded\r\n");
	      break;
	    }
	    else {
	      //xil_printf("mtds.begin() failed\r\n");
	      //delay(1000);
	    }
	  }

	  mtds.SetDisplayOrientation(dsoLandscape);

	   hdsDisp = mtds.GetDisplayDs();

	   hdsFng1 = mtds.GetDs();
	   hbmpFng1 = mtds.CreateBitmap(40, 40, 16);
	   mtds.SetDrawingSurface(hdsFng1, hbmpFng1);

	   mtds.SetDrwRop(hdsFng1, drwCopyPen);
	   mtds.SetPen(hdsFng1, penSolid);

	   mtds.SetFgColor(hdsFng1, clrGreen);
	   mtds.MoveTo(hdsFng1,  0,  0);
	   mtds.LineTo(hdsFng1, 39, 39);
	   mtds.MoveTo(hdsFng1, 39,  0);
	   mtds.LineTo(hdsFng1,  0, 39);

	   mtds.SetFgColor(hdsFng1, clrRed);
	   mtds.MoveTo(hdsFng1,  0,  0);
	   mtds.LineTo(hdsFng1, 39,  0);
	   mtds.LineTo(hdsFng1, 39, 39);
	   mtds.LineTo(hdsFng1,  0, 39);
	   mtds.LineTo(hdsFng1,  0,  0);

	   hdsFng2 = mtds.GetDs();
	   hbmpFng2 = mtds.CreateBitmap(40, 40, 16);
	   mtds.SetDrawingSurface(hdsFng2, hbmpFng2);

	   mtds.SetDrwRop(hdsFng2, drwCopyPen);
	   mtds.SetPen(hdsFng2, penSolid);
	   mtds.SetBrush(hdsFng2, hbrNull);

	   mtds.SetFgColor(hdsFng2, clrRed);
	   mtds.Ellipse(hdsFng2, 0, 0, 39, 39);
	   mtds.SetFgColor(hdsFng2, clrWhite);
	   mtds.MoveTo(hdsFng2, 19, 0);
	   mtds.LineTo(hdsFng2, 19, 39);
	   mtds.MoveTo(hdsFng2,  0, 19);
	   mtds.LineTo(hdsFng2, 39, 19);
	   HDS     hds;
	   		  MTCH    mtch;
	while(1){


//		  msCur = millis();
//		  if ((msCur - msStart) > 500) {
//		    msStart = msCur;
//		    fLedState = !fLedState;
//
//		  }
//
//		  if ((msCur - msDisp) > 3000) {
//		    msDisp = msCur;
//
//		    fRun = !fRun;
//
//		    //MtdsTest5();
//		  }

		  if (mtds.GetMsgStatus() != 0) {
		    mtds.GetMsg((MEVT *)&mtch);
		#if defined(PRINT_MSG)
		    Serial.println("----------------");
		    Serial.print("  tms = ");
		    Serial.print(mtch.tms, DEC);
		    Serial.print("  hwin = ");
		    Serial.print(mtch.hwin, HEX);
		    Serial.print("  msg = ");
		    Serial.println(mtch.msg, HEX);
		    Serial.print("  xco = ");
		    Serial.print(mtch.xco, DEC);
		    Serial.print("  yco = ");
		    Serial.print(mtch.yco, DEC);
		    Serial.print("  wgt = ");
		    Serial.print(mtch.wgt, DEC);
		    Serial.print("  spd = ");
		    Serial.println(mtch.spd, DEC);
		#endif
		    if ((mtch.msg >= msgFingerFirst) && (mtch.msg <= msgFingerLast)) {
		      switch (mtch.msg) {
		        case msgFinger1Down:
		          xcoPrv1 = mtch.xco - 20;
		          ycoPrv1 = mtch.yco - 40;
		          mtds.BitBlt(hdsDisp, xcoPrv1, ycoPrv1, 40, 40, hdsFng1, 0, 0, ropSrcCopy);
		          break;

		        case msgFinger1Move:
		          mtds.BitBlt(hdsDisp, xcoPrv1, ycoPrv1, 40, 40, hdsFng1, 0, 0, ropSrcInvert);
		          xcoPrv1 = mtch.xco - 20;
		          ycoPrv1 = mtch.yco - 40;
		          mtds.BitBlt(hdsDisp, xcoPrv1, ycoPrv1, 40, 40, hdsFng1, 0, 0, ropSrcCopy);
		          break;

		        case msgFinger1Up:
		          mtds.BitBlt(hdsDisp, xcoPrv1, ycoPrv1, 40, 40, hdsFng1, 0, 0, ropSrcInvert);
		          break;

		        case msgFinger2Down:
		          xcoPrv2 = mtch.xco - 20;
		          ycoPrv2 = mtch.yco - 40;
		          mtds.BitBlt(hdsDisp, xcoPrv2, ycoPrv2, 40, 40, hdsFng2, 0, 0, ropSrcCopy);
		          break;

		        case msgFinger2Move:
		          mtds.BitBlt(hdsDisp, xcoPrv2, ycoPrv2, 40, 40, hdsFng2, 0, 0, ropSrcInvert);
		          xcoPrv2 = mtch.xco - 20;
		          ycoPrv2 = mtch.yco - 40;
		          mtds.BitBlt(hdsDisp, xcoPrv2, ycoPrv2, 40, 40, hdsFng2, 0, 0, ropSrcCopy);
		          break;

		        case msgFinger2Up:
		          mtds.BitBlt(hdsDisp, xcoPrv2, ycoPrv2, 40, 40, hdsFng2, 0, 0, ropSrcInvert);
		          break;
		      }
		    }
		  }
	}


	return 0;
}
/* ------------------------------------------------------------ */
/*				GDI Test Functions								*/
/* ------------------------------------------------------------ */
/***	MtdsTest1()
**
**	Parameters:
**
**	Return Values:
**
**	Errors:
**
**	Description:
**		Test simple MTDS graphics functions. Set pixel, drawing lines and
**		drawing polylines.
*/

void MtdsTest5() {
  HDS 	hdsDisp;
  HDS 	hdsTest;
  HBMP	hbmpTest;

  hdsDisp = mtds.GetDisplayDs();
  hdsTest = mtds.GetDs();
  hbmpTest = mtds.CreateBitmap(40, 40, 16);
  if (hbmpTest == 0) {
    //xil_printf("MtdsTest5: CreateBitmap failed\r\n");
  }
  mtds.SetDrawingSurface(hdsTest, hbmpTest);

  mtds.SetDrwRop(hdsTest, drwCopyPen);
  mtds.SetPen(hdsTest, penSolid);

  mtds.SetFgColor(hdsTest, clrGreen);
  mtds.MoveTo(hdsTest,  0,  0);
  mtds.LineTo(hdsTest, 39, 39);
  mtds.MoveTo(hdsTest, 39,  0);
  mtds.LineTo(hdsTest,  0, 39);

  mtds.SetFgColor(hdsTest, clrRed);
  mtds.MoveTo(hdsTest,  0,  0);
  mtds.LineTo(hdsTest, 39,  0);
  mtds.LineTo(hdsTest, 39, 39);
  mtds.LineTo(hdsTest,  0, 39);
  mtds.LineTo(hdsTest,  0,  0);

  mtds.BitBlt(hdsDisp,  10,  10, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 100,  10, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 190,  10, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp,  10, 140, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 100, 140, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 190, 140, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp,  10, 270, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 100, 270, 40, 40, hdsTest, 0, 0, ropSrcCopy);
  mtds.BitBlt(hdsDisp, 190, 270, 40, 40, hdsTest, 0, 0, ropSrcCopy);

  mtds.DestroyBitmap(hbmpTest);
  mtds.ReleaseDs(hdsDisp);
  mtds.ReleaseDs(hdsTest);

}
