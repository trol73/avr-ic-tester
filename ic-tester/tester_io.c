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


// Коды операций для работы с регистрами. Используются методами valXXtoRegs()
#define OPERATION_COPY		0
#define OPERATION_SET		1
#define OPERATION_CLEAR		2

static void setPin28(regs_t *regs, uint8_t pin, bool val);
static bool getPin28(regs_t *regs, uint8_t pin);
static void val16toRegs(val16_t *val, regs_t *regs, uint8_t operation);
static void val24toRegs(val24_t *val, regs_t *regs, uint8_t operation);
static void val28toRegs(val28_t *val, regs_t *regs, uint8_t operation);

#define read_registers_16(reg)	regs.a = reg##A; regs.c == reg##C; regs.d = reg##D;
#define read_registers(reg)		regs.a = reg##A; regs.b = reg##B; regs.c == reg##C; regs.d = reg##D;
#define write_registers_16(reg)	reg##D = regs.d; reg##C = regs.c; reg##A = regs.a;
#define write_registers(reg)		reg##D = regs.d; reg##C = regs.c; reg##A = regs.a; reg##B = regs.b;

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


void TesterConfig16(val16_t *mask) {
	regs_t regs;

	read_registers_16(DDR);
	val16toRegs(mask, &regs, OPERATION_COPY);
	write_registers_16(DDR);
}

void TesterConfig24(val24_t *mask) {
	regs_t regs;
	
	read_registers(DDR);
	val24toRegs(mask, &regs, OPERATION_COPY);
	write_registers(DDR);
}


void TesterConfig28(val28_t *mask) {
	regs_t regs;
	
	read_registers(DDR);
	val28toRegs(mask, &regs, OPERATION_COPY);
	write_registers(DDR);
}


void TesterSet16(val16_t *mask0, val16_t *mask1) {
	regs_t regs;

	read_registers_16(PORT);
	val16toRegs(mask0, &regs, OPERATION_CLEAR);
	val16toRegs(mask1, &regs, OPERATION_SET);
	write_registers_16(PORT);
}

void TesterSet24(val24_t *mask0, val24_t *mask1) {
	regs_t regs;

	read_registers(PORT);
	val24toRegs(mask0, &regs, OPERATION_CLEAR);
	val24toRegs(mask1, &regs, OPERATION_SET);
	write_registers(PORT);
}

void TesterSet28(val28_t *mask0, val28_t *mask1) {
	regs_t regs;

	read_registers(PORT);
	val28toRegs(mask0, &regs, OPERATION_CLEAR);
	val28toRegs(mask1, &regs, OPERATION_SET);
	write_registers(PORT);
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


#define set_pin(reg_name, num)	if (val) regs->reg_name |= _BV(num); else regs->reg_name &= ~ _BV(num);

static void setPin28(regs_t *regs, uint8_t pin, bool val) {
	//  1  2  3  4  5  6  7  8  9 10 11 12 13 14   15 16 17 18 19 20 21 22 23 24 25 26 27 28	
	// D6 D5 D4 D3 D2 A7 A6 A5 A4 A0 A1 A2 A3 --   D0 D1 B3 B4 B6 C7 C6 C5 C4 C3 C2 C1 C0 D7

	if (pin <= 5) {
		set_pin(d, 7 - pin);
	} else if (pin <= 9) {
		set_pin(a, 13 - pin);
	} else if (pin <= 13) {
		set_pin(a, pin - 10);
	} else if (pin == 14) {
		// этот пин на земле, ничего не делаем
	} else if (pin <= 16) {
		set_pin(d, pin - 16);
	} else if (pin <= 18) {
		set_pin(b, pin - 14);
	} else if (pin <= 18) {
		set_pin(b, pin - 14);
	} else if (pin == 19) {
		set_pin(b, 6);
	} else if (pin <= 27) {
		set_pin(c, 27 - pin)
	} else if (pin == 28) {
		set_pin(d, 7);
	}
}

#define get_pin(reg_name, num)	(regs->reg_name & _BV(num))


static bool getPin28(regs_t *regs, uint8_t pin) {
	//  1  2  3  4  5  6  7  8  9 10 11 12 13 14   15 16 17 18 19 20 21 22 23 24 25 26 27 28	
	// D6 D5 D4 D3 D2 A7 A6 A5 A4 A0 A1 A2 A3 --   D0 D1 B3 B4 B6 C7 C6 C5 C4 C3 C2 C1 C0 D7
	if (pin <= 5) {
		return get_pin(d, 7 - pin);
	} else if (pin <= 9) {
		return get_pin(a, 13 - pin);
	} else if (pin <= 13) {
		return get_pin(a, pin - 10);
	} else if (pin == 14) {
		// этот пин на земле
		return 0;
	} else if (pin <= 16) {
		return get_pin(d, pin - 16);
	} else if (pin <= 18) {
		return get_pin(b, pin - 14);
	} else if (pin <= 18) {
		return get_pin(b, pin - 14);
	} else if (pin == 19) {
		return get_pin(b, 6);
	} else if (pin <= 27) {
		return get_pin(c, 27 - pin);
	} else if (pin == 28) {
		return get_pin(d, 7);
	}
	return 0;
}

bool getPinVal16(val16_t *val, uint8_t pin) {
	if (pin <= 8) {
		return (val->b0 & _BV(pin-1));
	} else {
		return (val->b1 & _BV(pin-9));
	}
}


bool getPinVal24(val24_t *val, uint8_t pin) {
	if (pin <= 8) {
		return (val->b0 & _BV(pin-1));
	} else if (pin <= 16) {
		return (val->b1 & _BV(pin-9));
	} else {
		return (val->b2 & _BV(pin-17));
	}
}


bool getPinVal28(val28_t *val, uint8_t pin) {
	if (pin <= 8) {
		return val->b0 & _BV(pin-1);
	} else if (pin <= 16) {
		return val->b1 & _BV(pin-9);
	} else if (pin <= 24) {
		return val->b2 & _BV(pin-17);
	} else {
		return val->b3 & _BV(pin-25);
	}
}


static void val16toRegs(val16_t *val, regs_t *regs, uint8_t operation) {
	for (uint8_t i = 1; i <= 16) {
		uint8_t outPin = i <= 8 ? i : i + 12;
		if (operation == OPERATION_COPY) {
			setPin(regs, outPin, getPinVal16(val, i));
		} else if (getPinVal16(val, i)) {
			setPin(regs, outPin, operation == OPERATION_SET);
		}
	}
}


static void val24toRegs(val24_t *val, regs_t *regs, uint8_t operation) {
	for (uint8_t i = 1; i <= 24) {
		uint8_t outPin = pin <= 12 ? i : i + 8;
		if (operation == OPERATION_COPY) {
			setPin(regs, outPin, getPinVal24(val, i));
		} else if (getPinVal24(val, i)) {
			setPin(regs, outPin, operation == OPERATION_SET);
		}
	}
}


static void val28toRegs(val28_t *val, regs_t *regs, uint8_t operation) {
	for (uint8_t i = 1; i <= 28) {
		if (operation == OPERATION_COPY) {
			setPin(regs, i, getPinVal28(val, i));
		} else if (getPinVal28(val, i)) {
			setPin(regs, outPin, operation == OPERATION_SET);
		}		
	}
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
