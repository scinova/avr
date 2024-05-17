#include "system.h"
#include "spi.h"
#include "st7735.h"
#include "gfx.h"
#include "fonts.h"
#include <stdio.h>

uint8_t buf[10];
uint32_t stamp = 0;
uint16_t framecount = 0;
uint16_t fps = 0;

font_t * font = &LiberationSansRegular16;

void setup() {
    spi_enable();
    st7735_enable(SPI_SS, PinB1);
}

void loop() {
    uint32_t now = system_ms();
    if (now % 1000 < stamp % 1000) {
	fps = framecount;
	framecount = 0;
    }
    stamp = now;
    framecount++;
    sprintf((char *)&buf, "TIME:%05lu", system_ms());
    draw_text(0, 0, (uint8_t *)&buf, font, White, Black, false);
    sprintf((char *)&buf, "FPS:%u", fps);
    draw_text(0, 30, (uint8_t *)&buf, font, White, Black, false);
}
