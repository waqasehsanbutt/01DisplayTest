#include "GeneralHeader.h"

extern void ILI9488_Init(void);
extern void LCD_FULL(unsigned int i); 
extern void ClearScreen(unsigned int i);
extern void show_picture(void);
extern void ILI9488_EnterSleep(void); 
extern void ILI9488_ExitSleep(void);
extern void ShowFilledSquare(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
extern void ShowSingleChar(unsigned int x,unsigned int y,unsigned char value,unsigned int dcolor,unsigned int bgcolor);
extern void ShowString(unsigned int x,unsigned int y,unsigned char *str,unsigned int dcolor,unsigned int bgcolor)	;
/* EOF */
