/*
*********************************************************************************************************
*
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

#include "button.h"
#include "button_dep.h"

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
	ButtonInitEx();
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
* Description : This function called by ButtonCycleUpdate to read hardware button IO state.
*
* Arguments   : None.
*
* Returns    : Button IO value.
*
* Notes      : None
*
*/
ButtonDataType ButtonGetIO_State(void)
{
	return  ButtonGetIO_StateEx();
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

ButtonDataType ButtonGetTwiceTap(void)
{
	return button.buttonsTwiceReleased.bVal;
}

