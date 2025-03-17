#ifndef _SCREEN_H
#define _SCREEN_H

#include "stdint.h"
#include "config.h"
#include "font8x8.h"

namespace Display
{

class Screen
{
private:
    void PrintChar(char c, uint16_t color);
    void PrintCharAt(uint8_t x, uint8_t y, unsigned char c, uint16_t color);
    void CursorNext();

protected:
    virtual void DrawChar(const uint8_t *f, uint16_t x, uint16_t y, uint8_t c);

    uint16_t _hResolution;
    uint16_t _hResolutionNoBorder;
    uint16_t _vResolution;
	uint16_t _startLine;
    uint16_t _horizontalBorder;
    uint8_t _verticalBorder;

    uint8_t* _font = (uint8_t*)font8x8;
    uint16_t _attribute = 0x3F01; // white on blue

public:
	Screen();
    uint8_t _cursor_x = 0;
    uint8_t _cursor_y = 0;
    uint16_t _pixelCount;
    uint16_t _attributeCount;

	void Clear();
	void SetFont(const uint8_t* font);
	void SetAttribute(uint16_t attribute);
	void SetCursorPosition(uint8_t x, uint8_t y);

	void Print(const char* str);
	void PrintAt(uint8_t x, uint8_t y, const char* str);
	void PrintAlignRight(uint8_t y, const char *str);
	void PrintAlignCenter(uint8_t y, const char *str);
    void PrintCharAt(uint8_t x, uint8_t y, unsigned char c);

	virtual ~Screen() = default;
};

}

#endif
