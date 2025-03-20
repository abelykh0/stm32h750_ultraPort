#ifndef _SPECTRUMSCREEN_H
#define _SPECTRUMSCREEN_H

#include "screen.h"

// 256x192 pixels
#define WIDTH 256
#define HEIGHT 192

// 32x24 attributes
#define ATTR_WIDTH 32
#define ATTR_HEIGHT 24

namespace z80
{

class VideoRam;

class SpectrumScreen: public Display::Screen
{
private:
	void Update8Pixels(VideoRam* videoRam, uint16_t address);
	void Update64Pixels(VideoRam* videoRam, uint16_t address);
	uint16_t ConvertSpectrumLine(uint16_t spectrum_line);
	uint16_t FromSpectrumColor(uint8_t sinclairColor);
	uint16_t GetPixelsOffset(uint16_t line, uint8_t character);

public:
	SpectrumScreen();

	void Update(VideoRam* videoRam, uint16_t address);

	// needs to be called every 32 frames
	//void Flash();
};

}

#endif
