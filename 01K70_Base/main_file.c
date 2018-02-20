#include <stdint.h>
#include "MK70F12.h"
#include "Delays.h"
#include "ILI9488_Display.h" 
#include "MultiFunctionBus.h"
#include "TouchPanel.h"

int i = 0;
volatile char update = 0;

void TouchHandler(void)
{
	update = 1;
}

#if defined (__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
   ILI9488_Init();
//	MultiFunctionBus_Init();
	TouchPanel_Init();
	TouchInterruptHandler(TouchHandler);
	EnableTouchInterrupt();
	while(1){
		if(update == 1)
		{
			if(i == 0)
			{
				LCD_FULL(0xf800);
				i = 1;
			}
			else if(i == 1)
			{
				LCD_FULL(0x07e0);
				i = 2;
			} 
			else if(i == 2)
			{
				LCD_FULL(0x001f);
				i = 3;
			}
			else if(i == 3)
			{
				show_picture();
				i = 0;
			}
			update = 0;
			EnableTouchInterrupt();	
		}
	}
}

/* EOF */
