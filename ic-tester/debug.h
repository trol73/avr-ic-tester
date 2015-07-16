/*
 * Copyright (c) 2009 by Oleg Trifonov <otrifonow@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <avr/pgmspace.h>

#if DEBUG

#include <stdint.h>






#define UART_RATE_VALUE  5			// значение скорости порта
/******************
f = 11.059 MHz
2400   UBRR=287
4800   UBRR=143
9600   UBRR=71
14400  UBRR=47
19200  UBRR=35
28800  UBRR=23
38400  UBRR=17
57600  UBRR=11
76800  UBRR=8
115200 UBRR=5 
*****************/


void uart_init();

void uart_putc(uint8_t c);

void uart_putc_hex(uint8_t b);
void uart_putw_hex(uint16_t w);
void uart_putdw_hex(uint32_t dw);

void uart_putw_dec(uint16_t w);
void uart_putdw_dec(uint32_t dw);

void uart_puts(const char* str);
void uart_puts_p(PGM_P str);

uint8_t uart_getc();

#else
	inline void uart_init() {
	}
#endif // DEBUG


inline void MSG_(const char *str) {
#if DEBUG
	uart_puts_p(str);
	uart_putc('\n');
#endif
}

inline void MSG_HEX_(const char *str, uint32_t val, uint8_t bytes) {
#if DEBUG
	uart_puts_p(str);
	if ( bytes == 1 )
		uart_putc_hex(val);
	else if ( bytes == 2 )
		uart_putw_hex(val);
	else if ( bytes == 4 )
		uart_putdw_hex(val);
	uart_putc('\n');
#endif
}


inline void MSG_DEC_(const char *str, uint32_t val) {
#if DEBUG
	uart_puts_p(str);
	uart_putdw_dec(val);
	uart_putc('\n');
#endif
}


#define MSG(str)								MSG_(PSTR(str))
#define MSG_HEX(str, val, bytes)			MSG_HEX_(PSTR(str), (val), bytes)
#define MSG_DEC(str, val)					MSG_DEC_(PSTR(str), (val))


#endif // _DEBUG_H_
