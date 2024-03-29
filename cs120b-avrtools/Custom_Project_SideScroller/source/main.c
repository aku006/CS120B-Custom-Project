/*	Author: aku006
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

//Update 7/23/2019: First demo done, Nokia LCD and custom characters working
//Update 7/25/2019: Custom project completed for class, may continue to work on this on my own time later

#include <avr/io.h>
//#include <avr/eeprom.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <util/delay.h>
#include "../header/nokia5110.h"
#include "../header/timer.h"
#include "../header/scheduler.h"
#include "../header/io.h"
#include <avr/eeprom.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Global variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define input (~PINB & 0x0F)
#define MAX 45

/* Stats used */
unsigned char gameTimeTens = 0; //Need to use two chars to represent each digit on Nokia screen
unsigned char gameTimeOnes = 0;
unsigned char gameScoreTens = 48;
unsigned char gameScoreOnes = 48;
unsigned char gameStaminaTens = 48;
unsigned char gameStaminaOnes = 53;
unsigned char highScoreTens = 48;
unsigned char highScoreOnes = 48;
unsigned char runCnt = 0; //Counts needed to help game run in real time with chosen periods
unsigned char updateCnt = 0;
unsigned short timerCnt = 0;
unsigned char gemCnt = 0;
unsigned char demonCnt = 0;
unsigned char fruitCnt = 0;
unsigned char playerPos = 17; //Player position inialized

/* Custom characters created as arrays */
unsigned char player[8] = { 0x18, 0x0C, 0x16, 0x1D, 0x1F, 0x16, 0x0C, 0x18 };
unsigned char gem[8] = { 0x04, 0x0A, 0x11, 0x15, 0x15, 0x11, 0x0A, 0x04 };
unsigned char demon[8] = { 0x11, 0x1F, 0x0E, 0x04, 0x1F, 0x15, 0x0E, 0x1B };
unsigned char fruit[8] = { 0x00, 0x0E, 0x04, 0x0E, 0x1F, 0x1F, 0x0E, 0x00 };

/* Character array  that contains what will be put out on top row */
const unsigned char TopSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
					0x20, 0x02, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20,0x20, 0x20, 0x20, 
					0x20, 0x20, 0x20, 0x20, 0x03, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20 };
/* Character array that contains what will be put out on bottom row */
const unsigned char BottomSource[MAX] = { 0x20, 0x20, 0x20, 0x02, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20, 0x20,
					 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
					 0x01, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20, 0x20, 0x20, 0x20, 0x20 }; 

//Smaller arrays that take from the source arrays and actually output them onto the 16x2 LCD
unsigned char top[16];
unsigned char bottom[16];

//Title
unsigned char title[31] = "GETAWAY SWIMMER: THE FISH THIEF";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LCD State Machine: Displays the game on the 16x2 LCD, including the player and all items generated
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* States for LCD */
enum lcd_States { l_init, l_menu, l_reset, l_startup, l_scroll, l_final, l_hold };

