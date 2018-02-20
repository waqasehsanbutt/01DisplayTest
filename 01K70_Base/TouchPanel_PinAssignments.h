/********************* Includes **********************/
#include "MK70F12.h"
#include "MultiFunctionBus_PinAssignments.h"
/********************* Includes **********************/

/********************** Macros ***********************/
#define TCH_PCS												(0)
#define TCH_PCS_PIN                                (4)
#define TCH_PCS_PORT                               PORTE
#define TCH_PCS_MUX											(2)

#define TCH_DTCT_PIN											(10)
#define TCH_DTCT_GPIO                              PTE
#define TCH_DTCT_PORT                              PORTE	

#define TCH_DTCT_IRQHandler								PORTE_IRQHandler
#define TCH_DTCT_IRQ											PORTE_IRQn
/********************** Macros ***********************/

/* EOF */
