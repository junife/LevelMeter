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
* This file list all function for button, such as initialize, update button state each power cycle. And 
all button APIs called by core.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#ifndef   BUTTON_GLOBALS
#define   BUTTON_GLOBALS
#endif

#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <util/delay.h>

#include "button.h"
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
/*
*********************************************************************************************************
*                                       VARIABLE DEFINITION
*********************************************************************************************************
*/
BUTTON_DATA button;

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static ButtonDataType ButtonGetIO_State(void);

/*
*********************************************************************************************************
*                                         BUTTON INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.Initial all data and variables for button API.It need to 
*               call before main loop.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/

void ButtonInit(void)
{
	DDRD  &= ~((1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3));	/* Set PD6-PD3 as input */
	PORTD |= (1<<PD6) | (1<<PD5) | (1<<PD4) | (1<<PD3);			/* Set PD6-PD3 as internal pull up */
	
	/* 
	Set PC7-PC4 as input as internal pull up
	Set PC3-PC0 as Tri-state (Hi-Z) 
	*/
	DDRC   = (0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	PORTC  = (1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4) | (0<<PC3) | (0<<PC2) | (0<<PC1) | (0<<PC0);
}



/*
*********************************************************************************************************
*                                         ButtonCycleUpdate
*
* Description : This function call in main loop each power cycle to update button state, which including
*				button press/release/held/hold-time.
*
* Arguments   : none
*
* Notes      : 1) buttonHoldTime increase each power cycle and will clear as zero after any button event 
*				  occur.
*
*              2) buttonsReleased and buttonsTwiceReleased will clear next power cycle, so buttons event
*				  need used by application which call after ButtonCycleUpdate()
*
*********************************************************************************************************
*/

void ButtonCycleUpdate(void)
{
	ButtonDataType b;											/* store last power cycle button value */
	ButtonDataType buttons;
	uint16_t holdTime;
	
	buttons = ButtonGetIO_State();
	buttons |= button.buttonsNextCycle.bVal;			/* Add in any buttons asserted by software */	
	button.buttonsNextCycle.bVal = 0;					/* and clear any that may have been set */
	b = button.buttonsLastCycle.bVal;
	holdTime = button.buttonHoldTime;
	
	if (button.buttonsReleased.bVal != 0) {		
														/* store value of release button */
		button.buttonsReleasedHistory.bVal = button.buttonsReleased.bVal;	
	}
	else {
		if (holdTime > BTN_DOUBLE_TAP) {
														/* Clear history value when interval time out */
			button.buttonsReleasedHistory.bVal = 0;	
		}
	}

	button.buttonsReleased.bVal = 0;					/* Assume no buttons released this time */
	button.buttonsTwiceReleased.bVal = 0;				/* Assume no buttons twice released this time */
  
	if (buttons == b) {									/* If the button situation has not changed in this past cycle */ 
		button.buttonsHeld.bVal = 0;					/* Assume no buttons held */
		if ( ++holdTime > BTN_MIN_HOLD_60) {
			button.buttonsHeld.bVal = buttons;
		}	
		button.buttonHoldTime = holdTime;
	}
	else {
														/* Here if the buttons have changed since last line cycle */
		button.buttonsLastCycle.bVal = buttons;
		buttons = (buttons ^ b) & b;					/* Select out only the ones that have changed */
		
		if ((buttons & button.buttonsHeld.bVal) == 0){													
			button.buttonHoldTime = 0;					/* Ignore the release of any buttons that were previously marked as held down */
		
			if ((holdTime < BTN_MIN_HOLD_60) && (holdTime > BTN_MIN_PRESS_60)){
				button.buttonsReleased.bVal = buttons;	
														/* Calculate twice release button value */
				button.buttonsTwiceReleased.bVal =	button.buttonsReleased.bVal &   
													button.buttonsReleasedHistory.bVal;
			}       
		}
	}
}

