#include "z80snapshot.h"

#include <string.h>
#include <stdio.h>

#include "emulator.h"
#include "emulator/z80main.h"
#include "emulator/z80emu/z80emu.h"
#include "emulator/SpectrumScreen.h"

/*
 Offset  Length  Description
 ---------------------------
 0       1       A register
 1       1       F register
 2       2       BC register pair (LSB, i.e. C, first)
 4       2       HL register pair
 6       2       0 to signal a version 2 or 3
 8       2       Stack pointer
 10      1       Interrupt register
 11      1       Refresh register (Bit 7 is not significant!)
 12      1       Bit 0  : Bit 7 of the R-register
                 Bit 1-3: Border color
                 Bit 4-7: No meaning
                 If byte 12 is 255, it has to be regarded as being 1
 13      2       DE register pair
 15      2       BC' register pair
 17      2       DE' register pair
 19      2       HL' register pair
 21      1       A' register
 22      1       F' register
 23      2       IY register (Again LSB first)
 25      2       IX register
 27      1       Interrupt flipflop, 0=DI, otherwise EI
 28      1       IFF2
 29      1       Bit 0-1: Interrupt mode (0, 1 or 2)
                 Bit 2  : 1=Issue 2 emulation
                 Bit 3  : 1=Double interrupt frequency
                 Bit 4-5: unused
                 Bit 6-7: 0=Cursor/Protek/AGF joystick
                 1=Kempston joystick
                 2=Sinclair 2 Left joystick (or user
                 defined, for version 3 .z80 files)
                 3=Sinclair 2 Right joystick
 30 0x1E 2       Length of additional header block (see below)
                 23 for version 2
                 54 or 55 for version 3
 ===========================
 32 0x20 2       Program counter
 34 0x22 1       Hardware mode, 0 for ZX Spectrum 48K
 35 0x23 1       If in 128 mode, contains last OUT to 0x7ffd
 ...

 Hereafter a number of memory blocks follow, each containing the compressed data of a 16K block.
 The compression method is very simple: it replaces repetitions of at least five equal bytes by
 a four-byte code ED ED xx yy, which stands for "byte yy repeated xx times".
 Only sequences of length at least 5 are coded. The exception is sequences consisting of ED's;
 if they are encountered, even two ED's are encoded into ED ED 02 ED.
 Finally, every byte directly following a single ED is not taken into a block, for example ED 6*00
 is not encoded into ED ED ED 06 00 but into ED 00 ED ED 05 00.

 The structure of a memory block is:
 Byte    Length  Description
 ---------------------------
 0       2       Length of compressed data (without this 3-byte header)
                 If length=0xffff, data is 16384 bytes long and not compressed
 2       1       Page number, for ZX Spectrum 48K:
                 8: 4000-7fff
                 4: 8000-bfff
                 5: c000-ffff
 3       [0]     Data

 */

struct FileHeader
{
	uint8_t A;
	uint8_t F;
	uint16_t BC;
	uint16_t HL;
	uint16_t Version;
	uint16_t SP;
	uint8_t InterruptRegister;
	uint8_t RefreshRegister;
	uint8_t Flags1;
	uint16_t DE;
	uint16_t BC_Dash;
	uint16_t DE_Dash;
	uint16_t HL_Dash;
	uint8_t A_Dash;
	uint8_t F_Dash;
	uint16_t IY;
	uint16_t IX;
	uint8_t InterruptFlipFlop;
	uint8_t IFF2;
	uint8_t Flags2;
	uint16_t AdditionalBlockLength;
	uint16_t PC;
	uint8_t HardwareMode;
	uint8_t PagingState;
}__attribute__((packed));

void DecompressPage(uint8_t *page, uint16_t pageLength, bool isCompressed,
		uint16_t maxSize, uint8_t* destMemory);
uint16_t CompressPage(uint8_t* page, uint8_t* destMemory);
void ReadState(FileHeader* header);
void SaveState(FileHeader* header);
void GetPageInfo(uint8_t* buffer, bool is128Mode, uint8_t pagingState, uint8_t* pageNumber, uint16_t* pageSize);

