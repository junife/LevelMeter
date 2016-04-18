/*
*********************************************************************************************************
*
* Module Description:
* This file list all extern function about button, define constant used by botton.
*********************************************************************************************************
*/

#ifndef BUTTON_DEP_H_
#define BUTTON_DEP_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include <stdint.h>
#include <stdbool.h>
#include "global.h"		// include our global settings

#ifdef   BUTTON_DEP_GLOBALS
#define  BUTTON_DEP_EXT
#else
#define  BUTTON_DEP_EXT  extern
#endif

/*
*********************************************************************************************************
*                                          BUTTON DATA TYPE DEFINE
* Choose uint8_t/uint16_t/uint32_t will support 8/16/32 buttons. BUTTON_DATA_TYPE define in global.h
*********************************************************************************************************
*/
#define ButtonDataType BUTTON_DATA_TYPE
/*
*********************************************************************************************************
*                                          DEFINE BUTTON BITs STRUCT
* Button Maps 32 bits for RF remote, more details see hardware schematic.
*	19		18		17		16
*	PBtn17	PBtn18	PBtn19	PBtn20
*	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0
*	PBtn1	PBtn5	PBtn9	PBtn13	PBtn2	PBtn6	PBtn10	PBtn14	PBtn3	PBtn7	PBtn11	PBtn15	PBtn4	PBtn8	PBtn12	PBtn16
*
* Butto map in board
PBtn18		PBtn17		PBtn20
	PBtn16		PBtn19

PBtn13		PBtn14		PBtn15
PBtn1 		PBtn5 		PBtn9
PBtn2 		PBtn6 		PBtn10
PBtn3 		PBtn7 		PBtn11
PBtn4 		PBtn8 		PBtn12
*********************************************************************************************************
*/
typedef struct button_t{
	union {
		ButtonDataType bVal;						/* whole buttons value */
		struct {
			ButtonDataType  PBtn16:1;					/* flag single button */
			ButtonDataType  PBtn12:1;					/* flag single button */
			ButtonDataType  PBtn8:1;					/* flag single button */
			ButtonDataType  PBtn4:1;					/* flag single button */
			ButtonDataType  PBtn15:1;					/* flag single button */
			ButtonDataType  PBtn11:1;					/* flag single button */
			ButtonDataType  PBtn7:1;					/* flag single button */
			ButtonDataType  PBtn3:1;					/* flag single button */
			ButtonDataType  PBtn14:1;					/* flag single button */
			ButtonDataType  PBtn10:1;					/* flag single button */
			ButtonDataType  PBtn6:1;					/* flag single button */
			ButtonDataType  PBtn2:1;					/* flag single button */
			ButtonDataType  PBtn13:1;					/* flag single button */
			ButtonDataType  PBtn9:1;					/* flag single button */
			ButtonDataType  PBtn5:1;					/* flag single button */
			ButtonDataType  PBtn1:1;					/* flag single button */
			ButtonDataType  PBtn20:1;					/* flag single button */
			ButtonDataType  PBtn19:1;					/* flag single button */
			ButtonDataType  PBtn18:1;					/* flag single button */
			ButtonDataType  PBtn17:1;					/* flag single button */						
		};
	};
} BUTTON_T;

BUTTON_DEP_EXT void ButtonInitEx(void);
BUTTON_DEP_EXT ButtonDataType ButtonGetIO_StateEx(void);

#endif	/* endif BUTTON_H */
