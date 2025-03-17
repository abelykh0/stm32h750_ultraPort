#include <screen/screen.h>
#include <string.h>

namespace Display
{

Screen::Screen()
{
}

void Screen::Clear()
{
	memset(VideoRam, this->_attribute, H_SIZE * V_SIZE);
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

	if (x >= TEXT_COLUMNS)
	{
		x = TEXT_COLUMNS - 1;
	}

	if (y >= TEXT_ROWS)
	{
		y = TEXT_ROWS - 1;
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
    uint8_t leftX = TEXT_COLUMNS - strlen(str);
    this->PrintAt(leftX, y, str);
}

void Screen::PrintAlignCenter(uint8_t y, const char *str)
{
    uint8_t leftX = (TEXT_COLUMNS - strlen(str)) / 2;
    this->PrintAt(leftX, y, str);
}

void Screen::PrintChar(char c, uint16_t color)
{
	switch (c)
	{
	case '\0': //null
		break;
	case '\n': //line feed
		if (this->_cursor_y < TEXT_ROWS - 1)
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
	if (c == 0)
	{
		c = 1;
	}
	uint8_t* character = (uint8_t*)f + (c * 8) - 8;

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

            VideoRam[x * 8 + j + H_SIZE * (y * 8 + i)] = color;
            //SetPixel(x * 8 + j, y * 8 + i, color);
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
	if (x < TEXT_COLUMNS - 1)
	{
		x++;
	}
	else
	{
		if (y < TEXT_ROWS - 1)
		{
			x = 0;
			y++;
		}
	}
	this->SetCursorPosition(x, y);
}

}
