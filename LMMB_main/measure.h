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
#include "global.h"

#ifdef   MEASURE_GLOBALS
#define  MEASURE_EXT
#else
#define  MEASURE_EXT  extern
#endif

#define ioLOW 		PIN_D.Bit2
#define ioHIGHT   	PIN_D.Bit3

/*
*********************************************************************************************************
*                                       CONST DEFINITION
*********************************************************************************************************
*/
#define ADC0_BUF_SIZE	20							/* ADC0 read buffer size 20 */
#define ADC1_BUF_SIZE	ADC0_BUF_SIZE				/* ADC1 read buffer size */
#define ADC0_MAX_SIZE	(64*2)						/* size for array store max value get from ADC0 sample datas (64*2) */
#define ADC1_MAX_SIZE	ADC0_MAX_SIZE				/* size for array store max value get from ADC1 sample datas */
#define MEASURE_FREQ	1000						/* the frequency of Read ADC, unit is Hz */
#define MEASURE_OCR2    (F_CPU/256/MEASURE_FREQ)	/* calculator OCR2 value */

/*
*********************************************************************************************************
*                                       CONST DEFINITION FOR SAMPLE ADC CHANNEL
*********************************************************************************************************
*/
#define MEASURE_ADC0	0							/* Get ADC value control by KONGZH1 */
#define MEASURE_ADC1	1							/* Get ADC value control by KONGZH2 */
#define MEASURE_CHS		2							/* Total ADC channels will use for measure */

/*
*********************************************************************************************************
*                                          DEFINE BUTTON BITs STRUCT
*********************************************************************************************************
*/
typedef struct calc_t{
	uint16_t sum;	/* calculatro sum value */
	uint16_t avg;	/* average */
	uint16_t max;	/* maximum */
	uint16_t min;	/* minimum */
} CALC_T;

/*
*********************************************************************************************************
*                                          DEFINE BUTTON STRUCT AND ALL VARIABLE
*********************************************************************************************************
*/
typedef struct measure_data {
	CALC_T calcResult[MEASURE_CHS];
} MEASURE_DATA;

/*
*********************************************************************************************************
* 												ERROR PREPROCESSORS MESSAGE                                          
*********************************************************************************************************
*/
#if (MEASURE_OCR2 > 0xff) || (MEASURE_OCR2 < 1)
#error "Bad OCR2 value, 1<MEASURE_OCR2<0xff, frequency range is 28800~225 Hz"
#endif
#if (ADC0_BUF_SIZE > 0xff)
#error "ADC0_BUF_SIZE is so big, not over 0xff"
#endif

MEASURE_EXT void MeasureInit(void);
MEASURE_EXT void MeasureCycleUpdate(void);
MEASURE_EXT void MeasureCycleUpdate(void);
MEASURE_EXT uint16_t MeasureGetResult0(void);

#endif

