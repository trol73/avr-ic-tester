/*
 * tester.c
 *
 * Created: 15.07.2015 10:53:35
 *  Author: Trol
 */ 



#include "tester_io.h"

#include <avr/io.h>

#include "debug.h"

bool testerFullMode = false;


/************************************************************************************************************************************************
              28                                     20                                    16                                    14
        _______  _______                      _______  _______                      _______  _______                      _______  _______
  D6  --| 1    \/   28 |-- D7           D6  --| 1    \/   20 |-- D7           D6  --| 1    \/   16 |-- D7           D6  --| 1    \/   14 |-- D7
  D5  --| 2         27 |-- C0           D5  --| 2         19 |-- C0           D5  --| 2         15 |-- C0           D5  --| 2         13 |-- C0
  D4  --| 3         26 |-- C1           D4  --| 3         18 |-- C1           D4  --| 3         14 |-- C1           D4  --| 3         12 |-- C1
  D3  --| 4         25 |-- C2           D3  --| 4         17 |-- C2           D3  --| 4         13 |-- C2           D3  --| 4         11 |-- C2
  D2  --| 5         24 |-- C3           D2  --| 5         16 |-- C3           D2  --| 5         12 |-- C3           D2  --| 5         10 |-- C3
  A7  --| 6         23 |-- C4           A7  --| 6         15 |-- C4           A7  --| 6         11 |-- C4           A7  --| 6          9 |-- C4
  A6  --| 7         22 |-- C5           A6  --| 7         14 |-- C5           A6  --| 7         10 |-- C5           A6  --| 7          8 |-- C5
  A5  --| 8         21 |-- C6           A5  --| 8         13 |-- C6           A5  --| 8          9 |-- C6                 ----------------
  A4  --| 9         20 |-- C7           A4  --| 9         12 |-- C7                 ----------------
  A0  --| 10        19 |-- B6           A0  --| 10        11 |-- B6
  A1  --| 11        18 |-- B4                 ----------------
  A2  --| 12*       17 |-- B3
  A3  --| 13*      *16 |-- D1
   |----| 14       *15 |-- D0
        ----------------  
		
	LCD:	B0, B1, B5, B7
	Btn0:	B2
	Btn1:	A2
	Btn2:	A3
	
************************************************************************************************************************************************/

typedef struct {
	uint8_t a, b, c, d;
} regs_t;


void TesterReset(bool fullMode) {
	DDRA = 0;
	DDRB &= ~(_BV(3) | _BV(4) | _BV(6));
	DDRC = 0;
	DDRD = 0;
	
	PORTA = fullMode ? 0 : _BV(2) | _BV(3);
	PORTB &= ~(_BV(3) | _BV(4) | _BV(6));
	PORTC = 0;
	PORTD = 0;
	
	testerFullMode = fullMode;
}


#define map_bit(maskBit, dest, destBit)		if (mask & _BV(maskBit)) dest |= _BV(destBit); else dest &= ~_BV(destBit)
#define map_bit_(maskBit, dest, destBit)		if (mask & (1L << (maskBit))) dest |= _BV(destBit); else dest &= ~_BV(destBit)


void TesterConfig16(uint16_t mask) {
	//MSG_HEX("configure ", mask, 2);
	// D6 D5 D4 D3 D2 A7 A6 A5   C6 C5 C4 C3 C2 C1 C0 D7
	uint8_t d = DDRD;
	uint8_t a = DDRA;
	uint8_t c = DDRC;
	
	map_bit(0, d, 6);
	map_bit(1, d, 5);
	map_bit(2, d, 4);
	map_bit(3, d, 3);
	map_bit(4, d, 2);
	map_bit(5, a, 7);
	map_bit(6, a, 6);
	map_bit(7, a, 5);
	map_bit(8, c, 6);
	map_bit(9, c, 5);
	map_bit(10, c, 4);
	map_bit(11, c, 3);
	map_bit(12, c, 2);
	map_bit(13, c, 1);
	map_bit(14, c, 0);
	map_bit(15, d, 7);

	DDRD = d;
	DDRC = c;
	DDRA = a;
}


