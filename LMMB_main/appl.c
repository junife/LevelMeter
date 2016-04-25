#ifndef   SSD_GLOBALS
#define   SSD_GLOBALS
#endif

#include <util/delay.h>
#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "button.h"
#include "button_dep.h"
#include "led.h"
#include "ssd.h"
#include "appl.h"
#include "timer.h"

cBuffer uartRxBuffer;				///< uart receive buffer
char greeting[20];
void UartRxFuncXX(unsigned char c);
char testChar;

/*
*********************************************************************************************************
*                                          APPLICATION VARIABLE DEFINE
*********************************************************************************************************
*/
APPL_DATA appl;

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void AppSelfDet(void);
static void AppUpdateTimers(void);
static void ApplDataInit(void);
static void AppDisplay(DISP_CODE DispCode, WORK_MODE mode);

void timerTest(void);

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
	
	// run the test
	timerTest();
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
	ApplDataInit();
	
	rprintf("Main board here!\n" );	
}

/*
*********************************************************************************************************
*                                         APPLICATION DATA INITIALIZATION
*
* Description : This function should be called by main function only once before main loop, it will
*               initial all application initiallization.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/

void ApplDataInit(void)
{
	/* Following for initial power on display steps */
	appl.workMode = PWR_ON;
	appl.CurEvent = NULL_EVENT;
	appl.LastEvent = NULL_EVENT;
	appl.second = 0;
	appl.ModeTimer = TIME_SWITCH_TOTAL;
	appl.DispCode= START_CHAR;
	appl.PwrOnSteps = DISP_START;
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
	
	AppUpdateTimers();

	/* work mode convert according to ModeTimer and push button events */
	switch(appl.workMode)
	{
		case PWR_ON:
			if((appl.ModeTimer != TIMER_INT_DISABLED && appl.ModeTimer != TIMER_TIMEOUT))
			{
				if(appl.ModeTimer % (TIME_CONTENT_SWITCH - 1) == 0)
				{
					AppDisplay(++appl.DispCode,PWR_ON);
				}
				
				if(ButtonJMP1HeldEvent() && (ButtonGetHoldTime() == TIME_B_HELD_RST))	/* Press/Held JMP1 after power on will set to factory default */
				{
					/* Following for initial power on display steps */
					appl.ModeTimer = TIME_SWITCH_TOTAL;
					appl.workMode = PWR_ON;
					appl.LastEvent = JP1_HELD;
					appl.DispCode= START_CHAR;
				}
			}
			else if(appl.ModeTimer == TIMER_TIMEOUT)
			{
				appl.ModeTimer = TIMER_INT_DISABLED;	/* Disable count */
				appl.workMode = NORMAL;					/* convert to NORMAL */
				appl.LastEvent = NULL_EVENT;			/* update  LastEvent */
				appl.DispCode = CURRENT_VALUE;			/* Initial to display material value in setting mode */
			}
			break;
			
		case NORMAL:			
			if(ButtonGetReleased())
			{
				if(ButtonJMP2ReleasedEvent())
				{
					appl.ModeTimer = TIME_D_CODE_HELD;	/* Disable count */
					appl.workMode = SETTING; 			/* convert to SETTING */
					appl.LastEvent = JP2_PRESS;			/* update  LastEvent */
				}
				else if(ButtonJMP1ReleasedEvent())
				{
				}
			}
			else if(ButtonGetHeld())
			{
				if(ButtonJMP2HeldEvent())//(ButtonJMP2HeldEvent() && (ButtonGetHoldTime()%(TIMER_1_SEC_60HZ/2) == 0))
				{
					appl.ModeTimer = TIME_D_CODE_HELD;	/* Disable count */
					appl.workMode = SETTING; 			/* convert to SETTING */
					appl.LastEvent = JP2_HELD;			/* update  LastEvent */
				}
				else if(ButtonJMP1HeldEvent())
				{
				}
			}
			else
			{
				/* Display current material, and display color change according to value compare with user setting */
				AppDisplay(appl.DispCode,NORMAL);
			}
			break;
			
		case SETTING:
			if((appl.ModeTimer != TIMER_INT_DISABLED && appl.ModeTimer != TIMER_TIMEOUT))
			{
				if(ButtonGetReleased())
				{
					if(ButtonJMP2ReleasedEvent())
					{
						if(--appl.DispCode < CURRENT_VALUE)
						{
							appl.DispCode = REMOTE_ADDR;
						}
						AppDisplay(appl.DispCode,SETTING);
						
						appl.ModeTimer = TIME_D_CODE_HELD;	/* set the time of mantain code display */
						appl.workMode = SETTING;			/* convert to SETTING */
						appl.LastEvent = JP2_PRESS; 		/* update  LastEvent */
					}
					else if(ButtonJMP1ReleasedEvent())
					{
						SSDDisplayDec(--appl.FullWarehouse, SSD_0HZ, SSD_GREEN);
						
						appl.ModeTimer = TIME_D_CODE_HELD;	/* set the time of mantain code display */
						appl.workMode = SETTING;			/* convert to SETTING */
						appl.LastEvent = JP1_PRESS;			/* update  LastEvent */
					}
				}
				else if(ButtonGetHeld())
				{
					if(ButtonJMP2HeldEvent() && (ButtonGetHoldTime()%(TIMER_1_SEC_60HZ/6) == 0))	/* fast increase */
					{
						if(++appl.DispCode > REMOTE_ADDR)
						{
							appl.DispCode = CURRENT_VALUE;
						}
						AppDisplay(appl.DispCode,SETTING);
						
						appl.ModeTimer = TIME_D_CODE_HELD;	/* set the time of mantain code display */
						appl.workMode = SETTING;			/* convert to SETTING */
						appl.LastEvent = JP2_HELD;			/* update  LastEvent */
					}
					else if(ButtonJMP1HeldEvent())
					{
						SSDDisplayDec(++appl.FullWarehouse, SSD_0HZ, SSD_AMBER);
						
						appl.ModeTimer = TIME_D_CODE_HELD;	/* set the time of mantain code display */
						appl.workMode = SETTING;			/* convert to SETTING */
						appl.LastEvent = JP1_HELD;			/* update  LastEvent */
					}
				}
				else
				{
				
				}
			}
			else if(appl.ModeTimer == TIMER_TIMEOUT)
			{
				appl.ModeTimer = TIMER_INT_DISABLED;	/* Disable count */
				appl.workMode = NORMAL; 				/* convert to NORMAL */
				appl.LastEvent = NULL_EVENT;			/* update  LastEvent */
				appl.DispCode = CURRENT_VALUE;			/* Initial to display material value in setting mode */
			}
			break;

			
		default:
			break;						
	}

