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
* This file list all extern function about button, define constant used by botton.
*********************************************************************************************************
*/

#ifndef ADC_H_
#define ADC_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include  <stdint.h>
#include  <stdbool.h>

#ifdef   ADC_GLOBALS
#define  ADC_EXT
#else
#define  ADC_EXT  extern
#endif

#define  ADPS2_0        ((1 << 2) | (0 << 1) | (0 << 0))    /* clock is F_CPU/16 */

ADC_EXT void 	 ADCInit 	 (void);
ADC_EXT uint16_t ReadADC10Bit(uint8_t channel);
ADC_EXT uint8_t  ReadADC8Bit (uint8_t channel);

#endif	/* endif BUTTON_H */

