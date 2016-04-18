#include <util/delay.h>
#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "button.h"
#include "button_dep.h"
//#include "ks0108.h"

cBuffer uartRxBuffer;				///< uart receive buffer
char greeting[20];
void UartRxFuncXX(unsigned char c);
char testChar;

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
	
	//glcdInit();	/* Need connect to LCD device because MCU will check lcd busy or not will cause waiting */
	
	//glcdPutStr(&greeting[0]);
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
	char tempChar;
	
	if(ButtonGetReleased())
	{
		if(ButtonJMP2ReleasedEvent())
		{
			rprintf("JMP2\n");
		}
		else if(ButtonJMP1ReleasedEvent())
		{
			rprintf("JMP1\n");
		}
	}
	
	if(ButtonGetHeld())
	{
		if(ButtonJMP2HeldEvent())
		{
			rprintf("H-JMP2\n");
		}
		
		else if(ButtonJMP1HeldEvent())
		{
			rprintf("H-JMP1\n");
		}
	}
	
	if(uartRxBuffer.size - bufferIsNotFull(&uartRxBuffer) != 0)
	{
		tempChar = bufferGetFromFront(&uartRxBuffer);
		//rprintf("char=%c\n",tempChar);
		rprintf("Type=%c\n",tempChar);
	}
}
