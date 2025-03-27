#ifndef __STARTUP_H__
#define __STARTUP_H__

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

void initialize();
void setup();
void loop();
bool onHardFault();

#ifdef __cplusplus
}
#endif

#endif /* __STARTUP_H__ */
