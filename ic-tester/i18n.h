/*
 * i18n.h
 *
 * Created: 18.07.2015 22:54:53
 *  Author: Trol
 */ 


#ifndef I18N_H_
#define I18N_H_

#define DEFSTR(name, string)						const char name[] PROGMEM = string;

DEFSTR(STR_UNKNOWN_OR_FAILED, "Unknown");
DEFSTR(STR_APPNAME, "IC-TESTER");
DEFSTR(STR_VERSION, "Version 0.6");
DEFSTR(STR_AUTODETECT, "Detect & Test");
DEFSTR(STR_CUSTOM_TEST, "Custom test");
DEFSTR(STR_MEM_TEST, "Memory test");
DEFSTR(STR_ABOUT, "About");

DEFSTR(STR_INSERT_CHIP, "Plug chip and");
DEFSTR(STR_AND_PRESS_BUTTON, "press \"Test\"");
DEFSTR(STR_TESTING, "Testing...");
DEFSTR(STR_BACK, "Back");
DEFSTR(STR_SETUP, "setup");
DEFSTR(STR_EXIT, "exit");
DEFSTR(STR_OUT, "out");


DEFSTR(STR_RU5, "аг5");
DEFSTR(STR_RU7, "аг7");
DEFSTR(STR_CHIP, "Chip:");
DEFSTR(STR_NONE, "none");
DEFSTR(STR_4164, "4164");
DEFSTR(STR_41256, "41256");
DEFSTR(STR_DIP_8, "DIP-8");
DEFSTR(STR_DIP_14, "DIP-14");
DEFSTR(STR_DIP_16, "DIP-16");
DEFSTR(STR_DIP_20, "DIP-20");

DEFSTR(STR_TTL, "TTL");
DEFSTR(STR_CMOS, "CMOS");

DEFSTR(STR_COPYRIGHT_DATE, "(c) 2015");
DEFSTR(STR_COPYRIGHT_NAME, "Oleg Trifonov");
DEFSTR(STR_COPYRIGHT_NAME_2, "Sergey Bagan");
//                          


#endif /* I18N_H_ */