/*
*********************************************************************************************************

* Copyright 2008 The Watt Stopper / Legrand
* as an unpublished work.
* All Rights Reserved.
*
* The information contained herein is confidential
* property of The Watt Stopper / Legrand.
* The use, copying, transfer or disclosure of such
* information is prohibited except by express written
* agreement with The Watt Stopper / Legrand.
*
* Project:PW/DSW 31x
* MCU:Atmega168P
* Compile condition: AVR Studio 6
* First written on March 2015, by Corner Hu.
*
* Module Description:
* This file list all function for ADC, includint ADC initial, ADC read functions
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#ifndef   ADC_GLOBALS
#define   ADC_GLOBALS
#endif

#include  <stdint.h>
#include  <stdbool.h>
#include  <avr/io.h>
#include  <avr/interrupt.h>
//#include  "global_x.h"
#include  "adc.h"

/*
*********************************************************************************************************
*                                         ADC INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.Initial ADC register
*
* Arguments   : none
*
* Returns    : none
*********************************************************************************************************
*/

void ADCInit (void)
{
	ADMUX  = (1 << REFS0);	/* use external AVCC as AREF,the result is right adjusted,10bits format */	
	ADCSRA = (1 << ADEN) | (0 << ADIE) | (1 << ADIF) | ADPS2_0;	/* ADC Enable, Clear interrupt flag,set ADC Prescaler Selections */
}

/*
*********************************************************************************************************
*                                         ReadADC10Bit
*
* Description : Covert one channel Analog to digital.
*
* Arguments   : none
*
* Returns    : 10bits convertion value
*********************************************************************************************************
*/

uint16_t ReadADC10Bit(uint8_t channel)
{
	uint16_t result;
	uint8_t  sreg_save;  
	
    sreg_save = SREG;						/* Save AVR Status Register */
	cli();									/* Global interrupt disable */
	
    ADMUX  &= 0xf0; 						/* Clear MUX3:0 */
	ADMUX  |= (channel&0x07);				/* Choose ADC channel */
    ADCSRA |= (1 << ADSC);					/* ADC start conversion */	
    while ((ADCSRA & (1 << ADIF)) == 0);	/* wait conversion finish */			
    result  = ADC;							/* Read ADC value */
	
    SREG = sreg_save;						/* Recover AVR Status Register */	
    return (result);
}

/*
*********************************************************************************************************
*                                         ReadADC8Bit
*
* Description : Covert one channel Analog to digital.
*
* Arguments   : none
*
* Returns    : 8bits convertion value
*********************************************************************************************************
*/

uint8_t ReadADC8Bit(uint8_t channel)
{
	uint16_t tempInt;
	tempInt = ReadADC10Bit(channel);
	return tempInt>>2;
}


