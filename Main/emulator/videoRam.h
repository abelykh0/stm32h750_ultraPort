#ifndef _VIDEORAM_H
#define _VIDEORAM_H

#include "SpectrumScreen.h"
#include "ram.h"

#define PIXELS_START ((uint16_t)0x4000)
#define ATTRIBUTES_START ((uint16_t)0x5800)

namespace z80
{

class VideoRam: public z80::Ram
{
private:
	// 0x4000..0x57ff (6,912 bytes)
	uint8_t _pixels[6912];

	// 0x5800..0x5aff (768 bytes)
	uint8_t _attributes[768];

	SpectrumScreen _screen;

public:

	VideoRam();

	void ShowScreenshot(const uint8_t* screenshot);

	virtual uint8_t ReadByte(uint16_t address) override;
	virtual uint16_t ReadWord(uint16_t address) override;
	virtual void WriteByte(uint16_t address, uint8_t value) override;
	virtual void WriteWord(uint16_t address, uint16_t value) override;
};

}

#endif
