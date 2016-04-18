/*
*********************************************************************************************************
*
*
* Module Description:
* All button hardware drivers for button.c/button.h
*********************************************************************************************************
*/


#ifndef   BUTTON_DEP_GLOBALS
#define   BUTTON_DEP_GLOBALS
#endif

#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include "button_dep.h"

/*
*********************************************************************************************************
*                                         BUTTON INITIALIZATION
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
void ButtonInitEx(void)
{
	DDRD  &= ~((1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3));	/* Set PD6-PD3 as input */
	PORTD |= (1<<PD6) | (1<<PD5) | (1<<PD4) | (1<<PD3);			/* Set PD6-PD3 as internal pull up */
	
	/* 
	Set PC7-PC4 as input as internal pull up
	Set PC3-PC0 as Tri-state (Hi-Z) 
	*/
	DDRC   = (0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	PORTC  = (1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4) | (0<<PC3) | (0<<PC2) | (0<<PC1) | (0<<PC0);
}

/*
*********************************************************************************************************
*                                         ButtonGetIO_StateEx
*
* Description : This function called by ButtonCycleUpdate
*
* Arguments   : None.
*
* Returns    : Button IO value.
*
* Notes      : 1) Button IO general set as internal/external pull-up, so press button IO will low. However,
*			      button structure define high when button press, so need reverse IO input value so that keep
*                 consistently.
*
*              2) Bit location in one byte need to map with BUTTON_T structure.
*
*			   3) Eg: button1->PINA7,button1->PINA6. io_state = (~PINA6<<1) | (~PINA7<<0)
*
* Button Maps 32 bits for RF remote, more details see hardware schematic.
*	19		18		17		16
*	PB17	PB18	PB19	PB20
*	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0
*	PB1		PB5		PB9		PB13	PB2		PB6		PB10	PB14	PB3		PB7		PB11	PB15	PB4		PB8		PB12	PB16
*********************************************************************************************************
*/
ButtonDataType ButtonGetIO_StateEx(void)
{
	ButtonDataType io_state = 0;
	ButtonDataType tempValue;
	ButtonDataType MatrixButtonValue;
	uint8_t Col;
	
	tempValue = (~PIND) & ((1<<PIND6) | (1<<PIND5) | (1<<PIND4) | (1<<PIND3));	/* Get PD6-PD3 IO state */
	tempValue >>= 3;	/* right rotate to low 4 bits location */
	
	for(Col = 0, MatrixButtonValue = 0; Col < 4; Col++)
	{
		/* 
		Set PC7-PC4 as input as internal pull up
		Set PC3-PC0 as Tri-state (Hi-Z)
		*/
		DDRC   = (0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
		PORTC  = (1<<PC7) | (1<<PC6) | (1<<PC5) | (1<<PC4) | (0<<PC3) | (0<<PC2) | (0<<PC1) | (0<<PC0);
	
		/* Set one Column as output with low */
		DDRC  |= 0x08>>Col;			/* Set the column as output */
		PORTC &= ~(0x08>>Col);		/* Set the column as low */
		
		/* read and save button state after the column which active as low */
		MatrixButtonValue <<= 4;	/* newer 4 button state store in lower four bits */
		MatrixButtonValue |= ((~PINC) & 0x00f0) >> 4;
	}
	
	io_state = (tempValue << 16) | MatrixButtonValue;
	return  io_state;
}

