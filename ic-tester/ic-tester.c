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
#include "ui.h"



//void onKeyPressed(uint8_t key) {
	//MSG_DEC("key ", key);
	//
	//
	
	//if (key == KEY_TEST) {
		//if (TestLogic()) {
			//uart_puts(GetDeviceName());
			//MSG("draw...");
			//init_lcd();
			//glcd_draw_string_xy(0, 0, GetDeviceName());
			//glcd_write();
			//MSG("draw!");
		//} else {
			//MSG("draw...");
			//init_lcd();
			//glcd_draw_string_xy_P(0, 0, STR_UNKNOWN_OR_FAILED);
			//glcd_write();
			//MSG("FAILED");
		//}
	//}
	
//}

int main(void) {
	KeyboardInit();
	
	uart_init();
	
	//glcd_draw_string_xy_P(0, 0, STR_APPNAME);
	//glcd_draw_string_xy_P(0, 10, STR_VERSION);
	//glcd_write();
	
	InitDisplay();
	Draw();

    while(1) {
		KeyboardCheck();
		

//		glcd_invert_area(1, 10, 20, 30);		
    }
}