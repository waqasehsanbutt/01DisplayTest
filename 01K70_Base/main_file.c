#include <stdint.h>
#include "LCDLatchingHandler.h"
#include "Delays.h"
#include "ILI9488_Display.h" 
#include "MultiFunctionBus.h"
#include "TouchPanel.h"
#include "SDCard.h"
#include "FontChip.h"
#include "TFTFlash.h"

int i = 0;
volatile char update = 0;

void TouchHandler(void)
{
  LCDLatchingHandler_Reset();
}

#if defined (__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
	LCDLatchingHandler_Init();
	ILI9488_Init();
	MultiFunctionBus_Init();
	TouchPanel_Init();
	SDCard_Init();
	FontChip_Init();
	TFTFlash_Init();
	AssignTouchInterruptHandler(TouchHandler);
	LCDLatchingHandler_BeforeDisplay();     
	EnableTouchInterrupt();
	CalibrateTouch();
//	ClearScreen(0xffff);
//	ShowString(30, 5, "HELLO! PLEASE TOUCH ME!",0,0xffff);
//   ShowString(80, 18, "TP TEST!",0,0xffff);
//	ShowFilledSquare(302, 9, 8, 0);
//	ShowFilledSquare(10, 236, 8, 0);
//	ShowFilledSquare(302, 462, 8, 0);
	LCDLactchingHandler_AfterDisplay();
	while (1)
		TouchPanel_Poll();
}

/* EOF */
