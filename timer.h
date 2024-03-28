#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void timer_enable();
void timer_disable();
uint64_t timer_us();
uint32_t timer_ms();
void delay_ms();

void enable_pwm_a();
void enable_pwm_b();
void output_pwm_a(uint8_t value);
void output_pwm_b(uint8_t value);
void disable_pwm_a();
void disable_pwm_b();
#ifdef __cplusplus
}
#endif
