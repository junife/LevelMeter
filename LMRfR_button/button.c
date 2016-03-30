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

#include  <stdint.h>
#include  <stdbool.h>
#include  <avr/pgmspace.h>
#include  "button.h"
#include  "io_cfg.h"
#include  "adc.h"

#include  "global_x.h"
#include  "avr_lib.h"

/*
*********************************************************************************************************
*                                       VARIABLE DEFINITION
*********************************************************************************************************
*/

BUTTON_DATA button;
static uint16_t ButtonData[BUTTON_BUFFER_SIZE];
cBuffer ButtonDataBuffer;

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  uint8_t  ButtonGetIO_State (void);
static  uint8_t  MapKeyboard(uint16_t keyboardADC);	
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

void  ButtonInit (void)
{
	bufferInit(&ButtonDataBuffer, (uint16_t *)ButtonData, BUTTON_BUFFER_SIZE); /* Initial queue */
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

void  ButtonCycleUpdate (void)
{
	uint8_t  b;											/* store last power cycle button value */
	uint8_t  buttons;
	uint8_t  dip9;
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
		if ( ++holdTime >= BTN_MIN_HOLD_60) {
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
		
			if ((holdTime < BTN_MIN_HOLD_60) && (holdTime >= BTN_MIN_PRESS_60)){
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
*********************************************************************************************************
*/
uint8_t  ButtonGetIO_State (void)
{
	uint8_t  io_state;
	uint16_t  tempchar;
	
	tempchar 	= ReadADC10Bit(BTN_ADC_CH);		
	io_state 	= MapKeyboard(tempchar);
	
	/* buttonHoldTime is reset as 0 when any button state was changed */
	if(ButtonGetHoldTime()%35 == 0){	/* press up or down button cause middle state cause DIP9 change, add that if remove the error value */
		button.dip9 = io_state & 0x01;	/* mask tow button olny get dip9 value */
	}
	
	io_state >>= 1;						/* Get two buttons from ADC result */
	
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

uint16_t  ButtonGetHoldTime (void)
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

uint8_t  ButtonGetReleased (void)
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

uint8_t  ButtonGetHeld (void)
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

uint8_t  ButtonGetTwiceTapEvent(void)
{
	return button.buttonsTwiceReleased.bVal;
}

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

bool  ButtonDownTapEvent (void)
{
	if (button.buttonsReleased.Down) {
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

bool  ButtonDownTwiceTapEvent (void)
{
	if (button.buttonsTwiceReleased.Down) {
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

bool  ButtonDownHeldEvent (void)
{
	if (button.buttonsHeld.Down) {
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

bool  ButtonUpTapEvent (void)
{
	if (button.buttonsReleased.Up) {
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

bool  ButtonUpTwiceTapEvent (void)
{
	if (button.buttonsTwiceReleased.Up) {
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

bool  ButtonUpHeldEvent (void)
{
	if (button.buttonsHeld.Up) {
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         MapKeyboard
*
* Description : Convert ADC value to get button state
*
* Arguments   : buuton ADC value
*
* Returns    :  1 means button is pressed or DIP9 is on postion
*				value	SW1(Up)	SW2(Dwon)	DIP9(bit0)
					7	1		1			1
					6	1		1			0
					5	1		0			1
					4	1		0			0
					3	0		1			1
					2	0		1			0
					1	0		0			1
					0	0		0			0

*********************************************************************************************************
*/

uint8_t MapKeyboard(uint16_t keyboardADC)
{
	uint8_t i;
	uint16_t ADCTemp;
	uint16_t sum;
	uint16_t average;
	
	static const uint16_t keyboardMap[] PROGMEM = 
	{
		0X24E,
		0X278,
		0X2A9,
		0X2DF,
		0X31E,
		0X36E,
		0X3CE,
		0X3FF
	};
	
	ADCTemp = bufferGetFromFront(&ButtonDataBuffer);		/* pop oldest data from buffer */
	bufferAddToEnd(&ButtonDataBuffer, keyboardADC);			/* push new data to buffer */

	for(i=0,sum=0; i<BUTTON_BUFFER_SIZE; i++){
		sum += bufferGetAtIndex(&ButtonDataBuffer, i);		/* get data before or after index  */
	}

	average = sum/BUTTON_BUFFER_SIZE;
		
	i = 0;
    while ((pgm_read_word(&keyboardMap[i])+BTN_ADC_OFFSET) < average )
    {
		i++;
    }
	
	return i;
}

