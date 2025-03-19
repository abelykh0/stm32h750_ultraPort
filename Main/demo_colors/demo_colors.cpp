#include <stdio.h>
#include "stm32h7xx_hal.h"
#include "rtc.h"
#include "rtc.h"
#include "usbh_hid.h"

#include "demo_colors.h"
#include "screen/screen.h"

Display::Screen screen;

void init_demo_colors()
{
	screen.SetAttribute(0x2A10);
	screen.Clear();

	// Frame
	screen.SetAttribute(0x2A10);
    screen.PrintAt(0, 0, "\xC9"); // ╔
    screen.PrintAt(TEXT_COLUMNS - 1, 0, "\xBB"); // ╗
    screen.PrintAt(0, TEXT_ROWS - 1, "\xC8"); // ╚
    screen.PrintAt(TEXT_COLUMNS - 1, TEXT_ROWS - 1, "\xBC"); // ╝
    for (uint16_t i = 1; i < TEXT_COLUMNS - 1; i++)
    {
        screen.PrintAt(i, 0, "\x0CD"); // ═
        screen.PrintAt(i, TEXT_ROWS - 1, "\x0CD"); // ═
    }
    for (uint16_t i = 1; i < TEXT_ROWS - 1; i++)
    {
        screen.PrintAt(0, i, "\x0BA"); // ║
        screen.PrintAt(TEXT_COLUMNS - 1, i, "\x0BA"); // ║
    }

    // 64 colors
	char buf[20];
    for (int i = 0; i < 64; i++)
    {
    	screen.SetAttribute((i << 8) | 0x10);
    	screen.PrintAt(3 + (i % 5) * 7, 3 + (i / 5) * 2, "\xDF\xDF\xDF\xDF\xDF\xDF"); // ▀▀▀▀▀▀

    	screen.SetAttribute(0x2A10);
    	sprintf(buf, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(i));
    	screen.PrintAt(3 + (i % 5) * 7, 2 + (i / 5) * 2, buf);
    }

	screen.SetAttribute(0x3F10);
}

int32_t loop_demo_colors()
{
	char showTime[20];

	RTC_DateTypeDef dateStruct;
	RTC_TimeTypeDef timeStruct;
	HAL_RTC_GetTime(&hrtc, &timeStruct, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &dateStruct, RTC_FORMAT_BIN); // important
	sprintf(showTime, " %.2d:%.2d:%.2d ", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);
    screen.PrintAlignCenter(0, showTime);

	return 0;
}

extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	if(USBH_HID_GetDeviceType(phost) == HID_MOUSE)  // if the HID is Mouse
	{
		HID_MOUSE_Info_TypeDef *Mouse_Info;
		Mouse_Info = USBH_HID_GetMouseInfo(phost);  // Get the info
		int X_Val = Mouse_Info->x;  // get the x value
		int Y_Val = Mouse_Info->y;  // get the y value
		if (X_Val > 127) X_Val -= 255;
		if (Y_Val > 127) Y_Val -= 255;
		char Uart_Buf[100];
		sprintf (Uart_Buf, "X=%d, Y=%d, Button1=%d, Button2=%d, Button3=%d\n", X_Val, Y_Val, \
				                                Mouse_Info->buttons[0],Mouse_Info->buttons[1], Mouse_Info->buttons[2]);
	    screen.PrintAlignCenter(26, Uart_Buf);
		//HAL_UART_Transmit(&huart2, (uint8_t *) Uart_Buf, len, 100);
	}

	if(USBH_HID_GetDeviceType(phost) == HID_KEYBOARD)  // if the HID is Mouse
	{
		uint8_t key;
		HID_KEYBD_Info_TypeDef *Keyboard_Info;
		Keyboard_Info = USBH_HID_GetKeybdInfo(phost);  // get the info
		key = USBH_HID_GetASCIICode(Keyboard_Info);  // get the key pressed
		char Uart_Buf[100];
		sprintf (Uart_Buf, "Key Pressed = %c\n", key);
	    screen.PrintAlignCenter(28, Uart_Buf);
		//HAL_UART_Transmit(&huart2, (uint8_t *) Uart_Buf, len, 100);
	}
}


