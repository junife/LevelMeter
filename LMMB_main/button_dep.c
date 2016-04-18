/*
*********************************************************************************************************
*
*
* Module Description:
* All button hardware drivers for button.c/button.h
*********************************************************************************************************
*/


#ifndef   BUTTON_DEP_GLOBALS
#define   BUTTON_DEP_GLOBALS
#endif

#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include "button_dep.h"
#include "button.h"

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
void ButtonInitEx(void)
{	
	/* 
	Set PC1-PC0 as input, the tow IO with external pull up 10K resistor.
	*/
	DDRC &= ((1<<DDC1) | (1<<DDC0));
}

/*
*********************************************************************************************************
*                                         ButtonGetIO_StateEx
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
* Button Maps 2 bits(Bit1 and bit0) connect to PC1(JMP2) and PC0(JMP1) in main board, more details see hardware schematic.
*********************************************************************************************************
*/
ButtonDataType ButtonGetIO_StateEx(void)
{
	ButtonDataType io_state;
	
	io_state = (~PINC) & ((1<<PINC1) | (1<<PINC0));	/* Get PC1-PC0 IO state */
	
	return  io_state;
}

/*
*********************************************************************************************************
*                                         ButtonJM1ReleasedEvent
*
* Description : This function called by application and get a button Released event
*
* Arguments   : none
*
* Returns    : == true          button have pressed in the past, now it is released.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonJMP1ReleasedEvent(void)
{
	if (button.buttonsReleased.JMP1) 
	{
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonJM1HeldEvent
*
* Description : This function called by application and get a button held event
*
* Arguments   : none
*
* Returns    : == true          button have pressed and held.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonJMP1HeldEvent(void)
{
	if (button.buttonsHeld.JMP1) 
	{
		return true;
	}
    
    return false;
}
/*
*********************************************************************************************************
*                                         ButtonJM2ReleasedEvent
*
* Description : This function called by application and get a button Released event
*
* Arguments   : none
*
* Returns    : == true          button have pressed in the past, now it is released.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonJMP2ReleasedEvent(void)
{
	if (button.buttonsReleased.JMP2) 
	{
		return true;
	}
    
    return false;
}

/*
*********************************************************************************************************
*                                         ButtonJM2HeldEvent
*
* Description : This function called by application and get a button held event
*
* Arguments   : none
*
* Returns    : == true          button have pressed and held.
*              == false          button doesn't press
*********************************************************************************************************
*/

bool ButtonJMP2HeldEvent(void)
{
	if (button.buttonsHeld.JMP2) 
	{
		return true;
	}
    
    return false;
}

