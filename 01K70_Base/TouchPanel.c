/********************* Includes **********************/
#include "GeneralHeader.h"
#include "TouchPanel_PinAssignments.h"
#include "TouchPanel.h"
#include "ILI9488_Display.h"
#include "Converters.h"
#include "SPI_HLD.h"
#include "Images.h"  
#include "TouchPanelCalibration.h"
#include <string.h>
/********************* Includes **********************/

/********************** Macros ***********************/
#define Z_THRESHOLD     							400
#define THRESHOLD										400
/********************** Macros ***********************/


/********************* Variables *********************/
static void (*touchCallBack)(void) = NULL;
static uint16_t xLoc = 0;
static uint16_t yLoc = 0;
static volatile bool isTouchPending = false;

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
	uint8_t valA[2];	
	SPI_GetRegVal(MF_SPI, reg, valA, 2, SPI_PUSHR_PCS(1 << TCH_PCS));
	uint16_t val = (valA[0] & 0x7F);
	val <<= 8;
	val |= valA[1];
	return val >> 3;
}

static int16_t bestTwoAvg( int16_t x , int16_t y , int16_t z ) {
  int16_t da, db, dc;
  int16_t reta = 0;
  if ( x > y ) da = x - y; else da = y - x;
  if ( x > z ) db = x - z; else db = z - x;
  if ( z > y ) dc = z - y; else dc = y - z;

  if ( da <= db && da <= dc ) reta = (x + y) >> 1;
  else if ( db <= da && db <= dc ) reta = (x + z) >> 1;
  else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

  return (reta);
}

static bool IsThresholdViolated(uint16_t x1, uint16_t x2, uint16_t x3)
{
	if(x1 > x2)
	{
		if(x1 - x2 > THRESHOLD)
			return false;
		if(x1 > x3)
		{
			if(x1 - x3 > THRESHOLD)
				return false;
		}
		else
		{
			if(x3 - x1 > THRESHOLD)
				return false;
		}
	}
	else
	{
		if(x2 - x1 > THRESHOLD)
			return false;
		if(x2 > x3)
		{
			if(x2 - x3 > THRESHOLD)
				return false;
		}
		else
		{
			if(x3 - x2 > THRESHOLD)
				return false;
		}
	}
	return true;
}

static inline uint16_t GetTouchLocationX(void)
{
	return GetTouchRegisterVal(0x90);
}

static inline uint16_t GetTouchLocationY(void)
{
	return GetTouchRegisterVal(0xD0);
}

static inline uint16_t GetTouchLocationZ1(void)
{
	return GetTouchRegisterVal(0xB0);
}

static inline uint16_t GetTouchLocationZ2(void)
{
	return GetTouchRegisterVal(0xC0);
}

bool Method1(void)
{
	
	int16_t x1, x2, x3, y1, y2, y3;
	int16_t z1 = GetTouchLocationZ1();
	int z = (int)z1 + 4095;
	int16_t z2 = GetTouchLocationZ2();
	z -= z2;
	if (z >= Z_THRESHOLD)
	{
		x1 = GetTouchLocationX();
		y1 = GetTouchLocationY();
		x2 = GetTouchLocationX();
		y2 = GetTouchLocationY();
		x3 = GetTouchLocationX();
		y3 = GetTouchLocationY();
	}
	else
		return false;
//	z1 = GetTouchLocationZ1();
//	z2 = GetTouchLocationZ2();
//	if ((z2 - z1) > Z_THRESHOLD)
//		return;
	xLoc = bestTwoAvg(x1, x2, x3);
	yLoc = bestTwoAvg(y1, y2, y3);
	return true;
}

bool Method2(void)
{
	uint16_t x1, x2, x3, y1, y2, y3;
	x1 = GetTouchLocationX();
	y1 = GetTouchLocationY();
	x2 = GetTouchLocationX();
	y2 = GetTouchLocationY();
	x3 = GetTouchLocationX();
	y3 = GetTouchLocationY();
	if(IsThresholdViolated(x1, x2, x3) == false)
		return false;
	if(IsThresholdViolated(y1, y2, y3) == false)
		return false;
	
	xLoc = (x1 + x2 + x3) / 3;
	yLoc = (y1 + y2 + y3) / 3;
	
	return true;
}

bool Method3(void)
{
	uint16_t x1, x2, x3, y1, y2, y3;
	x1 = GetTouchLocationX();
	y1 = GetTouchLocationY(); 
	uint8_t touchValDiffY = 1;
	uint8_t touchValDiffX = touchValDiffY * 2;
	uint16_t pressureLimit = 5000;
	bool isOk = false;
	int retries = 40;
	do 
	{
		x2 = x1;
		y2 = y1;
		x1 = GetTouchLocationX();
		y1 = GetTouchLocationY();
		uint16_t z1 = GetTouchLocationZ1();
		uint16_t z2 = GetTouchLocationZ2();
		
		double p = (((z2 * 1.0) / z1) - 1) * ( 4096 - (double)x1);
		
		isOk = (p > pressureLimit) ? true : false;
		
		if(isOk == true)
			isOk = (x1 == 0 || y1 == 0 || x2 == 0 || y2 == 0) ? false : true;
		if(isOk == true)
			isOk = x1 >= x2 ? ((x1 - x2) < touchValDiffX ? true :  false) : ((x2 - x1) < touchValDiffX ? true :  false);
		if(isOk == true)
			isOk = y1 >= y2 ? ((y1 - y2) < touchValDiffY ? true :  false) : ((y2 - y1) < touchValDiffY ? true :  false);
		
	} while((isOk == false) && (retries-- > 0));

	if(retries <= 0)
		return false;
	
	xLoc = x1;
	yLoc = y1;
	
	return true;
}

void GetTouchLocation(void)
{
	if(Method3() == true)
	{
		unsigned char ss[6];	
		ConvertUInt16ToText(xLoc, ss);
		ShowString(10,205,"X: ",0xf800,0xffff);
		ShowString(25,205,ss,0xf800,0xffff);	 
		ConvertUInt16ToText(yLoc, ss);
		ShowString(80,205,"Y: ",0xf800,0xffff);
		ShowString(95,205,ss,0xf800,0xffff);
	}
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
               SPI_CTAR_PCSSCK(0x00) |
               SPI_CTAR_PASC(0x00) |
               SPI_CTAR_PDT(0x00) |
               SPI_CTAR_PBR(0x00) |
               SPI_CTAR_CSSCK(0x00) |
               SPI_CTAR_ASC(0x00) |
               SPI_CTAR_DT(0x00) |
               SPI_CTAR_BR(0x05);       /* Set Clock and Transfer Attributes register */
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
	isTouchPending = true;
	if(touchCallBack != NULL)
			touchCallBack();
}

void TouchPanel_Poll(void)
{
	if(isTouchPending == true)
	{
		ConnectToTouchPanel();
		GetTouchLocation();
		EnableTouchInterrupt();
		isTouchPending = false;
	}
}

// initialize the LCD
void TouchPanel_Init(void)
{
	ENBL_CLKS;
	TouchPanel_InitPeripherals();
//	TouchPanel_SendInitSequence();
}



/* EOF */
