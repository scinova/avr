#ifndef _gfx_h_
#define _gfx_h_
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t offset;
	uint8_t length;
	uint8_t * size;
	uint32_t *data;
} font_t;

typedef union {
	uint8_t value[3];
	struct {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	}
} rgb888_t;

#define RGB565(c) (((c.red >> 3) << 11) | ((c.green >> 2) << 5) | c.blue >> 3)

#define Red (rgb888_t){255, 0, 0}
#define Green (rgb888_t){0, 255, 0}
#define Blue (rgb888_t){0, 0, 255}
#define Gray (rgb888_t){32, 32, 32}

#ifdef __cplusplus
extern "C" {
#endif
uint32_t font_data(font_t * font, uint8_t ch, uint8_t column);
void gfx_print(uint16_t x, uint16_t y, uint8_t * data, uint8_t len, bool rtl, font_t * font, rgb888_t color);
void gfx_draw_text_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t * data, uint8_t len, bool rtl, font_t * font, rgb888_t color);
void gfx_draw_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h, rgb888_t color);
#ifdef __cplusplus
}
#endif
#endif