/*
*********************************************************************************************************
*                                         ButtonReadIOs
*
* Description : This function called by ButtonCycleUpdate
*
* Arguments   : None.
*
* Returns    : Button IO value.
*
* Notes      : 1) Button IO general set as internal/external pull-up, so press button IO will low. However,
*			      button structure define high when button press, so need reverse IO input value so that keep
*                 consistently.
*
*              2) Bit location in one byte need to map with BUTTON_T structure.
*
*			   3) Eg: button1->PINA7,button1->PINA6. io_state = (~PINA6<<1) | (~PINA7<<0)
*
* Button Maps 32 bits for RF remote, more details see hardware schematic.
*	19		18		17		16
*	PB17	PB18	PB19	PB20
*	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0
*	PB1		PB5		PB9		PB13	PB2		PB6		PB10	PB14	PB3		PB7		PB11	PB15	PB4		PB8		PB12	PB16
*********************************************************************************************************
*/
ButtonDataType ButtonGetIO_State(void)
{
	ButtonDataType io_state = 0;
	ButtonDataType tempValue;
	ButtonDataType MatrixButtonValue;
	uint8_t Col;
	
	tempValue = (~PIND) & ((1<<PIND6) | (1<<PIND5) | (1<<PIND4) | (1<<PIND3));	/* Get PD6-PD3 IO state */
	tempValue >>= 3;	/* right rotate to low 4 bits location */
	
	for(Col = 0, MatrixButtonValue = 0; Col < 4; Col++)
	{
		/* 
		Set PC7-PC4 as input as internal pull up
		Set PC3-PC0 as Tri-state (Hi-Z)
		*/
		DDRC   = (0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
		PORTC  = (1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4) | (0<<PC3) | (0<<PC2) | (0<<PC1) | (0<<PC0);
	
		/* Set one Column as output with low */
		DDRC  |= 0x08>>Col;			/* Set the column as output */
		PORTC &= ~(0x08>>Col);		/* Set the column as low */
		
		/* read and save button state after the column which active as low */
		MatrixButtonValue <<= 4;	/* newer 4 button state store in lower four bits */
		MatrixButtonValue |= ((~PINC) & 0x00f0) >> 4;
	}
	
	io_state = (tempValue << 16) | MatrixButtonValue;
	return  io_state;
}

/*
*********************************************************************************************************
*                                         ButtonGetHoldTime
*
* Description : This function called by application and get button hold time
*
* Arguments   : none
*
* Returns    : button hold time
*
* Notes      : 1) This variable is a run time timer and clear after button press/release event occur.
*
*
*********************************************************************************************************
*/

uint16_t ButtonGetHoldTime(void)
{
	return button.buttonHoldTime;
}

/*
*********************************************************************************************************
*                                         ButtonGetReleased
*
* Description : This function called by application and get all buttons released state
*
* Arguments   : none
*
* Returns    : all buttons released state.
*
* Notes      : 1) if button one bit location is 1 means button have released, else not pressed. if the 
*				  function return 0 means no buttons released, else have button just released.
*
*
*********************************************************************************************************
*/

ButtonDataType ButtonGetReleased(void)
{
	return button.buttonsReleased.bVal;
}

/*
*********************************************************************************************************
*                                         ButtonGetHeld
*
* Description : This function called by application and get all buttons held state
*
* Arguments   : none
*
* Returns    : all buttons released state.
*
* Notes      : 1) if button one bit location is 1 means button have released, else not pressed. if the 
*				  function return 0 means no buttons released, else have button just released.
*
*
*********************************************************************************************************
*/

ButtonDataType ButtonGetHeld (void)
{
	return button.buttonsHeld.bVal;
}

/*
*********************************************************************************************************
*                                         ButtonGetTwiceTapEvent
*
* Description : This function called by application and get all buttons twice-press state
*
* Arguments   : none
*
* Returns    : all buttons released state.
*
* Notes      : None
*
*
*********************************************************************************************************
*/

ButtonDataType ButtonGetTwiceTapEvent(void)
{
	return button.buttonsTwiceReleased.bVal;
}

#if 0
/*
*********************************************************************************************************
*                                         ButtonDownTapEvent
*
* Description : This function called by application and get a button tap event
*
* Arguments   : none
*
* Returns    : == true          button have pressed in the past, now it is released.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonDownTapEvent (void)
{
	if (button.buttonsReleased.PB13) {
		return true;
	}
    
	return false;
}

/*
*********************************************************************************************************
*                                         ButtonDownTwiceTapEvent
*
* Description : This function called by application and get a button twice continuous tap event
*
* Arguments   : none
*
* Returns    : == true          button twice continuous tap event.
*              == false          no event
*********************************************************************************************************
*/

bool ButtonDownTwiceTapEvent (void)
{
	if (button.buttonsTwiceReleased.PB13) {
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonDownHeldEvent
*
* Description : This function called by application and get a button held event
*
* Arguments   : none
*
* Returns    : == true          button have held
*              == false          button doesn't held
*********************************************************************************************************
*/

bool ButtonDownHeldEvent (void)
{
	if (button.buttonsHeld.PB13) {
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonUpTapEvent
*
* Description : This function called by application and get a button tap event
*
* Arguments   : none
*
* Returns    : == true          button have pressed in the past, now it is released.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonUpTapEvent (void)
{
	if (button.buttonsReleased.PB13) {
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonUpTwiceTapEvent
*
* Description : This function called by application and get a button twice continuous tap event
*
* Arguments   : none
*
* Returns    : == true          button twice continuous tap event.
*              == false          no event
*********************************************************************************************************
*/

bool ButtonUpTwiceTapEvent(void)
{
	if (button.buttonsTwiceReleased.PB13) {
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonUpHeldEvent
*
* Description : This function called by application and get a button held event
*
* Arguments   : none
*
* Returns    : == true          button have held
*              == false          button doesn't held
*********************************************************************************************************
*/

bool ButtonUpHeldEvent(void)
{
	if (button.buttonsHeld.PB13) {
		return true;
	}
    
    return false;
}
#endif