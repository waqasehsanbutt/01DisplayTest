// XPT2046A_calib.h
/*  Three Point Calibration developed from
        A http://www.ti.com/lit/an/slyt277/slyt277.pdf
        B http://www.embedded.com/design/system-integration/4023968/How-To-Calibrate-Touch-Screens
    Relies on struct definitions in XPT2046A.h but these could be created locally if needed
    Calculations use long integers. This works without overflow with 240x320 TFT but 64 bit
    might be needed if screen is larger as noted in "B". Could also be implemented with floats.
    Algorithm is distributed using individual parameter functions to avoid register spill
    issues during calculations.
*/

/********************* Includes **********************/
#include "TouchPanelCalibration.h"
#include <string.h>
#include <stdlib.h>
/********************* Includes **********************/

/********************* Variables **********************/
CAL_PARAMS_PTR cal = NULL;
/********************* Variables **********************/

long _makeAn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return ((d[0].x - d[2].x) * (t[1].y - t[2].y)) -
        ((d[1].x - d[2].x) * (t[0].y - t[2].y));
}
 
long _makeBn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return ((t[0].x - t[2].x) * (d[1].x - d[2].x)) -
        ((d[0].x - d[2].x) * (t[1].x - t[2].x));
}
 
long _makeCn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return (t[2].x * d[1].x - t[1].x * d[2].x) * t[0].y +
        (t[0].x * d[2].x - t[2].x * d[0].x) * t[1].y +
        (t[1].x * d[0].x - t[0].x * d[1].x) * t[2].y;
}
 
long _makeDn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return ((d[0].y - d[2].y) * (t[1].y - t[2].y)) -
        ((d[1].y - d[2].y) * (t[0].y - t[2].y));
}
 
long _makeEn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return ((t[0].x - t[2].x) * (d[1].y - d[2].y)) -
        ((d[0].y - d[2].y) * (t[1].x - t[2].x));
}
 
long _makeFn(POINT_2D_PTR d, POINT_2D_PTR t)
{
    return (t[2].x * d[1].y - t[1].x * d[2].y) * t[0].y +
        (t[0].x * d[2].y - t[2].x * d[0].y) * t[1].y +
        (t[1].x * d[0].y - t[0].x * d[1].y) * t[2].y;
}

bool MakeCalibrationParams(POINT_2D_PTR pDisplayPoints, POINT_2D_PTR pTouchPoints, CAL_PARAMS_PTR cal)
{
    /* Touchpoints = Touchpad; ScreenPoints = TFT */
 
    cal->V = ((pTouchPoints[0].x - pTouchPoints[2].x) * (pTouchPoints[1].y - pTouchPoints[2].y)) -
        ((pTouchPoints[1].x - pTouchPoints[2].x) * (pTouchPoints[0].y - pTouchPoints[2].y));
 
 
    if (cal->V == 0) 
		 return false;
    cal->An = _makeAn(pDisplayPoints, pTouchPoints);
    cal->Bn = _makeBn(pDisplayPoints, pTouchPoints);
    cal->Cn = _makeCn(pDisplayPoints, pTouchPoints);
    cal->Dn = _makeDn(pDisplayPoints, pTouchPoints);
    cal->En = _makeEn(pDisplayPoints, pTouchPoints);
    cal->Fn = _makeFn(pDisplayPoints, pTouchPoints);
    return true;
}
 
void TouchPanelCalibration_Init(void)
{
	cal = (CAL_PARAMS_PTR)malloc(sizeof(CAL_PARAMS_STRUCT));
	// set initial values
}


/* EOF */
