/*
 * keyboard.h
 *
 * Created: 24.07.2015 19:55:45
 *  Author: Trol
 */ 

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
	(D4)  3  --|~W/R |     |    |
	           ------------------

*********************************************************************************************************/

#define set_RAS()
#define clr_RAS()
#define set_CAS()
#define clr_CAS()
#define set_WR()
#define clr_WR()

#define set_DI(val)
#define get_DO()


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
	c |= val & (_BV(3)|_BV(4)|_BV(5))
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
/*	
чтение бита памяти

A9 -> 0
RAS -> 1
CAS -> 1

A9 -> row >> 8

address(row)

RAS -> 0
WE -> 1

A9 -> col >> 8
address(col)

CAS -> 0

data <- DO

RAS -> 1
CAS -> 1


 */
}
/*


запись бита памяти

RAS -> 1
CAS -> 1

A9 -> row >> 8
address(row)

RAS -> 0
WE -> 0

DI -> val

A9 -> col >> 8
address(col)

CAS -> 0

RAS -> 1
CAS -> 1


регенерация памяти
вызывается раз в 4 мс
uint16_t rows;    // max rows number. 0x100 - 4164, 0x200 - 41256

RAS -> 1
CAS -> 1


for i = 0 .. rows:
	address(i)
	A9 -> i >> 8
	RAS -> 0
	RAS -> 1


*/
