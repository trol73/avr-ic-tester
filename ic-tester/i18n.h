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
DEFSTR(STR_VERSION, "Version 0.01");
DEFSTR(STR_AUTODETECT, "Detect & Test");
DEFSTR(STR_CUSTOM_TEST, "Custom test");
DEFSTR(STR_MEM_TEST, "Memory test");
DEFSTR(STR_ABOUT, "About");

DEFSTR(STR_INSERT_CHIP, "Plug chip and");
DEFSTR(STR_AND_PRESS_BUTTON, "press \"Test\"");
DEFSTR(STR_TESTING, "Testing...");

DEFSTR(STR_RU5, "аг5");
DEFSTR(STR_RU7, "аг7");
DEFSTR(STR_CHIP, "Chip:");
DEFSTR(STR_NONE, "none");
DEFSTR(STR_4164, "4164");
DEFSTR(STR_41256, "41256");


#endif /* I18N_H_ */