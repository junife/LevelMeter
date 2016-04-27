/*
*********************************************************************************************************
*
*
* Module Description:
* All Seven-segment display APIs, thie project use 4 sevent segment dipsplay with red/green color.
*********************************************************************************************************
*/

#ifndef   SSD_GLOBALS
#define   SSD_GLOBALS
#endif

#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/pgmspace.h>
//#include <util/delay.h>
#include "ssd.h"

#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library

/*
*********************************************************************************************************
*                                          COMMON ANODE SEVEN SEGMENT DISPAY CODE
* number 0-9 and index value from 0-9, code is 0xC0-0x90
* alphabet A,b,c,d,e,f and index value from 10-15 code is 0x88-0x8E
* special character ''null", "-"(Top dash),"_"(Bottom dash), and index value from 16-18 code is 0xFE-0xF7
*********************************************************************************************************
*/
static const uint8_t PROGMEM ssdCharTble[]=
//0     1      2      3       4      5      6       7      8       9       A     b       c      d       e      f       null   TopD  BotD
{0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFF,0xFE,0xF7};

/*
*********************************************************************************************************
*                                          LOCAL  FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void SSDUpdateComPin(void);
static void SSDUpdateOnOff(void);
static void SSDShitfOutData(void);
static uint32_t SSDHex2Dec(uint16_t hexValue);
static uint32_t SSDHex2Hex(uint16_t hexValue);

/*
*********************************************************************************************************
*                                          SEVEN SEGMENT DISPALY VARIABLE DEFINE
*********************************************************************************************************
*/
SSD_DATA SSD;

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
void SSDInit(void)
{	
	/* 
	Set PB7-PB4 as output to drive seven segment display, default display turn off
	*/
	DDRC &= ~((1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4));
	DDRC |= ((1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4));
	PORTC &= ~((1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4));

	SSDDisplayHex(0, SSD_R_OFF, SSD_RED);	/* Initial turn off all SSDs */
}

/*
*********************************************************************************************************
*                                         SSDCycleUpdate
*
* Description : This function call in main loop each power cycle to update SSD color and display data to SSD
*
* Arguments   : none
*
* Notes      :  none
*
*********************************************************************************************************
*/

void SSDCycleUpdate(void)
{
	SSDShitfOutData();
	SSDUpdateOnOff();
	SSDUpdateComPin();
}

/*
*********************************************************************************************************
*                                         SSDDisplayDec
*
* Description :  Set display attribute and display data as decimal without zero before number
*
*
* Arguments   : DispData, hex value.
*                    rate, which set SSD to flash or on/off, SSD_0HZ, SSD_1HZ, SSD_2HZ, SSD_3HZ and SSD_R_OFF
*                    color, set red green amber for SSD.
* Notes      :none
*
*********************************************************************************************************
*/
void SSDDisplayDec(uint16_t DispData,  SSD_RATE rate, SSD_COLOR color)
{
	SSD_DISP_T ValueCopy;
	
	SSD.DispVal.value = SSDHex2Dec(DispData);
	ValueCopy.value = SSD.DispVal.value;
	SSD.ssd.rate = rate;
	SSD.ssd.color = color;
	
	/* Set display null for all 4 seven segment displays */
	SSD.DispVal.thousands = pgm_read_byte(&ssdCharTble[SSD_DISP_NULL]);
	SSD.DispVal.hundred   = pgm_read_byte(&ssdCharTble[SSD_DISP_NULL]);
	SSD.DispVal.tens      = pgm_read_byte(&ssdCharTble[SSD_DISP_NULL]);
	SSD.DispVal.units     = pgm_read_byte(&ssdCharTble[SSD_DISP_NULL]);

	/* Set display remove zero before number */
	if(DispData > 1000)
	{
		SSD.DispVal.thousands = pgm_read_byte(&ssdCharTble[ValueCopy.thousands]);
		SSD.DispVal.hundred   = pgm_read_byte(&ssdCharTble[ValueCopy.hundred]);
		SSD.DispVal.tens	  = pgm_read_byte(&ssdCharTble[ValueCopy.tens]);
		SSD.DispVal.units	  = pgm_read_byte(&ssdCharTble[ValueCopy.units]);
	}
	else if(DispData > 100)
	{
		SSD.DispVal.hundred   = pgm_read_byte(&ssdCharTble[ValueCopy.hundred]);
		SSD.DispVal.tens	  = pgm_read_byte(&ssdCharTble[ValueCopy.tens]);
		SSD.DispVal.units	  = pgm_read_byte(&ssdCharTble[ValueCopy.units]);
	}
	else if(DispData > 10)
	{
		SSD.DispVal.tens	  = pgm_read_byte(&ssdCharTble[ValueCopy.tens]);
		SSD.DispVal.units	  = pgm_read_byte(&ssdCharTble[ValueCopy.units]);
	}
	else
	{
		SSD.DispVal.units	  = pgm_read_byte(&ssdCharTble[ValueCopy.units]);
	}
}

