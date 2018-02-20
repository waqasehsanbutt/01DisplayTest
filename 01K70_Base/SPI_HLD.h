/********************* Includes **********************/
#include "MK70F12.h"
/********************* Includes **********************/

#define WAIT_FOR_FIFO(spi)  				while ((spi->SR & SPI_SR_TFFF_MASK) == 0)
#define WAIT_FOR_TRANSMIT(spi)	   	while ((spi->SR & SPI_SR_TCF_MASK) == 0)  
#define WAIT_FOR_LAST_TRANSMIT(spi)	   while ((spi->SR & SPI_SR_EOQF_MASK) == 0)  
#define CLR_FLAGS(spi)	   				spi->SR = spi->SR;

/* EOF */

