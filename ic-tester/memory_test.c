/*
 * keyboard.h
 *
 * Created: 24.07.2015 19:55:45
 *  Author: Trol
 */ 

#include "memory_test.h"

#include <avr/io.h>
#include "debug.h"

/*********************************************************************************************************
	           __________________
	(D2)  5  --| A0  | РУ7 |    |
	(A6)  7  --| A1  |     |    |
	(A7)  6  --| A2  |     |    |
	(C3) 12  --| A3  |     | DO |--  14  (C1)
	(C4) 11  --| A4  |     |    |
	(C5) 10  --| A5  |     |    |
	(C2) 13  --| A6  |     |    |
	(C6)  9  --| A7  |     |    |
	(D6)  1  --| A8  |     |    |
	           |-----|     |----|
	(D5)  2  --| DI  |     |    |
	           |-----|     | 5V |-- 8   (A5)
	(D3)  4  --|~RAS |     |    |
	(C0) 15  --|~CAS |     | GND|-- 16  (D7)
	(D4)  3  --|~WE  |     |    |
	           ------------------

*********************************************************************************************************/

#define set_RAS()	PORTD |= _BV(3)
#define clr_RAS()	PORTD &= ~_BV(3)
#define set_CAS()	PORTC |= _BV(0)
#define clr_CAS()	PORTC &= ~_BV(0)
#define set_WE()	PORTD |= _BV(4)
#define clr_WE()	PORTD &= ~_BV(4)

#define set_DI(val)		if (val) PORTD |= _BV(5); else PORTD &= ~_BV(5)
#define get_DO()		((PINC & _BV(1)) != 0)

uint16_t rows;    // max rows number. 0x100 - 4164, 0x200 - 41256

uint16_t failures[9];
uint16_t successes[9];

void MemInit() {
	DDRC &= ~_BV(1);
	DDRA |= _BV(5)|_BV(6)|_BV(7);
	DDRC |= _BV(0)|_BV(2)|_BV(3)|_BV(4)|_BV(5)|_BV(6);
	DDRD |= _BV(2)|_BV(3)|_BV(4)|_BV(5)|_BV(6)|_BV(7);
	
	PORTD &= ~_BV(7);
	PORTA |= _BV(5);
//	PORTC |= _BV(1);
}

static void setAddress(uint16_t val) {
	// D2 A6 A7 C3 C4 C5 C2 C6 (D6)
	
	uint8_t a = PORTA & ~(_BV(6) | _BV(7));	
	uint8_t d = PORTD & ~(_BV(2) | _BV(6));
	uint8_t c = PORTC & ~(_BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6));

	if (val & _BV(0)) {
		d |= _BV(2);
	}
	if (val & _BV(1)) {
		a |= _BV(6);
	}
	if (val & _BV(2)) {
		a |= _BV(7);
	}
	c |= val & (_BV(3)|_BV(4)|_BV(5));
	if (val & _BV(6)) {
		c |= _BV(2);
	}
	if (val & _BV(7)) {
		c |= _BV(6);
	}
	if (val & _BV(8)) {
		d |= _BV(6);
	}
	
	PORTA = a;
	PORTC = c;
	PORTD = d;
}

/************************************************************************/
/* Чтение бита памяти                                                   */
/************************************************************************/
bool MemReadBit(uint16_t row, uint16_t col) {
	// A8 -> 0
	set_RAS();
	set_CAS();

	setAddress(row);

	clr_RAS();
	set_WE();

	setAddress(col);

	clr_CAS();

	bool result = get_DO();

	set_RAS();
	set_CAS();

	return result;
}


/************************************************************************/
/* Запись бита памяти                                                   */
/************************************************************************/
void MemWriteBit(uint16_t row, uint16_t col, bool val) {
	set_RAS();
	set_CAS();
	
	setAddress(row);
	
	clr_RAS();
	clr_WE();
	
	set_DI(val);
	
	setAddress(col);
	
	clr_CAS();
	
	set_RAS();
	set_CAS();
}

/************************************************************************/
/* Регенерация памяти, вызывать раз в 4 мс                              */
/************************************************************************/
void MemRegenerate() {
	set_RAS();
	set_CAS();
	
	for (uint16_t row = 0 ; row < rows; row++) {
		setAddress(row);
		clr_RAS();
		set_RAS();
	}
}



void MemTest() {
	rows = 9;
	// заполняем все нулями
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			MemWriteBit(row, col, 0);
		}
	}
	// проверяем нули
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			if (MemReadBit(row, col) == 0) {
				successes[row] |= _BV(col);
			} else {
				failures[row] |= _BV(col);
			}
		}
	}
	MemRegenerate();
	MemDebug();
	// заполняем все единицами
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			MemWriteBit(row, col, 1);
		}
	}
	// проверяем единицы
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			if (MemReadBit(row, col) != 0) {
				successes[row] |= _BV(col);
			} else {
				failures[row] |= _BV(col);
			}
		}
	}
	MemDebug();
}


void MemDebug() {
	MemWriteBit(0, 0, 1);
	MSG_DEC("TST 1 ", MemReadBit(0, 0));
	MSG_DEC("TST 1 ", MemReadBit(0, 0));
	MemWriteBit(0, 0, 0);
	MSG_DEC("TST 0 ", MemReadBit(0, 0));	
	MSG_DEC("TST 0 ", MemReadBit(0, 0));	
	MemWriteBit(0, 0, 1);
	MSG_DEC("TST 1 ", MemReadBit(0, 0));
	MSG_DEC("TST 1 ", MemReadBit(0, 0));
	MemWriteBit(0, 0, 0);
	MSG_DEC("TST 0 ", MemReadBit(0, 0));	
	MemWriteBit(0, 0, 0);
	
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			bool good = successes[row] | _BV(col);
			bool bad = failures[row] | _BV(col);
			char ch;
			if (good && !bad) {
				ch = '.';
			} else if (bad && !good) {
				ch = 'x';
			} else {
				ch = '?';
			}
			uart_putc(ch);
		}
		uart_putc('\n');
	}
}