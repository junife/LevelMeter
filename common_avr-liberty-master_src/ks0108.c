/*! \file ks0108.c \brief Graphic LCD driver for HD61202/KS0108 displays. */
//*****************************************************************************
//
// File Name	: 'ks0108.c'
// Title		: Graphic LCD driver for HD61202/KS0108 displays
// Author		: Pascal Stang - Copyright (C) 2001-2003
// Date			: 10/19/2002
// Revised		: 5/5/2003
// Version		: 0.5
// Target MCU	: Atmel AVR
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
// AVR specific includes
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
#endif

#include "global.h"
#include "ks0108.h"

// global variables
GrLcdStateType GrLcdState;

void outb_lcd_data_port(u08 port, u08 data);
void outb_lcd_ddr_port(u08 port, u08 data);
u08 inb_lcd_pin_port(u08 port);

/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/

void glcdInitHW(void)
{
	// initialize I/O ports
	// if I/O interface is in use
#ifdef GLCD_PORT_INTERFACE

	//TODO: make setup of chip select lines contingent on how
	// many controllers are actually in the display
	
	// initialize LCD control lines levels
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_LED);		/* Turn on LCD LED */
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS0);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS1);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS2);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS3);
	//cbi(GLCD_CTRL_PORT, GLCD_CTRL_RESET);
	
	// initialize LCD control port to output
	sbi(GLCD_CTRL_DDR, GLCD_CTRL_RS);
	sbi(GLCD_CTRL_DDR, GLCD_CTRL_RW);
	sbi(GLCD_CTRL_DDR, GLCD_CTRL_E);
	sbi(GLCD_CTRL_DDR, GLCD_CTRL_LED);
	sbi(GLCD_CTRL_DDRA, GLCD_CTRL_CS0);
	sbi(GLCD_CTRL_DDRA, GLCD_CTRL_CS1);
	sbi(GLCD_CTRL_DDRA, GLCD_CTRL_CS2);
	sbi(GLCD_CTRL_DDRA, GLCD_CTRL_CS3);
	//sbi(GLCD_CTRL_DDR, GLCD_CTRL_RESET);
	
	// initialize LCD data
	outb_lcd_data_port(GLCD_DATA_PORT, 0x00);
	// initialize LCD data port to output
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0xFF);
#endif
}

void glcdControllerSelect(u08 controller)
{
#ifdef GLCD_PORT_INTERFACE
	//TODO: make control of chip select lines contingent on how
	// many controllers are actually in the display

	// unselect all controllers
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS0);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS1);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS2);
	cbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS3);

	// select requested controller
	switch(controller)
	{
	case 0:	sbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS0); break;
	case 1:	sbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS1); break;
	case 2:	sbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS2); break;
	case 3:	sbi(GLCD_CTRL_PORTA, GLCD_CTRL_CS3); break;
	default: break;
	}
#endif
}

void glcdBusyWait(u08 controller)
{
#ifdef GLCD_PORT_INTERFACE
	cli();
	// wait until LCD busy bit goes to zero
	// select the controller chip
	glcdControllerSelect(controller);
	// do a read from control register
	outb_lcd_data_port(GLCD_DATA_PORT, 0xFF);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);		/* Clear RS, read instruction data */
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0x00);	/* Set data port as input */
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);		/* Set RW, read data */
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);		/* set enable */
	asm volatile ("nop"); asm volatile ("nop");
	while(inb_lcd_pin_port(GLCD_DATA_PIN) & GLCD_STATUS_BUSY)
	{
		cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);	/* clear enable */
		asm volatile ("nop"); asm volatile ("nop");
		asm volatile ("nop"); asm volatile ("nop");
		_delay_us(1);
		sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);	/* set enable */
		asm volatile ("nop"); asm volatile ("nop");
		asm volatile ("nop"); asm volatile ("nop");
		_delay_us(1);
	}
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);		/* clear enable */
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);		/* clear RW */
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0xFF);
	sei();
#else
	// sbi(MCUCR, SRW);			// enable RAM waitstate
	// wait until LCD busy bit goes to zero
	while(*(volatile unsigned char *)
		(GLCD_CONTROLLER0_CTRL_ADDR + GLCD_CONTROLLER_ADDR_OFFSET*controller) & GLCD_STATUS_BUSY);
	// cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}

void glcdControlWrite(u08 controller, u08 data)
{
#ifdef GLCD_PORT_INTERFACE
	cli();
	glcdBusyWait(controller);	// wait until LCD not busy
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0xFF);
	outb_lcd_data_port(GLCD_DATA_PORT, data);
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	_delay_us(1);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	sei();
#else
	//sbi(MCUCR, SRW);				// enable RAM waitstate
	glcdBusyWait(controller);		// wait until LCD not busy
	*(volatile unsigned char *) (GLCD_CONTROLLER0_CTRL_ADDR + GLCD_CONTROLLER_ADDR_OFFSET*controller) = data;
	//cbi(MCUCR, SRW);				// disable RAM waitstate
