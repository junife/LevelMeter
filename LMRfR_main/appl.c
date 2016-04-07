#include <util/delay.h>
#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "button.h"
#include "ks0108.h"

/*
*********************************************************************************************************
*                                         MCU INITIALIZATION
*
* Description : This function should be called by main function olny once before main loop.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/

void McuInit(void)
{
	// initialize our libraries
	// initialize the UART (serial port)
	uartInit();		/* sei() call in the function */
	// set the baud rate of the UART for our debug/reporting output
	uartSetBaudRate(9600);
	// initialize rprintf system
	rprintfInit(uartSendByte);
}
/*
*********************************************************************************************************
*                                         APPLICATION INITIALIZATION
*
* Description : This function should be called by main function only once before main loop, it will
*               initial all application initiallization.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/

void ApplInit(void)
{
	ButtonInit();
	
	glcdInit();	/* Need connect to LCD device because MCU will check lcd busy or not will cause waiting */
	
	glcdWriteChar('H');
	glcdWriteChar('E');
	glcdWriteChar('L');
	glcdWriteChar('L');
	glcdWriteChar('O');

	// Just print message for debug.
	rprintf("RF remote start!\n" );	
}

/*
*********************************************************************************************************
*                                         AppCycleUpdate
*
* Description : This function should be called by loop each power cycle.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/
char greeting[] = "a";
void AppCycleUpdate(void)
{	
	//if(button.buttonsReleased.bVal) rprintf("rl=%x\n", button.buttonsReleased.bVal);
	if(button.buttonsReleased.bVal) 
	{
		//rprintf("rl=%x\n", button.buttonsReleased.bVal);
		//rprintf("rh=%x\n", button.buttonsReleased.bVal>>16);
		//rprintfStr("a\n");
		//rputchar("\n");
		
	}
	
	//rprintfChar('a');
	glcdWriteChar('A');
	rprintf("\r\n\n\nWelcome to the timer library test program!\r\n");
}