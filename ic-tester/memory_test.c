/*
 * keyboard.h
 *
 * Created: 24.07.2015 19:55:45
 *  Author: Trol
 */ 

#include "memory_test.h"

#include <util/delay.h>
#include <avr/io.h>

#include "debug.h"
#include "tester_io.h"

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
	set_RAS();
	set_CAS();
	set_WE();

	setAddress(row);
	clr_RAS();

	setAddress(col);
	clr_CAS();

	_delay_us(1);
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
	
	_delay_us(1);
	
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
	//rows = 8;
	//MemDebug();
	//MemDebug();
	//MemDebug();
	//return;
	
	MemWriteBit(0, 0, 1);	MemRegenerate();
	MemWriteBit(0, 0, 1);	MemRegenerate();
	MemWriteBit(0, 0, 1);	MemRegenerate();
	MemWriteBit(5, 5, 0);	MemRegenerate();
	MemWriteBit(5, 5, 0);	MemRegenerate();
	MemWriteBit(5, 5, 0);	MemRegenerate();
	//_delay_us(10);
	uint8_t r1, r2, r3;
	r1 = MemReadBit(0, 0);	MemRegenerate();
	r2 = MemReadBit(0, 0);	MemRegenerate();
	r3 = MemReadBit(0, 0);	MemRegenerate();
	MSG_DEC("TST0 1 ", r1);
	MSG_DEC("TST0 1 ", r2);
	MSG_DEC("TST0 1 ", r3);

	r1 = MemReadBit(5, 5);	MemRegenerate();
	r2 = MemReadBit(5, 5);	MemRegenerate();
	r3 = MemReadBit(5, 5);	MemRegenerate();
	MSG_DEC("TST5 0 ", r1);
	MSG_DEC("TST5 0 ", r2);
	MSG_DEC("TST5 0 ", r3);
	
	
	MemWriteBit(0, 0, 0);	MemRegenerate();
	MemWriteBit(0, 0, 0);	MemRegenerate();
	MemWriteBit(0, 0, 0);	MemRegenerate();
	MemWriteBit(5, 5, 1);	MemRegenerate();
	MemWriteBit(5, 5, 1);	MemRegenerate();
	MemWriteBit(5, 5, 1);	MemRegenerate();
	
	r1 = MemReadBit(0, 0);	MemRegenerate();
	r2 = MemReadBit(0, 0);	MemRegenerate();
	r3 = MemReadBit(0, 0);	MemRegenerate();
	MSG_DEC("TST0 0 ", r1);
	MSG_DEC("TST0 0 ", r2);
	MSG_DEC("TST0 0 ", r3);
	//_delay_us(10);
	r1 = MemReadBit(5, 5);	MemRegenerate();
	r2 = MemReadBit(5, 5);	MemRegenerate();
	r3 = MemReadBit(5, 5);	MemRegenerate();
	MSG_DEC("TST5 1 ", r1);
	MSG_DEC("TST5 1 ", r2);
	MSG_DEC("TST5 1 ", r3);

	

	
	rows = 8;
	
	for (uint16_t row = 0; row < rows; row++) {
		failures[row] = 0;
		successes[row] = 0;
	}
	
	// заполняем все нулями
	for (uint16_t row = 0; row < rows; row += 32) {
		for (uint16_t col = 0; col < rows; col += 32) {
			MemWriteBit(row, col, 0);
			MemRegenerate();
		}
		MemRegenerate();
	}
	MemRegenerate();
	uint16_t size = 1 << rows;

	// проверяем нули
	for (uint16_t row = 0; row < size; row += 32) {
		for (uint16_t col = 0; col < size; col += 32) {
			uint16_t r = row / 32;
			uint32_t c = col / 32;
			if (MemReadBit(row, col) == 0) {
				successes[r] |= _BV(c);
			} else {
				failures[r] |= _BV(c);
			}
			MemRegenerate();
		}
		MemRegenerate();
	}
	MemRegenerate();
	MemDebug();
	
	for (uint16_t row = 0; row < rows; row++) {
		failures[row] = 0;
		successes[row] = 0;
	}
	
	// заполняем все единицами
	for (uint16_t row = 0; row < size; row += 32) {
		for (uint16_t col = 0; col < size; col +=32) {
			MemWriteBit(row, col, 1);
		}
		MemRegenerate();
	}
	MemRegenerate();
	// проверяем единицы
	for (uint16_t row = 0; row < size; row += 32) {
		for (uint16_t col = 0; col < size; col += 32) {
			uint16_t r = row / 32;
			uint32_t c = col / 32;
			if (MemReadBit(row, col) != 0) {
				successes[r] |= _BV(c);
			} else {
				failures[r] |= _BV(c);
			}
		}
		MemRegenerate();
	}
	MemDebug();
}


void MemDebug() {
/*	
	MemWriteBit(0, 0, 1);
	MemWriteBit(0, 0, 1);
	MemWriteBit(0, 0, 1);
	_delay_us(10);
	uint8_t r1, r2, r3;
	r1 = MemReadBit(0, 0);
	r2 = MemReadBit(0, 0);
	r3 = MemReadBit(0, 0);
	MSG_DEC("TST 1 ", r1);
	MSG_DEC("TST 1 ", r2);
	MSG_DEC("TST 1 ", r3);
	MemWriteBit(0, 0, 0);
	MemWriteBit(0, 0, 0);
	MemWriteBit(0, 0, 0);
	_delay_us(10);
	r1 = MemReadBit(0, 0);
	r2 = MemReadBit(0, 0);
	r3 = MemReadBit(0, 0);
	MSG_DEC("TST 0 ", r1);	
	MSG_DEC("TST 0 ", r2);	
	MSG_DEC("TST 0 ", r3);	
	MemWriteBit(5, 5, 1);
	MemWriteBit(5, 5, 1);
	MemWriteBit(5, 5, 1);
	_delay_us(10);
	r1 = MemReadBit(5, 5);
	r2 = MemReadBit(5, 5);
	r3 = MemReadBit(5, 5);
	MSG_DEC("TST 1 ", r1);
	MSG_DEC("TST 1 ", r2);
	MSG_DEC("TST 1 ", r3);
	MemWriteBit(5, 5, 0);
	MemWriteBit(5, 5, 0);
	MemWriteBit(5, 5, 0);
	_delay_us(10);
	r1 = MemReadBit(5, 5);
	r2 = MemReadBit(5, 5);
	r3 = MemReadBit(5, 5);
	MSG_DEC("TST 0 ", r1);	
	MSG_DEC("TST 0 ", r2);	
	MSG_DEC("TST 0 ", r3);	
	
	return;
*/	
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
			bool good = successes[row] & _BV(col);
			bool bad = failures[row] & _BV(col);
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
	uart_putc('\n');
	uart_putc('\n');
}