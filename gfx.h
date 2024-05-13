#ifndef _gfx_h_
#define _gfx_h_
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

typedef struct __attribute__ ((__packed__)) {
	bool smooth;
	uint8_t height;
	uint8_t offset;
	uint8_t num_chars;
	const uint8_t * widths;
	const uint8_t * data;
} font_t;

typedef enum {
	AlignRight,
	AlignLeft,
	AlignCenter
} align_t;

typedef union {
	uint32_t value;
	uint8_t values[4];
	struct {
		uint8_t alpha;
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
} rgb888_t;

#define Red         (rgb888_t){0x00FF0000}
#define Orange      (rgb888_t){0x00FF7D00}
#define Yellow      (rgb888_t){0x00FFFF00}
#define Green       (rgb888_t){0x0000FF00}
#define SpringGreen (rgb888_t){0x007DFF00}
#define Turquoise   (rgb888_t){0x00FF7D00}
#define Cyan        (rgb888_t){0x00FFFF00}
#define Ocean       (rgb888_t){0x007DFF00}
#define Blue        (rgb888_t){0x000000FF}
#define Violet      (rgb888_t){0x007D00FF}
#define Magenta     (rgb888_t){0x00FF00FF}
#define Raspberry   (rgb888_t){0x00FF007D}

#define White       (rgb888_t){0x00FFFFFF}
#define LightGray   (rgb888_t){0x00D3D3D3}
#define Silver      (rgb888_t){0x00C0C0C0}
#define Gray        (rgb888_t){0x00808080}
#define DarkGray    (rgb888_t){0x00A9A9A9}
#define Black       (rgb888_t){0x00000000}

#define RGB565(c) (((c.red >> 3) << 11) | ((c.green >> 2) << 5) | c.blue >> 3)

#ifdef __cplusplus
extern "C" {
#endif
uint16_t text_width(uint8_t * text, font_t * font);
void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, bool rtl);
void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, align_t align, bool rtl);
void draw_rectangle(uint16_t x_, uint16_t y_, uint16_t w, uint16_t h, rgb888_t color);
#ifdef __cplusplus
}
#endif
#endif
