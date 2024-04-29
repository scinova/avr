#ifndef _gfx_h_
#define _gfx_h_
#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__ ((__packed__)) {
	const uint8_t height;
	const uint8_t offset;
	const uint8_t num_chars;
	uint8_t * widths;
	uint8_t * data;
} font_t;

typedef enum {
	AlignRight,
	AlignLeft,
	AlignCenter
} align_t;

#ifdef __cplusplus
extern "C" {
#endif
void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, bool rtl);
uint16_t text_width(uint8_t * text, font_t * font);
void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, align_t align, bool rtl);
#ifdef __cplusplus
}
#endif
#endif
