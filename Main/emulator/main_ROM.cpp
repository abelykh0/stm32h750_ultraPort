#include "main_ROM.h"
#include "stm32h7xx_hal.h"

uint8_t* ROM = (uint8_t*)QSPI_BASE + 0x30000;

