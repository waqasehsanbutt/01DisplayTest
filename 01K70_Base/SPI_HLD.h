/********************* Includes **********************/
#include "GeneralHeader.h"
/********************* Includes **********************/

#define WAIT_FOR_FIFO(spi)  				while ((spi->SR & SPI_SR_TFFF_MASK) == 0)
#define WAIT_FOR_TRANSMIT(spi)	   	while ((spi->SR & SPI_SR_TCF_MASK) == 0)  
#define WAIT_FOR_LAST_TRANSMIT(spi)	   while ((spi->SR & SPI_SR_EOQF_MASK) == 0) 
#define WAIT_FOR_RECEIVE(spi)				while	((spi->SR & SPI_SR_RFDF_MASK) == 0)
#define CLR_FLAGS(spi)	   				spi->SR = spi->SR;
#define CLR_READ_FLAGS(spi)	   		spi->SR = SPI_SR_RFDF_MASK; 
#define CLR_WRITE_FLAG(spi)	   		spi->SR = SPI_SR_TFFF_MASK;

#define TXFIFO_FILL_CLR(spi)        	spi->SR = SPI_SR_TFFF_MASK;
#define RXFIFO_EMPTY_CLR(spi)        	spi->SR = SPI_SR_RFDF_MASK;

#define TXFIFO_FULL(spi)           		(spi->SR & SPI_SR_TFFF_MASK) == 0
#define RXFIFO_EMPTY(spi)           	(spi->SR & SPI_SR_RFDF_MASK) == 0

extern void SPI_GetRegVal(SPI_Type* spi, uint8_t reg, uint8_t * value, uint8_t noOfValues, uint32_t pcs);

/* EOF */

