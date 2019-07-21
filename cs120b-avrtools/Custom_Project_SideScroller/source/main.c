/*	Author: aku006
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include <util/delay.h>
#include "simAVRHeader.h"
#include "../header/nokia5110.h"
#endif

#define input (~PINC & 0x0F);

/* Example code */
/*
enum nokia_states {init, display} nokia_state;

void nokia_tick() {
	switch(nokia_state) {
		case init:
			nokia_state = display;
			break;
		case display:
			nokia_state = display;
			break;
		default:
			nokia_state = init;
			break;
	}
	switch(nokia_state) {
		case init:
			break;
		case display:
			nokia_lcd_init();
			nokia_lcd_clear();
			nokia_lcd_write_string("IT'S WORKING!",1);
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Nice!", 3);
			nokia_lcd_render();

			for (;;) {
				_delay_ms(1000);
			}
			break;
		default:
			break;
	}
}*/

int main(void) {
	while(1) {
		nokia_tick();
	}
	return 1;
}
