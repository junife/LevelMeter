/*
*********************************************************************************************************
*
* Module Description:
* This file list all extern function about button, define constant used by botton.
*********************************************************************************************************
*/

#ifndef LED_H_
#define LED_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include <stdint.h>
#include <stdbool.h>
#include "global.h"		// include our global settings

#ifdef   LED_GLOBALS
#define  LED_EXT
#else
#define  LED_EXT  extern
#endif


#define LED_NUM	2	/* define LED numbers, LED0 map to hardware FLAG1 and LED1 map to FLAG2 */

#define LED_ONE_SEC	60	/* one second need 60 cycles, simulater 60Hz power cycle */

typedef enum led_rate   {LED_0HZ=0, LED_1HZ=1, LED_2HZ=2, LED_3HZ=3,LED_OFF=4} LED_RATE;
typedef enum led_state   {S_ON=0, S_OFF=1} LED_STATE;

typedef union 
{
	uint8_t value;
	struct 
	{
		LED_RATE LEDRate:3;
		LED_STATE state:1;	/* state set by firmware according to computer result */
	};
}LED_T;

/*
*********************************************************************************************************
*                                          DEFINE LED STRUCT AND ALL VARIABLE
*********************************************************************************************************
*/
typedef struct led_data {
	LED_T led;
	uint8_t flashTimer;             /* Counter used to control LED flash state */
}LED_DATA;

LED_EXT void LEDInit(void);
LED_EXT void LEDCycleUpdate(void);
LED_EXT void LEDFlag1On(void);
LED_EXT void LEDFlag1Flash1Hz(void);
LED_EXT void LEDFlag1Flash2Hz(void);
LED_EXT void LEDFlag1Flash3Hz(void);
LED_EXT void LEDFlag1Off(void);
LED_EXT void LEDFlag2On(void);
LED_EXT void LEDFlag2Flash1Hz(void);
LED_EXT void LEDFlag2Flash2Hz(void);
LED_EXT void LEDFlag2Flash3Hz(void);
LED_EXT void LEDFlag2Off(void);

#endif	/* endif BUTTON_H */
