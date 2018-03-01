/********************* Includes **********************/
#include "GeneralHeader.h"

#if (!(defined(TFT_TYPE)))
      #error ("Invalid TFT!");
#endif

#if (TFT_TYPE == TFTM035_6)

#include "ILI9488_PinAssignments.h"
#include "ILI9488_Display.h"
#include "Font.h"
#include "SPI_HLD.h"
#include "Images.h"
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

static inline void Write_Cmd_Data(unsigned char* cmdVals, uint8_t noOfVals)
{   
	WAIT_FOR_FIFO(LCD_SPI);
	D_C_DATA();
	for(int i = 0; i < noOfVals - 1; i++)
	{
		WAIT_FOR_FIFO(LCD_SPI);
		LCD_SPI->PUSHR = SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS(1 << LCD_PCS) | *(cmdVals + i);
		CLR_WRITE_FLAG(LCD_SPI);
	}
	WAIT_FOR_FIFO(LCD_SPI);
	LCD_SPI->PUSHR = SPI_PUSHR_EOQ_MASK | SPI_PUSHR_PCS(1 << LCD_PCS) | *(cmdVals + noOfVals - 1);
	CLR_WRITE_FLAG(LCD_SPI);
	WAIT_FOR_LAST_TRANSMIT(LCD_SPI);
	CLR_FLAGS(LCD_SPI);
}

//// Write single data byte
//static inline void Write_Cmd_Data(unsigned char cmdVal)
//{    
//	D_C_DATA();
//	WAIT_FOR_FIFO(LCD_SPI);
//	LCD_SPI->PUSHR = SPI_PUSHR_PCS(LCD_PCS) | cmdVal;
//	WAIT_FOR_TRANSMIT(LCD_SPI);
//	CLR_FLAGS(LCD_SPI);
//}

// Write data from 565 16 bit mode
static inline void Write_Data(unsigned char DH, unsigned char DL)
{
	unsigned char colors[3]; 
	unsigned int LD = 0;

	// RGB565 TO RGB666
	LD = DH << 8;
	LD |= DL;

	// colors
	colors[0] = (0x1f & (LD >> 11)) * 2;
	colors[0] <<= 2;
	colors[1] = 0x3f & (LD >> 5);
	colors[1] <<= 2;
	colors[2] = (0x1f & LD) * 2;
	colors[2] <<= 2;

   Write_Cmd_Data(colors, 3);
}

//write data word
static inline void Write_Data_U16(unsigned int y)
{
	unsigned char m, n;
	m = y >> 8;
	n = y;
	Write_Data(m, n);
}

static inline void PositiveGammaControl(void)
{
	Write_Cmd(0xE0);
	uint8_t data[] = {0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void NegativeGammaControl(void)
{
	Write_Cmd(0xE1);
	uint8_t data[] = {0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void PowerControl1(void)
{  
	Write_Cmd(0xC0);
	uint8_t data[] = {0x17, 0x15};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void PowerControl2(void)
{
	Write_Cmd(0xC1);
	uint8_t data[] = {0x41};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void PowerControl3(void)
{
	Write_Cmd(0xC5);
	uint8_t data[] = {0x00, 0x12, 0x80};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void MemoryAccess(void)
{
	Write_Cmd(0x36);
	uint8_t data[] = {0x48};
	Write_Cmd_Data(data, sizeof(data));
}

static inline void InterfacePixelFormat(void)
{
	Write_Cmd(0x3A);
	uint8_t data[] = {0x66};
	Write_Cmd_Data(data, sizeof(data)); 
}

static inline void InterfaceModeControl(void)
{
	Write_Cmd(0xB0);
	uint8_t data[] = {0x80};
	Write_Cmd_Data(data, sizeof(data));  
}

static inline void FrameRateControl(void)
{
	Write_Cmd(0xB1);
	uint8_t data[] = {0xA0};
	Write_Cmd_Data(data, sizeof(data));	
}

static inline void DisplayInversionControl(void)
{	
	Write_Cmd(0xB4);
	uint8_t data[] = {0x02};
	Write_Cmd_Data(data, sizeof(data)); 	
}

static inline void DisplayFunctionControl(void)
{
	Write_Cmd(0xB6);
	uint8_t data[] = {0x02, 0x02};
	Write_Cmd_Data(data, sizeof(data)); 
}

static inline void SetImageFunction(void)
{  	
	Write_Cmd(0xE9);
	uint8_t data[] = {0x00};
	Write_Cmd_Data(data, sizeof(data)); 
}

static inline void AdjustControl(void)
{
	Write_Cmd(0xF7);
	uint8_t data[] = {0xA9, 0x51, 0x2C, 0x82};
	Write_Cmd_Data(data, sizeof(data)); 
}

// Enter sleep mode
void ILI9488_EnterSleep(void)
{
	Write_Cmd(0x28); //Display off
	Delay_ms(10);
	Write_Cmd(0x10); // Internal oscillator will be stopped
	Delay_ms(120);
	LGHT_OFF();
}

// Exit sleep mode
void ILI9488_ExitSleep(void)
{
	Write_Cmd(0x11); // Sleep out
	Delay_ms(120);
	Write_Cmd(0x29); //Display on
	LGHT_ON();
}

// send initialization sequence
static inline void ILI9488_SendInitSequence(void)
{
	Reset();

   PositiveGammaControl();
   NegativeGammaControl();

   PowerControl1();
	PowerControl2();
	PowerControl3();

   MemoryAccess();
   InterfacePixelFormat();
   InterfaceModeControl();
   FrameRateControl();
   DisplayInversionControl();
   DisplayFunctionControl();
   SetImageFunction();
   AdjustControl();

	ILI9488_ExitSleep();
}

void LCD_SetPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye)
{
	Write_Cmd(0x2A);
	uint8_t data[4] = {xs >> 8, xs & 0xff, xe >> 8, xe & 0xff};
	Write_Cmd_Data(data, 4);
	Write_Cmd(0x2B);
	data[0] = ys >> 8;
	data[1] = ys & 0xff;
	data[2] = ye >> 8;
	data[3] = ye & 0xff;
	Write_Cmd_Data(data, 4);
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

// show filled square
void ShowFilledSquare(uint16_t x, uint16_t y, uint16_t size, uint16_t color)
{
	LCD_SetPos(x, x + size - 1, y, y + size - 1);  
	for(int i = 0; i < (size* size); i++)
	{
		Write_Data(color >> 8, color);
	}
}

//show one character
//dcolor:character colour£¬gbcolor: background color
void ShowSingleChar(unsigned int x,unsigned int y,unsigned char value,unsigned int dcolor,unsigned int bgcolor)	
{
	unsigned char i,j;
	unsigned char *temp=Font_12x8_1;    
   LCD_SetPos(x,x+7,y,y+11);      
	temp+=(value-32)*12;
	for(j=0;j<12;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<(7-i)))!=0)
			{
				Write_Data(dcolor >>8 , dcolor);
			} 
			else
			{
				Write_Data(bgcolor >> 8, bgcolor);
			}   
		}
		temp++;
	 }
}
//show string
//dcolor:character colour£¬gbcolor: background color
void ShowString(unsigned int x,unsigned int y,unsigned char *str,unsigned int dcolor,unsigned int bgcolor)	
{
	unsigned int x1,y1;
	x1=x;
	y1=y;
	while(*str!='\0')
	{	
		ShowSingleChar(x1,y1,*str,dcolor,bgcolor);
		x1+=7;
		str++;
	}
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

#endif

/* EOF */
