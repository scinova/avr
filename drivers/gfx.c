#include <avr/pgmspace.h>
#include "gfx.h"
#include "string.h"
#include "ili9341.h"
#include "st7735.h"

#define write_block st7735_write_block
#define set_block st7735_set_block
#define set_pixel st7735_set_pixel

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, bool rtl) {
	if (strlen((const char *)text) > 40)
		return;
	uint8_t bh = ((font->height * (font->smooth * 2) - 1) / 8 + 1);
	int16_t offset = 0;

	for (int chx = 0; chx < strlen((const char *)text); chx++) {
		uint8_t chr = text[chx] - font->offset;
		uint16_t char_offset = 0;
		for (int i = 0; i < chr; i++)
			char_offset += font->widths[i] * bh;
		for (int cl = 0; cl < font->widths[chr]; cl++) {
			uint8_t rcl = (rtl ? font->widths[chr] - cl - 1 : cl);

			uint8_t pixels[2 * bh * 4];
			for (int b = 0; b < bh; b++) {
				uint8_t val = pgm_read_byte_near(font->data + (char_offset + bh * rcl + b));
				//uint8_t val = font->data[char_offset + bh * rcl + b];
				if (font->smooth) {
					for (int p = 0; p < 4; p++) {
						uint8_t v = (val >> (2 * p)) & 0b11;
						int ratio = ((uint8_t[]){0, 50, 70, 100})[v];
						rgb888_t rgb;
						rgb.red = map(ratio, 0, 100, bgcolor.red, color.red);
						rgb.green = map(ratio, 0, 100, bgcolor.green, color.green);
						rgb.blue = map(ratio, 0, 100, bgcolor.blue, color.blue);
						uint16_t col = RGB565(rgb);
						pixels[2 * (bh * b + p)] = col >> 8;
						pixels[2 * (bh * b + p) + 1] = col & 0xff;
						//set_pixel(100 + x + offset, y + 4 * b + p, col);
					}
				} else {
					for (int p = 0; p < 8; p++) {
						uint8_t v = (val >> p) & 1;
						uint16_t col;
						if (v)
							col = RGB565(color);
						else
							col = RGB565(bgcolor);
						//set_pixel(x + offset, y + 8 * b + p, col);
					}
				}
			}
			write_block(x + offset, y, 1, 4 * 4, &pixels[0]);
			if (rtl)
				offset--;
			else
				offset++;
		}
	}
}

uint16_t text_width(uint8_t * text, font_t * font) {
	uint16_t width = 0;
	for (int c = 0; c < strlen((const char *)text); c++)
		width += font->widths[text[c] - font->offset];
	return width;
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, rgb888_t color) {
	set_block(x, y, w, h, RGB565(color));
}

void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, rgb888_t color, rgb888_t bgcolor, align_t align, bool rtl) {
	//draw_rectangle((rtl ? x: width - 1 - x), y, width, h, bgcolor);	
	uint16_t pad = 0;
	uint16_t tw = text_width(text, font);
	if (align == AlignRight)
		pad = width - tw;
	else if (align == AlignCenter)
		pad = (width - tw) / 2;
	if (rtl)
		draw_text(x + pad + tw, y, text, font, color, bgcolor, rtl);
	else
		draw_text(x + pad, y, text, font, color, bgcolor, rtl);
}

