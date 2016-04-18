/*
*********************************************************************************************************
*
* Module Description:
* This file list all extern function about button, define constant used by botton.
*********************************************************************************************************
*/

#ifndef BUTTON_H_
#define BUTTON_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "global.h"		// include our global settings
#include "button_dep.h"

#ifdef   BUTTON_GLOBALS
#define  BUTTON_EXT
#else
#define  BUTTON_EXT  extern
#endif

/*
*********************************************************************************************************
*                                          DEFINE BUTTON STRUCT AND ALL VARIABLE
*********************************************************************************************************
*/
typedef struct button_data {
	BUTTON_T	buttonsNextCycle;       /* Buttons to be ORed into the buttons detected as pressed for next cycle */
	BUTTON_T	buttonsLastCycle;       /* Buttons seen pressed in last line cycle (read-only) */
	BUTTON_T	buttonsReleased;        /* The buttons just released, for the UI of this channel */
	BUTTON_T	buttonsHeld;            /* The buttons presently being held down, for the UI of this channel */
	BUTTON_T	buttonsReleasedHistory; /* Just judge button double release during 2 second */
	BUTTON_T	buttonsTwiceReleased;	/* The buttons just twice released during 2 seconds, for the UI of this channel */
	uint16_t    buttonHoldTime;         /* If buttonsHeld != 0, the hold time, in cycles, of the present button pattern */
} BUTTON_DATA;

/*
*********************************************************************************************************
*                                           BUTTON CONSTANT FOR DEBOUNCE
*********************************************************************************************************
*/
#define BTN_DOUBLE_TAP			(1*60)	/* button two times press interval for get a twice tap event */
#define BTN_MIN_HOLD_60			(35)	/* bigger than this value will assume as hold */
#define BTN_MIN_PRESS_60		(2)		/* bigger than this value will assume as press, else as debounce */

BUTTON_EXT  void     ButtonInit             (void);
BUTTON_EXT  void     ButtonCycleUpdate      (void);
BUTTON_EXT  uint16_t ButtonGetHoldTime      (void);
BUTTON_EXT  ButtonDataType  ButtonGetReleased      (void);
BUTTON_EXT  ButtonDataType  ButtonGetHeld          (void);
BUTTON_EXT  ButtonDataType  ButtonGetTwiceTap (void);
BUTTON_EXT BUTTON_DATA button;

#endif	/* endif BUTTON_H */
