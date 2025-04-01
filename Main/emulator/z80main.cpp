#include <string.h>
#include <stdio.h>
#include "startup.h"

#include "stm32h7xx_hal.h"
#include "usb_host.h"

#include "z80main.h"
#include "z80input.h"
#include "resources.h"
#include "emulator.h"

//#define BEEPER
#define CYCLES_PER_STEP 69888
#define RAM_AVAILABLE 0xC000

//Sound::Ay3_8912_state _ay3_8912;
uint8_t RamBuffer[RAM_AVAILABLE];
Z80_STATE _zxCpu;

static CONTEXT _zxContext;
static uint16_t _attributeCount;
static int _total;
static int _next_total = 0;
static uint8_t zx_data = 0;
static uint8_t frames = 0;
static uint32_t _ticks = 0;

extern "C"
{
    uint8_t readbyte(uint16_t addr);
    uint16_t readword(uint16_t addr);
    void writebyte(uint16_t addr, uint8_t data);
    void writeword(uint16_t addr, uint16_t data);
    uint8_t input(uint8_t portLow, uint8_t portHigh);
    void output(uint8_t portLow, uint8_t portHigh, uint8_t data);
}

void zx_setup()
{
	_attributeCount = ATTR_WIDTH * ATTR_HEIGHT;

    _zxContext.readbyte = readbyte;
    _zxContext.readword = readword;
    _zxContext.writeword = writeword;
    _zxContext.writebyte = writebyte;
    _zxContext.input = input;
    _zxContext.output = output;

#ifdef BEEPER
    // Sound
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

    zx_reset();
}

void zx_reset()
{
    memset(indata, 0xFF, 128);
	MainScreen.WriteBorderColor(0x07);
    Z80Reset(&_zxCpu);
}

int32_t zx_loop()
{
    int32_t result = -1;

    _total += Z80Emulate(&_zxCpu, _next_total - _total, &_zxContext);

    if (_total >= _next_total)
    {
        _next_total += CYCLES_PER_STEP;

        // flash every 32 frames
        frames++;
        if (frames > 31)
        {
            frames = 0;
        	MainScreen.Flash(&videoRam);
        }

        Z80Interrupt(&_zxCpu, 0xff, &_zxContext);

        // delay
        uint32_t currentTicks = HAL_GetTick();
        if (_ticks < currentTicks)
        {
        	_ticks = currentTicks;
        }
        while (HAL_GetTick() < _ticks)
        {
        }

		_ticks += 20; // 50 frames per second
    }

    return result;
}

extern "C" uint8_t readbyte(uint16_t addr)
{
    uint8_t res;
    if (addr >= (uint16_t)0x5B00)
    {
        uint16_t offset = addr - (uint16_t)0x5B00;
        if (offset < RAM_AVAILABLE)
        {
            res = RamBuffer[offset];
        }
        else
        {
            res = 0;
        }
    }
    else if (addr >= (uint16_t)0x4000)
    {
        // Screen pixels
    	res = videoRam.ReadByte(addr);
    }
    else
    {
        res = ROM[addr];
    }
    return res;
}

extern "C" uint16_t readword(uint16_t addr)
{
    return ((readbyte(addr + 1) << 8) | readbyte(addr));
}

extern "C" void writebyte(uint16_t addr, uint8_t data)
{
    if (addr >= (uint16_t)0x5B00)
    {
        uint16_t offset = addr - (uint16_t)0x5B00;
        if (offset < RAM_AVAILABLE)
        {
            RamBuffer[offset] = data;
        }
    }
    else if (addr >= (uint16_t)0x4000)
    {
        // Screen pixels
    	videoRam.WriteByte(addr, data);
    }
}

extern "C" void writeword(uint16_t addr, uint16_t data)
{
    writebyte(addr, (uint8_t)data);
    writebyte(addr + 1, (uint8_t)(data >> 8));
}

extern "C" uint8_t input(uint8_t portLow, uint8_t portHigh)
{
    if (portLow == 0xFE)
    {
    	// Keyboard

        switch (portHigh)
        {
        case 0xFE:
        case 0xFD:
        case 0xFB:
        case 0xF7:
        case 0xEF:
        case 0xDF:
        case 0xBF:
        case 0x7F:
            return indata[portHigh - 0x7F];
        case 0x00:
			{
				uint8_t result = indata[0xFE - 0x7F];
				result &= indata[0xFD - 0x7F];
				result &= indata[0xFB - 0x7F];
				result &= indata[0xF7 - 0x7F];
				result &= indata[0xEF - 0x7F];
				result &= indata[0xDF - 0x7F];
				result &= indata[0xBF - 0x7F];
				result &= indata[0x7F - 0x7F];
				return result;
			}
        }
    }

    // Sound (AY-3-8912)
    if (portLow == 0xFD)
    {
        switch (portHigh)
        {
        case 0xFF:
        	//return _ay3_8912.getRegisterData();
        	return 0;
        }
    }

    uint8_t data = zx_data;
    data |= (0xe0); /* Set bits 5-7 - as reset above */
    data &= ~0x40;
    return data;
}

extern "C" void output(uint8_t portLow, uint8_t portHigh, uint8_t data)
{
    switch (portLow)
    {
    case 0xFE:
    {
        // border color (no bright colors)
        uint8_t borderColor = (data & 0x07);
    	if ((indata[0x20] & 0x07) != borderColor)
    	{
    		MainScreen.WriteBorderColor(borderColor);
    	}

#ifdef BEEPER
        uint8_t sound = (data & 0x10);
    	if ((indata[0x20] & 0x10) != sound)
    	{
			if (sound)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			}
    	}
#endif

        indata[0x20] = data;
    }
    break;

    case 0xF5:
    {
        // Sound (AY-3-8912)
        switch (portHigh)
        {
        case 0xC0:
        	//_ay3_8912.selectRegister(data);
        	break;
        }
    }
    break;

    case 0xFD:
    {
        // Sound (AY-3-8912)
        switch (portHigh)
        {
        case 0xFF:
        	// Not sure if this one is correct
        	//_ay3_8912.selectRegister(data);
        	break;
        case 0xBF:
        	//_ay3_8912.setRegisterData(data);
        	break;
        }
    }
    break;

    default:
        zx_data = data;
        break;
    }
}
