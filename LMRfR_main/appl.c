#include <util/delay.h>
#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "button.h"
#include "ks0108.h"

char greeting[20];

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
	
	glcdPutStr(&greeting[0]);
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
void AppCycleUpdate(void)
{	
	static cnt;
	//if(button.buttonsReleased.bVal) rprintf("rl=%x\n", button.buttonsReleased.bVal);
	if(button.buttonsReleased.bVal) 
	{
		rprintf("rl=%x\n", button.buttonsReleased.bVal);
		rprintf("rh=%x\n", button.buttonsReleased.bVal>>16);
		
		if(button.buttonsReleased.PBtn13)
		{
			glcdSetAddress(0,0);
			char greeting[] = "PBtn13,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn13\n");
		}
		else if(button.buttonsReleased.PBtn14)
		{
			glcdSetAddress(0,1);
			char greeting[] = "PBtn14,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn14\n");
		}
		else if(button.buttonsReleased.PBtn15)
		{
			glcdSetAddress(0,2);
			char greeting[] = "PBtn15,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn15\n");
		}
		else if(button.buttonsReleased.PBtn1)
		{
			glcdSetAddress(0,3);
			char greeting[] = "PBtn1,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn1\n");
		}
		else if(button.buttonsReleased.PBtn5)
		{
			glcdSetAddress(0,4);
			char greeting[] = "PBtn5,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn5\n");
		}
		else if(button.buttonsReleased.PBtn9)
		{
			glcdSetAddress(0,5);
			char greeting[] = "PBtn9,Hello World!";
			glcdPutStr(&greeting[0]);
			rprintf("PBtn9\n");
		}
		else if(button.buttonsReleased.PBtn18)
		{
			//glcdSetAddress(0,1);
			glcdClearScreen();
			rprintf("PBtn18,Clear Home\n");
		}
	}	
	//rprintfChar('a');
	//glcdWriteChar(cnt++/2);
}
