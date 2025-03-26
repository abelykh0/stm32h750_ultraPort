#ifndef _SPECTRUMSCREEN_H
#define _SPECTRUMSCREEN_H

#include "screen.h"

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
	uint8_t _borderColor;
	bool _flashOn;

public:
	SpectrumScreen();

	void Update(VideoRam* videoRam, uint16_t address);

	uint8_t ReadBorderColor();
	void WriteBorderColor(uint8_t color);

	// must be called every 32 frames
	void Flash(VideoRam* videoRam);
};

}

#endif
