#ifndef __ZXMAIN_INCLUDED__
#define __ZXMAIN_INCLUDED__

#include "z80user.h"
#include "main_ROM.h"
#include "videoRam.h"
#include "SpectrumScreen.h"
//#include "Sound/ay3-8912-state.h"

using namespace Display;

extern z80::VideoRam videoRam;
//extern Sound::Ay3_8912_state _ay3_8912;
extern Z80_STATE _zxCpu;
extern uint8_t RamBuffer[];

void zx_setup();
int32_t zx_loop();
void zx_reset();

#endif
