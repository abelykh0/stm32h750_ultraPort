#ifndef __Z80SNAPSHOT_INCLUDED__
#define __Z80SNAPSHOT_INCLUDED__

#include <stdint.h>
//#include "fatfs.h"
#include "fx_api.h"

namespace z80
{

bool LoadZ80Snapshot(FX_FILE* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000]);
bool LoadScreenFromZ80Snapshot(FX_FILE* file, uint8_t buffer1[0x4000]);
bool LoadScreenshot(FX_FILE* file, uint8_t buffer1[0x4000]);
bool SaveZ80Snapshot(FX_FILE* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000]);

}

#endif
