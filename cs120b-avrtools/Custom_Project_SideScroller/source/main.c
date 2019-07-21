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
#include "../header/timer.h"
#include "../header/scheduler.h"
#endif

#define input (~PINC & 0x0F);

/* Example code */
enum nokia_States {nok_init, nok_display};

int displaySMTick(int state) {
	switch(state) {
		case nok_init:
			state = nok_display;
			break;
		case nok_display:
			state = nok_display;
			break;
		default:
			state = nok_init;
			break;
	}
	switch(state) {
		case nok_init:
			break;
		case nok_display:
			nokia_lcd_init();
			nokia_lcd_clear();
			nokia_lcd_write_string("IT'S WORKING!",1);
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Nice!", 3);
			nokia_lcd_render();
			break;
		default:
			break;
	}
	return state;
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRC = 0x00; PORTC = 0xFF;

	/* Insert your solution below */
	static task screen_Task;
	task *tasks[] = { &screen_Task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	/*Tasks*/
	screen_Task.state = nok_init;
	screen_Task.period = 5;
	screen_Task.elapsedTime = screen_Task.period;
	screen_Task.TickFct = &displaySMTick;

	TimerSet(1);
	TimerOn();

	unsigned short i;
	while(1) {
		for (i = 0; i < numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
		_delay_ms(1000);
	}
	return 0;
}