#endif
}

u08 glcdControlRead(u08 controller)
{
	register u08 data;
#ifdef GLCD_PORT_INTERFACE
	cli();
	glcdBusyWait(controller);		// wait until LCD not busy
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0x00);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	_delay_us(1);
	data = inb_lcd_pin_port(GLCD_DATA_PIN);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0xFF);
	sei();
#else
	//sbi(MCUCR, SRW);				// enable RAM waitstate
	glcdBusyWait(controller);		// wait until LCD not busy
	data = *(volatile unsigned char *) (GLCD_CONTROLLER0_CTRL_ADDR + GLCD_CONTROLLER_ADDR_OFFSET*controller);
	//cbi(MCUCR, SRW);				// disable RAM waitstate
#endif
	return data;
}

void glcdDataWrite(u08 data)
{
	register u08 controller = (GrLcdState.lcdXAddr/GLCD_CONTROLLER_XPIXELS);
#ifdef GLCD_PORT_INTERFACE
	cli();
	glcdBusyWait(controller);		// wait until LCD not busy
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0xFF);
	outb_lcd_data_port(GLCD_DATA_PORT, data);
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	_delay_us(1);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	sei();
#else
	//sbi(MCUCR, SRW);				// enable RAM waitstate
	glcdBusyWait(controller);		// wait until LCD not busy
	*(volatile unsigned char *) (GLCD_CONTROLLER0_CTRL_ADDR + GLCD_CONTROLLER_ADDR_OFFSET*controller) = data;
	//cbi(MCUCR, SRW);				// disable RAM waitstate
#endif
	// increment our local address counter
	GrLcdState.ctrlr[controller].xAddr++;
	GrLcdState.lcdXAddr++;
	if(GrLcdState.lcdXAddr >= GLCD_XPIXELS)
	{
		GrLcdState.lcdYAddr++;
		glcdSetYAddress(GrLcdState.lcdYAddr);
		glcdSetXAddress(0);
	}
}

u08 glcdDataRead(void)
{
	register u08 data;
	register u08 controller = (GrLcdState.lcdXAddr/GLCD_CONTROLLER_XPIXELS);
#ifdef GLCD_PORT_INTERFACE
	cli();
	glcdBusyWait(controller);		// wait until LCD not busy
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_RS);
	outb_lcd_ddr_port(GLCD_DATA_DDR, 0x00);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	sbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	asm volatile ("nop"); asm volatile ("nop");
	_delay_us(1);
	data = inb_lcd_pin_port(GLCD_DATA_PIN);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_E);
	cbi(GLCD_CTRL_PORT, GLCD_CTRL_RW);
	sei();
#else
	//sbi(MCUCR, SRW);				// enable RAM waitstate
	glcdBusyWait(controller);		// wait until LCD not busy
	data = *(volatile unsigned char *) (GLCD_CONTROLLER0_CTRL_ADDR + GLCD_CONTROLLER_ADDR_OFFSET*controller);
	//cbi(MCUCR, SRW);				// disable RAM waitstate
#endif
	// increment our local address counter
	GrLcdState.ctrlr[controller].xAddr++;
	GrLcdState.lcdXAddr++;
	if(GrLcdState.lcdXAddr >= GLCD_XPIXELS)
	{
		GrLcdState.lcdYAddr++;
		glcdSetYAddress(GrLcdState.lcdYAddr);
		glcdSetXAddress(0);
	}
	return data;
}

void glcdReset(u08 resetState)
{
	// reset lcd if argument is true
	// run lcd if argument is false
#ifdef GLCD_PORT_INTERFACE
	if(resetState)
		cbi(GLCD_CTRL_PORT, GLCD_CTRL_RESET);
	else
		sbi(GLCD_CTRL_PORT, GLCD_CTRL_RESET);
#endif
}

void glcdSetXAddress(u08 xAddr)
{
	u08 i;
	// record address change locally
	GrLcdState.lcdXAddr = xAddr;
	
	// clear y (col) address on all controllers
	for(i=0; i<GLCD_NUM_CONTROLLERS; i++)
	{
		glcdControlWrite(i, GLCD_SET_Y_ADDR | 0x00);
		GrLcdState.ctrlr[i].xAddr = 0;
	}

	// set y (col) address on destination controller
	glcdControlWrite((GrLcdState.lcdXAddr/GLCD_CONTROLLER_XPIXELS),
		GLCD_SET_Y_ADDR | (GrLcdState.lcdXAddr & 0x3F));
}

