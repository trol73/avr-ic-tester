/*
 * test.h
 *
 * Created: 15.07.2015 13:17:42
 *  Author: Trol
 */ 


#ifndef TEST_H_
#define TEST_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define CMD_RESET							0	// переводит все пины в режим чтения (не трогает нижние)
#define CMD_RESET_FULL					1	// переводит все пины в режим чтения (включая нижние)
#define CMD_INIT_16						2	// (mask[2]) настраивает порты ввода-вывода
#define CMD_INIT_24						3	// (mask[3]) настраивает порты ввода-вывода
#define CMD_INIT_28						4	// (mask[4]) настраивает порты ввода-вывода
#define CMD_SET_16						5	// (mask0[2], mask1[2]) выставляет логические 0 и 1 на выводах по маскам
#define CMD_SET_24						6	// (mask0[3], mask1[2]) выставляет логические 0 и 1 на выводах по маскам
#define CMD_SET_28						7	// (mask0[4], mask1[3]) выставляет логические 0 и 1 на выводах по маскам
#define CMD_TEST_16						8	// (mask0[2], mask1[2]) проверяет, что на выводах установлен ожидаемый уровень
#define CMD_TEST_24						9	// (mask0[3], mask1[2]) проверяет, что на выводах установлен ожидаемый уровень
#define CMD_TEST_28						10	// (mask0[4], mask1[3]) проверяет, что на выводах установлен ожидаемый уровень
#define CMD_PULSE_PLUS					11	// (pin[1]) подает положительный импульс (0 -> 1) на вывод
#define CMD_PULSE_MINUS					12	// (pin[1]) подает отрицательный импульс (1 -> 0) на вывод
#define CMD_SET_ALL_16					13	// (mask[2] устанавливает все пины по маске)
#define CMD_SET_ALL_24					14	// (mask[3] устанавливает все пины по маске)
#define CMD_SET_ALL_28					15	// (mask[4] устанавливает все пины по маске)
#define CMD_TEST_ALL_16					16	// (mask[2] проверяет уровни на всех пинах МС по маске)
#define CMD_TEST_ALL_24					17	// (mask[3] проверяет уровни на всех пинах МС по маске)
#define CMD_TEST_ALL_28					18	// (mask[4] проверяет уровни на всех пинах МС по маске)
#define CMD_SET_ALL_AND_TEST_16		19	// (mask_set[2], mask_test[2]) устанавливает и проверяет все пины
#define CMD_SET_ALL_AND_TEST_24		20	// (mask_set[3], mask_test[3]) устанавливает и проверяет все пины
#define CMD_SET_ALL_AND_TEST_28		21	// (mask_set[4], mask_test[4]) устанавливает и проверяет все пины
#define CMD_LAST_PULSE					22	// повторяет последний импульс
#define CMD_LAST_PULSE_AND_TEST_16	23	// (mask[2]) повторяет последний импульс и проверяет состояние всех пинов
#define CMD_LAST_PULSE_AND_TEST_24	24	// (mask[3]) повторяет последний импульс и проверяет состояние всех пинов
#define CMD_LAST_PULSE_AND_TEST_28	25	// (mask[4]) повторяет последний импульс и проверяет состояние всех пинов
#define CMD_TEST_Z_16					26	// (mask[2]) проверяет что пины находятся в Z-состоянии
#define CMD_TEST_Z_24					27 // (mask[3]) проверяет что пины находятся в Z-состоянии
#define CMD_TEST_Z_28					28 // (mask[4]) проверяет что пины находятся в Z-состоянии
#define CMD_TEST_OC_16					29 // (mask[2]) проверяет что пины являются выходами с открытым коллектором
#define CMD_TEST_OC_24					30 // (mask[3]) проверяет что пины являются выходами с открытым коллектором
#define CMD_TEST_OC_28					31 // (mask[4]) проверяет что пины являются выходами с открытым коллектором
#define CMD_REPEAT_PULSE				32	// (uint16) повторяет последний импульс несколько раз

#define CMD_PRINT_14						200

#define CMD_END							0xff	// признак окончания теста

#define TYPE_TTL		0
#define TYPE_CMOS		1

bool TestLogic(uint8_t type);

/************************************************************************
  Выполняет тест, читая программу из буфера                            
  После завершения возвращает результаты теста и переводит указатель на
  начало следующей команды
************************************************************************/
bool TestData();

char* GetDeviceName();



#endif // TEST_H_