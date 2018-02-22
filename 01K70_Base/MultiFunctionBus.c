/********************* Includes **********************/
#include "GeneralHeader.h"
#include "MultiFunctionBus_PinAssignments.h"
#include "TouchPanel.h"
#include "SPI_HLD.h"
#include "Images.h"
#include "Delays.h"
/********************* Includes **********************/

/********************** Macros ***********************/

/********************** Macros ***********************/
enum MF_MODE{
	NONE,
	TOUCH,
	SD,
	FONT,
	FLASH
};

/********************* Variables *********************/
enum MF_MODE mode = NONE; 
/********************* Variables **********************/
static inline void MultiFunctionBus_ConfigSPI(void)
{
	/* SPI2_MCR: MSTR=0,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=0,CLR_RXF=0,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
	MF_SPI->MCR = SPI_MCR_DCONF(0x00) |
             SPI_MCR_ROOE_MASK |
             SPI_MCR_PCSIS(0x01) |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;        /* Set Configuration register */
	/* SPI2_MCR: MSTR=1,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=1,DIS_RXF=1,CLR_TXF=1,CLR_RXF=1,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
	MF_SPI->MCR = SPI_MCR_MSTR_MASK |
             SPI_MCR_DCONF(0x00) |
             SPI_MCR_ROOE_MASK |
             SPI_MCR_PCSIS(0x01) |
             SPI_MCR_DIS_TXF_MASK |
             SPI_MCR_DIS_RXF_MASK |
             SPI_MCR_CLR_TXF_MASK |
             SPI_MCR_CLR_RXF_MASK |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;        /* Set Configuration register */
	/* SPI2_CTAR0: DBR=1,FMSZ=7,CPOL=0,CPHA=0,LSBFE=0,PCSSCK=0,PASC=0,PDT=0,PBR=1,CSSCK=0,ASC=0,DT=0,BR=3 */
	MF_SPI->CTAR[0] = SPI_CTAR_DBR_MASK |
               SPI_CTAR_FMSZ(0x07) |
               SPI_CTAR_PCSSCK(0x01) |
               SPI_CTAR_PASC(0x01) |
               SPI_CTAR_PDT(0x01) |
               SPI_CTAR_PBR(0x03) |
               SPI_CTAR_CSSCK(0x01) |
               SPI_CTAR_ASC(0x01) |
               SPI_CTAR_DT(0x01) |
               SPI_CTAR_BR(0x02);      /* Set Clock and Transfer Attributes register */
	/* SPI2_SR: TCF=1,TXRXS=0,??=0,EOQF=1,TFUF=1,??=0,TFFF=1,??=0,??=0,??=0,??=1,??=0,RFOF=1,??=0,RFDF=1,??=0,TXCTR=0,TXNXTPTR=0,RXCTR=0,POPNXTPTR=0 */
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
	/* SPI2_RSER: TCF_RE=0,??=0,??=0,EOQF_RE=0,TFUF_RE=0,??=0,TFFF_RE=0,TFFF_DIRS=0,??=0,??=0,??=0,??=0,RFOF_RE=0,??=0,RFDF_RE=0,RFDF_DIRS=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	MF_SPI->RSER = 0x00U;                   /* Set DMA Interrupt Request Select and Enable register */
	/* SPI2_MCR: HALT=0 */
	MF_SPI->MCR &= (uint32_t)~(uint32_t)(SPI_MCR_HALT_MASK);
}

// Initialize SPI IOs
static inline void MultiFunctionBus_InitSPIIOs(void)
{
	MF_SCK_PORT->PCR[MF_SCK_PIN] = PORT_PCR_MUX(MF_SCK_MUX);
	MF_SDO_PORT->PCR[MF_SDO_PIN] = PORT_PCR_MUX(MF_SDO_MUX);
	MF_SDI_PORT->PCR[MF_SDI_PIN] = PORT_PCR_MUX(MF_SDI_MUX);
}

// Initialize Peripherals
static inline void MultiFunctionBus_InitPeripherals(void)
{
	MultiFunctionBus_ConfigSPI();
	MultiFunctionBus_InitSPIIOs();
}

// initialize the LCD
void MultiFunctionBus_Init(void)
{
	ENBL_CLKS;
	MultiFunctionBus_InitPeripherals();
}

void ConnectToTouchPanel(void)
{
	if(mode != TOUCH)
	{
		ConfigureSPIForTouch();
		mode = TOUCH;
	}
}

void ConnectToSDCard(void)
{
	
}

void ConnectToFontChip(void)
{
	
}



/* EOF */