#if 1		
#if (FOR_DEBUG==1)
	rprintf("wM=%d\n",appl.workMode);
	rprintf("mT=%d,0x%x\n",appl.ModeTimer,appl.ModeTimer);
	rprintf("dC=%d\n\n",appl.DispCode);
	rprintf("lE=%d\n\n",appl.LastEvent);
#endif
#endif
	
#if 0	
	if(AppPwrOnCtrl() == true)
	{
		if(ButtonGetReleased())
		{
			if(ButtonJMP2ReleasedEvent())
			{
				if(--appl.DispCode > REMOTE_ADDR)
				{
					appl.DispCode = REMOTE_ADDR;
				}
				LEDFlag1On();
				SSDDisplayDec(appl.DispCode, SSD_0HZ, SSD_GREEN);
			}
			else if(ButtonJMP1ReleasedEvent())
			{
				//rprintf("JMP1\n");
				//LEDFlag1Off();
				//LEDFlag2Off();
				//SSDDisplayHex(0x1abc, SSD_0HZ, SSD_RED);
			}
		}
		else if(ButtonGetHeld())
		{
			if(ButtonJMP2HeldEvent() && (ButtonGetHoldTime()%(TIMER_1_SEC_60HZ/2) == 0))
			{
				if(++appl.DispCode > REMOTE_ADDR)
				{
					appl.DispCode = CURRENT_VALUE;
				}
				
				LEDFlag1Off();
				SSDDisplayDec(appl.DispCode, SSD_0HZ, SSD_RED);
			}
			else if(ButtonJMP1HeldEvent())
			{
				rprintf("H-JMP1\n");
				LEDFlag1Flash3Hz();
				LEDFlag2Flash3Hz();
				//SSDDisplayHex(0x1def, SSD_3HZ, SSD_RED);
			}
		}
		
		if(uartRxBuffer.size - bufferIsNotFull(&uartRxBuffer) != 0)
		{
			tempChar = bufferGetFromFront(&uartRxBuffer);
			//rprintf("char=%c\n",tempChar);
			rprintf("Type=%c\n",tempChar);
		}
	}
	else
	{
		if(ButtonJMP1HeldEvent() && (ButtonGetHoldTime() == TIME_B_HELD_RST))
		{
			/* Following for initial power on display steps */
			appl.PwrOnSteps = DISP_START;
			appl.ModeTimer = TIME_SWITCH_TOTAL;
		}
	}