bool z80::SaveZ80Snapshot(FIL* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000])
{
	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*)buffer1;
	SaveState(header);

	memset(&buffer1[sizeof(FileHeader)], 0, header->AdditionalBlockLength - 4);

	FRESULT writeResult;
	UINT bytesWritten;
	UINT bytesToWrite = sizeof(FileHeader) + header->AdditionalBlockLength - 4;
	writeResult = f_write(file, buffer1, bytesToWrite, &bytesWritten);
	if (writeResult != FR_OK || bytesWritten != bytesToWrite)
	{
		return false;
	}

	uint8_t pages[] = { 4, 5, 8 };

	for (int i = 0; i < 3; i++)
	{
		uint8_t pageNumber = pages[i];
		uint8_t* buffer = nullptr;

		switch (pageNumber)
		{
		case 8:
			buffer = buffer2;

			// 0x4000..0x5AFF
			videoRam.SaveScreenData(buffer);

			// 0x5B00..0x7FFF
			memcpy(&buffer[0x1B00], RamBuffer, 0x2500);

			break;
		case 4:
			buffer = &RamBuffer[0x8000 - 0x5B00];
			break;
		case 5:
			buffer = &RamBuffer[0xC000 - 0x5B00];
			break;
		}

		uint16_t pageSize = CompressPage(buffer, buffer1);

		buffer = (uint8_t*)buffer2;
		if (pageSize == 0x4000)
		{
			*buffer = 0xFF;
			buffer++;
			*buffer = 0xFF;
		}
		else
		{
			*buffer = pageSize & 0xFF;
			buffer++;
			*buffer = (pageSize & 0xFF00) >> 8;
		}
		buffer++;
		*buffer = pageNumber;

		writeResult = f_write(file, buffer2, 3, &bytesWritten);
		if (writeResult != FR_OK || bytesWritten != 3)
		{
			return false;
		}

		buffer = buffer1;

		int remainingBytesInPage = pageSize;
		do
		{
			bytesToWrite = remainingBytesInPage < _MIN_SS ? remainingBytesInPage : _MIN_SS;
			writeResult = f_write(file, buffer, bytesToWrite, &bytesWritten);
			if (writeResult != FR_OK || bytesWritten != bytesToWrite)
			{
				return false;
			}

			remainingBytesInPage -= bytesWritten;
			buffer += bytesWritten;
		} while (writeResult == FR_OK && remainingBytesInPage > 0);
	}

	return true;
}

