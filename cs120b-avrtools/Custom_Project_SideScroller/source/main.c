/*	Author: aku006
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

//Note: First demo completed as of 7/23/2019, Nokia LCD and custom characters working

#include <avr/io.h>
#include <avr/eeprom.h>
#ifdef _SIMULATE_
#include <util/delay.h>
#include "simAVRHeader.h"
#include "../header/nokia5110.h"
#include "../header/timer.h"
#include "../header/scheduler.h"
#include "../header/io.h"
#endif

#define input (~PINB & 0x0F)

/* Globals */
unsigned char gameTimeTens = 49;
unsigned char gameTimeOnes = 48;
unsigned char gameScoreTens = 48;
unsigned char gameScoreOnes = 48;
unsigned char gameStaminaTens = 48;
unsigned char gameStaminaOnes = 53;
unsigned char runCnt = 0;
unsigned char updateCnt = 0;
unsigned short timerCnt = 0;

unsigned char playerPos = 17;

unsigned char player[8] = { 0x18, 0x0C, 0x16, 0x1D, 0x1F, 0x16, 0x0C, 0x18 };
unsigned char gem[8] = { 0x04, 0x0A, 0x11, 0x15, 0x15, 0x11, 0x0A, 0x04 };
unsigned char demon[8] = { 0x11, 0x1F, 0x0E, 0x04, 0x1F, 0x15, 0x0E, 0x1B };

/* Nokia states, displays stats on the nokia screen */
enum nokia_States { n_init, n_run, n_update, n_final, n_hold };

int nokiaSMTick(int state) {
	/* Transitions */
	switch(state) {
		case n_init:
			state = n_run;
			break;
		case n_run:
			if (gameTimeOnes == 48 && gameTimeTens == 48) {
				state = n_final;
			}
			else if (runCnt >= 1) {
				runCnt = 0;
				state = n_update;
			}
			else {
				runCnt++;
				state = n_run;
			}
			break;
		case n_update:
			if (updateCnt >= 1) {
				updateCnt = 0;
				state = n_run;
			}
			else {
				updateCnt++;
				state = n_update;
			}
			break;
		case n_final:
			if (input & 0x04) {
				state = n_hold;
			}
			else {
				state = n_final;
			}
			break;
		case n_hold:
			if (input & 0x04) {
				state = n_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01))  {
				state = n_run;
			}
			else   {
				state = n_hold;
			}
			break;
		default:
			state = n_init;
			break;
	}
	switch(state) {
		case n_init:
			break;
		/* Together, run and update runs for about 1 second */
		case n_run:
			break;
		case n_update:
			/* If ones digit is 0 at this point, set that to 9 and*/
			/* subtract tens digit by one */
			if (timerCnt >= 2 && gameTimeOnes == 48) {
				timerCnt = 0;
				gameTimeTens = gameTimeTens - 1;
				gameTimeOnes = 57;
			}
			/* If ones digit is not 0, just subtract */
			else if (timerCnt >= 2 && gameTimeOnes != 48) {
				timerCnt = 0;
				gameTimeTens = gameTimeTens;
				gameTimeOnes = gameTimeOnes - 1;
			}
			else {
				timerCnt++;
			}
			
			if (gameScoreOnes == 57) {
				gameScoreTens = gameScoreTens + 1;
				gameScoreOnes = 48;
			}
			else if (gameScoreOnes != 57) {
				gameScoreOnes = gameScoreOnes + 1;
			}
			/* Update all new info */
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
			break;
		case n_final:
			nokia_lcd_clear();
			nokia_lcd_write_string("FINAL SCORE:", 1);
			nokia_lcd_set_cursor(25, 10);
			nokia_lcd_write_char(gameScoreTens, 2);
			nokia_lcd_set_cursor(40, 10);
			nokia_lcd_write_char(gameScoreOnes, 2);
			nokia_lcd_render();
			break;
		case n_hold:
			nokia_lcd_clear();
			nokia_lcd_write_string("You ready?", 1);
			nokia_lcd_set_cursor(20, 10);
			nokia_lcd_write_string("LET's", 2);
			nokia_lcd_set_cursor(25, 30);
			nokia_lcd_write_string("GO!", 2);
			nokia_lcd_render();

			gameTimeTens = 49;
			gameTimeOnes = 48;
			runCnt = 0;
			updateCnt = 0;
			timerCnt = 0;
			break;
		default:
			break;
	}
	/* Debugging part */
//	LCD_Cursor(10);
//	LCD_WriteData(state + '0');
	return state;
}

/* Player states */
enum player_States { p_init, p_wait, p_press, p_up, p_down };

int playerSMTick(int state) {
	switch(state) {
		case p_init:
			break;
		case p_wait:
			if (input == 0x01 || input == 0x02) {
				state = p_press;
			}
			else {
				state = p_wait;
			}
			break;
		case p_press:
			if (input == 0x01 && input != 0x02) {
				state = p_up;
			}
			else if (input != 0x01 && input == 0x02) {
				state = p_down;
			}
			break;
		case p_down:
			state = p_wait;
			break;
		case p_up:
			state = p_wait;
			break;
		default:
			state = p_init;
			break;
	}
	switch(state) {
		case p_init:
			LCD_Cursor(playerPos);
			LCD_WriteData(0);
			state = p_wait;
			break;
		case p_wait:
			playerPos = playerPos;
			LCD_Cursor(playerPos);
			LCD_WriteData(0);
			break;
		case p_press:
			break;
		case p_down:
			playerPos = 17;
			LCD_ClearScreen();
			LCD_Cursor(playerPos);
			LCD_WriteData(0);
			break;
		case p_up:
			playerPos = 1;
			LCD_ClearScreen();
			LCD_Cursor(playerPos);
			LCD_WriteData(0);
			break;
		default:
			break;
	}
	return state;
}

/* 16x2 LCD Screen display states, determines what is output on the 16x2 screen */
enum l_States { l_init, l_scroll, l_end, l_hold };

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	static task player_Task, nokia_Task;
	task *tasks[] = { &player_Task, &nokia_Task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	/*Tasks*/
	player_Task.state = p_init;
	player_Task.period = 1;
	player_Task.elapsedTime = 1;
	player_Task.TickFct = &playerSMTick;

	nokia_Task.state = n_init;
	nokia_Task.period = 1;
	nokia_Task.elapsedTime = 1;
	nokia_Task.TickFct = &nokiaSMTick;

	nokia_lcd_init();
	LCD_init();
	TimerSet(1);
	TimerOn();
	
	LCD_CreateCustom(0, player);
	LCD_CreateCustom(1, gem);
	LCD_CreateCustom(2, demon);

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

