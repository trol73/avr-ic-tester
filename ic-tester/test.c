/*
 * test.c
 *
 * Created: 15.07.2015 13:17:54
 *  Author: Trol
 */ 

#include "test.h"

#include "tester_io.h"
#include "debug.h"


char chipName[32];



static uint32_t pgm_read_3bytes(const uint8_t *buffer) {
	uint32_t b1 = pgm_read_byte(buffer);
	uint16_t b2 = pgm_read_byte(buffer+1);
	uint8_t b3 = pgm_read_byte(buffer+2);
	
	return (b1 << 16) + (b2 << 8) + b3;
}

bool TestData(uint8_t **ptr) {
	uint8_t succesCnt = 0;
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
			case CMD_INIT_14:			// (mask[2]) настраивает порты ввода-вывода
				{
					uint16_t mask = pgm_read_word(*ptr);
					*ptr += 2;
					TesterConfig14(mask);
				}
				break;
			case CMD_INIT_16:			// (mask[2]) настраивает порты ввода-вывода
				{
					uint16_t mask = pgm_read_word(*ptr);
					ptr += 2;
					TesterConfig16(mask);
				}
				break;
			case CMD_INIT_20:			// (mask[3]) настраивает порты ввода-вывода
				{
					uint32_t mask = pgm_read_3bytes(*ptr);
					ptr += 3;
					TesterConfig20(mask);
				}
				break;
			case CMD_INIT_28:			// (mask[3]) настраивает порты ввода-вывода
				{
					uint32_t mask = pgm_read_3bytes(*ptr);
					*ptr += 3;
					TesterConfig28(mask);
				}
				break;
			case CMD_SET_14:			// (mask0[2], mask1[2]) выставляет логические 0 и 1 на выводах по маскам
				{
					uint16_t mask0 = pgm_read_word(*ptr);
					*ptr += 2;
					uint16_t mask1 = pgm_read_word(*ptr);
					*ptr += 2;
					TesterSet14(mask0, mask1);
				}
				break;
			case CMD_SET_16:			// (mask0[2], mask1[2]) выставляет логические 0 и 1 на выводах по маскам
				{
					uint16_t mask0 = pgm_read_word(*ptr);
					*ptr += 2;
					uint16_t mask1 = pgm_read_word(*ptr);
					*ptr += 2;
					TesterSet16(mask0, mask1);
				}			
				break;
			case CMD_SET_20:			// (mask0[3], mask1[3]) выставляет логические 0 и 1 на выводах по маскам
				{
					uint32_t mask0 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					uint32_t mask1 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					TesterSet20(mask0, mask1);
				}
				break;
			case CMD_SET_28:			// (mask0[3], mask1[3]) выставляет логические 0 и 1 на выводах по маскам
				{
					uint32_t mask0 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					uint32_t mask1 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					TesterSet28(mask0, mask1);
				}
				break;
			case CMD_TEST_14:			// (mask0[2], mask1[2]) проверяет, что на выводах установлен ожидаемый уровень
				{
					uint16_t mask0 = pgm_read_word(*ptr);
					*ptr += 2;
					uint16_t mask1 = pgm_read_word(*ptr);
					*ptr += 2;
					if (TesterTest14(mask0, mask1)) {
MSG("success");
						succesCnt++;
					} else {
MSG("fail");
						failureCnt++;
					}
				}
				break;
			case CMD_TEST_16:			// (mask0[2], mask1[2]) проверяет, что на выводах установлен ожидаемый уровень
				{
					uint16_t mask0 = pgm_read_word(*ptr);
					*ptr += 2;
					uint16_t mask1 = pgm_read_word(*ptr);
					*ptr += 2;
					if (TesterTest16(mask0, mask1)) {
MSG("success");
						succesCnt++;
					} else {
MSG("fail");
						failureCnt++;
					}
				}
				break;
			case CMD_TEST_20:			// (mask0[3], mask1[3]) проверяет, что на выводах установлен ожидаемый уровень
				{
					uint32_t mask0 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					uint32_t mask1 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					if (TesterTest20(mask0, mask1)) {
						succesCnt++;
					} else {
						failureCnt++;
					}
				}
				break;
			case CMD_TEST_28:			// (mask0[3], mask1[3]) проверяет, что на выводах установлен ожидаемый уровень
				{
					uint32_t mask0 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					uint32_t mask1 = pgm_read_3bytes(*ptr);
					*ptr += 3;
					if (TesterTest28(mask0, mask1)) {
						succesCnt++;
					} else {
						failureCnt++;
					}
				}
				break;
				
			case CMD_END:				// признак окончания теста
				return failureCnt == 0;
		}
	}
}


#define val16(v)	((v) & 0xff), ((v) >> 8)

#define _(v)		(1 << (v-1))

const uint8_t LOGIC_DATA[] PROGMEM = {
	// 155 ЛА 3
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
	
	// 155 ЛА 11, 155 ЛА 13
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
	
	CMD_END	
};






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

const char* GetDeviceName() {
	return chipName;
}
