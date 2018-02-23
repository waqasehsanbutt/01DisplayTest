/********************* Includes **********************/
#include "GeneralHeader.h"
/********************* Includes **********************/

/********************* Macros **********************/
#define CAL_VAL                         	(0)
#define AN                              	(1)
#define BN                              	(2)
#define CN                              	(3)
#define DN                              	(4)
#define EN                              	(5)
#define FN                              	(6)

#define CAL_RADIUS								(7)
/********************* Macros **********************/

/********************* Structs **********************/
typedef struct POINT_2D* POINT_2D_PTR;
typedef struct POINT_2D POINT_2D_STRUCT;
struct POINT_2D{
   int16_t x, y;
};
typedef struct CAL_PARAMS* CAL_PARAMS_PTR;
typedef struct CAL_PARAMS CAL_PARAMS_STRUCT;
struct CAL_PARAMS {
    long An, Bn, Cn, Dn, En, Fn, V;
};

/********************* Structs **********************/

/******************** Variables *********************/
extern CAL_PARAMS_PTR cal;
/******************** Variables *********************/

extern void TouchPanelCalibration_Init(void);
extern bool MakeCalibrationParams(POINT_2D_PTR pDisplayPoints, POINT_2D_PTR pTouchPoints);
