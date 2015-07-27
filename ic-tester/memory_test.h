/*
 * memory_test.h
 *
 * Created: 27.07.2015 22:03:34
 *  Author: Trol
 */ 


#ifndef MEMORY_TEST_H_
#define MEMORY_TEST_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


void MemInit();
bool MemReadBit(uint16_t row, uint16_t col);
void MemWriteBit(uint16_t row, uint16_t col, bool val);
void MemRegenerate();

void MemTest();
void MemDebug();

#endif // MEMORY_TEST_H_