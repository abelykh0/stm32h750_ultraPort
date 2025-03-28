#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "stdint.h"

uint8_t GetScanCode(bool wait);

uint8_t GetAsciiCode();

#endif
