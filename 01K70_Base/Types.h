#if (!defined(__TYPES_H))
#define __TYPES_H

/********************* Includes **********************/
#include "MK70F12.h"
/********************* Includes **********************/

/********************* Macros **********************/
#define false 								((uint8_t)0)
#define true 								((uint8_t)1)							
/********************* Macros **********************/

#define ERR_OK								((uint8_t)0)
#define ERR_BUSY							((uint8_t)1)
#define ERR_MEMFULL						((uint8_t)2)

#define BUSY								((uint8_t)0)
#define DONE								((uint8_t)1)

typedef uint8_t bool;
typedef uint8_t uint_8;

#endif

/* EOF */
