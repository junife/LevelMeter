#ifndef APPL_H_
#define APPL_H_

#include "timer.h"
#include "common.h"

#ifdef  APPL_GLOBALS
#define APPL_EXT
#else
#define APPL_EXT  extern
#endif

/*
*********************************************************************************************************
*                                       CONST DEFINITION FOR APPLICATION TIME
*********************************************************************************************************
*/
#define TIME_CONTENT_SWITCH		(TIMER_1_SEC_60HZ*1)	/* content switch interval after power on, 2s */
#define TIME_SWITCH_TOTAL		(TIME_CONTENT_SWITCH*8)	/* content switch total time,items in PWR_ON_STEPS */
#define TIME_B_HELD_RST			(TIMER_1_SEC_60HZ)		/* press/held button reset to factory defalut  */
#define TIME_D_CODE_HELD		(TIMER_1_SEC_60HZ*3)	/* time for hold display code after chaneg, 3s */

#define CYCLEN_60HZ     		(F_CPU/1024/60)			/* 60Hz power cycle length, see TIMER0PRESCALE define and change 1024 value */

/*
*********************************************************************************************************
*                                       DEFINE BUTTON EVENT
* 
*********************************************************************************************************
*/
typedef enum button_event 
{
	NULL_EVENT=0,			/* null event */
	JP2_PRESS,				/* JP2 button press/release event */
	JP2_HELD,				/* JP2 button press/held event */
	JP1_PRESS,				/* JP1 button press/release event */
	JP1_HELD,				/* JP1 button press/held event */
}BUTTON_EVENT;

/*
*********************************************************************************************************
*                                       VARIABLE DEFINITION
*********************************************************************************************************
*/
typedef struct appl_data
{
	SYS_F system;
	APP_CONFIG config;
	WORK_MODE workMode;					/* work mode */
	BUTTON_EVENT CurEvent;				/* current push button event */
	BUTTON_EVENT LastEvent;				/* last time push button event */
	
	uint8_t  second;					/* count one second as other timers time base tick */
	uint16_t PwrOnTimer;				/* Timer for controling display content after powr on */	
	uint16_t ModeTimer;					/* Timer for work mode, if time out trigger mode convert */	
	
	DISP_CODE DispCode;
} APPL_DATA;

APPL_EXT void McuInit(void);
APPL_EXT void ApplInit(void);
APPL_EXT void AppCycleUpdate(void);
APPL_EXT void AppWaitZeroCrossing(void);
APPL_EXT APPL_DATA appl;

#endif	/* endif APPL_H_ */
