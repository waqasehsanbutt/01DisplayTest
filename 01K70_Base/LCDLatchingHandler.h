/********************* Includes **********************/
#include "GeneralHeader.h"
#include "LPTimer_LDD.h"
/********************* Includes **********************/

/********************* Macros **********************/
#define LCDLatching_TMR												LPTMR0
#define SLEEP_TIME													(1200)

#define LCDLatchingHandler_Disable()                     LPTimer_Disable(LCDLatching_TMR)
#define LCDLatchingHandler_Enable()                      LPTimer_Enable(LCDLatching_TMR)
#define LCDLatchingHandler_Reset()                       LPTimer_Reset(LCDLatching_TMR)
/********************* Macros **********************/

extern void LCDLatchingHandler_Init(void);
extern void LCDLatchingHandler_BeforeDisplay(void);
extern void LCDLactchingHandler_AfterDisplay(void);

/* EOF */

