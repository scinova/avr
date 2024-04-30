#include <avr/pgmspace.h>
#include "gfx.h"

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, bool rtl) {
	uint8_t bh = ((font->height - 1) / 8 + 1) * 2;
	int16_t offset = 0;
	for (int chx = 0; chx < strlen(text); chx++) {
		uint8_t chr = text[chx] - font->offset;
		uint16_t char_offset = 0;
		for (int i = 0; i < chr; i++)
			char_offset += font->widths[i] * bh;
		for (int cl = 0; cl < font->widths[chr]; cl++) {
			uint8_t rcl = (rtl ? font->widths[chr] - cl - 1 : cl);
			for (int b = 0; b < bh; b++) {
				uint8_t val = pgm_read_byte_near(font->data + (char_offset + bh * rcl + b));
				//uint8_t val = font->data[char_offset + bh * rcl + b];
				for (int p = 0; p < 4; p++) {
					uint8_t v = (val >> (2 * p)) & 0b11;
					int ratio = ((uint8_t[]){0, 50, 70, 100})[v];
					rgb888_t rgb;
					rgb.red = map(ratio, 0, 100, bgcolor.red, color.red);
					rgb.green = map(ratio, 0, 100, bgcolor.green, color.green);
					rgb.blue = map(ratio, 0, 100, bgcolor.blue, color.blue);
					uint16_t col = RGB565(rgb);
					ili9341_set_pixel(x + offset, y + 4 * b + p, col);
				}
			}
			if (rtl)
				offset--;
			else
				offset++;
		}
	}
}

uint16_t text_width(uint8_t * text, font_t * font) {
	uint16_t width = 0;
	for (int c = 0; c < strlen(text); c++)
		width += font->widths[text[c] - font->offset];
	return width;
}

void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, align_t align, bool rtl) {
	uint16_t pad = 0;
	if (align == AlignRight)
		pad = width - text_width(text, font);
	else if (align == AlignCenter)
		pad = (width - text_width(text, font)) / 2;
	if (rtl)
		draw_text(width - x - pad, y, text, font, color, bgcolor, rtl);
	else
		draw_text(x + pad, y, text, font, color, bgcolor, rtl);
}