void glcdSetYAddress(u08 yAddr)
{
	u08 i;
	// record address change locally
	GrLcdState.lcdYAddr = yAddr;
	// set page address for all controllers
	for(i=0; i<GLCD_NUM_CONTROLLERS; i++)
	{
		glcdControlWrite(i, GLCD_SET_PAGE | yAddr);
	}
}

/*************************************************************/
/********************* PUBLIC FUNCTIONS **********************/
/*************************************************************/

void glcdInit()
{
	u08 i;
	// initialize hardware
	glcdInitHW();
	// bring lcd out of reset
	//glcdReset(FALSE);
	// Turn on LCD
	for(i=0; i<GLCD_NUM_CONTROLLERS; i++)
	{
		glcdControlWrite(i, GLCD_ON_CTRL | GLCD_ON_DISPLAY);
	}
	// clear lcd
	glcdClearScreen();
	// initialize positions
	glcdHome();
}

void glcdHome(void)
{
	u08 i;
	// initialize addresses/positions
	glcdStartLine(0);
	glcdSetAddress(0,0);
	// initialize local data structures
	for(i=0; i<GLCD_NUM_CONTROLLERS; i++)
	{
		GrLcdState.ctrlr[i].xAddr = 0;
		GrLcdState.ctrlr[i].yAddr = 0;
	}
}

void glcdClearScreen(void)
{
	u08 pageAddr;
	u08 xAddr;

	// clear LCD
	// loop through all pages
	for(pageAddr=0; pageAddr<(GLCD_YPIXELS>>3); pageAddr++)
	{
		// set page address
		glcdSetAddress(0, pageAddr);
		// clear all lines of this page of display memory
		for(xAddr=0; xAddr<GLCD_XPIXELS; xAddr++)
		{
			glcdDataWrite(0x00);
		}
	}
}

void glcdStartLine(u08 start)
{
	glcdControlWrite(0, GLCD_START_LINE | start);
	glcdControlWrite(1, GLCD_START_LINE | start);
}

void glcdSetAddress(u08 x, u08 yLine)
{
	// set addresses
	glcdSetYAddress(yLine);
	glcdSetXAddress(x);
}

void glcdGotoChar(u08 line, u08 col)
{
	glcdSetAddress(col*6, line);
}

void glcdDelay(u16 p)			// 1-8us      ...2-13us     ...5-31us
{								// 10-60us    ...50-290us
	unsigned int i;				// 100-580us  ...500-2,9ms
	unsigned char j; 			// 1000-5,8ms ...5000-29ms
								// 10000-56ms ...30000-170ms
								// 50000-295ms...60000-345ms
//	for (i = 0; i < p; i++) for (j = 0; j < 10; j++) asm volatile ("nop");
	for (i = 0; i < p; i++) for (j = 0; j < 10; j++);
}


// Higher level functionality has been moved to the API-layer glcd.c/glcd.h

// Add code for support LCD data/cmd port in sperate io port.
void outb_lcd_data_port(u08 port, u08 data)
{
	u08 temp;
	u08 i;
	
	port = port;		/* avoid Warnings from compiler */
	
	PORTA &= 0xf0;		/* PA3-PA0 clear */
	temp = data>>4;		/* DB7-DB4 output to LCD */
	PORTA |= temp;		/* DB7-DB4 output to LCD */
	
	data &= 0x0f;		/* mask high 4 bits prepare output lower 4 bits to PORTA */
	 
	for(i = 0, temp = 0; i < 4; i++)
	{
		temp <<= 1;
		temp |= data&0x01?1:0;
		data >>= 1;
	}	
	PORTB &= 0xf0;		/* PB3-PB0 clear */
	PORTB |= temp;		/* DB3-DB0 output to LCD */
}

void outb_lcd_ddr_port(u08 port, u08 data)
{
	port = port;	/* avoid Warnings from compiler */
	
	if(data == 0)
	{
		DDRA &= 0xf0;	/* PA3-PA0 set as input */
		DDRB &= 0xf0;	/* PB3-PB0 set as input */  	
	}
	else
	{
		DDRA |= 0x0f;	/* PA3-PA0 set as output */
		DDRB |= 0x0f;	/* PB3-PB0 set as output */ 
	}
}

u08 inb_lcd_pin_port(u08 port)
{
	u08 tempPINB;
	u08 tempData;
	u08 data;
	u08 i;
	
	port = port;		/* avoid Warnings from compiler */
	
	data = (PINA<<4) & 0xf0;		/* DB7-DB4 read */
	
	/* DB3-DB0 read */
	for(i = 0, tempPINB = PINB, tempData = 0; i < 4; i++)
	{
		tempData <<= 1;
		tempData |= tempPINB&0x01?1:0;
		tempPINB >>= 1;
	}
	data |= (tempData & 0x0f);	/* Get whole 8 bits data */		
	
	return data;
}