void TesterConfig28(uint32_t mask) {
	// D6 D5 D4 D3 D2 A7 A6 A5 A4 A0 A1 A2 A3 --   D0 D1 B3 B4 B6 C7 C6 C5 C4 C3 C2 C1 C0 D7
	uint8_t d = DDRD;
	uint8_t a = DDRA;
	uint8_t c = DDRC;
	uint8_t b = DDRB;
	
	map_bit(0, d, 6);
	map_bit(1, d, 5);
	map_bit(2, d, 4);
	map_bit(3, d, 3);
	map_bit(4, d, 2);
	map_bit(5, a, 7);
	map_bit(6, a, 6);
	map_bit(7, a, 5);
	map_bit(8, a, 4);
	map_bit(9, a, 0);
	map_bit(10, a, 1);
	map_bit(11, a, 2);
	map_bit(12, a, 3);
	
	map_bit(14, d, 0);
	map_bit(15, d, 1);
	map_bit_(16, b, 3);
	map_bit_(17, b, 4);
	map_bit_(18, b, 6);
	map_bit_(19, c, 7);
	map_bit_(20, c, 6);
	map_bit_(21, c, 5);
	map_bit_(22, c, 4);
	map_bit_(23, c, 3);
	map_bit_(24, c, 2);
	map_bit_(25, c, 1);
	map_bit_(26, c, 0);
	map_bit_(27, d, 7);

	DDRD = d;
	DDRC = c;
	DDRA = a;	
	DDRB = b;
}

#define map_set(maskBit, dest, destBit)		if (mask0 & _BV(maskBit)) dest &= ~_BV(destBit); else if (mask1 & _BV(maskBit)) dest |= _BV(destBit);
#define map_set_(maskBit, dest, destBit)		if (mask0 & (1L << (maskBit))) dest &= ~_BV(destBit); else if (mask1 & (1L << (maskBit))) dest |= _BV(destBit);



void TesterSet16(uint16_t mask0, uint16_t mask1) {
	// D6 D5 D4 D3 D2 A7 A6 A5   C6 C5 C4 C3 C2 C1 C0 D7
	
	uint8_t d = PORTD;
	uint8_t c = PORTC;
	uint8_t a = PORTA;
	
	map_set(0, d, 6);
	map_set(1, d, 5);
	map_set(2, d, 4);
	map_set(3, d, 3);
	map_set(4, d, 2);
	map_set(5, a, 7);
	map_set(6, a, 6);
	map_set(7, a, 5);
	map_set(8, c, 6);	
	map_set(9, c, 5);
	map_set(10, c, 4);
	map_set(11, c, 3);
	map_set(12, c, 2);
	map_set(13, c, 1);
	map_set(14, c, 0);
	map_set(15, d, 7);
	
	PORTD = d;
	PORTC = c;
	PORTA = a;
}


void TesterSet28(uint32_t mask0, uint32_t mask1) {
	// D6 D5 D4 D3 D2 A7 A6 A5 A4 A0 A1 A2 A3 --   D0 D1 B3 B4 B6 C7 C6 C5 C4 C3 C2 C1 C0 D7
	uint8_t d = PORTD;
	uint8_t a = PORTA;
	uint8_t c = PORTC;
	uint8_t b = PORTB;
	
	map_set(0, d, 6);
	map_set(1, d, 5);
	map_set(2, d, 4);
	map_set(3, d, 3);
	map_set(4, d, 2);
	map_set(5, a, 7);
	map_set(6, a, 6);
	map_set(7, a, 5);
	map_set(8, a, 4);
	map_set(9, a, 0);
	map_set(10, a, 1);
	map_set(11, a, 2);
	map_set(12, a, 3);
	
	map_set(14, d, 0);
	map_set(15, d, 1);
	map_set_(16, b, 3);
	map_set_(17, b, 4);
	map_set_(18, b, 6);
	map_set_(19, c, 7);
	map_set_(20, c, 6);
	map_set_(21, c, 5);
	map_set_(22, c, 4);
	map_set_(23, c, 3);
	map_set_(24, c, 2);
	map_set_(25, c, 1);
	map_set_(26, c, 0);
	map_set_(27, d, 7);
	
	PORTD = d;
	PORTC = c;
	PORTA = a;
	PORTB = b;
}


