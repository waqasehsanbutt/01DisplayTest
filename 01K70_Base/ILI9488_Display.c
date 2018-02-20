/********************* Includes **********************/
#include "MK70F12.h"
#include "ILI9488_PinAssignments.h"
#include "ILI9488_Display.h"
#include "SPI_HLD.h"
#include "Images.h"
#include "Delays.h"
/********************* Includes **********************/

/********************** Macros ***********************/

/********************** Macros ***********************/


/********************* Variables *********************/

/********************* Variables **********************/
static inline void ILI9488_ConfigSPI(void)
{
	/* SPI2_MCR: MSTR=0,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=0,CLR_RXF=0,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
	LCD_SPI->MCR = SPI_MCR_DCONF(0x00) |
		SPI_MCR_ROOE_MASK |
		SPI_MCR_PCSIS(0x01) |
		SPI_MCR_SMPL_PT(0x00) |
		SPI_MCR_HALT_MASK;        /* Set Configuration register */
	/* SPI2_MCR: MSTR=1,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=1,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=1,DIS_RXF=1,CLR_TXF=1,CLR_RXF=1,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
	LCD_SPI->MCR = SPI_MCR_MSTR_MASK |
		SPI_MCR_DCONF(0x00) |
		SPI_MCR_ROOE_MASK |
		SPI_MCR_PCSIS(0x01) |
		//		SPI_MCR_DIS_TXF_MASK |
		SPI_MCR_DIS_RXF_MASK |
		SPI_MCR_CLR_TXF_MASK |
		SPI_MCR_CLR_RXF_MASK |
		SPI_MCR_SMPL_PT(0x00) |
		SPI_MCR_HALT_MASK;        /* Set Configuration register */
	/* SPI2_CTAR0: DBR=1,FMSZ=7,CPOL=0,CPHA=0,LSBFE=0,PCSSCK=0,PASC=0,PDT=0,PBR=1,CSSCK=0,ASC=0,DT=0,BR=3 */
	LCD_SPI->CTAR[0] = SPI_CTAR_DBR_MASK |
		SPI_CTAR_FMSZ(0x07) |
		SPI_CTAR_PCSSCK(0x00) |
		SPI_CTAR_PASC(0x00) |
		SPI_CTAR_PDT(0x00) |
		SPI_CTAR_PBR(0x00) |
		SPI_CTAR_CSSCK(0x00) |
		SPI_CTAR_ASC(0x00) |
		SPI_CTAR_DT(0x00) |
		SPI_CTAR_BR(0x00);       /* Set Clock and Transfer Attributes register */
	/* SPI2_SR: TCF=1,TXRXS=0,??=0,EOQF=1,TFUF=1,??=0,TFFF=1,??=0,??=0,??=0,??=1,??=0,RFOF=1,??=0,RFDF=1,??=0,TXCTR=0,TXNXTPTR=0,RXCTR=0,POPNXTPTR=0 */
	LCD_SPI->SR = SPI_SR_TCF_MASK |
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
	LCD_SPI->RSER = 0x00U;                   /* Set DMA Interrupt Request Select and Enable register */
	/* SPI2_MCR: HALT=0 */
	LCD_SPI->MCR &= (uint32_t)~(uint32_t)(SPI_MCR_HALT_MASK);
}

// Initialize SPI IOs
static inline void ILI9488_InitSPIIOs(void)
{
	LCD_PCS_PORT->PCR[LCD_PCS_PIN] = PORT_PCR_MUX(LCD_PCS_MUX);
	LCD_SCK_PORT->PCR[LCD_SCK_PIN] = PORT_PCR_MUX(LCD_SCK_MUX);
	LCD_SDO_PORT->PCR[LCD_SDO_PIN] = PORT_PCR_MUX(LCD_SDO_MUX);
	LCD_SDI_PORT->PCR[LCD_SDI_PIN] = PORT_PCR_MUX(LCD_SDI_MUX);
}

// Initialize Control IOs
static inline void ILI9488_InitControlIOs(void)
{
	// set as gpio
	D_C_PORT->PCR[D_C_PIN] = PORT_PCR_MUX(1);
	RESET_PORT->PCR[RESET_PIN] = PORT_PCR_MUX(1);
	LGHT_PORT->PCR[LGHT_PIN] = PORT_PCR_MUX(1);

	// set states
	D_C_GPIO->PDOR |= (1 << D_C_PIN);
	RESET_GPIO->PDOR |= (1 << RESET_PIN);
	LGHT_GPIO->PDOR |= (1 << LGHT_PIN);

	// set direction to output
	D_C_GPIO->PDDR |= (1 << D_C_PIN);
	RESET_GPIO->PDDR |= (1 << RESET_PIN);
	LGHT_GPIO->PDDR |= (1 << LGHT_PIN);
}