#endif	
}

/*
*********************************************************************************************************
*                                         AppUpdateTimers
*
* Description : This function should be called by loop each power cycle to update timers which used by applications.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/
void AppUpdateTimers(void)
{
	if(++appl.second > TIMER_1_SEC_60HZ) 
	{
		appl.second = 0;
#if 0		
#if (FOR_DEBUG==1)
			rprintf("wM=%d\n",appl.workMode);
			rprintf("mT=%d,0x%x\n",appl.ModeTimer,appl.ModeTimer);
			rprintf("dC=%d\n\n",appl.DispCode);
#endif
#endif
		
	}
	
	if(appl.ModeTimer != TIMER_TIMEOUT && appl.ModeTimer != TIMER_INT_DISABLED) appl.ModeTimer--;
}

#if 0
/*
*********************************************************************************************************
*                                         AppUpdateTimers
*
* Description : This function should be called by loop each power cycle to update timers which used by applications.
*
* Arguments   : none
*
* Returns    : true, power on control has finised. false, is processing
*********************************************************************************************************
*/
void AppPwrOnCtrl(void)
{
	if(appl.ModeTimer % (TIME_CONTENT_SWITCH - 1) == 0)
	{
		switch (appl.PwrOnSteps)
		{
			case DISP_START:
				SSDDisplayDec(DISP_START, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_RF_ADDR:
				SSDDisplayDec(DISP_RF_ADDR, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_CUR_MAT:
				SSDDisplayDec(DISP_CUR_MAT, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_FULL_WH:
				SSDDisplayDec(DISP_FULL_WH, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_EMPTY_WH:
				SSDDisplayDec(DISP_EMPTY_WH, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_H_PERCENT:
				SSDDisplayDec(DISP_H_PERCENT, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_L_PERCENT:
				SSDDisplayDec(DISP_L_PERCENT, SSD_0HZ, SSD_RED);
				break;
				
			case DISP_RP_TIME:
				SSDDisplayDec(DISP_RP_TIME, SSD_0HZ, SSD_RED);
				break;
				
			default:
				break;						
		}
		appl.PwrOnSteps++;
	}
}
#endif

/*
*********************************************************************************************************
*                                         AppUpdateTimers
*
* Description : This function should be called by loop each power cycle to update timers which used by applications.
*
* Arguments   : none
*
* Returns    : true, power on control has finised. false, is processing
*********************************************************************************************************
*/
void AppDisplay(DISP_CODE DispCode, WORK_MODE mode)
{
	switch(DispCode)
	{
		case START_CHAR:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(START_CHAR, SSD_0HZ, SSD_RED);
			break;
			
		case CURRENT_VALUE:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(CURRENT_VALUE, SSD_0HZ, SSD_RED);
			break;
			
		case FULL_WAREHOUSE:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(FULL_WAREHOUSE, SSD_0HZ, SSD_RED);
			break;
			
		case EMPTY_WAREHOUSE:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(EMPTY_WAREHOUSE, SSD_0HZ, SSD_RED);
			break;
			
		case ALARM_H_PERCENT:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(ALARM_H_PERCENT, SSD_0HZ, SSD_RED);
			break;
			
		case ALARM_L_PERCENT:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(ALARM_L_PERCENT, SSD_0HZ, SSD_RED);
			break;
			
		case RESPONSE_TIME:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(RESPONSE_TIME, SSD_0HZ, SSD_RED);
			break;
			
		case REMOTE_ADDR:
			if(mode == PWR_ON)
			{

			}
			else if(mode == NORMAL)
			{

			}
			else	/* setting mode */
			{

			}
			SSDDisplayDec(REMOTE_ADDR, SSD_0HZ, SSD_RED);
			break;
			
		default:
			break;						
	}
}

/*
*********************************************************************************************************
*                                         AppSelfDet
*
* Description : MCU self detection hardware and then set error code.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/
void AppSelfDet(void)
{
	appl.fSelfDetDone = true;
	appl.fSelfDetFail = false;
}

/*
*********************************************************************************************************
*                                         AppSelfDetGetState
*
* Description : Get self detection state to check wheter finish or not.
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/
bool AppSelfDetGetState(void)
{
	if(appl.fSelfDetDone == true)
	{
		return true;
	}
	return false;
}

/*
*********************************************************************************************************
*                                         AppSelfDetGetResult
*
* Description : Get self detection result.
*
* Arguments   : none
*
* Returns    : true/false means failed or pass
*********************************************************************************************************
*/
bool AppSelfDetGetResult(void)
{
	if(appl.fSelfDetFail == true)
	{
		return true;
	}
	return false;
}

void timerTest(void)
{
	// print a little intro message so we know things are working
	rprintf("\r\n\n\nWelcome to the timer library test program!\r\n");

	// initialize the timer system
	timerInit();

	// to use the internal timers to produce a calibrated delay,
	// use the timerPause() function.  timerPause takes a 16-bit
	// integer argument in milliseconds

	// example: wait for 1/2 of a second, or 500ms
	rprintf("\r\nTest of timerPause() function\r\n");
	rprintf("Here comes a 1/2-second delay...\r\n");
	timerPause(500);
	rprintf("Done!\r\n");


	// here's an example of using the timer library to do
	// pulse-width modulation or PWM.  PWM signals can be created on
	// any output compare (OCx) pin.  See your processor's data sheet
	// for more information on which I/O pins have output compare
	// capability.
	rprintf("\r\nTest of timer1 PWM output\r\n");

	// set the OC1x port pins to output
	// We need to do this so we can see and use the PWM signal
	// ** these settings are correct for most processors, but not for all
	sbi(DDRD, PD4);
	sbi(DDRD, PD5);

	// initialize timer1 for PWM output
	// - you may use 8,9, or 10 bit PWM resolution
	rprintf("Initializing timer1 for PWM\r\n");
	timer1PWMInit(8);

	// turn on the channel A PWM output of timer1
	// - this signal will come out on the OC1A I/O pin
	rprintf("Turning on timer1 channel A PWM output\r\n");
	timer1PWMAOn();

	// set the duty cycle of the channel A output
	// - let's try 25% duty, or 256*25% = 64
	rprintf("Setting duty cycle to 25%%\r\n");
	timer1PWMASet(254);

	// turn on channel B and set it to 75% duty cycle
	rprintf("Turning on channel B too, with 75%% duty\r\n");
	timer1PWMBOn();
	timer1PWMBSet(1);
	
#if 0
	// wait for 5 seconds
	rprintf("Pause for 5 seconds...\r\n");
	timerPause(5000);

	// now turn off all PWM on timer1
	rprintf("Turning off all PWM on timer1\r\n");
	timer1PWMOff();
#endif	
}