bool z80::LoadZ80Snapshot(FIL* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000])
{
	UINT bytesRead;
	UINT bytesToRead;

	bytesToRead = sizeof(FileHeader);
	FRESULT readResult = f_read(file, buffer1, bytesToRead, &bytesRead);
	if (readResult != FR_OK || bytesRead != bytesToRead)
	{
		return false;
	}

	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*) buffer1;
	ReadState(header);

	bool is128Mode;
	if (header->AdditionalBlockLength < 54)
	{
		// version 2
		is128Mode = (header->HardwareMode >= 3);
	}
	else
	{
		// version 3
		is128Mode = (header->HardwareMode >= 4);
	}

	uint8_t pagingState = header->PagingState;

	bytesToRead = header->AdditionalBlockLength - 4 + 3;
	readResult = f_read(file, buffer1, bytesToRead, &bytesRead);
	if (readResult != FR_OK || bytesRead != bytesToRead)
	{
		return false;
	}

	// Get pageSize and pageNumber
	uint16_t pageSize;
	uint8_t pageNumber;
	GetPageInfo(&buffer1[bytesToRead - 3], is128Mode, pagingState, &pageNumber, &pageSize);

	do
	{
		bool isCompressed = (pageSize != 0xFFFF);
		if (!isCompressed)
		{
			pageSize = 0x4000;
		}

		uint8_t* memory;
		switch (pageNumber)
		{
		case 8:
			memory = buffer2;
			break;
		case 4:
			memory = &RamBuffer[0x8000 - 0x5B00];
			break;
		case 5:
			memory = &RamBuffer[0xC000 - 0x5B00];
			break;
		default:
			memory = nullptr;
			break;
		}

		if (memory != nullptr)
		{
			// Read page into tempBuffer
			uint8_t* buffer = buffer1;
			int remainingBytesInPage = pageSize;
			do
			{
				bytesToRead = remainingBytesInPage < _MIN_SS ? remainingBytesInPage : _MIN_SS;
				readResult = f_read(file, buffer, bytesToRead, &bytesRead);
				if (readResult != FR_OK || bytesRead != bytesToRead)
				{
					return false;
				}

				remainingBytesInPage -= bytesRead;
				buffer += bytesRead;
			} while (readResult == FR_OK && remainingBytesInPage > 0);

			DecompressPage(buffer1, pageSize, isCompressed, 0, memory);

			if (pageNumber == 8)
			{
				// 0x4000..0x5AFF
				videoRam.ShowScreenshot(memory);

				// 0x5B00..0x7FFF
				memcpy(RamBuffer, &memory[0x1B00], 0x2500);
			}
		}
		else
		{
			// Move forward without reading
			readResult = f_lseek(file, f_tell(file) + pageSize);
			if (readResult != FR_OK)
			{
				return false;
			}
		}

		readResult = f_read(file, buffer1, 3, &bytesRead);
		if (readResult != FR_OK)
		{
			return false;
		}

		if (bytesRead == 3)
		{
			GetPageInfo(buffer1, is128Mode, pagingState, &pageNumber, &pageSize);
		}
		else
		{
			pageSize = 0;
		}

	} while (pageSize > 0);

	return true;
}

bool z80::LoadScreenFromZ80Snapshot(FIL* file, uint8_t buffer1[0x4000])
{
	UINT bytesRead;

	FRESULT readResult = f_read(file, buffer1, sizeof(FileHeader), &bytesRead);
	if (readResult != FR_OK || bytesRead != sizeof(FileHeader))
	{
		return false;
	}

	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*) buffer1;

	bool is128Mode;
	if (header->AdditionalBlockLength < 54)
	{
		// version 2
		is128Mode = (header->HardwareMode >= 3);
	}
	else
	{
		// version 3
		is128Mode = (header->HardwareMode >= 4);
	}

	uint8_t pagingState = header->PagingState;

	UINT bytesToRead = header->AdditionalBlockLength - 4 + 3;
	readResult = f_read(file, buffer1, bytesToRead, &bytesRead);
	if (readResult != FR_OK || bytesRead != bytesToRead)
	{
		return false;
	}

	// Get pageSize and pageNumber
	uint16_t pageSize;
	uint8_t pageNumber;
	GetPageInfo(&buffer1[bytesToRead - 3], is128Mode, pagingState, &pageNumber, &pageSize);

	do
	{
		bool isCompressed = (pageSize != 0xFFFF);
		if (!isCompressed)
		{
			pageSize = 0x4000;
		}

		if (pageNumber == 8)
		{
			// This page contains screenshoot

			// Read page into buffer1
			uint8_t* buffer = buffer1;
			int remainingBytesInPage = pageSize;
			do
			{
				UINT bytesToRead =
						remainingBytesInPage < _MIN_SS ?
								remainingBytesInPage : _MIN_SS;
				readResult = f_read(file, buffer, bytesToRead, &bytesRead);
				if (readResult != FR_OK || bytesRead != bytesToRead)
				{
					return false;
				}

				remainingBytesInPage -= bytesRead;
				buffer += bytesRead;
			} while (readResult == FR_OK && remainingBytesInPage > 0);

			uint8_t* buffer2 = &buffer1[0x2000];
			if (pageSize > 6912)
			{
				pageSize = 6912;
			}
			DecompressPage(buffer1, pageSize, isCompressed, 6912, buffer2);
			videoRam.ShowScreenshot(buffer2);
			return true;
		}
		else
		{
			// Move forward without reading
			readResult = f_lseek(file, f_tell(file) + pageSize);
			if (readResult != FR_OK)
			{
				return false;
			}
		}

		readResult = f_read(file, buffer1, 3, &bytesRead);
		if (readResult != FR_OK)
		{
			return false;
		}

		if (bytesRead == 3)
		{
			GetPageInfo(buffer1, is128Mode, pagingState, &pageNumber, &pageSize);
		}
		else
		{
			pageSize = 0;
		}

	} while (pageSize > 0);

	return true;
}

