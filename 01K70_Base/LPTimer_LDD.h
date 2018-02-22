/**************** Includes ****************/
#include "MK70F12.h"
/**************** Includes ****************/
#define LPTimer_Enable(LPTimer)     		LPTimer->CSR |= LPTMR_CSR_TEN_MASK; 
#define LPTimer_Disable(LPTimer)     		LPTimer->CSR &= ~(LPTMR_CSR_TEN_MASK);   
#define LPTimer_Reset(LPTimer)     			LPTimer_Disable(LPTimer) \
														LPTimer_Enable(LPTimer)
/**************** Macros ****************/

/**************** Macros ****************/
// Initialize LPTimer Default
extern void LPTimer_DefaultInit(LPTMR_Type * LPTimer, uint16_t secs);
// Set Timer for Interrupt
extern void LPTimer_SetForInterrupt(LPTMR_Type * LPTimer);
// Set call back method
extern void LPTimer_SetCallBack(void (*callback)(void));

/* EOF */
