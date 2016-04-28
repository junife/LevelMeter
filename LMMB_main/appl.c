#ifndef   SSD_GLOBALS
#define   SSD_GLOBALS
#endif

//#include <util/delay.h>
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
#include "measure.h"
#include "adc.h"

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
static void AppUpdateTimers(void);
static void ApplDataInit(void);
static void AppDisplay(DISP_CODE DispCode, WORK_MODE mode);
static void AppOutputCompare0(void);

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
	/* Initial uart */
	uartInit();	
	uartSetBaudRate(9600);
	rprintfInit(uartSendByte);

	/* Initial ADC */
	ADCInit();
	
	/* Initial T0, T1 and T2 */
	timerInit();
	sbi(DDRD, PD4);		/* set PD4(OC1B) as output */
	sbi(DDRD, PD5);		/* set PD5(OC1A) as output */
	timer1PWMInit(8);	/* 8 bit PWM resolution, phase correct PWM mode, FREQpwm=F_CPU/(2*N*TOP) */
	timer1PWMAOn();		/* turn on the channel A PWM output of timer1 */
	timer1PWMBOn();		/* turn on the channel B PWM output of timer1 */
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
	MeasureInit();
	
	timerAttach(TIMER0OUTCOMPARE_INT, AppOutputCompare0);
	
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
				
				if(uartRxBuffer.size - bufferIsNotFull(&uartRxBuffer) != 0)
				{
					tempChar = bufferGetFromFront(&uartRxBuffer);
					//rprintf("char=%c\n",tempChar);
					rprintf("Type=%c\n",tempChar);
				}
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
						SSDDisplayDec(--appl.config.FullWarehouse[DATA_SET], SSD_0HZ, SSD_GREEN);
						
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
						SSDDisplayDec(++appl.config.FullWarehouse[DATA_SET], SSD_0HZ, SSD_AMBER);
						
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
	//rprintf("wM=%d\n",appl.workMode);
	//rprintf("mT=%d,0x%x\n",appl.ModeTimer,appl.ModeTimer);
	rprintf("dC=%d\n\n",appl.DispCode);
	//rprintf("lE=%d\n\n",appl.LastEvent);
#endif
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
				SSDDisplayDec(CURRENT_VALUE, SSD_0HZ, SSD_RED);
			}
			else if(mode == NORMAL)
			{
				SSDDisplayDec(MeasureGetResult0(), SSD_0HZ, SSD_RED);
			}
			else	/* setting mode */
			{
				SSDDisplayDec(CURRENT_VALUE, SSD_0HZ, SSD_RED);
			}
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
*                                         AppWaitZeroCrossing
*
* Description : simulink AC power zero crossing.Simulate zero-crossing for hardware that lacks an AC zero-cross
*
* Arguments   : none
*
* Returns    : true/false means failed or pass
*********************************************************************************************************
*/
void AppWaitZeroCrossing(void)
{
	while(appl.system.fSysPwrZC == 0);
	appl.system.fSysPwrZC = 0;
}

/*
*********************************************************************************************************
*                                         AppOutputCompare0
*
* Description : simulink AC power zero crossing.Simulate zero-crossing for hardware that lacks an AC zero-cross
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/
void AppOutputCompare0(void)
{
	OCR0 = TCNT0 + CYCLEN_60HZ;
	appl.system.fSysPwrZC = 1;
}