// Initialize Peripherals
static inline void ILI9488_InitPeripherals(void)
{
	ILI9488_InitControlIOs();
	ILI9488_ConfigSPI();
	ILI9488_InitSPIIOs();
}

// Reset the controller
static inline void Reset(void)
{
	RESET_OFF();
	Delay_ms(10);
	RESET_ON();
	Delay_ms(15);
	RESET_OFF();
	Delay_ms(150);
}

// write single cmd byte
static inline void Write_Cmd(unsigned char cmd)
{
	WAIT_FOR_FIFO(LCD_SPI);
	D_C_CMD();
	LCD_SPI->PUSHR = SPI_PUSHR_PCS(1 << LCD_PCS) | cmd;
	WAIT_FOR_TRANSMIT(LCD_SPI);
	CLR_FLAGS(LCD_SPI);
}

// Write single data byte
static inline void Write_Cmd_Data(unsigned char cmdVal)
{
	WAIT_FOR_FIFO(LCD_SPI);
	LCD_SPI->PUSHR = SPI_PUSHR_PCS(LCD_PCS) | cmdVal;
	WAIT_FOR_TRANSMIT(LCD_SPI);
	CLR_FLAGS(LCD_SPI);
}

// Write data from 565 16 bit mode
static inline void Write_Data(unsigned char DH, unsigned char DL)
{
	unsigned char R1, G1, B1;
	unsigned int LD = 0;

	// RGB565 TO RGB666
	LD = DH << 8;
	LD |= DL;

	// colors
	R1 = (0x1f & (LD >> 11)) * 2;
	R1 <<= 2;
	G1 = 0x3f & (LD >> 5);
	G1 <<= 2;
	B1 = (0x1f & LD) * 2;
	B1 <<= 2;

	D_C_DATA();
	WAIT_FOR_FIFO(LCD_SPI);
	LCD_SPI->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1 << LCD_PCS) | R1;
	WAIT_FOR_FIFO(LCD_SPI);
	LCD_SPI->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1 << LCD_PCS) | G1;
	WAIT_FOR_FIFO(LCD_SPI);
	LCD_SPI->PUSHR = SPI_PUSHR_EOQ_MASK | SPI_PUSHR_PCS(1 << LCD_PCS) | B1;
	WAIT_FOR_LAST_TRANSMIT(LCD_SPI);
	CLR_FLAGS(LCD_SPI);
}

//write data word
static inline void Write_Data_U16(unsigned int y)
{
	unsigned char m, n;
	m = y >> 8;
	n = y;
	Write_Data(m, n);
}

// send initialization sequence
static inline void ILI9488_SendInitSequence(void)
{
	Reset();

	Write_Cmd(0xE0);
	Write_Cmd_Data(0x00);
	Write_Cmd_Data(0x03);
	Write_Cmd_Data(0x09);
	Write_Cmd_Data(0x08);
	Write_Cmd_Data(0x16);
	Write_Cmd_Data(0x0A);
	Write_Cmd_Data(0x3F);
	Write_Cmd_Data(0x78);
	Write_Cmd_Data(0x4C);
	Write_Cmd_Data(0x09);
	Write_Cmd_Data(0x0A);
	Write_Cmd_Data(0x08);
	Write_Cmd_Data(0x16);
	Write_Cmd_Data(0x1A);
	Write_Cmd_Data(0x0F);

	Write_Cmd(0xE1);
	Write_Cmd_Data(0x00);
	Write_Cmd_Data(0x16);
	Write_Cmd_Data(0x19);
	Write_Cmd_Data(0x03);
	Write_Cmd_Data(0x0F);
	Write_Cmd_Data(0x05);
	Write_Cmd_Data(0x32);
	Write_Cmd_Data(0x45);
	Write_Cmd_Data(0x46);
	Write_Cmd_Data(0x04);
	Write_Cmd_Data(0x0E);
	Write_Cmd_Data(0x0D);
	Write_Cmd_Data(0x35);
	Write_Cmd_Data(0x37);
	Write_Cmd_Data(0x0F);

	Write_Cmd(0xC0);      //Power Control 1 
	Write_Cmd_Data(0x17);    //Vreg1out 
	Write_Cmd_Data(0x15);    //Verg2out 

	Write_Cmd(0xC1);      //Power Control 2     
	Write_Cmd_Data(0x41);    //VGH,VGL 

	Write_Cmd(0xC5);      //Power Control 3 
	Write_Cmd_Data(0x00);
	Write_Cmd_Data(0x12);    //Vcom 
	Write_Cmd_Data(0x80);

	Write_Cmd(0x36);      //Memory Access 
	Write_Cmd_Data(0x48);

	Write_Cmd(0x3A);      // Interface Pixel Format 
	Write_Cmd_Data(0x66); 	  //18 bit    

	Write_Cmd(0XB0);      // Interface Mode Control 
	Write_Cmd_Data(0x80);     			 //SDO NOT USE

	Write_Cmd(0xB1);      //Frame rate 
	Write_Cmd_Data(0xA0);    //60Hz 

	Write_Cmd(0xB4);      //Display Inversion Control 
	Write_Cmd_Data(0x02);    //2-dot 

	Write_Cmd(0XB6);      //Display Function Control  RGB/MCU Interface Control 

	Write_Cmd_Data(0x02);    //MCU 
	Write_Cmd_Data(0x02);    //Source,Gate scan dieection 

	Write_Cmd(0XE9);      		// Set Image Functio
	Write_Cmd_Data(0x00);    // Disable 24 bit data

	Write_Cmd(0xF7);      		// Adjust Control 
	Write_Cmd_Data(0xA9);
	Write_Cmd_Data(0x51);
	Write_Cmd_Data(0x2C);
	Write_Cmd_Data(0x82);    // D7 stream, loose 

	Write_Cmd(0x11); 			//Sleep out
	Delay_ms(150);
	Write_Cmd(0x29);
}

