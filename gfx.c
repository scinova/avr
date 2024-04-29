#include <avr/pgmspace.h>
#include "gfx.h"
#include "ili9341.h"
#include <string.h>

void draw_text(uint16_t x, uint16_t y, const uint8_t * text, font_t * font, bool rtl) {
	uint8_t bh = ((font->height - 1) / 8 + 1) * 2;
	int16_t offset = 0;
	for (int chx = 0; chx < strlen((char *)text); chx++) {
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
					switch(v) {
						case 0:
							ili9341_set_pixel(x + offset, y + 4 * b + p, 0x0000);
							break;
						case 1:
							ili9341_set_pixel(x + offset, y + 4 * b + p, 0x6800);
							break;
						case 2:
							ili9341_set_pixel(x + offset, y + 4 * b + p, 0x7800);
							break;
						case 3:
							ili9341_set_pixel(x + offset, y + 4 * b + p, 0xF800);
							break;
					}
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
	for (int c = 0; c < strlen((char *)text); c++)
		width += font->widths[text[c] - font->offset];
	return width;
}

void draw_text_box(uint16_t x, uint16_t y, uint16_t width, uint16_t h, uint8_t * text, font_t * font, align_t align, bool rtl) {
	uint16_t pad = 0;
	if (align == AlignRight)
		pad = width - text_width(text, font);
	else if (align == AlignCenter)
		pad = (width - text_width(text, font)) / 2;
	if (rtl)
		draw_text(width - x - pad, y, text, font, rtl);
	else
		draw_text(x + pad, y, text, font, rtl);
}
