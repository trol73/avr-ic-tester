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

#include <util/delay.h>

char chipName[32];


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


bool TestData(uint8_t **ptr) {
	uint8_t succesCnt = 0;
	uint8_t failureCnt = 0;
	
	// копируем им€ микросхемы
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
			case CMD_RESET:				// переводит все пины в режим чтени€ (не трогает нижние)
				TesterReset(false);
				break;
			case CMD_RESET_FULL:		// переводит все пины в режим чтени€ (включа€ нижние)
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
					MSG_HEX("init_0 ", mask.b0, 1);
					MSG_HEX("init_1 ", mask.b1, 1);
					MSG_HEX("init_2 ", mask.b2, 1);
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
			case CMD_SET_16:			// (mask0[2], mask1[2]) выставл€ет логические 0 и 1 на выводах по маскам
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
			case CMD_SET_24:			// (mask0[3], mask1[3]) выставл€ет логические 0 и 1 на выводах по маскам
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
			case CMD_SET_28:			// (mask0[4], mask1[4]) выставл€ет логические 0 и 1 на выводах по маскам
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
			case CMD_TEST_16:			// (mask0[2], mask1[2]) провер€ет, что на выводах установлен ожидаемый уровень
				{
					val16_t mask0, mask1;
					pgm_read_val16(*ptr, &mask0);
					*ptr += 2;
					pgm_read_val16(*ptr, &mask1);
					*ptr += 2;
										
					if (failureCnt == 0) {
						if (TesterTest16(&mask0, &mask1)) {
	MSG("success");
							succesCnt++;
						} else {
	MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_TEST_24:			// (mask0[3], mask1[3]) провер€ет, что на выводах установлен ожидаемый уровень
				{
					val24_t mask0, mask1;
					pgm_read_val24(*ptr, &mask0);
					*ptr += 3;
					pgm_read_val24(*ptr, &mask1);
					*ptr += 3;
				
					if (failureCnt == 0) {
						if (TesterTest24(&mask0, &mask1)) {
							MSG("success");
							succesCnt++;
						} else {
							MSG("fail");
							failureCnt++;
						}
					}
				}
				break;
			case CMD_TEST_28:			// (mask0[4], mask1[4]) провер€ет, что на выводах установлен ожидаемый уровень
				{
					val28_t mask0, mask1;
					pgm_read_val28(*ptr, &mask0);
					*ptr += 4;
					pgm_read_val28(*ptr, &mask1);
					*ptr += 4;
					
					if (failureCnt == 0) {
						if (TesterTest28(&mask0, &mask1)) {
							MSG("success");
							succesCnt++;
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
					TesterSetPin(pin, 0);
					_delay_us(1);
					TesterSetPin(pin, 1);
				}
				break;
				
			case CMD_PULSE_MINUS:	// pin[1] подает отрицательный импульс (1 -> 0) на вывод
				{
					uint8_t pin = pgm_read_byte(*ptr);
					(*ptr)++;
					TesterSetPin(pin, 1);
					_delay_us(1);
					TesterSetPin(pin, 0);
				}			
				break;
				
			case CMD_END:				// признак окончани€ теста
				return failureCnt == 0;
		}
	}
}

/*
#define val16(v)	((v) & 0xff), ((v) >> 8)

#define _(v)		(1 << (v-1))

const uint8_t LOGIC_DATA[] PROGMEM = {
	// 155 Ћј 3
	'L', 'A', '3', 0,
	CMD_RESET,
	CMD_INIT_14,	val16(_(1)|_(2)|_(4)|_(5)|_(7)|_(9)|_(10)|_(12)|_(13)|_(14)),
	CMD_SET_14,		val16(_(7)),   val16(_(14)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),
	CMD_TEST_14,	val16(_(3)|_(6)|_(8)|_(11)),   val16(0),
	CMD_SET_14,		val16(_(7)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),   val16(_(14)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(2)|_(5)|_(10)|_(13)),   val16(_(14)|_(1)|_(4)|_(9)|_(12)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(1)|_(4)|_(9)|_(12)),   val16(_(14)|_(2)|_(5)|_(10)|_(13)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_END,
	
	// 155 Ћј 11, 155 Ћј 13
	'L', 'A', '1', '1', ',', 'L', 'A', '1', '3', 0,
	CMD_RESET,
	CMD_INIT_14,	val16(_(1)|_(2)|_(4)|_(5)|_(7)|_(9)|_(10)|_(12)|_(13)|_(14)),
	CMD_SET_14,		val16(_(7)),   val16(_(14)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(_(3)|_(6)|_(8)|_(11)),   val16(0),
	CMD_SET_14,		val16(_(7)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),   val16(_(14)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(2)|_(5)|_(10)|_(13)),   val16(_(14)|_(1)|_(4)|_(9)|_(12)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(1)|_(4)|_(9)|_(12)),   val16(_(14)|_(2)|_(5)|_(10)|_(13)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_END,	
	
	// 155 Ћј 18
	'L', 'A', '1', '8', 0,
	CMD_RESET,
	CMD_INIT_14,	val16(_(1)|_(2)|_(4)|_(5)|_(7)|_(9)|_(10)|_(12)|_(13)|_(14)),
	CMD_SET_14,		val16(_(7)),   val16(_(14)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(_(3)|_(6)|_(8)|_(11)),   val16(0),
	CMD_SET_14,		val16(_(7)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),   val16(_(14)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(2)|_(5)|_(10)|_(13)),   val16(_(14)|_(1)|_(4)|_(9)|_(12)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_SET_14,		val16(_(7)|_(1)|_(4)|_(9)|_(12)),   val16(_(14)|_(2)|_(5)|_(10)|_(13)|_(3)|_(6)|_(8)|_(11)),
	CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
	CMD_END,
	
	
	CMD_END	
};

*/




bool TestLogic() {
	uint8_t *ptr = (uint8_t*)&LOGIC_DATA[0];
	while (true) {
		uint8_t cmd = pgm_read_byte(ptr);
		if (cmd == CMD_END) {
			break;
		}
		bool result = TestData(&ptr);
		if (result) {
			TesterReset(false);
			return true;
		}
	}
	TesterReset(false);
	return false;
}

char* GetDeviceName() {
	return chipName;
}
