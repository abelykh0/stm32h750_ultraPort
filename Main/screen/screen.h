#ifndef _SCREEN_H
#define _SCREEN_H

#include "stdint.h"
#include "config.h"
#include "resources.h"

extern uint8_t VideoRam[H_SIZE * V_SIZE];
extern void LtdcInit();

namespace Display
{

class Screen
{
private:
    void PrintChar(char c, uint16_t color);
    void PrintCharAt(uint8_t x, uint8_t y, unsigned char c, uint16_t color);
    void CursorNext();
    void InvertColor();

protected:
    void DrawChar(const uint8_t *f, uint16_t x, uint16_t y, uint8_t c);
    void SetPixelNoOffset(uint16_t x, uint16_t y, uint8_t c);

    uint16_t _xOffset;
    uint16_t _yOffset;
	uint16_t _width;
	uint16_t _height;
	uint16_t _textRows;
	uint16_t _textColumns;

    uint8_t* _font = (uint8_t*)font8x8;
    uint16_t _attribute = 0x2A10; // white on blue
    bool _isCursorVisible = false;

public:
	Screen();
	Screen(uint16_t xOffset, uint16_t yOffset, uint16_t width, uint16_t height);
    uint8_t _cursor_x = 0;
    uint8_t _cursor_y = 0;

	void Clear();
	void SetFont(const uint8_t* font);
	void SetAttribute(uint16_t attribute);
	void SetCursorPosition(uint8_t x, uint8_t y);
	void ShowCursor();
	void HideCursor();

	void SetPixel(uint16_t x, uint16_t y, uint8_t c);
	uint8_t GetPixel(uint16_t x, uint16_t y);
	void Print(const char* str);
	void PrintAt(uint8_t x, uint8_t y, const char* str);
	void PrintAlignRight(uint8_t y, const char *str);
	void PrintAlignCenter(uint8_t y, const char *str);
    void PrintCharAt(uint8_t x, uint8_t y, unsigned char c);

	virtual ~Screen() = default;
};

}

#endif
