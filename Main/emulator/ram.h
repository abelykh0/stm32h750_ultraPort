#ifndef _RAM_H
#define _RAM_H

namespace z80
{

class Ram
{
public:
	virtual uint8_t ReadByte(uint16_t address) = 0;
	virtual uint16_t ReadWord(uint16_t address) = 0;
	virtual void WriteByte(uint16_t address, uint8_t value) = 0;
	virtual void WriteWord(uint16_t address, uint16_t value) = 0;
};

}

#endif
