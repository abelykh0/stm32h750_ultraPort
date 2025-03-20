#include "videoRam.h"
#include <string.h>

using namespace z80;

VideoRam::VideoRam()
{
}

void VideoRam::ShowScreenshot(const uint8_t* screenshot)
{
	memcpy(this->_pixels, screenshot, sizeof(this->_pixels));

	uint8_t* attributes = (uint8_t*)screenshot + 0x1800;
	for (uint16_t address = ATTRIBUTES_START; address < 0x5b00; address++)
	{
		this->WriteByte(address, *attributes);
		attributes++;
	}
}

uint8_t VideoRam::ReadByte(uint16_t address)
{
    if (address >= (uint16_t)0x5b00)
    {
    	// outside of video RAM
    	return 0;
    }
    else if (address >= ATTRIBUTES_START)
    {
        // Screen attributes
        return this->_attributes[address - ATTRIBUTES_START];
    }
    else if (address >= PIXELS_START)
    {
        // Screen pixels
        return this->_pixels[address - PIXELS_START];
    }

	// outside of video RAM
	return 0;
}

uint16_t VideoRam::ReadWord(uint16_t address)
{
    return ((this->ReadByte(address + 1) << 8) | this->ReadByte(address));
}

void VideoRam::WriteByte(uint16_t address, uint8_t value)
{
    if (address >= (uint16_t)0x5b00)
    {
    	// outside of video RAM
    	return;
    }
    else if (address >= ATTRIBUTES_START)
    {
        // Screen attributes
        this->_attributes[address - ATTRIBUTES_START] = value;

        // Update 8x8 pixels square
        this->_screen.Update(this, address);
    }
    else if (address >= PIXELS_START)
    {
        // Screen pixels
        this->_pixels[address - PIXELS_START] = value;

        // update 8 pixels
        this->_screen.Update(this, address);
    }

	// outside of video RAM
}

void VideoRam::WriteWord(uint16_t address, uint16_t value)
{
	this->WriteByte(address, (uint8_t)value);
	this->WriteByte(address + 1, (uint8_t)(value >> 8));
}
