/********************* Includes **********************/
#include "SPI_HLD.h"
/********************* Includes **********************/
static inline uint8_t ReceiveData(SPI_Type* spi, uint32_t data)
{
	while(TXFIFO_FULL(spi));
	spi->PUSHR = data;
	TXFIFO_FILL_CLR(spi);
	while(RXFIFO_EMPTY(spi));
	uint8_t val = spi->POPR;
	RXFIFO_EMPTY_CLR(spi);
	return val;
}


void SPI_GetRegVal(SPI_Type* spi, uint8_t reg, uint8_t * value, uint8_t noOfValues, uint32_t pcs)
{
	(void)ReceiveData(spi, SPI_PUSHR_CONT_MASK | pcs | reg);
	Delay_Cycles(20);
   for(int i = 0; i < noOfValues - 1; i++)
	{
		*(value++)  = ReceiveData(spi, SPI_PUSHR_CONT_MASK | pcs);
		Delay_Cycles(20);
	}
	*(value)  = ReceiveData(spi, pcs);
	CLR_FLAGS(spi);
}

/* EOF */
