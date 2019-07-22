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

#define input (~PINB & 0x0F)

/* Globals */
unsigned char gameTimeTens = 51;
unsigned char gameTimeOnes = 48;
unsigned char gameScoreTens = 48;
unsigned char gameScoreOnes = 48;
unsigned char gameStaminaTens = 48;
unsigned char gameStaminaOnes = 53;
unsigned short gameCnt = 1000;

enum nokia_States { n_init, /*n_menu,*/ n_display, /*n_final*/ };

int nokiaSMTick(int state) {
	/* Transitions */
	switch(state) {
		case n_init:
//			state = n_menu;
			state = n_display;
			break;
/*		case n_menu:
			if (input == 0x01) {
				state = n_display;
			}
			else {
				state = n_menu;
			}
			break;*/
		case n_display:
/*			if (gameTime == 0) {
				state = n_final;
			}
			else {
				state = n_display;
			}*/
			state = n_display;
			break;
/*		case n_final:
			if (input == 0x01) {
				state = n_menu;
			}
			else {
				state = n_final;
			}
			break;*/
		default:
			state = n_init;
			break;
	}
	switch(state) {
		case n_init:
			break;
		case n_display:
			nokia_lcd_clear();
			nokia_lcd_write_string("SCOR: ", 1);
			nokia_lcd_set_cursor(50, 0);
			nokia_lcd_write_char(gameScoreTens, 1);
			nokia_lcd_set_cursor(56, 0);
			nokia_lcd_write_char(gameScoreOnes, 1);
			nokia_lcd_set_cursor(0, 15);
			nokia_lcd_write_string("STAM: ", 1);
			nokia_lcd_set_cursor(50, 15);
			nokia_lcd_write_char(gameStaminaTens, 1);
			nokia_lcd_set_cursor(56, 15);
			nokia_lcd_write_char(gameStaminaOnes, 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("TIME: ", 1);
			nokia_lcd_set_cursor(50, 30);
			nokia_lcd_write_char(gameTimeTens, 1);
			nokia_lcd_set_cursor(56, 30);
			nokia_lcd_write_char(gameTimeOnes, 1);
			nokia_lcd_render();
		default:
			break;
	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;

	static task player_Task;
	task *tasks[] = { &player_Task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	/*Tasks*/
	player_Task.state = n_init;
	player_Task.period = 1;
	player_Task.elapsedTime = 1;
	player_Task.TickFct = &nokiaSMTick;

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
	return 1;	
}
