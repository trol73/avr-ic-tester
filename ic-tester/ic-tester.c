/*
 * ic_tester.c
 *
 * Created: 14.07.2015 21:30:16
 *  Author: Trol
 */ 



#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>


#include "lib/glcd/glcd.h"
#include <util/delay.h>

#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "keyboard.h"
#include "test.h"

static void init_lcd() {
    glcd_init();
    glcd_set_contrast(100);
    glcd_clear();
    glcd_clear();
}

void onKeyPressed(uint8_t key) {
	MSG_DEC("key ", key);
	if (key == KEY_TEST) {
		if (TestLogic()) {
			uart_puts(GetDeviceName());
		} else {
			MSG("FAILED");
		}
	}
	
}

int main(void) {
	KeyboardInit();
	init_lcd();
	uart_init();

    while(1) {
		KeyboardCheck();

//		glcd_invert_area(1, 10, 20, 30);		
    }
}