#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include <stdio.h>

#include "w25qxx_qspi.h"
#include "fatfs.h"

static void MapFlash();

extern "C" void initialize()
{
}

extern "C" void setup()
{
	HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_SET);

	// I2C Serial EEPROM 2K  (AT24C02_1)

	// QSPI Flash 8MB (W25Q128JVSIM_TR)
	MapFlash();

	// Read from QSPI Flash
	uint8_t data[16];
	memcpy(data, (uint8_t*)QSPI_BASE, 16);

	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);

	if (f_mount(&SDFatFS, (TCHAR*)u"0:/", 1) == FR_OK)
	{
		FIL file;
		if (f_open(&file, (const TCHAR*)u"0:/Keyboard720x400.bmp", FA_READ) == FR_OK)
		{
			f_close(&file);
		}

		f_mount(nullptr, nullptr, 1);
	}
}

extern "C" void loop()
{
	HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
	HAL_Delay (1000);
}

static void MapFlash()
{
	w25qxx_Init();
	w25qxx_EnterQPI();
	w25qxx_Startup(w25qxx_NormalMode); // w25qxx_DTRMode
}
