/*
*********************************************************************************************************
*
* Module Description:
* This file list all extern function about Seven-segment display, define constant used by Seven-segment display.
*********************************************************************************************************
*/

#ifndef SSD_H_
#define SSD_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include "global.h"		// include our global settings

#ifdef   SSD_GLOBALS
#define  SSD_EXT
#else
#define  SSD_EXT  extern
#endif

/*
*********************************************************************************************************
*                                          DEFINE SSD IO CONTROL
*********************************************************************************************************
*/
#define  SSD_RED_SET()	{PORTC |= (1<<PC4);}
#define  SSD_RED_CLR()	{PORTC &= ~(1<<PC4);}
#define  SSD_GRN_SET()	{PORTC |= (1<<PC5);}
#define  SSD_GRN_CLR()	{PORTC &= ~(1<<PC5);}
#define  SSD_DAT_SET()	{PORTC |= (1<<PC6);}
#define  SSD_DAT_CLR()	{PORTC &= ~(1<<PC6);}
#define  SSD_CLK_SET()	{PORTC |= (1<<PC7);}
#define  SSD_CLK_CLR()	{PORTC &= ~(1<<PC7);}

#define SSD_NUM		2		/* define LED numbers, LED0 map to hardware FLAG1 and LED1 map to FLAG2 */
#define SSD_ONE_SEC	60		/* one second need 60 cycles, simulater 60Hz power cycle */

/*
*********************************************************************************************************
*                                          DEFINE SSD STRUCT AND ALL VARIABLE
*********************************************************************************************************
*/
typedef enum ssd_rate {SSD_0HZ=0, SSD_1HZ=1, SSD_2HZ=2, SSD_3HZ=3,SSD_R_OFF=4} SSD_RATE;
typedef enum ssd_state {SSD_ON=1, SSD_OFF=0} SSD_STATE;
typedef enum ssd_color {SSD_RED,SSD_GREEN,SSD_AMBER} SSD_COLOR;

typedef union 
{
	uint8_t value;
	struct 
	{
		SSD_RATE rate:3;
		SSD_COLOR color:2;
		SSD_STATE state:1;
	};
}SSD_T;

typedef union 
{
	uint32_t value;
	struct 
	{
		uint32_t units:8;
		uint32_t tens:8;
		uint32_t hundred:8;
		uint32_t thousands:8;
	};
}SSD_DISP_T;


/*
*********************************************************************************************************
*                                          DEFINE LED STRUCT AND ALL VARIABLE
*********************************************************************************************************
*/
typedef struct ssd_data {
	SSD_T ssd;
	uint8_t flashTimer;		/* Counter used to control ssd flash state */
	SSD_DISP_T DispVal;		/* Display value */		
}SSD_DATA;

/*
*********************************************************************************************************
*                                          COMMON ANODE SEVEN SEGMENT DISPAY CODE
* number 0-9 and index value from 0-9, code is 0xC0-0x90
* alphabet A,b,c,d,e,f and index value from 10-15 code is 0x88-0x8E
* special character "-"(Top dash),"_"(Bottom dash), and index value from 16-17 code is 0xFE-0xF7
*********************************************************************************************************
*/
#define SSD_DISP_NULL			16
#define SSD_DISP_TOP_DASH		17
#define SSD_DISP_BOTTOM_DASH	18

SSD_EXT void SSDInit(void);
SSD_EXT void SSDCycleUpdate(void);
SSD_EXT void SSDDisplayDec(uint16_t DispData,  SSD_RATE rate, SSD_COLOR color);
SSD_EXT void SSDDisplayHex(uint16_t DispData,  SSD_RATE rate, SSD_COLOR color);
#endif	/* endif SSD_H_ */
