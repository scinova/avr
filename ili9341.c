#include "ili9341.h"
#include "gpio.h"
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

#define BIT(x) (1<<x)

pin_t _cspin;
pin_t _dcpin;

typedef enum {
	NOP = 0x00,
	SOFTWARE_RESET = 0x01,
	READ_INDENTIFICATION = 0x04,
	READ_DISPLAY_STATUS = 0x09,
	READ_DISPLAY_POWER_MODE = 0xa,
	READ_MEMORY_ACCESS_CONTROL = 0x0b,
	READ_DISPLAY_PIXEL_FORMAT = 0xc,
	READ_DISPLAY_IMAGE_FORMAT = 0xc,
	READ_SELF_DIAGNOSTIC = 0xF,
	//
	ENTER_SLEEP_MODE = 0x10,
	SLEEP_OUT = 0x11,
	PARTIAL_DISPLAY_MODE_ON = 0x12,
	NORMAL_DISPLAY_MODE_ON = 0x13,
	//
	DISPLAY_INVERSION_OFF = 0x20,
	DISPLAY_INVERSION_ON = 0x21,
	GAMMA_SET = 0x26,
	DISPLAY_OFF = 0x28,
	DISPLAY_ON = 0x29,
	COLUMN_ADDRESS_SET = 0x2a,
	PAGE_ADDRESS_SET = 0x2b,
	MEMORY_WRITE = 0x2c,
	COLOR_SET = 0x2d,
	MEMORY_READ = 0x2e,
	//
	PARTIAL_AREA = 0x30,
	VERTICAL_SCROLLING_DEFINITION = 0x33,
	TEARING_EFFECT_LINE_OFF = 0x34,
	TEARING_EFFECT_LINE_ON = 0x35 ,
	MEMORY_ACCESS_CONTROL = 0x36,
	VERTICAL_SCROLLING_START_ADDRESS = 0x37,
	IDLE_MODE_OFF = 0x38,
	IDLE_MODE_ON = 0x39,
	PIXEL_FORMAT_SET = 0x3A,
	WRITE_MEMORY_CONTINUE = 0x3C,
	READ_MEMORY_CONTINUE = 0x3D,
	//
	RGB_INTERFACE_SIGNAL_CONTROL = 0xB0,
	FRAME_RATE_CONTROL = 0xB1,
	FRAME_RATE_CONTROL2 = 0xB2,
	FRAME_RATE_CONTROL3 = 0xB3,
	DISPLAY_INVERSION_CONTROL = 0xB4,
	BLANKING_PORCH_CONTROL = 0xB5,
	DISPLAY_FUNCTION_CONTROL = 0xB6,
	ENTRY_MODE_SET = 0xB7,
	BACKLIGHT_CONTROL1 = 0xB8,
	BACKLIGHT_CONTROL2 = 0xB9,
	BACKLIGHT_CONTROL3 = 0xBA,
	BACKLIGHT_CONTROL4 = 0xBB,
	BACKLIGHT_CONTROL5 = 0xBC,
	BACKLIGHT_CONTROL6 = 0xBD,
	BACKLIGHT_CONTROL7 = 0xBE,
	BACKLIGHT_CONTROL8 = 0xBF,
	//
	POWER_CONTROL1 = 0xC0,
	POWER_CONTROL2 = 0xC1,
	VCOM_CONTROL1 = 0xC5,
	VCOM_CONTROL2 = 0xC7,
	//
	READ_ID1 = 0xDA,
	READ_ID2 = 0xDB,
	READ_ID3 = 0xDC,
	//
	POSITIVE_GAMMA_CORRECTION = 0xE0,
	NEGATIVE_GAMMA_CORRECTION = 0xE1,
	INTERFACE_CONTROL = 0xF6,
	// extended
	POWER_CONTROL_A = 0xCB,
	POWER_CONTROL_B = 0xCF,
	DRIVER_TIMING_CONTROL_A = 0xE8,
	DRIVER_TIMING_CONTROL_b = 0xEA,
	POWER_ON_SEQUENCE_CONTROL = 0xED,
	PUMP_RATIO_CONTROL = 0xF7
} ili9341_command_t;

