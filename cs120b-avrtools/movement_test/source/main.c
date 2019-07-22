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
//#include "../header/nokia5110.h"
//#include "../header/timer.h"
//#include "../header/scheduler.h"
#endif

#define input (~PINB & 0x0F)
/*
typedef struct _player {
	unsigned char x;
	unsigned char y;
} player;

player character[7];

enum player_States { p_init, p_wait, p_press, p_up, p_down };

int playerSMTick(int state) {
	unsigned char i = 0;
	switch(state) {
		case p_init:
			break;
		case p_wait:
			if (input == 0x01 || input == 0x02) {
				state = p_up;
			}
			else {
				state = p_wait;
			}
			state = p_wait;
			break;
		case p_press:
			if (input == 0x01 && input != 0x02) {
				state = p_up;
			}
			else if (input != 0x01 && input == 0x02) {
				state = p_down;
			}
			break;
		case p_up:
			state = p_wait;
			break;
		case p_down:
			state = p_wait;
			break;
		default:
			state = p_init;
			break;
	}
	switch(state) {
		case p_init:
			for (i = 0; i < 7; i++) {
				nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
			}
			i = 0;
			nokia_lcd_render();
			state = p_wait;
			break;
		case p_wait:
			for (i = 0; i < 7; i++) {
				nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
			}
			nokia_lcd_render();
			break;
		case p_press:
			break;
		case p_up:
			if (character[0].y == 1) {
				for (i = 0; i < 7; i++) {
					nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
				}
			}
			else {
				for (i = 0; i < 7; i++) {
					nokia_lcd_set_pixel(character[i].x, character[i].y, 0);
				}
				for (i = 0; i < 7; i++) {
					character[i].y = character[i].y - 1;
					nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
				}
			}
			i = 0;
			nokia_lcd_render();
			break;
		case p_down:
			if (character[7].y == 42) {
				for (i = 0; i < 7; i++) {
					nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
				}
			}
			else {
				for (i = 0; i < 7; i++) {
					nokia_lcd_set_pixel(character[i].x, character[i].y, 0);
				}
				for (i = 0; i < 7; i++) {
					character[i].y = character[i].y + 1;
					nokia_lcd_set_pixel(character[i].x, character[i].y, 1);
				}
			}
			i = 0;
			nokia_lcd_render();
			break;
		default:
			break;
	}
}*/
			
int main(void) {
	/* Insert DDR and PORT initializations */
/*	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;

	character[0].x = 1;
	character[1].x = 2;
	character[2].x = 1;
	character[3].x = 2;
	character[4].x = 3;
	character[5].x = 2;
	character[6].x = 1;

	character[0].y = 1;
	character[1].y = 2;
	character[2].y = 3;
	character[3].y = 3;
	character[4].y = 3;
	character[5].y = 4;
	character[6].y = 5;

	static task player_Task;
	task *tasks[] = { &player_Task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
*/
	/*Tasks*/
/*	player_Task.state = p_init;
	player_Task.period = 1;
	player_Task.elapsedTime = 1;
	player_Task.TickFct = &playerSMTick;

	nokia_lcd_init();
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
	return 1;*/
	return 1;	
}
