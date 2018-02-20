/********************* Includes **********************/
#include "MK70F12.h"
/********************* Includes **********************/

/********************** Macros ***********************/
#define D_C_PIN												(12)
#define D_C_PORT												PORTE
#define D_C_GPIO												PTE
#define D_C_DATA()											D_C_GPIO->PSOR = (1 << D_C_PIN);   
#define D_C_CMD()												D_C_GPIO->PCOR = (1 << D_C_PIN);

#define RESET_PIN												(11)
#define RESET_PORT											PORTE
#define RESET_GPIO											PTE    
#define RESET_ON()											RESET_GPIO->PCOR = (1 << RESET_PIN);   
#define RESET_OFF()											RESET_GPIO->PSOR = (1 << RESET_PIN);

#define LGHT_PIN												(9)
#define LGHT_PORT												PORTA
#define LGHT_GPIO												PTA
#define LGHT_ON()												LGHT_GPIO->PSOR = (1 << LGHT_PIN);   
#define LGHT_OFF()											LGHT_GPIO->PCOR = (1 << LGHT_PIN);

#define LCD_SPI												SPI2

#define LCD_PCS												(0)
#define LCD_PCS_PIN                                (11)
#define LCD_PCS_PORT                               PORTD
#define LCD_PCS_MUX											(2)

#define LCD_SCK_PIN                                (12)  
#define LCD_SCK_PORT                               PORTD
#define LCD_SCK_MUX											(2)

#define LCD_SDO_PIN                                (14)  
#define LCD_SDO_PORT                               PORTD
#define LCD_SDO_MUX											(2)

#define LCD_SDI_PIN                                (13)  
#define LCD_SDI_PORT                               PORTD
#define LCD_SDI_MUX											(2)

#define ENBL_CLKS												SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK; \
																	SIM->SCGC3 |= SIM_SCGC3_DSPI2_MASK;
/********************** Macros ***********************/

/* EOF */