#define map_test(maskBit, pin, pinBit)		if (mask0 & _BV(maskBit)) {	if (pin & _BV(pinBit)) result = false; } else if (mask1 & _BV(maskBit)) { if (!(pin & _BV(pinBit))) result = false;	}
#define map_test_(maskBit, pin, pinBit)		if (mask0 & (1L << (maskBit))) {	if (pin & _BV(pinBit)) result = false; } else if (mask1 & (1L << (maskBit))) { if (!(pin & _BV(pinBit))) result = false;	}	



bool TesterTest16(uint16_t mask0, uint16_t mask1) {
	// D6 D5 D4 D3 D2 A7 A6 A5   C6 C5 C4 C3 C2 C1 C0 D7
	bool result = true;
	
	map_test(0, PIND, 6);
	map_test(1, PIND, 5);
	map_test(2, PIND, 4);
	map_test(3, PIND, 3);
	map_test(4, PIND, 2);
	map_test(5, PINA, 7);
	map_test(6, PINA, 6);		
	map_test(7, PINA, 5);	
	map_test(8, PINC, 6);		
	map_test(9, PINC, 5);
	map_test(10, PINC, 4);
	map_test(11, PINC, 3);
	map_test(12, PINC, 2);
	map_test(13, PINC, 1);
	map_test(14, PINC, 0);
	map_test(15, PIND, 7);
	
	MSG_DEC("test ", (uint8_t)result);
	
	if (!result) {
		TesterDebugStatus(16);
	}
	
	return result;
}


bool TesterTest28(uint32_t mask0, uint32_t mask1) {
	bool result = true;
	
	map_test(0, PIND, 6);
	map_test(1, PIND, 5);
	map_test(2, PIND, 4);
	map_test(3, PIND, 3);
	map_test(4, PIND, 2);
	map_test(5, PINA, 7);
	map_test(6, PINA, 6);
	map_test(7, PINA, 5);
	map_test(8, PINA, 4);
	map_test(9, PINA, 0);
	map_test(10, PINA, 1);
	map_test(11, PINA, 2);
	map_test(12, PINA, 3);
	
	map_test(14, PIND, 0);
	map_test(15, PIND, 1);
	map_test_(16, PINB, 3);
	map_test_(17, PINB, 4);
	map_test_(18, PINB, 6);
	map_test_(19, PINC, 7);
	map_test_(20, PINC, 6);
	map_test_(21, PINC, 5);
	map_test_(22, PINC, 4);
	map_test_(23, PINC, 3);
	map_test_(24, PINC, 2);
	map_test_(25, PINC, 1);
	map_test_(26, PINC, 0);
	map_test_(27, PIND, 7);	
	
	MSG_DEC("test ", (uint8_t)result);
	
	if (!result) {
		TesterDebugStatus(16);
	}	
	
	return result;
}

bool IsFullMode() {
	return testerFullMode;
}

static void setPin(regs_t * regs, uint8_t pin) {
	
}


static bool getPin(regs_t *regs, uint8_t pin) {
	
}

void TesterSetPin(uint8_t pin, bool level) {
	//  1  2  3  4  5  6  7  8  9 10 11 12 13 14   15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// D6 D5 D4 D3 D2 A7 A6 A5 A4 A0 A1 A2 A3 --   D0 D1 B3 B4 B6 C7 C6 C5 C4 C3 C2 C1 C0 D7
	if (pin <= 5) {
		if (level) {
			PORTD |= _BV(7 - pin);
		} else {
			PORTD &= ~_BV(7 - pin);
		}
	} else if (pin <= 9) {
		if (level) {
			PORTA |= _BV(13 - pin);
		} else {
			PORTA &= ~_BV(13 - pin);
		}
	} else if (pin <= 13) {
		if (level) {
			PORTA |= _BV(pin - 10);
		} else {
			PORTA &= ~_BV(pin - 10);
		}
	} else if (pin == 14) {
		// этот пин на земле, ничего не делаем
	} else if (pin <= 16) {
		if (level) {
			PORTD |= _BV(pin - 16);
		} else {
			PORTD &= ~_BV(pin - 16);
		}
	} else if (pin <= 18) {
		if (level) {
			PORTB |= _BV(pin - 14);
		} else {
			PORTB &= ~_BV(pin - 14);
		}
	} else if (pin == 19) {
		if (level) {
			PORTB |= _BV(6);
		} else {
			PORTB &= ~_BV(6);
		}
	} else if (pin <= 27) {
		if (level) {
			PORTC |= _BV(27 - pin);
		} else {
			PORTC &= ~_BV(27 - pin);
		}		
	} else if (pin == 28) {
		if (level) {
			PORTD |= _BV(7);
		} else {
			PORTD &= ~_BV(7);
		}		
	}
}


