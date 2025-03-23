#include <screen/screen.h>
#include <string.h>

uint8_t VideoRam[H_SIZE * V_SIZE];// __attribute__(( section(".sram2") ));

namespace Display
{

Screen::Screen() : Screen(0, 0, H_SIZE, V_SIZE)
{
}

Screen::Screen(uint16_t xOffset, uint16_t yOffset, uint16_t width, uint16_t height)
{
	this->_xOffset = xOffset;
	this->_yOffset = yOffset;
	this->_width = width;
	this->_height = height;
	this->_textColumns = this->_width / 8;
	this->_textRows = this->_height / 8;
}

void Screen::Clear()
{
	// background color
	uint8_t color = this->_attribute & 0x03F;

    for (int x = this->_xOffset; x < this->_xOffset + this->_width; x++)
    {
        for (int y = this->_yOffset; y < this->_yOffset + this->_height; y++)
        {
            VideoRam[x + H_SIZE * y] = color;
        }
    }
}

void Screen::SetFont(const uint8_t* font)
{
	this->_font = (uint8_t*)font;
}

void Screen::SetAttribute(uint16_t attribute)
{
	this->_attribute = attribute;
}

void Screen::SetCursorPosition(uint8_t x, uint8_t y)
{
	if (this->_cursor_x == x && this->_cursor_y == y)
	{
		return;
	}

	if (x >= this->_textColumns)
	{
		x = this->_textColumns - 1;
	}

	if (y >= this->_textRows)
	{
		y = this->_textRows - 1;
	}

	this->_cursor_x = x;
	this->_cursor_y = y;
}

void Screen::Print(const char* str)
{
    if (this->_font == nullptr)
    {
        return;
    }

    while (*str)
    {
        this->PrintChar(*str++, this->_attribute);
    }
}

void Screen::PrintAt(uint8_t x, uint8_t y, const char* str)
{
    this->SetCursorPosition(x, y);
    this->Print(str);
}

void Screen::PrintAlignRight(uint8_t y, const char *str)
{
    uint8_t leftX = this->_textColumns - strlen(str);
    this->PrintAt(leftX, y, str);
}

void Screen::PrintAlignCenter(uint8_t y, const char *str)
{
    uint8_t leftX = (this->_textColumns - strlen(str)) / 2;
    this->PrintAt(leftX, y, str);
}

void Screen::PrintChar(char c, uint16_t color)
{
	switch (c)
	{
	case '\0': //null
		break;
	case '\n': //line feed
		if (this->_cursor_y < this->_textRows - 1)
		{
			this->SetCursorPosition(0, this->_cursor_y + 1);
		}
		break;
	case '\b': //backspace
		if (this->_cursor_x > 0)
		{
			this->PrintCharAt(this->_cursor_x - 1, this->_cursor_y, ' ', color);
			this->SetCursorPosition(this->_cursor_x - 1, this->_cursor_y);
		}
		break;
	case 13: //carriage return
		this->_cursor_x = 0;
		break;
	default:
	{
		uint8_t x = this->_cursor_x;
		uint8_t y = this->_cursor_y;
		this->CursorNext();
		this->PrintCharAt(x, y, c, color);
	}
		break;
	}
}

void Screen::DrawChar(const uint8_t* f, uint16_t x, uint16_t y, uint8_t c)
{
	uint8_t* character = (uint8_t*)f + (c * 8);

    for (int i = 0; i < 8; i++)
    {
    	uint8_t line = character[i];
        for (int j = 0; j < 8; j++)
        {
        	uint8_t color;
            if ((line << j) & 0x80)
            {
            	// foreground color
            	color = (this->_attribute >> 8) & 0x03F;
            }
            else
            {
            	// background color
            	color = this->_attribute & 0x03F;
            }

            this->SetPixel(x * 8 + j, y * 8 + i, color);
        }
    }
}

void Screen::PrintCharAt(uint8_t x, uint8_t y, unsigned char c, uint16_t color)
{
	this->_attribute = color;
	this->DrawChar(this->_font, x, y, c);
}

void Screen::PrintCharAt(uint8_t x, uint8_t y, unsigned char c)
{
	this->PrintCharAt(x, y, c, this->_attribute);
}

void Screen::CursorNext()
{
	uint8_t x = this->_cursor_x;
	uint8_t y = this->_cursor_y;
	if (x < this->_textColumns - 1)
	{
		x++;
	}
	else
	{
		if (y < this->_textRows - 1)
		{
			x = 0;
			y++;
		}
	}
	this->SetCursorPosition(x, y);
}

void Screen::SetPixel(uint16_t x, uint16_t y, uint8_t c)
{
	uint32_t offset = (H_SIZE * (this->_yOffset + y)) + (this->_xOffset + x);
	if (offset >= H_SIZE * V_SIZE)
	{
		// out of screen area
		return;
	}

	VideoRam[offset] = c;
}

}