int lcdSMTick(int state) {
	/* Local variables */
	unsigned char it; //Used to traverse the smaller top and bottom arrays
	static unsigned char sourcePos; //Traverse the source array when determining what to put into smaller arrays

	/* Transition states */
	switch(state) {
		case l_init:
			state = l_startup;
			break;
		case l_startup:
			state = l_menu;
			break;
		case l_menu:
			if (input & 0x04) {
				state = l_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01)) {
				state = l_menu;
			}
			else if (input & 0x08) {
				state = l_init;
			}
			else {
				state = l_menu;
			}
			break;
		case l_hold:
			if (input & 0x04) {
				state = l_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01)) {
				state = l_reset;
			}
			else {
				state = l_scroll;
			}
			break;
		case l_reset:
			state = l_scroll;
			break;
		case l_scroll:
			if (gameTimeTens == 48 && gameTimeOnes == 48) {
				state = l_final;
			}
			else if (input & 0x08) {
				state = l_init;
			}
			else {
				state = l_scroll;
			}
			break;
		case l_final:
			if (input & 0x08) {
				state = l_init;
			}
			else if (input & 0x04) {
				state = l_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01)) {
				state = l_final;
			}
			else {
				state = l_final;
			}
			break;
		default:
			state = l_init;
			break;
	}
	/* Actions */
	switch(state) {
		case l_init:
			break;
		case l_startup:
			sourcePos = 17;
			strcpy(top, TopSource);
			strcpy(bottom, BottomSource);
			break;
		case l_menu:
			LCD_DisplayString(1, title);
			break;
		case l_hold:
			LCD_ClearScreen();
			sourcePos = 17;
			strcpy(top, TopSource);
			strcpy(bottom, BottomSource);
			break;
		case l_reset:
			break; 
		case l_scroll:
			//Outputs whatever is in top and bottom arrays at the moment
			for (it = 0; it < 16; ++it) {
				LCD_Cursor(it + 1);
				LCD_WriteData(top[it]);
				LCD_Cursor(it + 17);
				LCD_WriteData(bottom[it]);
			}
			//Updates either 1 or 17 to display the player
			LCD_Cursor(playerPos);
			LCD_WriteData(0);
			// Remove the first character from top and bottom arrays and add next characters from source arrays to the back
			if (sourcePos < MAX) {
				for (it = 0; it < 15; it++) {
					top[it] = top[it + 1];
				}
				top[15] = TopSource[sourcePos];
				for (it = 0; it < 15; it++) {
					bottom[it] = bottom[it + 1];
				}
				bottom[15] = BottomSource[sourcePos];
				sourcePos++;
			}
			else {
				sourcePos = 0;
			}
			break;
		case l_final:
			LCD_DisplayString(1, "TIME'S UP!");
			sourcePos = 0;
			break;
		default:
			break;
	}
	return state;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Nokia State Machine: Displays stats on a Nokia 5110 LCD
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* States for Nokia */
enum nokia_States { n_init, n_init2, n_menu, n_reset, n_run, n_update, n_final, n_hold };

