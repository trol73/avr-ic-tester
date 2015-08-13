/*
 * test.c
 *
 * Created: 15.07.2015 13:17:54
 *  Author: Trol
 */ 

#include "test.h"

#include "tester_io.h"
#include "debug.h"

#include "data.h"
#include "ui.h"
#include "keyboard.h"

#include <util/delay.h>
#include <string.h>

char chipName[32];
char testResultName[128];
uint8_t lastPulse;

static void pgm_read_val16(const uint8_t *buffer, val16_t *val) {
	val->b0 = pgm_read_byte(buffer);
	val->b1 = pgm_read_byte(buffer+1);
}

static void pgm_read_val24(const uint8_t *buffer, val24_t *val) {
	val->b0 = pgm_read_byte(buffer);
	val->b1 = pgm_read_byte(buffer+1);
	val->b2 = pgm_read_byte(buffer+2);
}

static void pgm_read_val28(const uint8_t *buffer, val28_t *val) {
	val->b0 = pgm_read_byte(buffer);
	val->b1 = pgm_read_byte(buffer+1);
	val->b2 = pgm_read_byte(buffer+2);
	val->b3 = pgm_read_byte(buffer+3);
}

static void pulse() {
	if (lastPulse < 0x80) {
		TesterSetPin(lastPulse, 0);
		_delay_us(1);
		TesterSetPin(lastPulse, 1);
	} else {
		TesterSetPin(lastPulse-0x80, 0);
		_delay_us(1);
		TesterSetPin(lastPulse-0x80, 1);		
	}
}


