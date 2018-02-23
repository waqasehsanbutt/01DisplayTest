/********************* Includes **********************/
#include "GeneralHeader.h"
#include "TouchPanel_PinAssignments.h"
#include "TouchPanel.h"
#include "ILI9488_Display.h"
#include "Converters.h"
#include "SPI_HLD.h"
#include "Images.h"  
#include "TouchPanelCalibration.h"
/********************* Includes **********************/

/********************** Macros ***********************/
#define Z_THRESHOLD     							400
#define THRESHOLD										400
/********************** Macros ***********************/


/********************* Variables *********************/
static void (*touchCallBack)(void) = NULL;
//static uint16_t xLoc = 0;
//static uint16_t yLoc = 0;
static volatile bool isTouchPending = false;
static POINT_2D_PTR lastTouchPos = NULL;
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

static inline int16_t GetTouchRegisterVal(uint8_t reg)
{
	uint8_t valA[2];	
	SPI_GetRegVal(MF_SPI, reg, valA, 2, SPI_PUSHR_PCS(1 << TCH_PCS));
	uint16_t val = (valA[0] & 0x7F);
	val <<= 8;
	val |= valA[1];
	return val >> 3;
}


static inline int16_t GetTouchLocationX(uint8_t pFlag)
{
	return GetTouchRegisterVal(0x90 | pFlag);
}

static inline int16_t GetTouchLocationY(uint8_t pFlag)
{
	return GetTouchRegisterVal(0xD0 | pFlag);
}

static inline int16_t GetTouchLocationZ1(uint8_t pFlag)
{
	return GetTouchRegisterVal(0xB0 | pFlag);
}

static inline int16_t GetTouchLocationZ2(uint8_t pFlag)
{
	return GetTouchRegisterVal(0xC0 | pFlag);
}

static inline uint8_t GetXYAtZValid(int16_t* x, int16_t* y, uint8_t triesLeft)
{
	int16_t z[2];
	int16_t xT, yT;
	int tH = 60000;
	
	while (triesLeft > 0)
	{
//		xT < 100 ? z[1] / z[0] > ((tH / xT) + 1) :
		triesLeft--;
		
		xT = GetTouchLocationX(1);
		
		z[0] = GetTouchLocationZ1(1);
		z[1] = GetTouchLocationZ2(1);
		
		if(xT == 0)
			continue;
		
		if (z[1] - z[0] > 2500)
			continue;
		
		xT = GetTouchLocationX(1);
		yT = GetTouchLocationY(1);
//		xT = GetTouchLocationX(1);
//		yT = GetTouchLocationY(1);
//		xT = GetTouchLocationX(1);
//		yT = GetTouchLocationY(1);
		
		z[0] = GetTouchLocationZ1(1);
		z[1] = GetTouchLocationZ2(0);
		
		if(xT == 0)
			continue;
		
		if (z[1] - z[0] > 2500)
			continue;
		
		*x = xT;
		*y = yT;
		
		triesLeft++;
		break;		
	}
	
	return triesLeft;
}	

bool Method5(bool isCalibration)
{
	int16_t x[2], y[2], z[2];
	
	uint8_t triesLeft = GetXYAtZValid(x, y, 20);
	if (triesLeft == 0)
		return false;
	
	uint8_t touchValDiffY = 8;
	uint8_t touchValDiffX = touchValDiffY * 2;
	bool isOk = false;
	
	do 
	{
		x[1] = x[0];
		y[1] = y[0];
		triesLeft = GetXYAtZValid(x, y, triesLeft);
		if (triesLeft == 0)
			return false;
		
		isOk = (x[0] == 0 || y[0] == 0 || x[1] == 0 || y[1] == 0) ? false : true;
		if(isOk == true)
			isOk = x[0] >= x[1] ? ((x[0] - x[1]) < touchValDiffX ? true :  false) : ((x[1] - x[0]) < touchValDiffX ? true :  false);
		if(isOk == true)
			isOk = y[0] >= y[1] ? ((y[0] - y[1]) < touchValDiffY ? true :  false) : ((y[1] - y[0]) < touchValDiffY ? true :  false);
		
	} while(isOk == false);
	
	x[0] = (y[0] + y[1])/2;
	y[0] = (x[0] + x[1])/2;
	
	if(isCalibration == false)
	{
		lastTouchPos->x = ((cal->An * x[0]) + (cal->Bn * y[0]) + cal->Cn) / cal->V;
		lastTouchPos->y = ((cal->Dn * x[0]) + (cal->En * y[0]) + cal->Fn) / cal->V;
	}
	else
	{
		lastTouchPos->x = x[0];
		lastTouchPos->y = y[0];
	}
	
	return true;
}

