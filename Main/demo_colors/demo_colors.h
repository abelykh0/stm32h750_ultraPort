#ifndef _DEMO_COLORS_H_
#define _DEMO_COLORS_H_

#include <stdint.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#ifdef __cplusplus
extern "C" {
#endif

void init_demo_colors();

#ifdef __cplusplus
}
#endif

#endif
