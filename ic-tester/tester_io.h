/*
 * tester.h
 *
 * Created: 15.07.2015 10:53:47
 *  Author: Trol
 */ 


#ifndef TESTER_IO_H_
#define TESTER_IO_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "debug.h"

typedef union {
	struct {
		unsigned p1 : 1;
		unsigned p2 : 1;
		unsigned p3 : 1;
		unsigned p4 : 1;
		unsigned p5 : 1;
		unsigned p6 : 1;
		unsigned p7 : 1;
		unsigned p8 : 1;
		unsigned p9 : 1;
		unsigned p10 : 1;
		unsigned p11 : 1;
		unsigned p12 : 1;
		unsigned p13 : 1;
		unsigned p14 : 1;
		unsigned p15 : 1;
		unsigned p16 : 1;
	};
	struct {
		uint8_t b1, b0;	
	};
} val16_t;


typedef union {
	struct {
		unsigned p1 : 1;
		unsigned p2 : 1;
		unsigned p3 : 1;
		unsigned p4 : 1;
		unsigned p5 : 1;
		unsigned p6 : 1;
		unsigned p7 : 1;
		unsigned p8 : 1;
		unsigned p9 : 1;
		unsigned p10 : 1;
		unsigned p11 : 1;
		unsigned p12 : 1;
		unsigned p13 : 1;
		unsigned p14 : 1;
		unsigned p15 : 1;
		unsigned p16 : 1;
		unsigned p17 : 1;
		unsigned p18 : 1;
		unsigned p19 : 1;
		unsigned p20 : 1;
		unsigned p21 : 1;
		unsigned p22 : 1;
		unsigned p23 : 1;
		unsigned p24 : 1;
	};
	struct {
		uint8_t b2, b1, b0;	
	};
} val24_t;


typedef union {
	struct {
		unsigned p1 : 1;
		unsigned p2 : 1;
		unsigned p3 : 1;
		unsigned p4 : 1;
		unsigned p5 : 1;
		unsigned p6 : 1;
		unsigned p7 : 1;
		unsigned p8 : 1;
		unsigned p9 : 1;
		unsigned p10 : 1;
		unsigned p11 : 1;
		unsigned p12 : 1;
		unsigned p13 : 1;
		unsigned p14 : 1;
		unsigned p15 : 1;
		unsigned p16 : 1;
		unsigned p17 : 1;
		unsigned p18 : 1;
		unsigned p19 : 1;
		unsigned p20 : 1;
		unsigned p21 : 1;
		unsigned p22 : 1;
		unsigned p23 : 1;
		unsigned p24 : 1;
		unsigned p25 : 1;
		unsigned p26 : 1;
		unsigned p27 : 1;
		unsigned p28 : 1;
	};
	struct {
		uint8_t b3, b2, b1, b0;	
	};
} val28_t;


/************************************************************************
  Устанавливает все пины на чтение и отключает подтяжку                
  Если fullMode установлен, то будут инициализированы нижние пины
  (клавиши и UART)                                                      
************************************************************************/
void TesterReset(bool fullMode);

/************************************************************************
  Конфигурирует верхние пины на чтение/запись согласно битовой маске   
  Младший бит соответствует первому пину, старший - последнему
  Пин конфигурируется на запись если бит установлен                    
************************************************************************/
void TesterConfig16(val16_t *mask);
void TesterConfig24(val24_t *mask);
void TesterConfig28(val28_t *mask);


/************************************************************************
  Устанавливает пины по маске mask1 и сбрасывает по маске mask0
************************************************************************/
void TesterSet16(val16_t *mask0, val16_t *mask1);
void TesterSet24(val24_t *mask0, val24_t *mask1);
void TesterSet28(val28_t *mask0, val28_t *mask1);

void TesterSetAll16(val16_t *mask);
void TesterSetAll24(val24_t *mask);
void TesterSetAll28(val28_t *mask);
/************************************************************************
  Проверяет, что биты маски mask1 установлены, а биты маски mask0 сброшены
************************************************************************/
bool TesterTest16(val16_t *mask0, val16_t *mask1);
bool TesterTest24(val24_t *mask0, val24_t *mask1);
bool TesterTest28(val28_t *mask0, val28_t *mask1);


bool TesterTestAll16(val16_t *mask);
bool TesterTestAll24(val24_t *mask);
bool TesterTestAll28(val28_t *mask);

/************************************************************************
  Возвращает true, если все пины сейчас задействованы для теста микросхемы
  В этом случае дополнительные клавиши и UART недоступны
************************************************************************/
bool IsFullMode();


/************************************************************************/
/* Выводит в UART состояние пинов МС                                    */
/************************************************************************/
void TesterDebugStatus(uint8_t pins);


bool getPinVal16(val16_t *val, uint8_t pin);
bool getPinVal24(val24_t *val, uint8_t pin);
bool getPinVal28(val28_t *val, uint8_t pin);


void TesterSetPin(uint8_t pin, bool level);

void invertPinDirection(uint8_t pin, uint8_t package);

void invertPinOut(uint8_t pin, uint8_t package);

void ReadAll24(val24_t *ddr, val24_t *port, val24_t *pin);

#endif // TESTER_IO_H_