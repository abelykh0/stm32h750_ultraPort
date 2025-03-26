#include "emulator.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32h7xx_hal.h"

#include "emulator/z80main.h"
#include "emulator/z80snapshot.h"
#include "emulator/z80emu/z80emu.h"
#include "keyboard/keyboard.h"

#define DEBUG_BAND_HEIGHT (DEBUG_ROWS * 8 * 2)

extern RTC_HandleTypeDef hrtc;

uint8_t _buffer16K_1[0x4000];
uint8_t _buffer16K_2[0x4000];
static uint8_t* _savedScreenData = &_buffer16K_2[0x2100];

z80::SpectrumScreen MainScreen;
z80::VideoRam videoRam(&MainScreen);
Display::Screen DebugScreen(0, SCREEN_Y_OFFSET, H_SIZE, V_SIZE - SCREEN_Y_OFFSET);

static bool _showingKeyboard;
static bool _helpShown;

void startVideo()
{
	DebugScreen.Clear();
	MainScreen.Clear();
}

void showErrorMessage(const char* errorMessage)
{
	DebugScreen.SetAttribute(0x0310); // red on blue
	DebugScreen.PrintAlignCenter(2, errorMessage);
	DebugScreen.SetAttribute(0x3F10); // white on blue
}

void showTitle(const char* title)
{
	DebugScreen.SetAttribute(0x3F00); // white on black
	DebugScreen.PrintAlignCenter(0, title);
	DebugScreen.SetAttribute(0x3F10); // white on blue
}

void showKeyboardSetup()
{
	saveState();
	_showingKeyboard = true;

	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	DebugScreen.PrintAlignCenter(2, "Press any key to return");

	videoRam.ShowScreenshot(spectrumKeyboard);
	MainScreen.WriteBorderColor(0); // Black
}

bool showKeyboardLoop()
{
	if (!_showingKeyboard)
	{
		return false;
	}

	int8_t scanCode = GetScanCode(true);
	if (scanCode == 0)
	{
		return true;
	}

	_showingKeyboard = false;
	restoreState(true);
	return false;
}

void toggleHelp()
{
	if (_helpShown)
	{
		clearHelp();
	}
	else
	{
		showHelp();
	}
}

void clearHelp()
{
	//DebugScreen.HideCursor();
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

	_helpShown = false;
}

void showHelp()
{
	//DebugScreen.HideCursor();
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

	DebugScreen.PrintAt(0, 0, "F1  - show / hide help");
	DebugScreen.PrintAt(0, 1, "F2  - save snapshot to SD card");
	DebugScreen.PrintAt(0, 2, "F3  - load snapshot from SD card");
	DebugScreen.PrintAt(0, 3, "F5  - reset");
	DebugScreen.PrintAt(0, 4, "F10 - show keyboard layout");
	DebugScreen.PrintAt(0, 5, "F12 - show registers");

	_helpShown = true;
}

void restoreHelp()
{
	if (_helpShown)
	{
		showHelp();
	}
	else
	{
		clearHelp();
	}
}

void saveState()
{
	videoRam.SaveScreenData(_savedScreenData);
}

void restoreState(bool restoreScreen)
{
	if (restoreScreen)
	{
		videoRam.RestoreScreenData(_savedScreenData);
	}

	restoreHelp();
}

void showRegisters()
{
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	showTitle("Registers. ESC - clear");

    char* buf = (char*)_buffer16K_1;

    sprintf(buf, "PC %04x  AF %04x  AF' %04x  I %02x",
        _zxCpu.pc, _zxCpu.registers.word[Z80_AF],
        _zxCpu.alternates[Z80_AF], _zxCpu.i);
    DebugScreen.PrintAlignCenter(2, buf);
    sprintf(buf, "SP %04x  BC %04x  BC' %04x  R %02x",
        _zxCpu.registers.word[Z80_SP], _zxCpu.registers.word[Z80_BC],
        _zxCpu.alternates[Z80_BC], _zxCpu.r);
    DebugScreen.PrintAlignCenter(3, buf);
    sprintf(buf, "IX %04x  DE %04x  DE' %04x  IM %x",
        _zxCpu.registers.word[Z80_IX], _zxCpu.registers.word[Z80_DE],
        _zxCpu.alternates[Z80_DE], _zxCpu.im);
    DebugScreen.PrintAlignCenter(4, buf);
    sprintf(buf, "IY %04x  HL %04x  HL' %04x      ",
        _zxCpu.registers.word[Z80_IY], _zxCpu.registers.word[Z80_HL],
        _zxCpu.alternates[Z80_HL]);
    DebugScreen.PrintAlignCenter(5, buf);
}
