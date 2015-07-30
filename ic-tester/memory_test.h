/*
 * memory_test.h
 *
 * Created: 27.07.2015 22:03:34
 *  Author: Trol
 */ 


#ifndef MEMORY_TEST_H_
#define MEMORY_TEST_H_

/************************************************************************/
/* Результаты проверки ячеек                                            */
/************************************************************************/
#define TEST_CELL_GOOD		0
#define TEST_CELL_BAD		1
#define TEST_CELL_UNKNOWN	2


#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


void MemInit();
// возвращает прочитанный бит или 0xff если не удалось прочитать
uint8_t MemReadBit(uint16_t row, uint16_t col);
void MemWriteBit(uint16_t row, uint16_t col, bool val);
void MemRegenerate();

void MemTest();
void MemDebug();

/************************************************************************/
/* Возвращает TEST_CELL_xxx для проверенной ячейки                      */
/************************************************************************/
uint8_t MemTestGetCell(uint8_t row, uint8_t col);

/************************************************************************/
/* Возвращает 8 для РУ5 и 9 для РУ7                                     */
/************************************************************************/
uint8_t MemGetRows();


#endif // MEMORY_TEST_H_