#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "usbh_hid.h"
#include "usb_host.h"

#include "SDCard.h"
#include "screen.h"
#include "emulator.h"
#include "keyboard/keyboard.h"
#include "emulator/z80snapshot.h"

#define FILE_COLUMNS 3
#define FILE_COLUMNWIDTH (TEXT_COLUMNS / FILE_COLUMNS)

typedef TCHAR FileName[_MAX_LFN + 1];

static char* _snapshotName = (char*)&_buffer16K_1[2];
static FileName* _fileNames = (FileName*) _buffer16K_2;
static int16_t _selectedFile = 0;
static int16_t _fileCount;
static bool _loadingSnapshot = false;
static bool _savingSnapshot = false;

FRESULT mount()
{
	FRESULT result = f_mount(&SDFatFS, (const TCHAR*) SDPath, 1);
	if (result == FR_OK)
	{
		// turn on built-in LED
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	}

	return result;
}

void unmount()
{
	f_mount(nullptr, (TCHAR const*)SDPath, 1);

	// turn off built-in LED
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
}

void GetFileCoord(uint8_t fileIndex, uint8_t* x, uint8_t* y)
{
	*x = fileIndex / (DEBUG_ROWS - 1) * (FILE_COLUMNWIDTH + 1);
	*y = 1 + fileIndex % (DEBUG_ROWS - 1);
}

TCHAR* TruncateFileName(TCHAR* fileName)
{
	int maxLength = FILE_COLUMNWIDTH - 1;
	TCHAR* result = (TCHAR*) _buffer16K_1;
	strncpy(result, fileName, _MAX_LFN);
	TCHAR* extension = strrchr(result, '.');
	if (extension != nullptr)
	{
		*extension = '\0';
	}

	result[maxLength - 1] = '\0';
	return result;
}

void noScreenshot()
{
	MainScreen.Clear();
	MainScreen.SetAttribute(0x0310); // red on blue
	MainScreen.PrintAlignCenter(11, "Error reading selected file");
	MainScreen.SetAttribute(0x3F10); // white on blue
}

void SetSelection(uint8_t selectedFile)
{
	if (_fileCount == 0)
	{
		return;
	}

	_selectedFile = selectedFile;

	uint8_t x, y;
	GetFileCoord(selectedFile, &x, &y);
	DebugScreen.PrintAt(x, y, "\x10"); // ►

	FRESULT fr;

	// Show screenshot for the selected file
	fr = mount();
	if (fr == FR_OK)
	{
		FIL file;
		bool scrFileFound = false;

		TCHAR* fileName = _fileNames[selectedFile];

		// Try to open file with the same name and .SCR extension
		TCHAR* scrFileName = (TCHAR*) _buffer16K_1;
		strncpy(scrFileName, fileName, _MAX_LFN + 1);
		TCHAR* extension = strrchr(scrFileName, '.');
		if (extension != nullptr)
		{
			strncpy(extension, ".scr", 5);
			fr = f_open(&file, scrFileName, FA_READ | FA_OPEN_EXISTING);
			if (fr == FR_OK)
			{
				if (!z80::LoadScreenshot(&file, _buffer16K_1))
				{
					noScreenshot();
				}
				f_close(&file);
				scrFileFound = true;
			}
		}

		if (!scrFileFound)
		{
			fr = f_open(&file, fileName, FA_READ | FA_OPEN_EXISTING);
			if (fr == FR_OK)
			{
				if (!z80::LoadScreenFromZ80Snapshot(&file, _buffer16K_1))
				{
					noScreenshot();
				}
				f_close(&file);
			}
		}

		unmount();
	}
}

void loadSnapshot(const TCHAR* fileName)
{
	FRESULT fr = mount();
	if (fr == FR_OK)
	{
		FIL file;
		fr = f_open(&file, fileName, FA_READ | FA_OPEN_EXISTING);
		z80::LoadZ80Snapshot(&file, _buffer16K_1, _buffer16K_2);
		f_close(&file);

		unmount();
	}
}

bool saveSnapshot(const TCHAR* fileName)
{
	bool result = false;
	FRESULT fr = mount();
	if (fr == FR_OK)
	{
		FIL file;
		fr = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);

		if (fr == FR_OK)
		{
			result = z80::SaveZ80Snapshot(&file, _buffer16K_1, _buffer16K_2);
			f_close(&file);
		}

		unmount();
	}

	return result;
}

static int fileCompare(const void* a, const void* b)
{
	TCHAR* file1 = (TCHAR*)_buffer16K_1;
	for (int i = 0; i <= _MAX_LFN; i++){
		file1[i] = tolower(((TCHAR*)a)[i]);
	}

	TCHAR* file2 = (TCHAR*)&_buffer16K_1[_MAX_LFN + 2];
	for (int i = 0; i <= _MAX_LFN; i++){
		file2[i] = tolower(((TCHAR*)b)[i]);
	}

	return strncmp(file1, file2, _MAX_LFN + 1);
}

bool saveSnapshotSetup()
{
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

	showTitle("Save snapshot. ENTER, ESC, BS");

	FRESULT fr = mount();
	if (fr != FR_OK)
	{
		return false;
	}

	// Unmount file system
	unmount();

	DebugScreen.PrintAt(0, 2, "Enter file name:");
	DebugScreen.SetCursorPosition(0, 3);
	DebugScreen.ShowCursor();
	memset(_snapshotName, 0, _MAX_LFN + 1);
	_savingSnapshot = true;

	return true;
}

