/*
 * LevelMeterRfRemote.c
 *
 * Created: 3/27/2016 17:00:30
 * Author : junife
 */ 

#include <avr/io.h>

extern void funx(void);
extern void button_x();
int main(void)
{
    /* Replace with your application code */
	GLCD_EnableController(0);
	funx();
	button_x();
    while (1) 
    {
    }
}

