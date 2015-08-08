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
              28                                 24                                20                                16                               14
        _______  _______                  _______  _______                  _______  _______                  _______  _______                  _______  _______
  D6  --| 1    \/   28 |-- D7       D6  --| 1    \/   24 |-- D7       D6  --| 1    \/   20 |-- D7       D6  --| 1    \/   16 |-- D7       D6  --| 1    \/   14 |-- D7
  D5  --| 2         27 |-- C0       D5  --| 2         23 |-- C0       D5  --| 2         19 |-- C0       D5  --| 2         15 |-- C0       D5  --| 2         13 |-- C0
  D4  --| 3         26 |-- C1       D4  --| 3         22 |-- C1       D4  --| 3         18 |-- C1       D4  --| 3         14 |-- C1       D4  --| 3         12 |-- C1
  D3  --| 4         25 |-- C2       D3  --| 4         21 |-- C2       D3  --| 4         17 |-- C2       D3  --| 4         13 |-- C2       D3  --| 4         11 |-- C2
  D2  --| 5         24 |-- C3       D2  --| 5         20 |-- C3       D2  --| 5         16 |-- C3       D2  --| 5         12 |-- C3       D2  --| 5         10 |-- C3
  A7  --| 6         23 |-- C4       A7  --| 6         19 |-- C4       A7  --| 6         15 |-- C4       A7  --| 6         11 |-- C4       A7  --| 6          9 |-- C4
  A6  --| 7         22 |-- C5       A6  --| 7         18 |-- C5       A6  --| 7         14 |-- C5       A6  --| 7         10 |-- C5       A6  --| 7          8 |-- C5
  A5  --| 8         21 |-- C6       A5  --| 8         17 |-- C6       A5  --| 8         13 |-- C6       A5  --| 8          9 |-- C6             ----------------
  A4  --| 9         20 |-- C7       A4  --| 9         16 |-- C7       A4  --| 9         12 |-- C7             ----------------
  A0  --| 10        19 |-- B6       A0  --| 10        15 |-- B6       A0  --| 10        11 |-- B6
  A1  --| 11        18 |-- B4       A0  --| 11        14 |-- B4             ----------------
  A2  --| 12*       17 |-- B3       A0  --| 12        13 |-- B3
  A3  --| 13*      *16 |-- D1             ----------------
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


//  оды операций дл€ работы с регистрами. »спользуютс€ методами valXXtoRegs()
#define OPERATION_COPY		0
#define OPERATION_SET		1
#define OPERATION_CLEAR		2

static void setPin28(regs_t *regs, uint8_t pin, bool val);
static bool getPin28(regs_t *regs, uint8_t pin);

static void val16toRegs(val16_t *val, regs_t *regs, uint8_t operation);
static void val24toRegs(val24_t *val, regs_t *regs, uint8_t operation);
static void val28toRegs(val28_t *val, regs_t *regs, uint8_t operation);

static void regsToVal16(regs_t *regs, val16_t *val);
static void regsToVal24(regs_t *regs, val24_t *val);
static void regsToVal28(regs_t *regs, val28_t *val);


#define read_registers_16(reg)		regs.a = reg##A; regs.c = reg##C; regs.d = reg##D;
#define read_registers(reg)			regs.a = reg##A; regs.b = reg##B; regs.c = reg##C; regs.d = reg##D;
#define write_registers_16(reg)		reg##D = regs.d; reg##C = regs.c; reg##A = regs.a;
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




bool TesterTest16(val16_t *mask0, val16_t *mask1) {
	regs_t regs;
	val16_t val;
	
	read_registers_16(PIN);
	regsToVal16(&regs, &val);
	
	for (uint8_t pin = 1; pin <= 16; pin++) {
		bool pinValue = getPinVal16(&val, pin);
		if ((pinValue && getPinVal16(mask0, pin)) || (!pinValue && getPinVal16(mask1, pin))) {
			MSG_DEC("fail ", pin);
			TesterDebugStatus(16);
			return false;
		}
	}
	return true;
}

