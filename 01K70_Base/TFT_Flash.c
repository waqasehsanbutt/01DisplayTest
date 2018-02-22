/********************* Includes **********************/
#include "GeneralHeader.h"
#include "TFTFlash_PinAssignments.h"
#include "TFTFlash.h"
#include "SPI_HLD.h"
#include "Images.h"
#include "Delays.h"
#include <string.h>
/********************* Includes **********************/

/********************** Macros ***********************/

/********************** Macros ***********************/


/********************* Variables *********************/

/********************* Variables **********************/
// Initialize SPI IOs
static inline void TFTFlash_InitSPIIOs(void)
{
	FLASH_PCS_PORT->PCR[FLASH_PCS_PIN] = PORT_PCR_MUX(FLASH_PCS_MUX) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
}

// Initialize Control IOs
static inline void TFTFlash_InitControlIOs(void)
{
}

// Initialize Peripherals
static inline void TFTFlash_InitPeripherals(void)
{
	TFTFlash_InitControlIOs();
	TFTFlash_InitSPIIOs();
}

void ConfigureSPIForFlash(void)
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

// initialize the LCD
void TFTFlash_Init(void)
{
	ENBL_CLKS;
	TFTFlash_InitPeripherals();
//	TFTFlash_SendInitSequence();
}



/* EOF */
