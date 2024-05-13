#include "st7735.h"
#include "system.h"
#include "gpio.h"
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

static pin_t _cspin;
static pin_t _dcpin;

typedef enum {
	// system commands
	NOP       = 0x00,
	SWRESET   = 0x01, // Sowtware Reset
	RDDID     = 0x04, // Read Display ID
	RDDST     = 0x09, // Read Display Status
	RDDPM     = 0x0A, // Read Display Power Mode
	RDDMADCTL = 0x0B, // Read Display MADCTL
	RDDCOLMOD = 0x0C, // Read Display Pixel Format
	RDDIM     = 0x0D, // Read Display Image Mode
	RDDSM     = 0x0E, // Read Display Signal Mode
	RDDSDR    = 0x0F, // Read Display Self-Diagnostic Result
	SLPIN     = 0x10, // Sleep In
	SLPOUT    = 0x11, // Sleep Out
	PTLON     = 0x12, // Partial Display Mode On
	NORON     = 0x13, // Normal Display Mode On
	INVOFF    = 0x20, // Display Inversion Off
	INVON     = 0x21, // Display Inversion On
	GAMSET    = 0x26, // Gamma Set
	DISPOFF   = 0x28, // Display Inversion On
	DISPON    = 0x29, // Display Inversion On
	CASET     = 0x2A, // Column Address Set
	RASET     = 0x2B, // Row Address Set
	RAMWR     = 0x2C, // Memory Write
	RGBSET		= 0x2D, // Color setting for 4K, 65K and 262K
	RAMRD     = 0x2E, // Memory Read
	PTLAR			= 0x30, // Partial Area
	SCRLAR		= 0x31, // Scroll Area Set
	TEFF      = 0x34, // Tearing Effect Line Off
	TEON      = 0x35, // Tearing Efect Line Off
	MADCTL    = 0x36, // Memory Access Control
	VSCSAD    = 0x37, // Vertical Scroll Start Address
	IDLOFF    = 0x38, // Idle Mode Off
	IDLON			= 0x39, // Idle Mode On
	COLMOD    = 0x3A, // Interface Pixel Format
	RDID1     = 0xDA,
	RDID2     = 0xDB,
	RDID3     = 0xDC,
	// panel commands
	FRMCTR1   = 0xb1, // Frame rate control 1, use by default
	FRMCTR2   = 0xb2, // Frame Rate Control (In Idle mode / 8-colors)
	FRMCTR3   = 0xb3, // Frame Rate Control (In Partial mode / full colors)
	INVCTR    = 0xb4, // Display inversion, use by default
	PWCTR1    = 0xC0, // Power control 1
	PWCTR2    = 0xC1,
	PWCTR3    = 0xC2, // Power control 3
	PWCTR4    = 0xC3, // Power Control 4 (in Idle mode/ 8-colors,
	PWCTR5    = 0xC4, // Power Control 5 (in Partial mode/ full-colors,
	VMCTR1    = 0xC5, // VCom control 1
	VMOFCTR   = 0xC7,
	WRID2     = 0xD1,
	WRID3     = 0xD2,
	NVFCTR1   = 0xD9,
	NVFCTR2   = 0xDE,
	NVFCTR3   = 0xDF,
	GMCTRP1   = 0xE0, // positive gamma correction
	GMCTRN1   = 0xE1, // negative gamma correction
	GCV				= 0xFC, // Gate Pump Clock Frequency Variable
} st7735_command_t;

#define MADCTL_MX  0x80 // row address order 0=top to bottom, 1=bottom to top
#define MADCTL_MY  0x40 // column addres order 0=left to right, 1=right to left
#define MADCTL_MV  0x20 // row/column exchange
#define MADCTL_ML  0x10 // vertical refresh order
#define MADCTL_BGR 0x08 // rgb/grb order
#define MADCTL_MH  0x04 // horizontal refresh order

static void chip_select() {
	pin_reset(_cspin);
}

static void chip_deselect() {
	pin_set(_cspin);
}

static void command(uint8_t cmd) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
}

static void write_data(uint8_t data) {
	pin_set(_dcpin);
	spi_transfer8(data);
}

static void command1(uint8_t cmd, uint8_t p1) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
	pin_set(_dcpin);
	spi_transfer8(p1);
}

static void command2(uint8_t cmd, uint8_t p1, uint8_t p2) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
	pin_set(_dcpin);
	spi_transfer8(p1);
	spi_transfer8(p2);
}

static void command3(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
	pin_set(_dcpin);
	spi_transfer8(p1);
	spi_transfer8(p2);
	spi_transfer8(p3);
}