bool z80::LoadScreenshot(FIL* file, uint8_t buffer1[0x4000])
{
	FRESULT readResult;
	UINT bytesRead;
	int remainingBytes = 6912;
	uint8_t* buffer = buffer1;

	do
	{
		UINT bytesToRead = remainingBytes < _MIN_SS ? remainingBytes : _MIN_SS;
		readResult = f_read(file, buffer, bytesToRead, &bytesRead);
		if (readResult != FR_OK || bytesRead != bytesToRead)
		{
			return false;
		}

		remainingBytes -= bytesRead;
		buffer += bytesRead;
	} while (remainingBytes > 0);

	videoRam.ShowScreenshot(buffer1);
	return true;
}

void DecompressPage(uint8_t *page, uint16_t pageLength, bool isCompressed,
		uint16_t maxSize, uint8_t* destMemory)
{
	uint16_t size = 0;
	uint8_t* memory = destMemory;
	for (int i = 0; i < pageLength; i++)
	{
		if (i < pageLength - 4)
		{
			if (page[i] == 0x00 && page[i + 1] == 0xED && page[i + 2] == 0xED
					&& page[i + 3] == 0x00)
			{
				break;
			}

			if (isCompressed && page[i] == 0xED && page[i + 1] == 0xED)
			{
				i += 2;
				int repeat = page[i++];
				uint8_t value = page[i];
				for (int j = 0; j < repeat; j++)
				{
					*memory = value;
					memory++;

					size++;
					if (maxSize > 0 && size >= maxSize)
					{
						return;
					}
				}

				continue;
			}
		}

		*memory = page[i];
		memory++;

		size++;
		if (maxSize > 0 && size >= maxSize)
		{
			return;
		}
	}
}

void ReadState(FileHeader* header)
{
	// If byte 12 is 255, it has to be regarded as being 1
	if (header->Flags1 == 255)
	{
		header->Flags1 = 1;
	}

	_zxCpu.registers.byte[Z80_A] = header->A;
	_zxCpu.registers.byte[Z80_F] = header->F;
	_zxCpu.registers.word[Z80_BC] = header->BC;
	_zxCpu.registers.word[Z80_HL] = header->HL;
	_zxCpu.registers.word[Z80_SP] = header->SP;
	_zxCpu.i = header->InterruptRegister;
	_zxCpu.r = (header->RefreshRegister & 0x7F)
			| ((header->Flags1 & 0x01) << 7);
	_zxCpu.im = header->Flags2 & 0x3;
	_zxCpu.registers.word[Z80_DE] = header->DE;
	_zxCpu.alternates[Z80_BC] = header->BC_Dash;
	_zxCpu.alternates[Z80_DE] = header->DE_Dash;
	_zxCpu.alternates[Z80_HL] = header->HL_Dash;
	_zxCpu.alternates[Z80_AF] = header->F_Dash | (header->A_Dash << 8);
	_zxCpu.registers.word[Z80_IY] = header->IY;
	_zxCpu.registers.word[Z80_IX] = header->IX;
	_zxCpu.iff1 = header->InterruptFlipFlop;
	_zxCpu.iff2 = header->IFF2;
	_zxCpu.pc = header->PC;

	uint8_t borderColor = (header->Flags1 & 0x0E) >> 1;
	MainScreen.WriteBorderColor(borderColor);
}

