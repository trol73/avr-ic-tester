/*
 * ui.h
 *
 * Created: 27.07.2015 20:17:00
 *  Author: Trol
 */ 

#ifndef _UI_H_
#define _UI_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#define SCREEN_MAIN_MENU				0
#define SCREEN_CHIP_AUTO_TEST			1
#define SCREEN_CUSTOM_TEST			2
#define SCREEN_MEMORY_TEST			3
#define SCREEN_ABOUT				4


void InitDisplay();

uint8_t GetScreen();

void Draw();

void onKeyPressed(uint8_t key);



#endif	// _UI_H_