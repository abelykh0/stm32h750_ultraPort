#include "SpectrumScreen.h"
#include "VideoRam.h"

namespace z80
{

SpectrumScreen::SpectrumScreen()
	: Screen(32, 32, WIDTH, HEIGHT)
{
}

// ATTR_WIDTH bytes in a line, HEIGHT lines
void SpectrumScreen::Update8Pixels(VideoRam* videoRam, uint16_t address)
{
	uint16_t offset = address - PIXELS_START;
	uint16_t spectrumLine = offset / ATTR_WIDTH;
	uint8_t character = offset % ATTR_WIDTH;

	uint16_t y = this->ConvertSpectrumLine(spectrumLine);
	uint8_t sinclairAttribute = videoRam->ReadByte(ATTRIBUTES_START + (y / 8 * ATTR_WIDTH) + character);
	uint16_t attribute = this->FromSpectrumColor(sinclairAttribute);
	uint8_t foregroundColor = (attribute >> 8) & 0x03F;
	uint8_t backgroundColor = attribute & 0x03F;
	uint8_t line = videoRam->ReadByte(address);

	for (int i = 0; i < 8; i++)
	{
    	uint8_t color;
        if ((line << i) & 0x80)
        {
        	color = foregroundColor;
        }
        else
        {
        	color = backgroundColor;
        }

		this->SetPixel(character * 8 + i, y, color);
	}
}

void SpectrumScreen::Update64Pixels(VideoRam* videoRam, uint16_t address)
{
	uint16_t attributeOffset = address - ATTRIBUTES_START;
	uint16_t line = (attributeOffset / ATTR_WIDTH) * 8;
	uint8_t character = attributeOffset % ATTR_WIDTH;
	uint16_t pixelsAddress = this->GetPixelsOffset(line, character) + PIXELS_START;

	for (uint8_t i = 0; i < 8; i++)
	{
		this->Update8Pixels(videoRam, pixelsAddress);
		pixelsAddress += 0x0100;
	}
}

void SpectrumScreen::Update(VideoRam* videoRam, uint16_t address)
{
    if (address >= (uint16_t)0x5b00)
    {
    	// outside of video RAM
    	return;
    }
    else if (address >= ATTRIBUTES_START)
    {
        // update 8x8 pixels square
    	this->Update64Pixels(videoRam, address);
    }
    else if (address >= PIXELS_START)
    {
        // update 8 pixels
        this->Update8Pixels(videoRam, address);
    }

	// outside of video RAM
}

// Convert ZX Spectrum video RAM line to real screen line
uint16_t SpectrumScreen::ConvertSpectrumLine(uint16_t spectrum_line)
{
	// ZX Sinclair addressing
	// Y7-Y6-Y2-Y1-Y0 Y5-Y4-Y3
	//  7  6  5  4  3  2  1  0

	uint16_t y012 = (spectrum_line & 0b00111000) >> 3;
	uint16_t y345 = (spectrum_line & 0b00000111) << 3;
	uint16_t y67 = spectrum_line & 0b11000000;

    return y012 | y345 | y67;
}

uint16_t SpectrumScreen::GetPixelsOffset(uint16_t line, uint8_t character)
{
	// ZX Sinclair addressing
	// 00-00-00-Y7-Y6-Y2-Y1-Y0 Y5-Y4-Y3-x4-x3-x2-x1-x0
	//          12 11 10  9  8  7  6  5  4  3  2  1  0

	uint32_t y012 = ((line & 0B00000111) << 8);
	uint32_t y345 = ((line & 0B00111000) << 2);
	uint32_t y67 = ((line & 0B11000000) << 5);
	character &= 0B00011111;

	return (y012 | y345 | y67) + character;
}

uint16_t SpectrumScreen::FromSpectrumColor(uint8_t sinclairColor)
{
	// Sinclair: Flash-Bright-PaperG-PaperR-PaperB-InkG-InkR-InkB
	//               7      6      5      4      3    2    1    0
	// Our colors: 00-PaperB01-PaperG01-PaperR01 : 00-InkB01-InkG01-InkR01
	//                      54       32       10 :        54     32     10

	bool bright = ((sinclairColor & 0B01000000) != 0);

	uint16_t ink = ((sinclairColor & 0B00000100) << 8); // InkG
	ink |= ((sinclairColor & 0B00000010) << 7);         // InkR
	ink |= ((sinclairColor & 0B00000001) << 12);        // InkB
	if (bright)
	{
		ink |= (ink << 1);
	}

	uint16_t paper = ((sinclairColor & 0B00100000) >> 3); // PaperG
	paper |= ((sinclairColor & 0B00010000) >> 4);         // PaperR
	paper |= ((sinclairColor & 0B00001000) << 1);         // PaperB
	if (bright)
	{
		paper |= (paper << 1);
	}

	uint16_t result = ink | paper;

	if (bright)
	{
		// This is only needed to correctly read back "bright black" color
		result |= 0x4000;
	}

	if ((sinclairColor & 0B10000000) != 0)
	{
		// Blink
		result |= 0x8000;
	}

	return result;
}

}