/*
*********************************************************************************************************
*                                         SSDDisplayDec
*
* Description :  Set display attribute and display data as hexadecimal
*
*
* Arguments   : DispData, hex value.
*                    rate, which set SSD to flash or on/off, SSD_0HZ, SSD_1HZ, SSD_2HZ, SSD_3HZ and SSD_R_OFF
*                    color, set red green amber for SSD.
* Notes      :none
*
*********************************************************************************************************
*/
void SSDDisplayHex(uint16_t DispData,  SSD_RATE rate, SSD_COLOR color)
{
	SSD.DispVal.value = SSDHex2Hex(DispData);
	SSD.ssd.rate = rate;
	SSD.ssd.color = color;
	
	SSD.DispVal.thousands = pgm_read_byte(&ssdCharTble[SSD.DispVal.thousands]);
	SSD.DispVal.hundred   = pgm_read_byte(&ssdCharTble[SSD.DispVal.hundred]);
	SSD.DispVal.tens      = pgm_read_byte(&ssdCharTble[SSD.DispVal.tens]);
	SSD.DispVal.units     = pgm_read_byte(&ssdCharTble[SSD.DispVal.units]);
}

/*
*********************************************************************************************************
*                                         SSDUpdateComPin
*
* Description :  Update IO state to give/cut power to red/green common anode pin
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void SSDUpdateComPin(void)
{
	if(SSD.ssd.state == SSD_OFF)		/* if ssd is in off state, cut power to green and color */
	{
		SSD_RED_CLR();
		SSD_GRN_CLR();
	}
	else
	{
		if(SSD.ssd.color == SSD_RED)		/* dispay red color */
		{
			SSD_RED_SET();	/* give power to red ssd common pin */
			SSD_GRN_CLR();	/* cut power to green ssd common pin */
		}
		else if(SSD.ssd.color == SSD_GREEN) /* dispay green color */
		{
			SSD_RED_CLR();
			SSD_GRN_SET();
		}
		else								/* dispay amber color */
		{
			SSD_RED_SET();
			SSD_GRN_SET();
		}
	}
}

/*
*********************************************************************************************************
*                                         SSDUpdateOnOff
*
* Description :  Update SSDs on/off according to rate value
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void SSDUpdateOnOff(void)
{
	SSD_RATE rate;
	uint8_t ft;				/* flash timer */

	rate = SSD.ssd.rate;
	ft = SSD.flashTimer;
	
	ft += rate;
	if((rate == SSD_R_OFF) || (ft > SSD_ONE_SEC))
	{
		ft = 0;
		SSD.ssd.state = SSD_OFF;
	}
	SSD.flashTimer = ft;
	
	if((rate == SSD_0HZ) || (ft > (SSD_ONE_SEC/2)))
	{
		SSD.ssd.state = SSD_ON;
	}
}

/*
*********************************************************************************************************
*                                         SSDShitfOutData
*
* Description :  Shift out data 74HC164(an 8-bit serial-in/parallel-out shift register). Chip lock bit when clk in rising edge. 
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
void SSDShitfOutData(void)
{
	uint8_t i;
	uint8_t bitVal;
	uint32_t DispValTemp = SSD.DispVal.value; 
	
	for(i = 0; i < 32; i++)
	{
		bitVal = 0;
		if(DispValTemp & 0x80000000)
		{
			bitVal = 1;
		}
		DispValTemp <<= 1;

		SSD_CLK_CLR();
		SSD_DAT_CLR();
		if(bitVal)
		{
			SSD_DAT_SET();
		}
		SSD_CLK_SET();
	}
}

/*
*********************************************************************************************************
*                                         SSDHex2Dec
*
* Description : Hex convert to decimal
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
uint32_t SSDHex2Dec(uint16_t hexValue)
{
	SSD_DISP_T dataSTR;
	dataSTR.value = 0;
	
	dataSTR.thousands = hexValue/1000;
	dataSTR.hundred   = hexValue%1000/100;
	dataSTR.tens      = hexValue%1000%100/10;
	dataSTR.units     = hexValue%1000%100%10/1;
	
	return dataSTR.value; 
}

/*
*********************************************************************************************************
*                                         SSDHex2Dec
*
* Description : Hex convert to hex and sperate store in data structure.
*
*
* Arguments   : none
*
* Notes      :none
*
*********************************************************************************************************
*/
uint32_t SSDHex2Hex(uint16_t hexValue)
{
	SSD_DISP_T dataSTR;
	dataSTR.value = 0;
	
	dataSTR.thousands = (hexValue >> 12) & 0x000f;
	dataSTR.hundred   = (hexValue >> 8) & 0x000f;
	dataSTR.tens      = (hexValue >> 4) & 0x000f;
	dataSTR.units     = (hexValue >> 0) & 0x000f;
	
	return dataSTR.value; 
}
