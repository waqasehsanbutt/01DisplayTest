/********************* Includes **********************/
#include "GeneralHeader.h"
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
static uint16_t xLoc = 0;
static uint16_t yLoc = 0;
/********************* Variables **********************/
// Initialize SPI IOs
static inline void TouchPanel_InitSPIIOs(void)
{
	TCH_PCS_PORT->PCR[TCH_PCS_PIN] = PORT_PCR_MUX(TCH_PCS_MUX) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
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

static inline uint16_t GetTouchRegisterVal(uint8_t reg)
{
	WAIT_FOR_FIFO(MF_SPI);
	MF_SPI->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1 << TCH_PCS) | reg;
	WAIT_FOR_RECEIVE(MF_SPI); 	
	(void)MF_SPI->POPR;
	CLR_READ_FLAGS(MF_SPI);
	Delay_Cycles(200);
	MF_SPI->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1 << TCH_PCS) | 0x00;
	WAIT_FOR_RECEIVE(MF_SPI); 	
	uint16_t val = (MF_SPI->POPR) << 8;
	CLR_READ_FLAGS(MF_SPI);     
	Delay_Cycles(200);
	MF_SPI->PUSHR = SPI_PUSHR_PCS(1 << TCH_PCS) | 0x00;
	WAIT_FOR_RECEIVE(MF_SPI); 	
	val |= (MF_SPI->POPR);
	CLR_FLAGS(MF_SPI);     
	Delay_Cycles(200);
	return val >> 3;
}

static inline uint16_t GetTouchLocationX(void)
{
	return GetTouchRegisterVal(0x90);
}

static inline uint16_t GetTouchLocationY(void)
{
	return GetTouchRegisterVal(0xD0);
}

void GetTouchLocation(void)
{
	xLoc = GetTouchLocationX();
	Delay_ms(1);
	yLoc = GetTouchLocationY();
}

void ConfigureSPIForTouch(void)
{
	  /* SPI1_MCR: MSTR=0,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=0,CLR_RXF=0,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
  MF_SPI->MCR = SPI_MCR_DCONF(0x00) |
             SPI_MCR_ROOE_MASK |
             SPI_MCR_PCSIS(0x01) |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;        /* Set Configuration register */
  /* SPI1_MCR: MSTR=1,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=1,CLR_RXF=1,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
  MF_SPI->MCR = SPI_MCR_MSTR_MASK |
             SPI_MCR_DCONF(0x00) |
             SPI_MCR_ROOE_MASK |
             SPI_MCR_PCSIS(0x01) |
             SPI_MCR_CLR_TXF_MASK |
             SPI_MCR_CLR_RXF_MASK |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;        /* Set Configuration register */
  /* SPI1_CTAR0: DBR=0,FMSZ=7,CPOL=0,CPHA=0,LSBFE=0,PCSSCK=1,PASC=1,PDT=1,PBR=3,CSSCK=1,ASC=1,DT=4,BR=2 */
  MF_SPI->CTAR[0] = SPI_CTAR_DBR_MASK |
               SPI_CTAR_FMSZ(0x07) |
               SPI_CTAR_PCSSCK(0x01) |
               SPI_CTAR_PASC(0x01) |
               SPI_CTAR_PDT(0x01) |
               SPI_CTAR_PBR(0x01) |
               SPI_CTAR_CSSCK(0x01) |
               SPI_CTAR_ASC(0x01) |
               SPI_CTAR_DT(0x04) |
               SPI_CTAR_BR(0x07);      /* Set Clock and Transfer Attributes register */
  /* SPI1_SR: TCF=1,TXRXS=0,??=0,EOQF=1,TFUF=1,??=0,TFFF=1,??=0,??=0,??=0,??=1,??=0,RFOF=1,??=0,RFDF=1,??=0,TXCTR=0,TXNXTPTR=0,RXCTR=0,POPNXTPTR=0 */
  MF_SPI->SR = SPI_SR_TCF_MASK |
            SPI_SR_EOQF_MASK |
            SPI_SR_TFUF_MASK |
            SPI_SR_TFFF_MASK |
            SPI_SR_RFOF_MASK |
            SPI_SR_RFDF_MASK |
            SPI_SR_TXCTR(0x00) |
            SPI_SR_TXNXTPTR(0x00) |
            SPI_SR_RXCTR(0x00) |
            SPI_SR_POPNXTPTR(0x00) |
            0x00200000U;               /* Clear flags */
  /* SPI1_RSER: TCF_RE=0,??=0,??=0,EOQF_RE=0,TFUF_RE=0,??=0,TFFF_RE=0,TFFF_DIRS=0,??=0,??=0,??=0,??=0,RFOF_RE=0,??=0,RFDF_RE=0,RFDF_DIRS=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  MF_SPI->RSER = 0x00U;                   /* Set DMA Interrupt Request Select and Enable register */
  /* SPI1_MCR: HALT=0 */
  MF_SPI->MCR &= (uint32_t)~(uint32_t)(SPI_MCR_HALT_MASK);
}

// enable touch interrupts
void EnableTouchInterrupt()
{
	TCH_DTCT_PORT->ISFR = (1 << TCH_DTCT_PIN);
	// 
	NVIC_ClearPendingIRQ(TCH_DTCT_IRQ);
	NVIC_EnableIRQ(TCH_DTCT_IRQ);
}

// Disable touch interrupts
void DisableTouchInterrupt()
{
	TCH_DTCT_PORT->ISFR = (1 << TCH_DTCT_PIN);
	// 
	NVIC_ClearPendingIRQ(TCH_DTCT_IRQ);
	NVIC_DisableIRQ(TCH_DTCT_IRQ);
}

// Assign a touch interrupt handler
void AssignTouchInterruptHandler(void (*ptr)(void))
{
	touchCallBack = ptr;
}

// Touch interrupt
void TCH_DTCT_IRQHandler(void)
{  
	DisableTouchInterrupt();  
	ConnectToTouchPanel();
	GetTouchLocation();
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