void SaveState(FileHeader* header)
{
	header->Version = 0;
	header->HardwareMode = 0;
	header->PagingState = 0;
	header->AdditionalBlockLength = 54;

	header->A = _zxCpu.registers.byte[Z80_A];
	header->F = _zxCpu.registers.byte[Z80_F];
	header->BC = _zxCpu.registers.word[Z80_BC];
	header->HL = _zxCpu.registers.word[Z80_HL];
	header->SP = _zxCpu.registers.word[Z80_SP];
	header->InterruptRegister = _zxCpu.i;
	header->RefreshRegister = _zxCpu.r;
	header->DE = _zxCpu.registers.word[Z80_DE];
	header->BC_Dash = _zxCpu.alternates[Z80_BC];
	header->DE_Dash = _zxCpu.alternates[Z80_DE];
	header->HL_Dash = _zxCpu.alternates[Z80_HL];
	header->F_Dash = _zxCpu.alternates[Z80_AF] & 0xFF;
	header->A_Dash = (_zxCpu.alternates[Z80_AF] & 0xFF00) >> 8;
	header->IY = _zxCpu.registers.word[Z80_IY];
	header->IX = _zxCpu.registers.word[Z80_IX];
	header->InterruptFlipFlop = _zxCpu.iff1;
	header->IFF2 = _zxCpu.iff2;
	header->PC = _zxCpu.pc;

	// Bit 0  : Bit 7 of the R-register
	// Bit 1-3: Border color
	header->Flags1 = (_zxCpu.r & 0x80) >> 7;
	uint8_t border = MainScreen.ReadBorderColor();
	header->Flags1 |= (border & 0x38) >> 2;

	// Bit 0-1: Interrupt mode (0, 1 or 2)
	header->Flags2 = _zxCpu.im & 0x03;
}

void GetPageInfo(uint8_t* buffer, bool is128Mode, uint8_t pagingState, uint8_t* pageNumber, uint16_t* pageSize)
{
	*pageSize = buffer[0];
	*pageSize |= buffer[1] << 8;
	*pageNumber = buffer[2];

	if (is128Mode)
	{
		switch (*pageNumber)
		{
		case 8:
			// 0x4000..0x7FFF
			break;
		case 4:
			// 0x8000..0xBFFF
			break;
		default:
			if (*pageNumber == (pagingState & 0x03) + 3)
			{
				*pageNumber = 5; // 0xC000..0xFFFF
			}
			else
			{
				// skip it
				*pageNumber = 0;
			}
			break;
		}
	}
}

uint8_t CountEqualBytes(uint8_t* address, uint8_t* maxAddress)
{
	int result;
	uint8_t byteValue = *address;

	for (result = 1; result < 255; result++)
	{
		address++;
		if (byteValue != *address
			|| address >= maxAddress)
		{
			break;
		}
	}

	return (uint8_t)result;
}

uint16_t CompressPage(uint8_t* page, uint8_t* destMemory)
{
	uint16_t size = 0;
	uint8_t* maxAddress = page + 0x4000;
	bool isPrevoiusSingleED = false;

	for (uint8_t* memory = page; memory < maxAddress; memory++)
	{
		uint8_t byteValue = *memory;
		uint8_t equalBytes;

		if (isPrevoiusSingleED)
		{
			// A byte directly following a single 0xED is not taken into a block
			equalBytes = 1;
		}
		else
		{
			equalBytes = CountEqualBytes(memory, maxAddress);
		}

		uint8_t minRepeats;
		if (byteValue == 0xED)
		{
			minRepeats = 2;
		}
		else
		{
			minRepeats = 5;
		}

		if (equalBytes >= minRepeats)
		{
			*destMemory = 0xED;
			destMemory++;
			*destMemory = 0xED;
			destMemory++;
			*destMemory = equalBytes;
			destMemory++;

			memory += equalBytes - 1;
			size += 3;
		}

		*destMemory = byteValue;
		destMemory++;
		size++;

		isPrevoiusSingleED = (byteValue == 0xED && memory < maxAddress && *(memory + 1) != 0xED);
	}

	return size;
}