int nokiaSMTick(int state) {
	/* Local variable */
	unsigned char eeScore; //Stores the high score into EEPROM

	/* Transitions */
	switch(state) {
		case n_init:
			state = n_init2;
			break;
		case n_init2:
			state = n_menu;
			break;
		case n_menu:
			if (input & 0x04) {
				state = n_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01)) {
				state = n_menu;
			}
			else {
				state = n_menu;
			}
			break;
		case n_hold:
			if (input & 0x04) {
				state = n_hold;
			}
			else if (!(input & 0x04) && !(input & 0x02) && !(input & 0x01))  {
				state = n_reset;
			}
			else   {
				state = n_hold;
			}
			break;
		case n_reset:
			state = n_run;
			break;
		case n_run:
			if (gameTimeOnes == 48 && gameTimeTens == 48) {
				state = n_final;
			}
			//Reset button
			else if (input & 0x08) {
				state = n_init;
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
			//reset while updating
			if (input & 0x08) {
				state = n_init;
			}
			else if (updateCnt >= 1) {
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
			//reset in final screen
			else if (input & 0x08) {
				state = n_init;
			}
			else {
				state = n_final;
			}
			break;
		default:
			state = n_init;
			break;
	}
	/* Actions */
	switch(state) {
		case n_init:
			break;
		case n_init2:
			//First time firing up eeprom_read_byte
			if (eeprom_read_byte((const uint8_t*)5) >= 255) {
				highScoreTens = 48;
				highScoreOnes = 48;
			}
			//If a high score is stored in here, get the high score
			else {
				eeScore = eeprom_read_byte((const uint8_t*)5);
				highScoreTens = (eeScore / 10) + 48;
				highScoreOnes = (eeScore % 10) + 48;
			}

			break;
		/* Display menu, will never come back here */
		case n_menu:
			nokia_lcd_clear();
			nokia_lcd_write_string("Gem:    +1 pnt", 1);
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Demon:  -1 pnt", 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("Fruit: +10 STM", 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("BPress: -1 STM", 1);
			nokia_lcd_set_cursor(0, 40);
			nokia_lcd_write_string("HOLD RED BUTT.", 1);
			nokia_lcd_render();
			break;
		case n_hold:
			nokia_lcd_clear();
			nokia_lcd_write_string("You ready?", 1);
			nokia_lcd_set_cursor(10, 10);
			nokia_lcd_write_string("LET('s)", 2);
			nokia_lcd_set_cursor(25, 30);
			nokia_lcd_write_string("GO!", 2);
			nokia_lcd_render();

			break;
		/* Reset stats each time */
		case n_reset:
			gameTimeTens = 49;
			gameTimeOnes = 53;
			runCnt = 0;
			updateCnt = 0;
			timerCnt = 0;
			break;
		case n_run:
			break;
		case n_update:
			/* If ones digit is 0 at this point, set that to 9 and*/
			/* subtract tens digit by one */
			if (timerCnt >= 1 && gameTimeOnes == 48) {
				timerCnt = 0;
				gameTimeTens = gameTimeTens - 1;
				gameTimeOnes = 57;
			}
			/* If ones digit is not 0, just subtract */
			else if (timerCnt >= 1 && gameTimeOnes != 48) {
				timerCnt = 0;
				gameTimeTens = gameTimeTens;
				gameTimeOnes = gameTimeOnes - 1;
			}
			else {
				timerCnt++;
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
			_delay_ms(50);
			break;
		case n_final:
			/* Update the high score here, since it needs to be displayed immediately afterwards */
			if (gameScoreTens > highScoreTens) {
				highScoreTens = gameScoreTens;
				highScoreOnes = gameScoreOnes;
			}
			else if (gameScoreTens == highScoreTens && gameScoreOnes > highScoreOnes) {
				highScoreTens = gameScoreTens;
				highScoreOnes = gameScoreOnes;
			}
			else {
				highScoreTens = highScoreTens;
				highScoreOnes = highScoreOnes;
			}
			eeScore = ((highScoreTens - 48) * 10) + (highScoreOnes - 48);
			eeprom_update_byte((uint8_t*)5, eeScore);

			nokia_lcd_clear();
			nokia_lcd_write_string("FINAL SCORE:", 1);
			nokia_lcd_set_cursor(73, 0);
			nokia_lcd_write_char(gameScoreTens, 1);
			nokia_lcd_set_cursor(79, 0);
			nokia_lcd_write_char(gameScoreOnes, 1);
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("HIGH SCORE:", 1);
			nokia_lcd_set_cursor(73, 10);
			nokia_lcd_write_char(highScoreTens, 1);
			nokia_lcd_set_cursor(79, 10);
			nokia_lcd_write_char(highScoreOnes, 1);
			nokia_lcd_set_cursor(0, 25);
			nokia_lcd_write_string("Hold red butt.", 1);
			nokia_lcd_set_cursor(0, 35);
			nokia_lcd_write_string("to play again", 1);
			nokia_lcd_render();
			break;
		default:
			break;
	}
	return state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Player State Machine: Controller state, moves the player character up and down
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* States for player */
enum player_States { p_init, p_wait, p_press, p_up, p_upHold, p_down, p_downHold };

int playerSMTick(int state) {
	/* Transitions */
	switch(state) {
		case p_init:
			break;
		case p_wait:
			if (input & 0x08) {
				state = p_init;
			}
			//Move up or down as long as there is stamina remaining
			else if ((input == 0x01 || input == 0x02) && ((gameStaminaTens > 48) || ((gameStaminaTens == 48) && (gameStaminaOnes > 48)))) {
				state = p_press;
			}
			else {
				state = p_wait;
			}
			break;
		case p_press:
			if (input == 0x01 && input != 0x02) {
				state = p_upHold;
			}
			else if (input != 0x01 && input == 0x02) {
				state = p_downHold;
			}
			break;
		case p_upHold:
			if ((input & 0x01)) {
				state = p_upHold;
			}
			else {
				state = p_up;
			}
			break;
		case p_downHold:
			if ((input & 0x02)) {
				state = p_downHold;
			}
			else {
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
	/* Actions */
	switch(state) {
		case p_init:
			playerPos = 17; //Remove this if needed
			state = p_wait;
			break;
		case p_wait:
			playerPos = playerPos;
			break;
		case p_press:
			break;
		case p_downHold:
			break;
		case p_upHold:
			break;
		case p_down:
			//Dealing with stamina points, lose a point for every press
			//If ones digit is 0, set that to 9 and subtract tens place by 1
			if (gameStaminaOnes == 48) {
				gameStaminaTens = gameStaminaTens - 1;
				gameStaminaOnes = 57;
			}
			//If ones digit is not 0, subtract by 1
			else {
				gameStaminaOnes = gameStaminaOnes - 1;
			}
			playerPos = 17;
			break;
		case p_up:
			if (gameStaminaOnes == 48) {
				gameStaminaTens = gameStaminaTens - 1;
				gameStaminaOnes = 57;
			}
			else {
				gameStaminaOnes = gameStaminaOnes - 1;
			}
			playerPos = 1;
			break;
		default:
			break;
	}
	return state;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Items State Machine: Determines how player interacts with items
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* States for items */
enum item_States { i_init, i_startup, i_reset, i_wait, i_upScore, i_downScore, i_upStam };

int itemSMTick(int state) {
	/* Transitions */
	switch(state) {
		case i_init:
			state = i_startup;
			break;
		case i_startup:
			if (input & 0x08) {
				state = i_init;
			}
			else if (gameTimeTens == 48 && gameTimeOnes == 48) {
				state = i_startup;
			}
			else if ((gameTimeTens > 48) || (gameTimeTens == 48 && gameTimeOnes > 48)) {
				state = i_reset;
			}
			else {
				state = i_startup;
			}
			break;
		case i_reset:
			state = i_wait;
			break;
		case i_wait:
			if (input & 0x08) {
				state = i_init;
			}
			else if (gameTimeTens == 48 && gameTimeOnes == 48) {
				state = i_startup;
			}
			else if ((top[0] == 0x01 && playerPos == 1) || (bottom[0] == 0x01 && playerPos == 17)) {
				state = i_upScore;
			}
			else if ((top[0] == 0x02 && playerPos == 1) || (bottom[0] == 0x02 && playerPos == 17)) {
				state = i_downScore;
			}
			else if ((top[0] == 0x03 && playerPos == 1) || (bottom[0] == 0x03 && playerPos == 17)) {
				state = i_upStam;
			}
			else {
				state = i_wait;
			}
			break;
		case i_upScore:
			state = i_wait;
			break;
		case i_downScore:
			state = i_wait;
			break;
		case i_upStam:
			state = i_wait;
			break;
		default:
			state = i_init;
			break;
	}
	/* Actions */
	switch(state) {
		case i_init:
			break;
		case i_startup:
			break;
		case i_reset:
			gemCnt = 0;
			demonCnt = 0;
			fruitCnt = 0;
			gameScoreTens = 48;
			gameScoreOnes = 48;
			gameStaminaTens = 48;
			gameStaminaOnes = 53;
			break;
		case i_wait:
			break;
		case i_upScore:
			//gemCnt needs to be at least 2 for a score to be counted
			//If score is 99, just reset gemCnt, don't add any more to the score
			if (gameScoreTens == 57 && gameScoreOnes == 57 && gemCnt >= 2) {
				gemCnt = 0;
				gameScoreTens = gameScoreTens;
				gameScoreOnes = gameScoreOnes;
			}
			//If ones digit is 9 and gemCnt is at least 2, increment tens digit by one and set ones digit to 0
			else if (gameScoreOnes == 57 && gemCnt >= 2) {
				gemCnt = 0;
				gameScoreTens = gameScoreTens + 1;
				gameScoreOnes = 48;
			}
			//If ones digit is less than 9 and gemCnt is at least 2, just update ones digit
			else if (gameScoreOnes != 57 && gemCnt >= 2){
				gemCnt = 0;
				gameScoreOnes = gameScoreOnes + 1;
			}
			//Otherwise increment gemCnt
			else {
				gemCnt++;
			}
			break;
		case i_downScore:
			// If score is zero, keep it there
			if (gameScoreTens == 48 && gameScoreOnes == 48 && demonCnt >= 2) {
				demonCnt = 0;
				gameScoreTens = gameScoreTens;
				gameScoreOnes = gameScoreOnes;
			}
			//If ones digit is 0 and gemCnt is at least 2, decrement tens by 1 and set ones to 9
			else if (gameScoreOnes == 48 && gameScoreTens > 48 && demonCnt >= 2) {
				demonCnt = 0;
				gameScoreTens = gameScoreTens - 1;
				gameScoreOnes = 57;
			}
			//If ones digit is greater than 0, decrement ones
			else if (gameScoreOnes != 48 && demonCnt >= 2) {
				demonCnt = 0;
				gameScoreOnes = gameScoreOnes - 1;
			}
			else {
				demonCnt++;
			}
			break;
		case i_upStam:
			//If stamina is in the nineties, automatically convert to 99
			if (gameStaminaTens == 57 && fruitCnt >= 2) {
				fruitCnt = 0;
				gameStaminaTens = 57;
				gameStaminaOnes = 57;
			}
			//If tens is not in nineties, increment tens by one
			else if (gameStaminaTens < 57 && fruitCnt >= 2) {
				fruitCnt = 0;
				gameStaminaTens = gameStaminaTens + 1;
			}
			else {
				fruitCnt++;
			}
			break;
		default:
			break;
	}
	return state;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Main function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	static task player_Task, item_Task, nokia_Task, lcd_Task;
	task *tasks[] = { &player_Task, &item_Task, &lcd_Task, &nokia_Task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	/*Tasks*/
	player_Task.state = p_init;
	player_Task.period = 25;
	player_Task.elapsedTime = 25;
	player_Task.TickFct = &playerSMTick;

	item_Task.state = i_init;
	item_Task.period = 25;
	item_Task.elapsedTime = 25;
	item_Task.TickFct = &itemSMTick;

	lcd_Task.state = l_init;
	lcd_Task.period = 150;
	lcd_Task.elapsedTime = 150;
	lcd_Task.TickFct = &lcdSMTick;

	nokia_Task.state = n_init;
	nokia_Task.period = 150;
	nokia_Task.elapsedTime = 150;
	nokia_Task.TickFct = &nokiaSMTick;
	
	nokia_lcd_init();
	LCD_init();
	TimerSet(1);
	TimerOn();
	
	/* Save custom characters to locations */
	LCD_CreateCustom(0, player);
	LCD_CreateCustom(1, gem);
	LCD_CreateCustom(2, demon);
	LCD_CreateCustom(3, fruit);

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
	}
	return 1;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Simpler maps held here, used for testing
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Testing for gems */
//const unsigned char BottomSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20, 0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20 };
//const unsigned char TopSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x20, 0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20,0x01,0x20,0x20,0x20,0x20,0x20 };

/* Testing for demons */
//const unsigned char BottomSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20, 0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20 };
//const unsigned char TopSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x20, 0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20,0x02,0x20,0x20,0x20,0x20,0x20 };

/*Testing for fruit */
//const unsigned char BottomSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x03, 0x20, 0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20 };
//const unsigned char TopSource[MAX] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x03, 0x20, 0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20,0x03,0x20,0x20,0x20,0x20,0x20 };
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
