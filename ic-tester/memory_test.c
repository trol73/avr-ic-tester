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

#define MEGA32

#ifdef MEGA32
uint16_t failures_0[8*8];
uint16_t successes_0[8*8];
uint16_t failures_1[8*8];
uint16_t successes_1[8*8];

#else

uint8_t failures_0[8];
uint8_t successes_0[8];
uint8_t failures_1[8];
uint8_t successes_1[8];

#endif

void MemInit() {
	DDRC &= ~_BV(1);
	DDRA |= _BV(5)|_BV(6)|_BV(7);
	DDRC |= _BV(0)|_BV(2)|_BV(3)|_BV(4)|_BV(5)|_BV(6);
	DDRD |= _BV(2)|_BV(3)|_BV(4)|_BV(5)|_BV(6)|_BV(7);
	
	PORTD &= ~_BV(7);
	PORTA |= _BV(5);
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
uint8_t MemReadBit(uint16_t row, uint16_t col) {
	set_RAS();
	set_CAS();
	set_WE();

	setAddress(row);
	clr_RAS();

	setAddress(col);
	clr_CAS();

	_delay_us(1);
	
	// выполняем проверку выхода несколько раз чтобы выявить возможный шум (актуально если не подключено микросхемы)
	uint8_t result = get_DO();
	for (uint8_t i = 0; i < 3; i++) {
		if (get_DO() != result) {
			result = 0xff;
			break;
		}
	}

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
	
#ifdef MEGA32
	for (uint8_t row = 0; row < 8*8; row++) {
#else
	for (uint8_t row = 0; row < 8; row++) {
#endif			
		failures_0[row] = 0;
		successes_0[row] = 0;
		failures_1[row] = 0;
		successes_1[row] = 0;		
	}

	uint16_t size = 1 << rows;
	uint8_t numberPerCell = size >> 3;

	
	// заполняем все нулями
	for (uint16_t row = 0; row < size; row++) {
		for (uint16_t col = 0; col < size; col++) {
			MemWriteBit(row, col, 0);
		}
		MemRegenerate();
	}
	set_DI(0);
	// проверяем нули
	for (uint16_t row = 0; row < size; row++) {
		for (uint16_t col = 0; col < size; col++) {
			uint16_t r = row / numberPerCell;
			uint32_t c = col / numberPerCell;
			uint8_t val = MemReadBit(row, col);
			
#ifdef MEGA32	
			if (val == 0) {
				successes_0[r*8+c]++;
			} else {
				failures_0[r*8+c]++;
			}
#else
			if (val == 0) {
				successes_0[r] |= _BV(c);
			} else {
				failures_0[r] |= _BV(c);
			}
#endif			
		}
		MemRegenerate();
	}
	//MemDebug();
	//
	//for (uint16_t row = 0; row < rows; row++) {
		//failures[row] = 0;
		//successes[row] = 0;
	//}
	
	// заполняем все единицами
	for (uint16_t row = 0; row < size; row++) {
		for (uint16_t col = 0; col < size; col++) {
			MemWriteBit(row, col, 1);
		}
		MemRegenerate();
	}

	set_DI(0);
	// проверяем единицы
	for (uint16_t row = 0; row < size; row++) {
		for (uint16_t col = 0; col < size; col++) {
			uint16_t r = row / numberPerCell;
			uint32_t c = col / numberPerCell;
			uint8_t val = MemReadBit(row, col);
#ifdef MEGA32
			if (val == 1) {
				successes_1[r*8+c]++;
			} else {
				failures_1[r*8+c]++;
			}
#else			
			if (val == 1) {
				successes_1[r] |= _BV(c);
			} else {
				failures_1[r] |= _BV(c);
			}
#endif			
		}
		MemRegenerate();
	}
	MemDebug();
	MSG("test done");
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

	for (uint16_t row = 0; row < 8; row++) {
		for (uint16_t col = 0; col < 8; col++) {
#ifdef MEGA32 
			uart_putdw_dec(successes_0[row*8+col]);
			uart_putc(' ');
			uart_putc(' ');			
#else
			bool good = successes_0[row] & _BV(col);
			bool bad = failures_0[row] & _BV(col);
			char ch;
			if (good && !bad) {
				ch = '.';
			} else if (bad && !good) {
				ch = 'x';
			} else {
				ch = '?';
			}
			uart_putc(ch);
#endif			
		}
		uart_putc('\n');
	}
	uart_putc('\n');
	uart_putc('\n');
	
	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t col = 0; col < rows; col++) {
#ifdef MEGA32
			uart_putdw_dec(successes_1[row*8+col]);
			uart_putc(' ');
			uart_putc(' ');
#else
			bool good = successes_1[row] & _BV(col);
			bool bad = failures_1[row] & _BV(col);
			char ch;
			if (good && !bad) {
				ch = '.';
			} else if (bad && !good) {
				ch = 'x';
			} else {
				ch = '?';
			}
			uart_putc(ch);
#endif
		}
		uart_putc('\n');
	}
	uart_putc('\n');
	uart_putc('\n');	
	
}


uint8_t MemTestGetCell(uint8_t row, uint8_t col) {
#ifdef MEGA32
	bool good = successes_0[row*8+col] > 0  && successes_1[row*8+col] > 0;
	bool bad = failures_0[row*8+col] > 0 || failures_1[row*8+col] > 0;
#else
	bool good = (successes_0[row] & _BV(col)) && (successes_1[row] & _BV(col));
	bool bad = (failures_0[row] & _BV(col)) || (failures_1[row] & _BV(col));
#endif

	if (good && !bad) {
		return TEST_CELL_GOOD;
	}
	if (bad && !good) {
		return TEST_CELL_BAD;
	}
	return TEST_CELL_UNKNOWN;
}