bool saveSnapshotLoop()
{
	if (!_savingSnapshot)
	{
		return false;
	}

	int8_t scanCode = GetScanCode(true);
	if (scanCode == 0)
	{
		return true;
	}

	uint8_t x = DebugScreen._cursor_x;
	switch (scanCode)
	{
	case KEY_BACKSPACE:
		if (DebugScreen._cursor_x > 0)
		{
			DebugScreen.PrintAt(DebugScreen._cursor_x - 1, DebugScreen._cursor_y, " ");
			DebugScreen.SetCursorPosition(DebugScreen._cursor_x - 1, DebugScreen._cursor_y);
			_snapshotName[DebugScreen._cursor_x] = '\0';
		}
		break;

	case KEY_ENTER:
	case KEY_KEYPAD_ENTER:
		DebugScreen.HideCursor();
		DebugScreen.PrintAt(0, 5, "Saving...                  ");
		strcat(_snapshotName,".z80");
		if (saveSnapshot(_snapshotName))
		{
			_savingSnapshot = false;
			restoreState(false);
			return false;
		}
		else
		{
			DebugScreen.SetAttribute(0x0310); // red on blue
			DebugScreen.PrintAt(0, 5, "Error saving file");
			DebugScreen.SetAttribute(0x3F10); // white on blue
			DebugScreen.SetCursorPosition(x, 3);
			DebugScreen.ShowCursor();
		}
		break;

	case KEY_ESCAPE:
		_savingSnapshot = false;
		restoreState(false);
		return false;

	default:
		char character = GetAsciiCode();
		if (DebugScreen._cursor_x < FILE_COLUMNWIDTH && character != '\0'
			&& character != '\\' && character != '/' && character != ':'
			&& character != '*' && character != '?' && character != '"'
			&& character != '<' && character != '>' && character != '|')
		{
			char* text = (char*)_buffer16K_1;
			text[0] = character;
			_snapshotName[DebugScreen._cursor_x] = character;
			text[1] = '\0';
			DebugScreen.Print(text);
		}
		break;
	}

	return true;
}

bool loadSnapshotSetup()
{
	saveState();

	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	MainScreen.WriteBorderColor(0x01);

	showTitle("Load snapshot. ENTER, ESC, \x18, \x19, \x1A, \x1B"); // ↑, ↓, →, ←

	FRESULT fr = mount();
	if (fr != FR_OK)
	{
		return false;
	}

	DIR folder;
	FILINFO fileInfo;
	uint8_t maxFileCount = (DEBUG_ROWS - 1) * FILE_COLUMNS;
	_fileCount = 0;
	bool result = true;

	fr = f_findfirst(&folder, &fileInfo, (const TCHAR*) "/",
			(const TCHAR*) "*.z80");

	if (fr == FR_OK)
	{
		for (int fileIndex = 0; fileIndex < maxFileCount && fileInfo.fname[0];
				fileIndex++)
		{
			strncpy(_fileNames[fileIndex], fileInfo.fname, _MAX_LFN + 1);
			_fileCount++;

			fr = f_findnext(&folder, &fileInfo);
			if (fr != FR_OK)
			{
				result = false;
				break;
			}
		}
	}
	else
	{
		result = false;
	}

	// Sort files alphabetically
	if (_fileCount > 0)
	{
		qsort(_fileNames, _fileCount, _MAX_LFN + 1, fileCompare);
	}

	for (int y = 1; y < DEBUG_ROWS; y++)
	{
		DebugScreen.PrintAt(FILE_COLUMNWIDTH, y, "\xB3"); // │
		DebugScreen.PrintAt(FILE_COLUMNWIDTH * 2 + 1, y, "\xB3"); // │
	}

	uint8_t x, y;
	for (int fileIndex = 0; fileIndex < _fileCount; fileIndex++)
	{
		GetFileCoord(fileIndex, &x, &y);
		DebugScreen.PrintAt(x + 1, y, TruncateFileName(_fileNames[fileIndex]));
	}

	SetSelection(_selectedFile);

	// Unmount file system
	unmount();

	if (result)
	{
		_loadingSnapshot = true;
	}

	return result;
}

bool loadSnapshotLoop()
{
	if (!_loadingSnapshot)
	{
		return false;
	}

	int8_t scanCode = GetScanCode(true);
	if (scanCode == 0)
	{
		return true;
	}

	uint8_t previousSelection = _selectedFile;

	switch (scanCode)
	{
	case KEY_UPARROW:
		if (_selectedFile > 0)
		{
			_selectedFile--;
		}
		break;

	case KEY_DOWNARROW:
		if (_selectedFile < _fileCount - 1)
		{
			_selectedFile++;
		}
		break;

	case KEY_LEFTARROW:
		if (_selectedFile >= DEBUG_ROWS - 1)
		{
			_selectedFile -= DEBUG_ROWS - 1;
		}
		break;

	case KEY_RIGHTARROW:
		if (_selectedFile + DEBUG_ROWS <= _fileCount)
		{
			_selectedFile += DEBUG_ROWS - 1;
		}
		break;

	case KEY_ENTER:
	case KEY_KEYPAD_ENTER:
		loadSnapshot(_fileNames[_selectedFile]);
		_loadingSnapshot = false;
		restoreState(false);
		return false;

	case KEY_ESCAPE:
		_loadingSnapshot = false;
		restoreState(true);
		return false;
	}

	if (previousSelection == _selectedFile)
	{
		return true;
	}

	uint8_t x, y;
	GetFileCoord(previousSelection, &x, &y);
	DebugScreen.PrintAt(x, y, " ");

	SetSelection(_selectedFile);
	return true;
}
