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

typedef union {
	uint8_t value[3];
	struct {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
} rgb888_t;

#define Red (rgb888_t){255, 0, 0}
#define Orange (rgb888_t){255, 125, 0}
#define Yellow (rgb888_t){255, 255, 0}
#define Green (rgb888_t){0, 255, 0}
#define SpringGreen (rgb888_t){125, 255, 0}
#define Turquoise (rgb888_t){0, 255, 125}
#define Cyan (rgb888_t){0, 255, 255}
#define Ocean (rgb888_t){0, 125, 255}
#define Blue (rgb888_t){0, 0, 255}
#define Violet (rgb888_t){125, 0, 255}
#define Magenta (rgb888_t){255, 0, 255}
#define Raspberry (rgb888_t){255, 0, 125}

#define White (rgb888_t){255, 255, 255}
#define LightGray (rgb888_t){0xD3, 0xD3, 0xD3}
#define Silver (rgb888_t){0xC0, 0xC0, 0xC0}
#define Gray (rgb888_t){0x80, 0x80, 0x80}
#define DarkGray (rgb888_t){0xA9, 0xA9, 0xA9}
#define Black (rgb888_t){0, 0, 0}

#define RGB565(c) (((c.red >> 3) << 11) | ((c.green >> 2) << 5) | c.blue >> 3)

#ifdef __cplusplus
extern "C" {
#endif
uint16_t text_width(uint8_t * text, font_t * font);
void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, bool rtl);
void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, align_t align, bool rtl);
#ifdef __cplusplus
}
#endif
#endif
