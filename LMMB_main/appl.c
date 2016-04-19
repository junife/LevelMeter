#include <util/delay.h>
#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "button.h"
#include "button_dep.h"
#include "led.h"
#include "ssd.h"

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
	LEDInit();
	SSDInit();
	
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
	static uint16_t cnt;
	cnt++;

	if(ButtonGetReleased())
	{
		if(ButtonJMP2ReleasedEvent())
		{
			rprintf("JMP2\n");
			LEDFlag1On();
			LEDFlag2On();
			SSDDisplayDec(9, SSD_0HZ, SSD_GREEN);
		}
		else if(ButtonJMP1ReleasedEvent())
		{
			rprintf("JMP1\n");
			LEDFlag1Off();
			LEDFlag2Off();
			SSDDisplayHex(0x1abc, SSD_0HZ, SSD_RED);
		}
	}

	if(ButtonGetHeld())
	{
		if(ButtonJMP2HeldEvent())
		{
			rprintf("H-JMP2\n");
			LEDFlag1Flash1Hz();
			LEDFlag2Flash1Hz();
			SSDDisplayDec(0x1def, SSD_1HZ, SSD_AMBER);
		}
		
		else if(ButtonJMP1HeldEvent())
		{
			rprintf("H-JMP1\n");
			LEDFlag1Flash3Hz();
			LEDFlag2Flash3Hz();
			SSDDisplayHex(0x1def, SSD_3HZ, SSD_RED);
		}
	}
	
	if(uartRxBuffer.size - bufferIsNotFull(&uartRxBuffer) != 0)
	{
		tempChar = bufferGetFromFront(&uartRxBuffer);
		//rprintf("char=%c\n",tempChar);
		rprintf("Type=%c\n",tempChar);
	}
}
