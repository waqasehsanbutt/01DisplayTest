/**************** Includes ****************/
#include "GeneralHeader.h"
#include "LPTimer_LDD.h"
#include "Types.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/**************** Includes ****************/

/**************** Macros ****************/

/**************** Macros ****************/

/************** Variables ***************/
static void (*callbackPtr)(void) = NULL;
/************** Variables ***************/
// Initialize LPTimer Default
void LPTimer_DefaultInit(LPTMR_Type * LPTimer, uint16_t secs)
{
	// Enable timer clock 
	SIM->SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
	// clear pending flag
	LPTimer->CSR = LPTMR_CSR_TCF_MASK;
	// compare value
	LPTimer->CMR = secs;
	// initial value
//	LPTimer->CNR = 0;   ?????????
//	LPTimer->CSR = 0x44;
	// prescalar 1024 and use 1KHz internal clock
	LPTimer->PSR = LPTMR_PSR_PRESCALE(9) | LPTMR_PSR_PCS(1);
	// lowest possible priority
	NVIC_SetPriority(LPTimer_IRQn, 6);
}

// Set Timer for Interrupt
void LPTimer_SetForInterrupt(LPTMR_Type * LPTimer)
{
	// Enable Interrupt
	LPTimer->CSR |= LPTMR_CSR_TCF_MASK | LPTMR_CSR_TIE_MASK;
	// NVIC
	NVIC_ClearPendingIRQ(LPTimer_IRQn);
	NVIC_EnableIRQ(LPTimer_IRQn);
}

// Set call back method
void LPTimer_SetCallBack(void (*callback)(void))
{
	 callbackPtr = callback;
}

// interrupt handler
void  LPTimer_IRQHandler(void)
{
	callbackPtr();
	// callback function
	LPTMR0->CSR = LPTMR0->CSR;
}

/* EOF */