bool TestData(uint8_t **ptr) {
//	uint8_t succesCnt = 0;
	uint8_t failureCnt = 0;
	
	// копируем имя микросхемы
	for (uint8_t i = 0; i < sizeof(chipName); i++) {
		char ch = pgm_read_byte((*ptr)++);
		chipName[i] = ch;
		if (ch == 0) {
			break;
		}
	}
		
	for (;;) {
		uint8_t cmd = pgm_read_byte(*ptr);
		(*ptr)++;
		MSG_DEC("cmd ", cmd);
		
		switch (cmd) {
			case CMD_RESET:				// переводит все пины в режим чтения (не трогает нижние)
				TesterReset(false);
				break;
			case CMD_RESET_FULL:		// переводит все пины в режим чтения (включая нижние)
				TesterReset(true);
				break;
			case CMD_INIT_16:			// (mask[2]) настраивает порты ввода-вывода
				{
					val16_t mask;
					pgm_read_val16(*ptr, &mask);
					*ptr += 2;
					TesterConfig16(&mask);
				}
				break;
			case CMD_INIT_24:			// (mask[3]) настраивает порты ввода-вывода
				{
					val24_t mask;
					pgm_read_val24(*ptr, &mask);
					*ptr += 3;
					TesterConfig24(&mask);
				}
				break;				
			case CMD_INIT_28:			// (mask[4]) настраивает порты ввода-вывода
				{
					val28_t mask;
					pgm_read_val28(*ptr, &mask);
					*ptr += 4;
					TesterConfig28(&mask);
				}
				break;
			case CMD_SET_16:			// (mask0[2], mask1[2]) выставляет логические 0 и 1 на выводах по маскам
				{
					val16_t mask0, mask1;
					pgm_read_val16(*ptr, &mask0);
					*ptr += 2;
					pgm_read_val16(*ptr, &mask1);
					*ptr += 2;
					
					if (failureCnt == 0) {
						TesterSet16(&mask0, &mask1);
					}
				}			
				break;
			case CMD_SET_24:			// (mask0[3], mask1[3]) выставляет логические 0 и 1 на выводах по маскам
				{
					val24_t mask0, mask1;
					pgm_read_val24(*ptr, &mask0);
					*ptr += 3;
					pgm_read_val24(*ptr, &mask1);
					*ptr += 3;
				
					if (failureCnt == 0) {
						TesterSet24(&mask0, &mask1);
					}
				}
				break;
			case CMD_SET_28:			// (mask0[4], mask1[4]) выставляет логические 0 и 1 на выводах по маскам
				{
					val28_t mask0, mask1;
					pgm_read_val28(*ptr, &mask0);
					*ptr += 4;
					pgm_read_val28(*ptr, &mask1);
					*ptr += 4;
					
					if (failureCnt == 0) {
						TesterSet28(&mask0, &mask1);
					}
				}
				break;
			case CMD_TEST_16:			// (mask0[2], mask1[2]) проверяет, что на выводах установлен ожидаемый уровень
				{
					val16_t mask0, mask1;
					pgm_read_val16(*ptr, &mask0);
					*ptr += 2;
					pgm_read_val16(*ptr, &mask1);
					*ptr += 2;
										
					if (failureCnt == 0) {
						if (TesterTest16(&mask0, &mask1)) {
	MSG("success");
							//succesCnt++;
						} else {
	MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_TEST_24:			// (mask0[3], mask1[3]) проверяет, что на выводах установлен ожидаемый уровень
				{
					val24_t mask0, mask1;
					pgm_read_val24(*ptr, &mask0);
					*ptr += 3;
					pgm_read_val24(*ptr, &mask1);
					*ptr += 3;
				
					if (failureCnt == 0) {
						if (TesterTest24(&mask0, &mask1)) {
							MSG("success");
							//succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_TEST_28:			// (mask0[4], mask1[4]) проверяет, что на выводах установлен ожидаемый уровень
				{
					val28_t mask0, mask1;
					pgm_read_val28(*ptr, &mask0);
					*ptr += 4;
					pgm_read_val28(*ptr, &mask1);
					*ptr += 4;
					
					if (failureCnt == 0) {
						if (TesterTest28(&mask0, &mask1)) {
							MSG("success");
							//succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
				
			case CMD_PULSE_PLUS:	// pin[1] подает положительный импульс (0 -> 1) на вывод
				{
					uint8_t pin = pgm_read_byte(*ptr);
					(*ptr)++;
					lastPulse = pin;
					pulse();
				}
				break;
				
			case CMD_PULSE_MINUS:	// pin[1] подает отрицательный импульс (1 -> 0) на вывод
				{
					uint8_t pin = pgm_read_byte(*ptr);
					(*ptr)++;
					lastPulse = pin + 0x80;
					pulse();
				}			
				break;
			case CMD_SET_ALL_AND_TEST_16:	// (mask_set[2], mask_test[2]) устанавливает и проверяет все пины
			case CMD_SET_ALL_16:	// (mask[2] устанавливает все пины по маске)
				{
					val16_t mask;
					pgm_read_val16(*ptr, &mask);
					*ptr += 2;
					
					if (failureCnt == 0) {
						TesterSetAll16(&mask);
					}
				}
				if (cmd == CMD_SET_ALL_16) {
					break;
				}
			
			case CMD_TEST_ALL_16:	// (mask[2] проверяет уровни на всех пинах МС по маске)
				{
					val16_t mask;
					pgm_read_val16(*ptr, &mask);
					*ptr += 2;
					
					if (failureCnt == 0) {
						if (TesterTestAll16(&mask)) {
							MSG("success");
							//succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}			
				break;
			case CMD_SET_ALL_AND_TEST_24:	// (mask_set[3], mask_test[3]) устанавливает и проверяет все пины
			case CMD_SET_ALL_24:	// (mask[3] устанавливает все пины по маске)
				{
					val24_t mask;
					pgm_read_val24(*ptr, &mask);
					*ptr += 3;
					
					if (failureCnt == 0) {
						TesterSetAll24(&mask);
					}
				}
				if (cmd == CMD_SET_ALL_24) {
					break;
				}
			case CMD_TEST_ALL_24:	// (mask[3] проверяет уровни на всех пинах МС по маске)
				{
					val24_t mask;
					pgm_read_val24(*ptr, &mask);
					*ptr += 3;
					
					if (failureCnt == 0) {
						if (TesterTestAll24(&mask)) {
							MSG("success");
							//succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_SET_ALL_AND_TEST_28:	// (mask_set[4], mask_test[4]) устанавливает и проверяет все пины				
			case CMD_SET_ALL_28:	// (mask[4] устанавливает все пины по маске)
				{
					val28_t mask;
					pgm_read_val28(*ptr, &mask);
					*ptr += 4;
					
					if (failureCnt == 0) {
						TesterSetAll28(&mask);
					}
				}
				if (cmd == CMD_SET_ALL_28) {
					break;
				}
			case CMD_TEST_ALL_28:	// (mask[4] проверяет уровни на всех пинах МС по маске)
				{
					val28_t mask;
					pgm_read_val28(*ptr, &mask);
					*ptr += 4;
					
					if (failureCnt == 0) {
						if (TesterTestAll28(&mask)) {
							MSG("success");
							//succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;

			case CMD_LAST_PULSE:	// повторяет последний импульс
				if (lastPulse < 0x80) {
					TesterSetPin(lastPulse, 0);
					_delay_us(1);
					TesterSetPin(lastPulse, 1);	
				} else {
					TesterSetPin(lastPulse-0x80, 1);
					_delay_us(1);
					TesterSetPin(lastPulse-0x80, 0);					
				}
				break;
			case CMD_LAST_PULSE_AND_TEST_16:	// (mask[2]) повторяет последний импульс и проверяет состояние всех пинов
				pulse();
				{
					val16_t mask;
					pgm_read_val16(*ptr, &mask);
					*ptr += 2;
					
					if (failureCnt == 0) {
						if (TesterTestAll16(&mask)) {
							MSG("success");
							//succesCnt++;
							} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_LAST_PULSE_AND_TEST_24:	// (mask[3]) повторяет последний импульс и проверяет состояние всех пинов
				pulse();
				{
					val24_t mask;
					pgm_read_val24(*ptr, &mask);
					*ptr += 3;
					
					if (failureCnt == 0) {
						if (TesterTestAll24(&mask)) {
							MSG("success");
							//succesCnt++;
							} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_LAST_PULSE_AND_TEST_28:	// (mask[2]) повторяет последний импульс и проверяет состояние всех пинов
				pulse();
				{
					val28_t mask;
					pgm_read_val28(*ptr, &mask);
					*ptr += 4;
					
					if (failureCnt == 0) {
						if (TesterTestAll28(&mask)) {
							MSG("success");
							//succesCnt++;
							} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
				
				
			case CMD_END:				// признак окончания теста
				return failureCnt == 0;
				
			case CMD_PRINT_14:
				TesterDebugStatus(14);
				break;
		}
	}
}


bool TestLogic() {
	uint8_t *ptr = (uint8_t*)&LOGIC_DATA[0];
	testResultName[0] = 0;
	bool result = false;
	while (true) {
		uint8_t cmd = pgm_read_byte(ptr);
		if (cmd == CMD_END) {
			break;
		}
		if (TestData(&ptr)) {
			result = true;
			uint8_t len = strlen(testResultName);
			if (len > 0) {
				testResultName[len] = ',';
				testResultName[len+1] = 0;
			}
			strcat(testResultName, chipName);
		}
	}
	TesterReset(false);
	
	return result;
}

char* GetDeviceName() {
	return testResultName;
}
