#include "resources.h"
#include "stm32h7xx_hal.h"

uint8_t* ROM = (uint8_t*)QSPI_BASE + 0x30000;              // opense.bin
uint8_t* spectrumKeyboard = (uint8_t*)QSPI_BASE + 0x34000; // keyboard.bin
uint8_t* font8x8 = (uint8_t*)QSPI_BASE + 0x37000;          // font8x8
