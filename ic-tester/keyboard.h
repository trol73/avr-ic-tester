/*
 * keyboard.h
 *
 * Created: 15.07.2015 13:55:45
 *  Author: Trol
 */ 


#ifndef KEYBOARD_H_
#define KEYBOARD_H_


#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>


#define KEY_TEST	0
#define KEY_UP		1
#define KEY_DOWN	2


inline bool key_test_pressed() {
	return (PINB & _BV(2)) == 0;
}

inline bool key_up_pressed() {
	return (PINA & _BV(2)) == 0;
}

inline bool key_down_pressed() {
	return (PINA & _BV(3)) == 0;
}




void KeyboardInit();
void KeyboardCheck();
bool KeyboardRepeated(uint8_t key);

void onKeyPressed(uint8_t key);


#endif /* KEYBOARD_H_ */