void TesterDebugStatus(uint8_t pins) {
	// D6 D5 D4 D3 D2 A7 A6 A5    C6 C5 C4 C3 C2 C1 C0 D7
	
	uint8_t delta = 16 - pins;
	
	uart_putdw_dec(1); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(6) ? '1' : '0'); uart_putc(PORTD & _BV(6) ? '1' : '0'); uart_putc(PIND & _BV(6) ? '1' : '0'); 
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(16-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(7) ? '1' : '0'); uart_putc(PORTD & _BV(7) ? '1' : '0'); uart_putc(PIND & _BV(7) ? '1' : '0'); 
	uart_putc('\n');
	
	uart_putdw_dec(2); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(5) ? '1' : '0'); uart_putc(PORTD & _BV(5) ? '1' : '0'); uart_putc(PIND & _BV(5) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(15-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(0) ? '1' : '0'); uart_putc(PORTC & _BV(0) ? '1' : '0'); uart_putc(PINC & _BV(0) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(3); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(4) ? '1' : '0'); uart_putc(PORTD & _BV(4) ? '1' : '0'); uart_putc(PIND & _BV(4) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(14-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(1) ? '1' : '0'); uart_putc(PORTC & _BV(1) ? '1' : '0'); uart_putc(PINC & _BV(1) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(4); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(3) ? '1' : '0'); uart_putc(PORTD & _BV(3) ? '1' : '0'); uart_putc(PIND & _BV(3) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(13-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(2) ? '1' : '0'); uart_putc(PORTC & _BV(2) ? '1' : '0'); uart_putc(PINC & _BV(2) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(5); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(2) ? '1' : '0'); uart_putc(PORTD & _BV(2) ? '1' : '0'); uart_putc(PIND & _BV(2) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(12-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(3) ? '1' : '0'); uart_putc(PORTC & _BV(3) ? '1' : '0'); uart_putc(PINC & _BV(3) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(6); uart_putc(':'); uart_putc(' '); uart_putc(DDRA & _BV(7) ? '1' : '0'); uart_putc(PORTA & _BV(7) ? '1' : '0'); uart_putc(PINA & _BV(7) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(11-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(4) ? '1' : '0'); uart_putc(PORTC & _BV(4) ? '1' : '0'); uart_putc(PINC & _BV(4) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(7); uart_putc(':'); uart_putc(' '); uart_putc(DDRA & _BV(6) ? '1' : '0'); uart_putc(PORTA & _BV(6) ? '1' : '0'); uart_putc(PINA & _BV(6) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(10-delta); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(5) ? '1' : '0'); uart_putc(PORTC & _BV(5) ? '1' : '0'); uart_putc(PINC & _BV(5) ? '1' : '0');
	uart_putc('\n');
	
	if (pins == 16) {
		uart_putdw_dec(8); uart_putc(':'); uart_putc(' '); uart_putc(DDRA & _BV(5) ? '1' : '0'); uart_putc(PORTA & _BV(5) ? '1' : '0'); uart_putc(PINA & _BV(5) ? '1' : '0');
		uart_putc(' '); uart_putc(' '); uart_putc(' '); uart_putc(' ');
		uart_putc(' '); uart_putdw_dec(9); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(6) ? '1' : '0'); uart_putc(PORTC & _BV(6) ? '1' : '0'); uart_putc(PINC & _BV(6) ? '1' : '0');
		uart_putc('\n');	
	}
}