void LCD_SetPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye)
{
	Write_Cmd(0x2A);
	Write_Cmd_Data(xs >> 8);
	Write_Cmd_Data(xs & 0xff);
	Write_Cmd_Data(xe >> 8);
	Write_Cmd_Data(xe & 0xff);
	Write_Cmd(0x2B);
	Write_Cmd_Data(ys >> 8);
	Write_Cmd_Data(ys & 0xff);
	Write_Cmd_Data(ye >> 8);
	Write_Cmd_Data(ye & 0xff);

	Write_Cmd(0x2C);
}

// show full LCD box
void LCD_FULL(unsigned int i)
{
	unsigned int w, u;
	LCD_SetPos(0, 319, 0, 479);

	for (w = 0; w < 320; w++)
		Write_Data_U16(~i);

	for (w = 0; w < 478; w++)
	{
		Write_Data_U16(~i);
		for (u = 0; u < 318; u++)
			Write_Data_U16(i);
		Write_Data_U16(~i);
	}

	for (w = 0; w < 320; w++)
		Write_Data_U16(~i);
}

// show default picture
void show_picture(void)
{
	unsigned char i, j;
	unsigned int m = 0, n = 0, k = 0;
	LCD_SetPos(0, 319, 0, 479);

	for (j = 0; j < 160; j++)
	{
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[m++]);
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[n++]);
		for (i = 0; i < 80; i++)
			Write_Data_U16(pic[k++]);
		k += 40;
	}

	m = 0;
	n = 0;
	k = 0;
	for (j = 0; j < 160; j++)
	{
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[m++]);
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[n++]);
		for (i = 0; i < 80; i++)
			Write_Data_U16(pic[k++]);
		k += 40;
	}

	m = 0;
	n = 0;
	k = 0;
	for (j = 0; j < 160; j++)
	{
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[m++]);
		for (i = 0; i < 120; i++)
			Write_Data_U16(pic[n++]);
		for (i = 0; i < 80; i++)
			Write_Data_U16(pic[k++]);
		k += 40;
	}

	return;
}

// Enter sleep mode
void ILI9488_EnterSleep(void)
{
	Write_Cmd(0x28); //Display off
	Delay_ms(10);
	Write_Cmd(0x10); // Internal oscillator will be stopped
	Delay_ms(120);
}

// Exit sleep mode
void ILI9488_ExitSleep(void)
{
	Write_Cmd(0x11); // Sleep out
	Delay_ms(120);
	Write_Cmd(0x29); //Display on
}

// all display one colour
void ClearScreen(unsigned int bColor)
{
	unsigned int i, j;
	LCD_SetPos(0, 319, 0, 479);
	for (i = 0; i < 320; i++)
	{
		for (j = 0; j < 480; j++)
			Write_Data_U16(bColor);
	}
}

// initialize the LCD
void ILI9488_Init(void)
{
	ENBL_CLKS;
	ILI9488_InitPeripherals();
	ILI9488_SendInitSequence();
}



/* EOF */