bool GetTouchLocation(bool isCalibration)
{
	if (Method5(isCalibration) == false)
			return false;
	unsigned char ss[6];	
	ConvertIntToText(lastTouchPos->x, ss);
	ShowString(10,205,"X: ",0xf800,0xffff);
	ShowString(25,205,ss,0xf800,0xffff);	 
	ConvertIntToText(lastTouchPos->y, ss);
	ShowString(80,205,"Y: ",0xf800,0xffff);
	ShowString(95,205,ss,0xf800,0xffff);
	return true;
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

void GetTouch(void)
{
	bool isOk = false;
	do
	{
//		while(isTouchPending == false);
		ConnectToTouchPanel();
		isOk = GetTouchLocation(true);
		EnableTouchInterrupt();
		isTouchPending = false;
	}while(isOk == false);
}

// calibrate touch
void CalibrateTouch(void)
{
	uint8_t points = 3;
	
	POINT_2D_PTR pDisplayPoints = (POINT_2D_PTR)malloc(sizeof(POINT_2D_STRUCT) * points);
	POINT_2D_PTR pTouchPoints = (POINT_2D_PTR)malloc(sizeof(POINT_2D_STRUCT) * points);
	
	ConnectToTouchPanel();
	
	// set screen

	pDisplayPoints->x = 260; pDisplayPoints->y = 60;
	(pDisplayPoints + 1)->x = 160; (pDisplayPoints + 1)->y = 240;	
	(pDisplayPoints + 2)->x = 60; (pDisplayPoints + 2)->y = 420;
//	(pDisplayPoints + 3)->x = 160; (pDisplayPoints + 3)->y = 10;	
//	(pDisplayPoints + 4)->x = 160; (pDisplayPoints + 4)->y = 240;
//	(pDisplayPoints + 5)->x = 160; (pDisplayPoints + 5)->y = 470;	
//	(pDisplayPoints + 6)->x = 10; (pDisplayPoints + 6)->y = 10;
//	(pDisplayPoints + 7)->x = 10; (pDisplayPoints + 7)->y = 240;	
//	(pDisplayPoints + 8)->x = 10; (pDisplayPoints + 8)->y = 470;
	
	ClearScreen(0xFFFF);
	
	// set values
	for (int i = 0 ; i < points ; i++)
	{		
		if(i > 0)
			ShowFilledSquare((pDisplayPoints + i - 1)->x - CAL_RADIUS, (pDisplayPoints + i - 1)->y - CAL_RADIUS, CAL_RADIUS, 0xFFFF);	
		else
			ShowFilledSquare((pDisplayPoints + points - 1)->x - CAL_RADIUS, (pDisplayPoints + points - 1)->y - CAL_RADIUS, CAL_RADIUS, 0xFFFF);	
		ShowFilledSquare((pDisplayPoints + i)->x - CAL_RADIUS, (pDisplayPoints + i)->y - CAL_RADIUS, CAL_RADIUS, 0);
		GetTouch();
		Delay_ms(2000);
		(pTouchPoints + i)->x = lastTouchPos->x;
		(pTouchPoints + i)->y = lastTouchPos->y;
	}
	MakeCalibrationParams(pDisplayPoints, pTouchPoints);
	
//	while(1);
	
}

void TouchPanel_Poll(void)
{
//	if(isTouchPending == true)
//	{
		ConnectToTouchPanel();
		GetTouchLocation(false);
		EnableTouchInterrupt();
		isTouchPending = false;
//	}
}

// initialize the LCD
void TouchPanel_Init(void)
{
	ENBL_CLKS;
	TouchPanel_InitPeripherals();
	TouchPanelCalibration_Init();
	lastTouchPos = (POINT_2D_PTR)malloc(sizeof(POINT_2D_STRUCT));
//	TouchPanel_SendInitSequence();
}



/* EOF */
