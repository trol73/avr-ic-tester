/*
 * keyboard.c
 *
 * Created: 15.07.2015 13:55:35
 *  Author: Trol
 */ 

#include "keyboard.h"

#include "tester_io.h"

#define KEY_PRESS_TIME		5
#define KEY_REPEAT_TIME		50


uint8_t keyRepeatCount[3];

void KeyboardCheck() {
	if (key_test_pressed()) {
		if (keyRepeatCount[KEY_TEST] < 0xff) {
			keyRepeatCount[KEY_TEST]++;
		}
	} else {
		keyRepeatCount[KEY_TEST] = 0;
	}
	
	if (keyRepeatCount[KEY_TEST] == KEY_PRESS_TIME) {
		onKeyPressed(KEY_TEST);
	}
	
	if (!IsFullMode()) {
		if (key_up_pressed()) {
			if (keyRepeatCount[KEY_UP] < 0xff) {
				keyRepeatCount[KEY_UP]++;
			}
		} else {
			keyRepeatCount[KEY_UP] = 0;
		}
	
		if (key_down_pressed()) {
			if (keyRepeatCount[KEY_DOWN] < 0xff) {
				keyRepeatCount[KEY_DOWN]++;
			}
		} else {
			keyRepeatCount[KEY_DOWN] = 0;
		}
		
		if (keyRepeatCount[KEY_UP] == KEY_PRESS_TIME) {
			onKeyPressed(KEY_UP);
		}
		if (keyRepeatCount[KEY_DOWN] == KEY_PRESS_TIME) {
			onKeyPressed(KEY_DOWN);
		}
	}

}



void KeyboardInit() {
	DDRB &= ~_BV(2);
	PORTB |= _BV(2);
	DDRA &= ~(_BV(2) | _BV(3));
	PORTA |= _BV(2) | _BV(3);

	keyRepeatCount[KEY_TEST] = 0;
	keyRepeatCount[KEY_UP] = 0;
	keyRepeatCount[KEY_DOWN] = 0;
}


bool KeyboardRepeated(uint8_t key) {
	return keyRepeatCount[key] > KEY_REPEAT_TIME;
}