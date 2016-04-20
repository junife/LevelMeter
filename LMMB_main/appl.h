#ifndef APPL_H_
#define APPL_H_

#ifdef  APPL_GLOBALS
#define APPL_EXT
#else
#define APPL_EXT  extern
#endif

/*
*********************************************************************************************************
*                                       CONST DEFINITION
*********************************************************************************************************
*/
#define APPL_ARRAY_MAP_TO_EEPROM	10	/* */ 

/*
*********************************************************************************************************
*                                       CONST DEFINITION FOR TIMERS
*********************************************************************************************************
*/
#define TIMER_INT_DISABLED		0xffff					// - 2 bytes timer is disabled.
#define TIMER_CHAR_DISABLED  	0xff					// - 1 byte timer is disabled.
#define TIMER_TIMEOUT         	0						// - 1 or 2 bytes timer times out.
#define TIMER_1_SEC_60HZ		(1*60)					/* 1 second,globle define for based timer count in 60Hz */ 
#define TIMER_1_MINS			(60)					/* 1 minutes,globle define */ 
#define TIMER_RTRIG_MAN_M		(0.5*TIMER_1_MINS)		/* retrigger time in manual on mode */ 
#define TIMER_RTRIG_AUTO_M		(0.5*TIMER_1_MINS)		/* retrigger time in auto on mode */ 

#define TIMER_3_MINS			(3*TIMER_1_MINS)	
#define TIMER_5_MINS			(5*TIMER_1_MINS)	
#define TIMER_10_MINS			(10*TIMER_1_MINS)	
#define TIMER_15_MINS			(15*TIMER_1_MINS)	
#define TIMER_30_MINS			(30*TIMER_1_MINS)	

/*
*********************************************************************************************************
*                                       CONST DEFINITION FOR APPLICATION TIME
*********************************************************************************************************
*/
#define TIME_CONTENT_SWITCH		(TIMER_1_SEC_60HZ*2)	/* content switch interval after power on */
#define TIME_SWITCH_TOTAL		(TIME_CONTENT_SWITCH*8)	/* content switch total time,items in PWR_ON_STEPS */

/*
*********************************************************************************************************
*                                       DEFINE DISPLAY CODE
* 
*********************************************************************************************************
*/
typedef enum disp_code 
{
	CURRENT_VALUE=0,		/* Display current measure value */
	FULL_WAREHOUSE,		/* Display full warehouse value or setting it */	
	EMPTY_WAREHOUSE, 		/* Display empty warehouse value or setting it */	
	ALARM_H_PERCENT, 		/* Display high percent threshold value or setting it */	
	ALARM_L_PERCENT,		/* Display low percent threshold value or setting it */
	RESPONSE_TIME,		/* Display response time or setting it */
	REMOTE_ADDR,			/* Address for setting remote by RF hand remote when address is  match */
}DISP_CODE;

/*
*********************************************************************************************************
*                                       DEFINE DISPLAY CODE
* 
*********************************************************************************************************
*/
typedef enum pwr_on_steps 
{
	DISP_START=0,			/* Display 4 character after power on */
	DISP_RF_ADDR,			/* Display RF remote address */	
	DISP_CUR_MAT, 			/* Display current measured material value */	
	DISP_FULL_WH, 			/* Display full warehouse value setted by users */	
	DISP_EMPTY_WH,			/* Display empty warehouse value setted by users */
	DISP_H_PERCENT, 		/* Display high percent threshold value setted by users */
	DISP_L_PERCENT, 		/* Display low percent threshold value setted by users */
	DISP_RP_TIME,			/* Display response time value setted by users */
}PWR_ON_STEPS;

/*
*********************************************************************************************************
*                                       VARIABLE DEFINITION
*********************************************************************************************************
*/
typedef struct appl_data
{
	union
	{
		uint16_t flags;		/* flag value for application */
		struct 
		{
			uint16_t fSelfDetDone:1;	/* Set means MCU has finished self detection after power on */	
			uint16_t fSelfDetFail:1;	/* Set means self detection failed, will display error code to user via SSD or tow LEDs */	
			uint16_t fAlarmHigher:1;	/* Set material higher than set value */	
			uint16_t fAlarmLower:1;		/* Set material lower than set value */	
		};
	};
	
	union
	{
		uint8_t ArrayMapToEEPROM[10];	/* the array map all RAM variable need to store to eeprom */
		struct 
		{
			uint16_t FullWarehouse;	
			uint16_t EmptyWarehouse;
			uint8_t  AlarmHighPercent;	/* Alarm when material high than the variable */
			uint8_t  AlarmLowPercent;	/* Alarm when material low than the variable */
			uint16_t ResponceTime;		/* Response  time */	
			uint16_t RemoteAddr;		/* Address for remote setting */	
		};
	};

	uint8_t  second;					/* count one second as other timers time base tick */
	uint16_t PwrOnTimer;				/* Timer for controling display content after powr on*/	
	
	DISP_CODE DispCode;
	PWR_ON_STEPS PwrOnSteps;
} APPL_DATA;

APPL_EXT void McuInit(void);
APPL_EXT void ApplInit(void);
APPL_EXT void AppCycleUpdate(void);
#endif	/* endif APPL_H_ */
