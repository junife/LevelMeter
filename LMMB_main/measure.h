/*
*********************************************************************************************************
*
* Module Description:
* This file list all extern function about button, define constant used by botton.
*********************************************************************************************************
*/

#ifndef MEASURE_H_
#define MEASURE_H_

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "global.h"		// include our global settings

#ifdef   MEASURE_GLOBALS
#define  MEASURE_EXT
#else
#define  MEASURE_EXT  extern
#endif

MEASURE_EXT void MeasureInit(void);
MEASURE_EXT void MeasureCycleUpdate(void);
MEASURE_EXT void MeasureCycleUpdate(void);
MEASURE_EXT void Temp(void);

#endif

