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
	ClearScreen(0xffff);
	ShowFilledSquare(0, 0, 120, 0);
	LCDLactchingHandler_AfterDisplay();
	EnableTouchInterrupt();
	while (1);
}

/* EOF */
