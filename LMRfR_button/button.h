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
*                                          DEFINE BUTTON BITs STRUCT
*********************************************************************************************************
*/
typedef struct button_t{
	union {
		uint8_t bVal;						/* whole buttons value */
		struct {
			uint8_t  Down:1;				/* flag single button */
			uint8_t  Up:1;					/* flag single button */
			uint8_t  unused2_7:6;
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
	uint8_t		dip9;					/* for DCx3 DIP9 combine with two push buttons, need to pick up from button ADC result */
} BUTTON_DATA;

/*
*********************************************************************************************************
*                                           BUTTON CONSTANT
*********************************************************************************************************
*/
#define BTN_DOUBLE_TAP			(1*60)	/* button two times press interval for get a twice tap event */
#define BTN_MIN_HOLD_60			(35)	/* bigger than this value will assume as hold */
#define BTN_MIN_PRESS_60		(4)		/* bigger than this value will assume as press, else as debounce */
#define BTN_ADC_OFFSET 			(4)		/* Offset value, set bigger than zero because actural ADC value out of range and a litter bigger than table value */ 
#define BTN_ADC_CH 				(1)		/* button adc channel */ 

/*
*********************************************************************************************************
*                                          Button buffer size define
*********************************************************************************************************
*/
#define BUTTON_BUFFER_SIZE 		2

BUTTON_EXT  void     ButtonInit             (void);
BUTTON_EXT  void     ButtonCycleUpdate      (void);
BUTTON_EXT  uint16_t ButtonGetHoldTime      (void);
BUTTON_EXT  uint8_t  ButtonGetReleased      (void);
BUTTON_EXT  uint8_t  ButtonGetHeld          (void);
BUTTON_EXT  uint8_t  ButtonGetTwiceTapEvent (void);
BUTTON_EXT  bool	 ButtonDownTapEvent     (void);
BUTTON_EXT  bool  	 ButtonB1TwiceTapEvent  (void);
BUTTON_EXT  bool  	 ButtonB1HeldEvent      (void);
BUTTON_EXT  bool  	 ButtonUpTapEvent       (void);
BUTTON_EXT  bool	 ButtonB2TwiceTapEvent	(void);
BUTTON_EXT  bool	 ButtonB2HeldEvent      (void);
BUTTON_EXT BUTTON_DATA button;

#endif	/* endif BUTTON_H */
