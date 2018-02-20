/********************* Includes **********************/
#include "MK70F12.h"
#include "TouchPanel_PinAssignments.h"
#include "TouchPanel.h"
#include "SPI_HLD.h"
#include "Images.h"
#include "Delays.h"
#include <string.h>
/********************* Includes **********************/

/********************** Macros ***********************/

/********************** Macros ***********************/


/********************* Variables *********************/
static void (*touchCallBack)(void) = NULL;
/********************* Variables **********************/
// Initialize SPI IOs
static inline void TouchPanel_InitSPIIOs(void)
{
	TCH_PCS_PORT->PCR[TCH_PCS_PIN] = PORT_PCR_MUX(TCH_PCS_MUX);
}

// Initialize Control IOs
static inline void TouchPanel_InitControlIOs(void)
{
	// set as gpio // interrupt on falling edge
	TCH_DTCT_PORT->PCR[TCH_DTCT_PIN] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(0xA);
	// set direction to input
	TCH_DTCT_GPIO->PDDR &= ~(1 << TCH_DTCT_PIN);
	//
	NVIC_SetPriority(TCH_DTCT_IRQ, 6);
}

// Initialize Peripherals
static inline void TouchPanel_InitPeripherals(void)
{
	TouchPanel_InitControlIOs();
	TouchPanel_InitSPIIOs();
}

void EnableTouchInterrupt()
{
	TCH_DTCT_PORT->ISFR = (1 << TCH_DTCT_PIN);
	// 
	NVIC_ClearPendingIRQ(TCH_DTCT_IRQ);
	NVIC_EnableIRQ(TCH_DTCT_IRQ);
}

void DisableTouchInterrupt()
{
	TCH_DTCT_PORT->ISFR = (1 << TCH_DTCT_PIN);
	// 
	NVIC_ClearPendingIRQ(TCH_DTCT_IRQ);
	NVIC_DisableIRQ(TCH_DTCT_IRQ);
}

void TouchInterruptHandler(void (*ptr)(void))
{
	touchCallBack = ptr;
}

void PORTE_IRQHandler(void)
{  
	DisableTouchInterrupt();   
	if(touchCallBack != NULL)
		touchCallBack();
}

// initialize the LCD
void TouchPanel_Init(void)
{
	ENBL_CLKS;
	TouchPanel_InitPeripherals();
//	TouchPanel_SendInitSequence();
}



/* EOF */