// MEMORY_ACCESS_CONTROL 0x0b
#define MY 7 // row address order 0=top to bottom, 1=bottom to top
#define MX 6 // column addres order 0=left to right, 1=right to left
#define MV 5 // row/column exchange
#define ML 4 // vertical refresh order
#define BGR 3 // rgb/grb order
#define MH 2 // horizontal refresh order
// DISPLAY_FUNCTION_CONTROL 0xB6
#define PT0 0
#define PT1 1
#define PTG0 2
#define PTG1 3
//
#define ISC0 0
#define ISC1 1
#define ISC2 2
#define ISC3 3
#define ISC 0b00001111
#define SM 4
#define SS 5
#define GS 6
#define REV 7
//
#define NL 0b00111111

static void transmit_command(ili9341_command_t cmd) {
  pin_reset(_dcpin);
  pin_reset(_cspin);
	spi_transfer8(cmd);
  pin_set(_cspin);
}

static void transmit_data(uint8_t data) {
  pin_set(_dcpin);
  pin_reset(_cspin);
	spi_transfer8(data);
  pin_set(_cspin);
}

static void transmit_data16(uint16_t data) {
  pin_set(_dcpin);
  pin_reset(_cspin);
	spi_transfer8((uint8_t)(data >> 8));
	spi_transfer8(data );
  pin_set(_cspin);
}

static void command(uint8_t cmd) {
	transmit_command(cmd);
}

static void command1(uint8_t cmd, uint8_t p1) {
	transmit_command(cmd);
	transmit_data(p1);
}

static void command2(uint8_t cmd, uint8_t p1, uint8_t p2) {
	transmit_command(cmd);
	transmit_data(p1);
	transmit_data(p2);
}

static void command3(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3) {
	transmit_command(cmd);
	transmit_data(p1);
	transmit_data(p2);
	transmit_data(p3);
}

static void command4(uint8_t cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
	transmit_command(cmd);
	transmit_data(p1);
	transmit_data(p2);
	transmit_data(p3);
	transmit_data(p4);
}

void ili9341_enable(pin_t cspin, pin_t dcpin) {
	_cspin = cspin;
	_dcpin = dcpin;
  pin_mode(_cspin, Output);
  pin_mode(_dcpin, Output);
	command(SOFTWARE_RESET);
	//_delay_ms(50);
	command(DISPLAY_OFF);
	command1(POWER_CONTROL1, 0x23); // 4.60v
	command1(POWER_CONTROL2, 0x10); // ???
	command2(VCOM_CONTROL1, 0x2B, 0x2B); // 3.775V
	command1(VCOM_CONTROL2, 0xC0); // no offset voltage
	command1(MEMORY_ACCESS_CONTROL, BIT(MX) | BIT(BGR));
	command2(VERTICAL_SCROLLING_START_ADDRESS, 0, 0);
	command1(PIXEL_FORMAT_SET, 0x55); // 16bit/px
	command2(FRAME_RATE_CONTROL, 0, 0x1B);
	command3(DISPLAY_FUNCTION_CONTROL, BIT(PTG1), BIT(REV) + 2, 0x27);
	// TODO: set gamma
	command(SLEEP_OUT);
	//_delay_ms(150);
	command(DISPLAY_ON);
	//_delay_ms(200);
}

void ili9341_disable() {
	command(DISPLAY_OFF);
	command(ENTER_SLEEP_MODE);
}

void ili9341_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
	uint8_t xh = x >> 8, xl = x & 0xff;
	uint8_t yh = y >> 8, yl = y & 0xff;
	uint8_t ch = (uint16_t)color >> 8, cl = (uint16_t)(color & 0xff);
	command4(COLUMN_ADDRESS_SET, xh, xl, xh, xl);
	command4(PAGE_ADDRESS_SET, yh, yl, yh, yl);
	command(MEMORY_WRITE);
	transmit_data(ch);
	transmit_data(cl);
}
