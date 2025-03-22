#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include "screen.h"
#include "emulator/SpectrumScreen.h"
#include "fatfs.h"
#include "screen.h"

using namespace Display;

#define DEBUG_COLUMNS (H_SIZE / 8)
#define SCREEN_Y_OFFSET 256
#define DEBUG_ROWS ((V_SIZE - SCREEN_Y_OFFSET) / 8)

extern Display::Screen DebugScreen;
extern z80::SpectrumScreen MainScreen;
extern uint8_t _buffer16K_1[0x4000];
extern uint8_t _buffer16K_2[0x4000];

void showKeyboardSetup();
bool showKeyboardLoop();

void setDateTimeSetup();
bool setDateTimeLoop();

void showHelp();
void clearHelp();
void toggleHelp();
void restoreHelp();

void initializeVideo();
void startVideo();
void showErrorMessage(const char* errorMessage);
void showTitle(const char* title);

void saveState();
void restoreState(bool restoreScreen);

void showRegisters();

#endif /* __EMULATOR_H__ */
