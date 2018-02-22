/********************* Includes **********************/
#include "LCDLatchingHandler.h"
#include "ILI9488_Display.h"
/********************* Includes **********************/

/********************* Variables **********************/
volatile bool isInSleepMode = false;
/********************* Variables **********************/
static inline void LCDFreeHandler(void)
{
	isInSleepMode = true;
	ILI9488_EnterSleep();
	LCDLatchingHandler_Disable();
}

void LCDLatchingHandler_Init(void)
{
	LPTimer_DefaultInit(LCDLatching_TMR, SLEEP_TIME);
	LPTimer_SetCallBack(LCDFreeHandler);
	LPTimer_SetForInterrupt(LCDLatching_TMR);
}

void LCDLatchingHandler_BeforeDisplay(void)
{
	LCDLatchingHandler_Disable();
	if (isInSleepMode == true)
	{
		ILI9488_ExitSleep();
		isInSleepMode = false;
	}
}

void LCDLactchingHandler_AfterDisplay(void)
{
	LCDLatchingHandler_Enable();
}

/* EOF */
