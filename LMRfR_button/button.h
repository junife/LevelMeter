/*
*********************************************************************************************************

* Copyright 2008 The Watt Stopper / Legrand
* as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of The Watt Stopper / Legrand.
* The use, copying, transfer or disclosure of such
* information is prohibited except by express written
* agreement with The Watt Stopper / Legrand.
*
* Project:PW/DSW 31x
* MCU:Atmega168P
* Compile condition: AVR Studio 6
* First written on March 2015, by Corner Hu.
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

#include  <stdint.h>
#include  <stdbool.h>

#ifdef   BUTTON_GLOBALS
#define  BUTTON_EXT
#else
#define  BUTTON_EXT  extern
#endif

/*
*********************************************************************************************************
*                                          BUTTON DATA TYPE DEFINE
* Choose uint8_t/uint16_t/uint32_t will support 8/16/32 buttons. 
*********************************************************************************************************
*/
#define ButtonDataType uint32_t
/*
*********************************************************************************************************
*                                          DEFINE BUTTON BITs STRUCT
* Button Maps 32 bits for RF remote, more details see hardware schematic.
*	19		18		17		16
*	PBtn17	PBtn18	PBtn19	PBtn20
*	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0
*	PBtn1	PBtn5	PBtn9	PBtn13	PBtn2	PBtn6	PBtn10	PBtn14	PBtn3	PBtn7	PBtn11	PBtn15	PBtn4	PBtn8	PBtn12	PBtn16
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
*                                           BUTTON CONSTANT
*********************************************************************************************************
*/
#define BTN_DOUBLE_TAP			(1*60)	/* button two times press interval for get a twice tap event */
#define BTN_MIN_HOLD_60			(35)	/* bigger than this value will assume as hold */
#define BTN_MIN_PRESS_60		(2)		/* bigger than this value will assume as press, else as debounce */

/*
*********************************************************************************************************
*                                          Button buffer size define
*********************************************************************************************************
*/
#define BUTTON_BUFFER_SIZE 		2

BUTTON_EXT  void     ButtonInit             (void);
BUTTON_EXT  void     ButtonCycleUpdate      (void);
BUTTON_EXT  uint16_t ButtonGetHoldTime      (void);
BUTTON_EXT  ButtonDataType  ButtonGetReleased      (void);
BUTTON_EXT  ButtonDataType  ButtonGetHeld          (void);
BUTTON_EXT  ButtonDataType  ButtonGetTwiceTapEvent (void);
BUTTON_EXT  bool	 ButtonDownTapEvent     (void);
BUTTON_EXT  bool  	 ButtonB1TwiceTapEvent  (void);
BUTTON_EXT  bool  	 ButtonB1HeldEvent      (void);
BUTTON_EXT  bool  	 ButtonUpTapEvent       (void);
BUTTON_EXT  bool	 ButtonB2TwiceTapEvent	(void);
BUTTON_EXT  bool	 ButtonB2HeldEvent      (void);
BUTTON_EXT BUTTON_DATA button;

#endif	/* endif BUTTON_H */
