#include <stdio.h>
#include "stm32h7xx_hal.h"
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



