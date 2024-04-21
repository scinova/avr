#include <stdint.h>
#include <stdbool.h>
#include "gfx.h"
#include "ili9341.h"

uint32_t font_data(font_t * font, uint8_t ch, uint8_t column) {
	uint8_t offset = 0;
	for (int i = 0; i < ch; i++)
		offset += font->size[i];
	return font->data[offset + column];
}

void gfx_set_pixel(uint16_t x, uint16_t y, rgb888_t color) {
	ili9341_set_pixel(x, y, RGB565(color));
}

void gfx_print(uint16_t x_, uint16_t y_, uint8_t * data, uint8_t len, bool rtl, font_t * font, rgb888_t color) {
	uint16_t xoff = 0;
	rgb888_t color1 = (rgb888_t){.red = color.red * 0.8, .green = color.green * 0.8, .blue = color.blue * 0.8};
	rgb888_t color2 = (rgb888_t){.red = color.red * 0.5, .green = color.green * 0.5, .blue = color.blue * 0.5};
	rgb888_t colors[] = {(rgb888_t){0, 0, 0}, color2, color1, color};
	for (int i = 0; i < len; i++) {
		uint8_t size = font->size[i];
		for (uint8_t x = 0; x < size; x++) {
			uint8_t ch = *(data + i);
			uint32_t d = font_data(font, ch - font->offset, (rtl ? size - x - 1 : x));
			for (uint8_t y = 0; y < 16; y++) {
				uint8_t px = (d >> (2 * y)) &0x3;
				gfx_set_pixel((rtl ? x_ - xoff : x_ + xoff), y_ + y, colors[px]);
			}
			xoff += 1;
		} 
	}
}

void gfx_draw_text_box(uint16_t x_, uint16_t y_, uint16_t w, uint16_t h, uint8_t * data, uint8_t len, bool rtl, font_t * font, rgb888_t color) {
	uint16_t text_size = 0;
	for (int i = 0; i < len; i++)
		text_size += font->size[i];
	uint16_t pad = ((w - text_size) / 2); // center
	gfx_print((rtl ? w - pad : x_ + pad), y_, data, len, rtl, font, color);
}

void gfx_draw_box(uint16_t x_, uint16_t y_, uint16_t w, uint16_t h, rgb888_t color) {
	for (int y = y_; y < y_ + h; y++)
		for (int x = x_; x < x_ + w; x++)
			gfx_set_pixel(x, y, color);
}
