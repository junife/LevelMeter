/*
*********************************************************************************************************
*
*
* Module Description:
* All LED APIs
*********************************************************************************************************
*/
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include "led.h"

#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library

#ifndef   LED_GLOBALS
#define   LED_GLOBALS
#endif

static void LEDUpdateState(void);

LED_DATA LEDs[LED_NUM];

/*
*********************************************************************************************************
*                                         LED INITIALIZATION
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
void LEDInit(void)
{	
	uint8_t i;
	
	/* 
	Set PB2-PB1 as output to drive to LEDs, default turn off all LEDs
	*/
	DDRB &= ~((1<<DDB2) | (1<<DDB1));
	DDRB |= ((1<<DDB2) | (1<<DDB1));
	PORTB |= ((1<<DDB2) | (1<<DDB1));
	
	for(i = 0; i < LED_NUM; i++)
	{
		LEDs[i].led.LEDRate = LED_OFF;
		LEDs[i].led.state = S_OFF;
		LEDs[i].flashTimer = LED_ONE_SEC;
	}
}

/*
*********************************************************************************************************
*                                         LEDCycleUpdate
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

void LEDCycleUpdate(void)
{
	LED_RATE rate;
	uint8_t ft;				/* flash timer */
	uint8_t i;

	for(i = 0; i < LED_NUM; i++)
	{
		rate = LEDs[i].led.LEDRate;
		ft = LEDs[i].flashTimer;
		
		ft += rate;
		if((rate == LED_OFF) || (ft > LED_ONE_SEC))
		{
			ft = 0;
			LEDs[i].led.state = S_OFF;
		}
		LEDs[i].flashTimer = ft;
		
		if((rate == LED_0HZ) || (ft > (LED_ONE_SEC/2)))
		{
			LEDs[i].led.state = S_ON;
		}
	}
	
	LEDUpdateState();
}

/*
*********************************************************************************************************
*                                         LEDUpdateState
*
* Description :  Update IO state to turn on/off LED
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDUpdateState(void)
{
	uint8_t state = 0;		/* default is turn off LED */

	if(LEDs[0].led.state == S_OFF)
	{
		state |= (1<<DDB1);
	}
	
	if(LEDs[1].led.state == S_OFF)
	{
		state |= (1<<DDB2);
	}

	PORTB &= ~((1<<DDB2) | (1<<DDB1));
	PORTB |= state;
}

/*
*********************************************************************************************************
*                                         LEDFlag1Set
*
* Description :  Set rate for LED flag1
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1Set(LED_RATE rate)
{
	LEDs[0].led.LEDRate = rate;
}

/*
*********************************************************************************************************
*                                         LEDFlag1On
*
* Description : LED turn on
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1On(void)
{
	LEDFlag1Set(LED_0HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash1Hz
*
* Description : LED flash at 1 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1Flash1Hz(void)
{
	LEDFlag1Set(LED_1HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash2Hz
*
* Description : LED flash at 2 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1Flash2Hz(void)
{
	LEDFlag1Set(LED_2HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash3Hz
*
* Description : LED flash at 3 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1Flash3Hz(void)
{
	LEDFlag1Set(LED_3HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1On
*
* Description : LED turn off
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag1Off(void)
{
	LEDFlag1Set(LED_OFF);
}

/*
*********************************************************************************************************
*                                         LEDFlag2Set
*
* Description :  Set rate for LED flag2
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2Set(LED_RATE rate)
{
	LEDs[1].led.LEDRate = rate;
}

/*
*********************************************************************************************************
*                                         LEDFlag1On
*
* Description : LED turn on
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2On(void)
{
	LEDFlag2Set(LED_0HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash1Hz
*
* Description : LED flash at 1 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2Flash1Hz(void)
{
	LEDFlag2Set(LED_1HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash2Hz
*
* Description : LED flash at 2 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2Flash2Hz(void)
{
	LEDFlag2Set(LED_2HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1Flash3Hz
*
* Description : LED flash at 3 Hz
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2Flash3Hz(void)
{
	LEDFlag2Set(LED_3HZ);
}

/*
*********************************************************************************************************
*                                         LEDFlag1On
*
* Description : LED turn off
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void LEDFlag2Off(void)
{
	LEDFlag2Set(LED_OFF);
}


