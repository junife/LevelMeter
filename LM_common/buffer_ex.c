/*! \file buffer.c \brief Multipurpose byte buffer structure and methods. */
//*****************************************************************************
//
// File Name	: 'buffer.c'
// Title		: Multipurpose byte buffer structure and methods
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 9/23/2001
// Revised		: 9/23/2001
// Version		: 1.0
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "buffer_ex.h"
//#include "global_ex.h"

#ifdef  COMPILER_TYPE_WIN_GCC_
#ifndef CRITICAL_SECTION_START_EX
#define CRITICAL_SECTION_START_EX	asm("nop")
#define CRITICAL_SECTION_END_EX	asm("nop")
#endif
#endif // COMPILER_TYPE_AVR_IAR

#ifndef CRITICAL_SECTION_START_EX
#include "global.h"
#include "avr/io.h"
#define CRITICAL_SECTION_START_EX	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END_EX	SREG = _sreg
#endif

// global variables

// initialization

void bufferInitEx(cBufferEx* buffer, BUFFER_TYPE *start, uint16_t size)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// set start pointer of the buffer
	buffer->dataptr = start;
	buffer->size = size;
	// initialize index and length
	buffer->dataindex = 0;
	buffer->datalength = 0;
	// end critical section
	CRITICAL_SECTION_END_EX;
}

// access routines
BUFFER_TYPE  bufferGetFromFrontEx(cBufferEx* buffer)
{
	BUFFER_TYPE data = 0;
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// check to see if there's data in the buffer
	if(buffer->datalength)
	{
		// get the first character from buffer
		data = buffer->dataptr[buffer->dataindex];
		// move index down and decrement length
		buffer->dataindex++;
		if(buffer->dataindex >= buffer->size)
		{
			buffer->dataindex -= buffer->size;
		}
		buffer->datalength--;
	}
	// end critical section
	CRITICAL_SECTION_END_EX;
	// return
	return data;
}

void bufferDumpFromFrontEx(cBufferEx* buffer, uint16_t numbytes)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// dump numbytes from the front of the buffer
	// are we dumping less than the entire buffer?
	if(numbytes < buffer->datalength)
	{
		// move index down by numbytes and decrement length by numbytes
		buffer->dataindex += numbytes;
		if(buffer->dataindex >= buffer->size)
		{
			buffer->dataindex -= buffer->size;
		}
		buffer->datalength -= numbytes;
	}
	else
	{
		// flush the whole buffer
		buffer->datalength = 0;
	}
	// end critical section
	CRITICAL_SECTION_END_EX;
}

BUFFER_TYPE bufferGetAtIndexEx(cBufferEx* buffer, uint16_t index)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// return character at index in buffer
	BUFFER_TYPE data = buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
	// end critical section
	CRITICAL_SECTION_END_EX;
	return data;
}


uint8_t bufferAddToEndEx(cBufferEx* buffer, BUFFER_TYPE data)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// make sure the buffer has room
	if(buffer->datalength < buffer->size)
	{
		// save data byte at end of buffer
		buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
		// increment the length
		buffer->datalength++;
		// end critical section
		CRITICAL_SECTION_END_EX;
		// return success
		return 1;
	}
	// end critical section
	CRITICAL_SECTION_END_EX;
	// return failure
	return 0;
}

uint16_t bufferIsNotFullEx(cBufferEx* buffer)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// check to see if the buffer has room
	// return true if there is room
	uint16_t bytesleft = (buffer->size - buffer->datalength);
	// end critical section
	CRITICAL_SECTION_END_EX;
	return bytesleft;
}

void bufferFlushEx(cBufferEx* buffer)
{
	// begin critical section
	CRITICAL_SECTION_START_EX;
	// flush contents of the buffer
	buffer->datalength = 0;
	// end critical section
	CRITICAL_SECTION_END_EX;
}
