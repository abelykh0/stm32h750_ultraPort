#ifndef _Z80INPUT_H_
#define _Z80INPUT_H_

#include <stdint.h>

extern uint8_t indata[128];
bool OnKey(uint8_t scanCode);

#endif
