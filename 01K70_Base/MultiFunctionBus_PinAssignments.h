/********************* Includes **********************/
#include "MK70F12.h"
/********************* Includes **********************/

#if (!defined(MFP_PA))
#define MFP_PA

/********************** Macros ***********************/
#define MF_SPI													SPI1

#define MF_SCK_PIN                                	(2)  
#define MF_SCK_PORT                               	PORTE
#define MF_SCK_MUX											(2)

#define MF_SDO_PIN                                	(1)  
#define MF_SDO_PORT                               	PORTE
#define MF_SDO_MUX											(7)

#define MF_SDI_PIN                                	(3)  
#define MF_SDI_PORT                               	PORTE
#define MF_SDI_MUX											(7)

#define ENBL_CLKS												SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; \
																	SIM->SCGC6 |= SIM_SCGC6_DSPI1_MASK;
/********************** Macros ***********************/

#endif

/* EOF */
