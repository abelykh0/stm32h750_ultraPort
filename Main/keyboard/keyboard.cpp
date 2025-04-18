#include "keyboard.h"
#include "usbh_hid.h"
#include "usb_host.h"
#include "emulator/z80input.h"

static uint8_t lastScanCode;
static HID_KEYBD_Info_TypeDef lastInfo;

uint8_t GetScanCode(bool wait)
{
	if (wait)
	{
		while (lastScanCode == 0)
		{
			HAL_Delay(10);
		}
	}

	uint8_t result = lastScanCode;
	lastScanCode = 0;
	return result;
}

uint8_t GetAsciiCode()
{
	return USBH_HID_GetASCIICode(&lastInfo);
}

extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef* phost)
{
	if (USBH_HID_GetDeviceType(phost) == HID_KEYBOARD)
	{
		memset(indata, 0xFF, sizeof(indata));

		HID_KEYBD_Info_TypeDef* keyboardInfo = USBH_HID_GetKeybdInfo(phost);

		if (keyboardInfo->lshift || keyboardInfo->rshift)
		{
			OnKey(KEY_LEFTSHIFT);
		}

		if (keyboardInfo->lctrl || keyboardInfo->rctrl)
		{
			OnKey(KEY_LEFTCONTROL);
		}

		lastScanCode = 0;
		for (uint8_t i = 0; i < 6; i++)
		{
			uint8_t key = keyboardInfo->keys[i];
			if (key == 0)
			{
				break;
			}

			lastScanCode = key;
			lastInfo = *keyboardInfo;
			OnKey(key);
		}
	}
}
