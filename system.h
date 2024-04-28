#ifndef _system_h
#define _system_h
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint64_t system_us();
uint32_t system_ms();
void delay_ms(uint32_t ms);
void setup();
void loop();
#ifdef __cplusplus
}
#endif
#endif
