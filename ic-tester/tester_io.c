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

}

#define map_set(maskBit, dest, destBit)		if (mask0 & _BV(maskBit)) dest &= ~_BV(destBit); else if (mask1 & _BV(maskBit)) dest |= _BV(destBit);



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

}


#define map_test(maskBit, pin, pinBit)		if (mask0 & _BV(maskBit)) {	if (pin & _BV(pinBit)) result = false; } else if (mask1 & _BV(maskBit)) { if (!(pin & _BV(pinBit))) result = false;	}



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
	
	TesterDebugStatus();
	
	return result;
}

bool TesterTest20(uint32_t mask0, uint32_t mask1) {
	bool result = true;
	
	return result;
}

bool TesterTest28(uint32_t mask0, uint32_t mask1) {
	bool result = true;
	
	return result;
}

bool IsFullMode() {
	return testerFullMode;
}


void TesterDebugStatus() {
	// D6 D5 D4 D3 D2 A7 A6 A5    C6 C5 C4 C3 C2 C1 C0 D7
	
	uart_putdw_dec(1); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(6) ? '1' : '0'); uart_putc(PORTD & _BV(6) ? '1' : '0'); uart_putc(PIND & _BV(6) ? '1' : '0'); 
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(14); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(7) ? '1' : '0'); uart_putc(PORTD & _BV(7) ? '1' : '0'); uart_putc(PIND & _BV(7) ? '1' : '0'); 
	uart_putc('\n');
	
	uart_putdw_dec(2); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(5) ? '1' : '0'); uart_putc(PORTD & _BV(5) ? '1' : '0'); uart_putc(PIND & _BV(5) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(13); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(0) ? '1' : '0'); uart_putc(PORTC & _BV(0) ? '1' : '0'); uart_putc(PINC & _BV(0) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(3); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(4) ? '1' : '0'); uart_putc(PORTD & _BV(4) ? '1' : '0'); uart_putc(PIND & _BV(4) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(12); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(1) ? '1' : '0'); uart_putc(PORTC & _BV(1) ? '1' : '0'); uart_putc(PINC & _BV(1) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(4); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(3) ? '1' : '0'); uart_putc(PORTD & _BV(3) ? '1' : '0'); uart_putc(PIND & _BV(3) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(11); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(2) ? '1' : '0'); uart_putc(PORTC & _BV(2) ? '1' : '0'); uart_putc(PINC & _BV(2) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(5); uart_putc(':'); uart_putc(' '); uart_putc(DDRD & _BV(2) ? '1' : '0'); uart_putc(PORTD & _BV(2) ? '1' : '0'); uart_putc(PIND & _BV(2) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(10); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(3) ? '1' : '0'); uart_putc(PORTC & _BV(3) ? '1' : '0'); uart_putc(PINC & _BV(3) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(6); uart_putc(':'); uart_putc(' '); uart_putc(DDRA & _BV(7) ? '1' : '0'); uart_putc(PORTA & _BV(7) ? '1' : '0'); uart_putc(PINA & _BV(7) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(9); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(4) ? '1' : '0'); uart_putc(PORTC & _BV(4) ? '1' : '0'); uart_putc(PINC & _BV(4) ? '1' : '0');
	uart_putc('\n');
	
	uart_putdw_dec(7); uart_putc(':'); uart_putc(' '); uart_putc(DDRA & _BV(6) ? '1' : '0'); uart_putc(PORTA & _BV(6) ? '1' : '0'); uart_putc(PINA & _BV(6) ? '1' : '0');
	uart_putc(' '); uart_putc(' '); uart_putc(' '); uart_putc(' ');
	uart_putdw_dec(8); uart_putc(':'); uart_putc(' '); uart_putc(DDRC & _BV(5) ? '1' : '0'); uart_putc(PORTC & _BV(5) ? '1' : '0'); uart_putc(PINC & _BV(5) ? '1' : '0');
	uart_putc('\n');
}