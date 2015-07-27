/*
 * ui.c
 *
 * Created: 27.07.2015 20:16:58
 *  Author: Trol
 */ 

#include "ui.h"

#include <string.h>
#include <stdio.h>

#include "lib/glcd/glcd.h"
#include "lib/glcd/fonts/font5x7.h"

#include "debug.h"

#include "keyboard.h"
#include "i18n.h"


#define LINES_PER_SCREEN			5	// строк на экране
#define LINES_DY					9	// расстояние между строками


uint8_t screen;
uint8_t selectedIndex;



static void glcd_drawCenteredStr_p(const char *str, uint8_t y, uint8_t dx);
static void glcd_drawCenteredStr(const char *str, uint8_t y, uint8_t dx);

uint8_t GetScreen() {
	return screen;
}


static void drawMainMenu() {
	glcd_drawCenteredStr_p(i18n(STR_AUTODETECT), 1, 1);
	glcd_drawCenteredStr_p(i18n(STR_CUSTOM_TEST), 1+LINES_DY, 1);
	glcd_drawCenteredStr_p(i18n(STR_MEM_TEST), 1+2*LINES_DY, 1);
	glcd_drawCenteredStr_p(i18n(STR_ABOUT), 1+3*LINES_DY, 1);
	
	// выделяем текущий
	glcd_invert_area(5, selectedIndex*LINES_DY, GLCD_LCD_WIDTH-10, LINES_DY);
}


static void drawChipAutoTest() {
	
}

static void drawCustomTest() {
	// 84 x 48
	// >[# ]       >[##] 	
	// <[##]       >[##]    CFG
	// >[##]       <[##]    SET
	// >[ #]       <[##]    EXIT
	//-----------------
	// >[# ]       >[##]
	// >[# ]       <[##]
	// >[# ]       <[##]
	// >[  ]       <[##]

}

static void drawMemoryTest() {
	// сетка 9x9    4x4 или 5x5
	uint8_t size = 8;
	for (uint8_t row = 0; row < size; row++) {
		for (uint8_t col = 0; col < size; col++) {
			
		}
	}	
}

static void drawAboutScreen() {
	
}

void Draw() {
	glcd_tiny_set_font(Font5x7, 5, 7, 32, 0xff);
	glcd_clear_buffer();

	switch(screen) {
		case SCREEN_MAIN_MENU:
			drawMainMenu();
			break;
		case SCREEN_CHIP_AUTO_TEST:
			drawChipAutoTest();
			break;
		case SCREEN_CUSTOM_TEST:
			drawCustomTest();
			break;
		case SCREEN_MEMORY_TEST:
			drawMemoryTest();
			break;
		case SCREEN_ABOUT:
			drawAboutScreen();
			break;
	}
	glcd_write();
}




static void handleMainMenu(uint8_t key) {
	switch(key) {
		case KEY_UP:
			if (selectedIndex > 0) {
				selectedIndex--;
			}
			break;
		case KEY_DOWN:
			if (selectedIndex < 3) {
				selectedIndex++;
			}
			break;
		case KEY_TEST:
			if (selectedIndex == 0) {
				screen = SCREEN_CHIP_AUTO_TEST;
			} else if (selectedIndex == 1) {
				screen = SCREEN_CUSTOM_TEST;
			} else if (selectedIndex == 2) {
				screen = SCREEN_MEMORY_TEST;
			} else {
				screen = SCREEN_ABOUT;
			}
			break;
	}
}

static void handleChipAutoTest(uint8_t key) {
	if (key == KEY_TEST) {
		
	} else {
		screen = SCREEN_MAIN_MENU;
	}
}

static void handleCustomTest(uint8_t key) {
	
}

static void handleMemoryTest(uint8_t key) {
	
}

static void handleAbout(uint8_t key) {
	
}



void onKeyPressed(uint8_t key) {
	switch(screen) {
		case SCREEN_MAIN_MENU:
			handleMainMenu(key);
			break;
		case SCREEN_CHIP_AUTO_TEST:
			handleChipAutoTest(key);
			break;
		case SCREEN_CUSTOM_TEST:
			handleCustomTest(key);
			break;
		case SCREEN_MEMORY_TEST:
			handleMemoryTest(key)
			break;
		case SCREEN_ABOUT:
			handleAbout(key);
			break;
	}
	Draw();	
}



// Рисует строку, отцентрированную по горизонтали
static void glcd_drawCenteredStr_p(const char *str, uint8_t y, uint8_t dx) {
	uint8_t len = strlen_P(str);
	uint8_t x = (GLCD_LCD_WIDTH - len*5 - (len-1)*dx)/2;
	while (1) {
		char c = pgm_read_byte(str++);
		if (!c) {
			return;
		}
		x += glcd_draw_char_xy(x, y, c) + dx;
		c++;
	}
}


static void glcd_drawCenteredStr(const char *str, uint8_t y, uint8_t dx) {
	uint8_t len = strlen(str);
	uint8_t x;
	if (len <= 15) {
		x = (GLCD_LCD_WIDTH - len*5 - (len-1)*dx)/2;
	} else {
		x = 0;
	}
	uint8_t i = 0;
	while (1) {
		char c = str[i++];
		if (!c) {
			return;
		}
		x += glcd_draw_char_xy(x, y, c) + dx;
		c++;
	}
}
