
//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVR project global include
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

#include  <stdint.h>

// global AVRLIB defines
#include "avrlibdefs.h"
// global AVRLIB types definitions
#include "avrlibtypes.h"

/*
*********************************************************************************************************
*                                        IO PORT BITS STRUCT
* structure to allow bit field operations, name conversions: PORTA.0 -> PORT_A.Bit0  PORTB.7 -> PORT_B.Bit7
*********************************************************************************************************
*/

typedef struct bits {
	uint8_t Bit0:1;
	uint8_t Bit1:1;
	uint8_t Bit2:1;
	uint8_t Bit3:1;
	uint8_t Bit4:1;
	uint8_t Bit5:1;
	uint8_t Bit6:1;
	uint8_t Bit7:1;
} BITS;

/*
*********************************************************************************************************
*                                        IO PORT BITS STRUCT
* define all the ports of your microcontroller, add more ports depending on the available mcu ports
*********************************************************************************************************
*/
#define PORT_A	(* (volatile BITS *) &PORTA)
#define PIN_A	(* (volatile BITS *) &PINA)
#define DDR_A	(* (volatile BITS *) &DDRA)

#define PORT_B	(* (volatile BITS *) &PORTB)
#define PIN_B	(* (volatile BITS *) &PINB)
#define DDR_B	(* (volatile BITS *) &DDRB)

#define PORT_C	(* (volatile BITS *) &PORTC)
#define PIN_C	(* (volatile BITS *) &PINC)
#define DDR_C	(* (volatile BITS *) &DDRC)

#define PORT_D	(* (volatile BITS *) &PORTD)
#define PIN_D	(* (volatile BITS *) &PIND)
#define DDR_D	(* (volatile BITS *) &DDRD)

#define FOR_DEBUG	1		/* Set generate codes for debug */

// project/system dependent defines

// CPU clock speed
//#define F_CPU        16000000               		// 16MHz processor
//#define F_CPU        14745000               		// 14.745MHz processor
//#define F_CPU        8000000               		// 8MHz processor
//#define F_CPU        7372800               		// 7.37MHz processor
//#define F_CPU        4000000               		// 4MHz processor
//#define F_CPU        3686400               		// 3.69MHz processor
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond

#define BUTTON_DATA_TYPE        uint8_t

#endif
