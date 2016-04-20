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
static bool AppPwrOnCtrl(void);
static void ApplDataInit(void);

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
	/* Following for power on display steps */
	appl.PwrOnSteps = DISP_START;
	appl.PwrOnTimer = TIME_SWITCH_TOTAL;
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

	if(AppPwrOnCtrl() == true)
	{
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
	}
	
	if(appl.PwrOnTimer != TIMER_TIMEOUT && appl.PwrOnTimer != TIMER_INT_DISABLED) appl.PwrOnTimer--;
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
bool AppPwrOnCtrl(void)
{
	if((appl.PwrOnTimer != TIMER_INT_DISABLED && appl.PwrOnTimer != TIMER_TIMEOUT))
	{
		if(appl.PwrOnTimer % (TIME_CONTENT_SWITCH - 1) == 0)
		{
#if (FOR_DEBUG==1)
						rprintf("tm=%d\n",appl.PwrOnTimer);
						rprintf("st=%x\n",appl.PwrOnSteps);
#endif
			switch (appl.PwrOnSteps)
			{
				case DISP_START:
					break;
					
				case DISP_RF_ADDR:
					break;
					
				case DISP_CUR_MAT:
					break;
					
				case DISP_FULL_WH:
					break;
					
				case DISP_EMPTY_WH:
					break;
					
				case DISP_H_PERCENT:
					break;
					
				case DISP_L_PERCENT:
					break;
					
				case DISP_RP_TIME:
					break;
					
				default:
					break;						
			}
			appl.PwrOnSteps++;
		}
		return false;
	}
	else if(appl.PwrOnTimer == TIMER_TIMEOUT)
	{
		appl.PwrOnTimer = TIMER_INT_DISABLED;
#if (FOR_DEBUG==1)
		rprintf("tm=%d\n",appl.PwrOnTimer);
		rprintf("st=%x\n",appl.PwrOnSteps);
#endif
	}
	
	return true;
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