static void command4(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
	pin_set(_dcpin);
	spi_transfer8(p1);
	spi_transfer8(p2);
	spi_transfer8(p3);
	spi_transfer8(p4);
}

static void command6(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5, uint8_t p6) {
	pin_reset(_dcpin);
	spi_transfer8(cmd);
	pin_set(_dcpin);
	spi_transfer8(p1);
	spi_transfer8(p2);
	spi_transfer8(p3);
	spi_transfer8(p4);
	spi_transfer8(p5);
	spi_transfer8(p6);
}

void st7735_set_rotation(int rotation) {
	chip_select();
	switch (rotation) {
		case 0:
				command1(MADCTL, 0);
				break;
		case 1:
				command1(MADCTL, MADCTL_MX | MADCTL_MV);
				break;
		case 2:
				command1(MADCTL, MADCTL_MY);
				break;
		case 3:
				command1(MADCTL, MADCTL_MY | MADCTL_MV);
				break;
	}
	chip_deselect();
}

void st7735_enable(pin_t cspin, pin_t dcpin) {
	_cspin = cspin;
	_dcpin = dcpin;
  pin_mode(_cspin, Output);
	pin_set(_cspin);
  pin_mode(_dcpin, Output);
	chip_select();
	command(SWRESET);
	delay_ms(50);
	command(SLPOUT);
	delay_ms(500);
	command3(FRMCTR1,
      1,
      0x2C,
      0x2D);
	//_delay_ms(10);
	command3(FRMCTR2,
      1,
      0x2C,
      0x2D);
	command6(FRMCTR3,
    1, 0x2C, 0x2D,
    1, 0x2C, 0x2D);
  command1(INVCTR, 7);
	command3(PWCTR1,
		0xA2, // avdd=5v, gvdd=4.6v
		0x02, // gvcl=-4.6v
		0x84); // mode=auto, vrhp5,vrhn5=0
	//_delay_ms(10);
	command1(PWCTR2,
			0xC5); // vgh25=2.4v vghbt=1(vgh=3*avdd-0.5) vgh=14.5v vgl=-10v
	command2(PWCTR3, // opamp curent in normal/full mode
		0x0A, // ap=small sap=medium-low
		0x00); // boost frequency
	command2(PWCTR4, // opamp current in idle/8-colors mode
		0x8A, // ap=small sap=medium-low
		0x2A); // boost frequency
	command2(PWCTR5, // opamp current in partial/full-color mode
		0x8A, // ap=small sap=medium-low
		0xEE);//0x2A); // boost frequency
	command1(VMCTR1, 0x0E); // -0.775
	command(INVOFF);
	command1(COLMOD, 0x05); // 16bit/px
	// gamma 0xE0 02 1c 07 12 37 32 29 2d 29 25 2b 39 00 01 03 10
	// gamma 0xE1 03 1d 07 06 2e 2c 29 2d 2e 2e 37 3f 00 00 03 01
	command(NORON);
	command(DISPON);
	delay_ms(100);
	st7735_set_rotation(0);
	chip_deselect();
}

void st7735_disable() {
	chip_select();
	command(DISPOFF);
	command(SLPIN);
	chip_deselect();
}

void st7735_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
	chip_select();
	uint8_t xh = x >> 8, xl = x & 0xff;
	uint8_t yh = y >> 8, yl = y & 0xff;
	uint8_t ch = (uint16_t)color >> 8, cl = (uint16_t)(color & 0xff);
	command4(CASET, xh, xl, xh, xl);
	command4(RASET, yh, yl, yh, yl);
	command(RAMWR);
	write_data(ch);
	write_data(cl);
	chip_deselect();
}

void st7735_write_block(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t * data) {
	uint16_t x2 = x + w - 1;
	uint16_t y2 = y + h - 1;
	chip_select();
	command4(CASET, x >> 8, x & 0xFF, x2 >> 8, x2 & 0xFF);
	command4(RASET, y >> 8, y & 0xFF, y2 >> 8, y2 & 0xFF);
	command(RAMWR);
	for (int i = 0; i < w * h * 2; i++)
		write_data(data[i]);
	chip_deselect();
}

void st7735_set_block(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	uint16_t x2 = x + w - 1;
	uint16_t y2 = y + h - 1;
	chip_select();
	command4(CASET, x >> 8, x & 0xFF, x2 >> 8, x2 & 0xFF);
	command4(RASET, y >> 8, y & 0xFF, y2 >> 8, y2 & 0xFF);
	command(RAMWR);
	for (int i = 0; i < w * h; i++) {
		write_data(color >> 8);
		write_data(color & 0xFF);
	}
	chip_deselect();
}