bool TesterTest24(val24_t *mask0, val24_t *mask1) {
	regs_t regs;
	val24_t val;
	
	read_registers(PIN);
	regsToVal24(&regs, &val);
	
	for (uint8_t pin = 1; pin <= 24; pin++) {
		bool pinValue = getPinVal24(&val, pin);
		if ((pinValue && getPinVal24(mask0, pin)) || (!pinValue && getPinVal24(mask1, pin))) {
			MSG_DEC("fail on pin ", pin);
			if (getPinVal24(mask0, pin)) {
				MSG("expected: 0");
			}
			if (getPinVal24(mask1, pin)) {
				MSG("expected: 1");
			}
			MSG_DEC("found: ", pinValue);
			TesterDebugStatus(24);
			return false;
		}
	}
	return true;
}

bool TesterTest28(val28_t *mask0, val28_t *mask1) {
	regs_t regs;
	val28_t val;
	
	read_registers(PIN);
	regsToVal28(&regs, &val);
	
	for (uint8_t pin = 1; pin <= 28; pin++) {
		bool pinValue = getPinVal28(&val, pin);
		if ((pinValue && getPinVal28(mask0, pin)) || (!pinValue && getPinVal28(mask1, pin))) {
			MSG_DEC("fail ", pin);
			TesterDebugStatus(28);
			return false;
		}
	}
	return true;
}


bool IsFullMode() {
	return testerFullMode;
}


#define set_pin(reg_name, num)	if (val) regs->reg_name |= _BV(num); else regs->reg_name &= ~ _BV(num);

/************************************************************************/
/* ”станавливает состо€ние пина в структуре регистров                   */
/************************************************************************/
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
		set_pin(d, pin - 15);
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

/************************************************************************/
/* ¬озвращает значение пина по регистрам                                */
/************************************************************************/
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
	for (uint8_t i = 1; i <= 16; i++) {
		uint8_t outPin = i <= 8 ? i : i + 12;
		if (operation == OPERATION_COPY) {
			setPin28(regs, outPin, getPinVal16(val, i));
		} else if (getPinVal16(val, i)) {
			setPin28(regs, outPin, operation == OPERATION_SET);
		}
	}
}


static void val24toRegs(val24_t *val, regs_t *regs, uint8_t operation) {
	for (uint8_t i = 1; i <= 24; i++) {
		uint8_t outPin = i <= 12 ? i : i + 4;
		if (operation == OPERATION_COPY) {
			setPin28(regs, outPin, getPinVal24(val, i));
		} else if (getPinVal24(val, i)) {
			setPin28(regs, outPin, operation == OPERATION_SET);
		}
	}
}


static void val28toRegs(val28_t *val, regs_t *regs, uint8_t operation) {
	for (uint8_t i = 1; i <= 28; i++) {
		if (operation == OPERATION_COPY) {
			setPin28(regs, i, getPinVal28(val, i));
		} else if (getPinVal28(val, i)) {
			setPin28(regs, i, operation == OPERATION_SET);
		}		
	}
}


static void setVal16Pin(val16_t *val, uint8_t pin, bool status) {
	if (pin <= 8) {
		if (status) {
			val->b0 |= _BV(pin-1);
		} else {
			val->b0 &= ~_BV(pin-1);
		}
	} else {
		if (status) {
			val->b1 |= _BV(pin-9);
		} else {
			val->b1 &= ~_BV(pin-9);
		}		
	}	
}

static void setVal24Pin(val24_t *val, uint8_t pin, bool status) {
	if (pin <= 8) {
		if (status) {
			val->b0 |= _BV(pin-1);
		} else {
			val->b0 &= ~_BV(pin-1);
		}
	} else if (pin <= 16) {
		if (status) {
			val->b1 |= _BV(pin-9);
		} else {
			val->b1 &= ~_BV(pin-9);
		}
	} else {
		if (status) {
			val->b2 |= _BV(pin-17);
		} else {
			val->b2 &= ~_BV(pin-17);
		}		
	}
}

static void setVal28Pin(val28_t *val, uint8_t pin, bool status) {
	if (pin <= 8) {
		if (status) {
			val->b0 |= _BV(pin-1);
		} else {
			val->b0 &= ~_BV(pin-1);
		}
	} else if (pin <= 16) {
		if (status) {
			val->b1 |= _BV(pin-9);
		} else {
			val->b1 &= ~_BV(pin-9);
		}
	} else if (pin <= 24) {
		if (status) {
			val->b2 |= _BV(pin-17);
		} else {
			val->b2 &= ~_BV(pin-17);
		}
	} else {
		if (status) {
			val->b3 |= _BV(pin-25);
		} else {
			val->b3 &= ~_BV(pin-25);
		}		
	}
}


