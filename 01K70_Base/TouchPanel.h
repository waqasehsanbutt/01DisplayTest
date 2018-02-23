/********************* Includes **********************/
#include "MultiFunctionBus.h"
/********************* Includes **********************/
extern void TouchPanel_Init(void);
extern void EnableTouchInterrupt(void);
extern void DisableTouchInterrupt(void);
extern void AssignTouchInterruptHandler(void (*ptr)(void));
extern void ConfigureSPIForTouch(void);
extern void TouchPanel_Poll(void);
/* EOF */
