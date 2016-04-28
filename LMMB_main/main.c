//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
//#include <util/delay.h>
#include "appl.h"
#include "button.h"
#include "led.h"
#include "ssd.h"
#include "measure.h"

//----- Begin Code ------------------------------------------------------------
int main(void)
{	
	/*
	Calll all initial function
	*/
	McuInit();
	ApplInit();

	while(1)
	{
		AppWaitZeroCrossing();
		ButtonCycleUpdate();
		AppCycleUpdate();
		LEDCycleUpdate();
		SSDCycleUpdate();
		MeasureCycleUpdate();
	}
	return 0;
}