static void regsToVal16(regs_t *regs, val16_t *val) {
	for (uint8_t i = 1; i <= 16; i++) {
		uint8_t pin28 = i <= 8 ? i : i + 12;
		bool v = getPin28(regs, pin28);
		setVal16Pin(val, i, v);
	}
}

static void regsToVal24(regs_t *regs, val24_t *val) {
	for (uint8_t i = 1; i <= 24; i++) {
		uint8_t pin28 = i <= 12 ? i : i + 4;
		bool v = getPin28(regs, pin28);
		setVal24Pin(val, i, v);
	}
}

static void regsToVal28(regs_t *regs, val28_t *val) {
	for (uint8_t i = 1; i <= 28; i++) {
		bool v = getPin28(regs, i);
		setVal28Pin(val, i, v);
	}
}

void TesterSetPin(uint8_t pin, bool level) {
	regs_t regs;
	val28_t val;

	read_registers(PORT);
	val.b0 = 0;
	val.b1 = 0;
	val.b2 = 0;
	val.b3 = 0;
	setVal28Pin(&val, pin, 1);
	val28toRegs(&val, &regs, level ? OPERATION_SET : OPERATION_CLEAR);
	write_registers(PORT);
}


void TesterDebugStatus(uint8_t pins) {
	regs_t regs;
	regs_t regsDdr, regsPort, regsPin;
	read_registers(DDR);
	regsDdr = regs;
	read_registers(PORT);
	regsPort = regs;
	read_registers(PIN);
	regsPin = regs;
	
	val28_t valDdr, valPort, valPin;
	regsToVal28(&regsDdr, &valDdr);
	regsToVal28(&regsPort, &valPort);
	regsToVal28(&regsPin, &valPin);
	
	MSG_DEC("pins= ", pins);
	for (uint8_t i = 1; i <= pins/2; i++) {
		uint8_t pin = i;
		uart_putdw_dec(pin); uart_putc(':'); uart_putc(' '); uart_putc(getPinVal28(&valDdr, pin) ? '1' : '0'); uart_putc(getPinVal28(&valPort, pin) ? '1' : '0'); uart_putc(getPinVal28(&valPin, pin) ? '1' : '0'); 
		uart_putc(' '); uart_putc(' '); uart_putc(' ');
		pin = pins - i + 1;
		uint8_t delta = 28 - pins;
		uart_putdw_dec(pin); uart_putc(':'); uart_putc(' '); uart_putc(getPinVal28(&valDdr, pin+delta) ? '1' : '0'); uart_putc(getPinVal28(&valPort, pin+delta) ? '1' : '0'); uart_putc(getPinVal28(&valPin, pin+delta) ? '1' : '0'); 
		uart_putc('\n');
	}
}

void invertPinDirection(uint8_t pin, uint8_t package) {
	regs_t regs;
	val24_t val;

	uint8_t pin24 = pin <= package/2 ? pin : pin + 24 - package;
	
	read_registers(DDR);
	regsToVal24(&regs, &val);
	bool v = getPinVal24(&val, pin24);
	setVal24Pin(&val, pin24, !v);
	val24toRegs(&val, &regs, OPERATION_COPY);
	write_registers(DDR);
}

void invertPinOut(uint8_t pin, uint8_t package) {
	regs_t regs;
	val24_t val;

	uint8_t pin24 = pin <= package/2 ? pin : pin + 24 - package;
	
	read_registers(PORT);
	regsToVal24(&regs, &val);
	bool v = getPinVal24(&val, pin24);
	setVal24Pin(&val, pin24, !v);
	val24toRegs(&val, &regs, OPERATION_COPY);
	write_registers(PORT);	
}


void ReadAll24(val24_t *ddr, val24_t *port, val24_t *pin) {
	regs_t regs;
	
	read_registers(DDR);
	regsToVal24(&regs, ddr);
	read_registers(PORT);
	regsToVal24(&regs, port);
	read_registers(PIN);
	regsToVal24(&regs, pin);	
}