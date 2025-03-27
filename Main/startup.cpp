#include "stm32h7xx_hal.h"
#include <stdio.h>

#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usb_host.h"
#include "usbh_hid.h"

#include "w25qxx_qspi.h"
#include "fatfs.h"
#include "config.h"
#include "emulator.h"
#include "sdcard.h"
#include "lcd.h"
#include "emulator/videoRam.h"
#include "emulator/z80main.h"
#include "emulator/z80input.h"
#include "keyboard/keyboard.h"

static void MapFlash();
extern z80::VideoRam videoRam;
Display::Screen fullScreen;

extern "C" void initialize()
{
	MX_GPIO_Init();

	// On this board required to keep running
	HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_SET);

	PrepareClut();
}

extern "C" void setup()
{
	MapFlash();

	LtdcInit();
	HAL_PWREx_EnableUSBVoltageDetector();

	fullScreen.Clear();

	HAL_TIM_Base_Start_IT(&htim1);

	//videoRam.ShowScreenshot((uint8_t*)QSPI_BASE);
	zx_setup();
}

extern "C" void loop()
{
	if (loadSnapshotLoop())
	{
		return;
	}

	if (saveSnapshotLoop())
	{
		return;
	}

	if (showKeyboardLoop())
	{
		return;
	}

	zx_loop();
	int8_t result = GetScanCode(false);
	switch (result)
	{
	case KEY_ESCAPE:
		clearHelp();
		break;

	case KEY_F1:
		toggleHelp();
		break;

	case KEY_F2:
		if (!saveSnapshotSetup())
		{
			showErrorMessage("Cannot initialize SD card");
		}
		break;

	case KEY_F3:
		if (!loadSnapshotSetup())
		{
			showErrorMessage("Error when loading from SD card");
		}
		break;

	case KEY_F5:
		zx_reset();
		showHelp();
		break;

	case KEY_F10:
		showKeyboardSetup();
		break;

	case KEY_F12:
		showRegisters();
		break;
	}
}

extern "C" bool onHardFault()
{
	uint32_t cfsr = SCB->CFSR; // Configurable Fault Status Register
	uint32_t hfsr = SCB->HFSR; // Hard Fault Status Register
	uint32_t mmfar = SCB->MMFAR; // Memory Management Fault Address
	uint32_t bfar = SCB->BFAR; // Bus Fault Address
	char buffer[20];
	sprintf(buffer, "%08lX", hfsr);
	return true;
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	// Can't do it, will hang, it is using HAL_Delay
	//MX_USB_HOST_Process();
}

static void MapFlash()
{
	w25qxx_Init();
	w25qxx_EnterQPI();
	w25qxx_Startup(w25qxx_NormalMode); // w25qxx_DTRMode
}
