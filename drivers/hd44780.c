#include "hd44780.h"
#include "gpio.h"

typedef enum {
	CLEAR_DISPLAY = 0x01,
	RETURN_HOME = 0x02,
	ENTRY_MODE_SET = 0x04,
	DISPLAY_ON_OFF_CONTROL = 0x08,
	CURSOR_OR_DISPLAY_SHIFT = 0x10,
	FUNCTION_SET = 0x20,
	SET_CGRAM_ADDRESS = 0x40,
	SET_DDRAM_ADDRESS = 0x80,
} hd44780_command_t;

static pin_t _rs_pin;
static pin_t _rw_pin;
static pin_t _e_pin;
static port_t _data_port;

static void hd44780_write(uint8_t data, bool rs) {
	// TODO: delay
	pin_reset(_e_pin);
	pin_write(_rs_pin, rs);
	pin_reset(_rw_pin);
	port_mode(_data_port, Output);
	port_write_byte(_data_port, data);
	pin_set(_e_pin);
	pin_reset(_e_pin);
	pin_set(_rw_pin);
	port_mode(_data_port, Input);
	while (port_read_byte(_data_port) & 0x80);
}

static void hd44780_write_command(uint8_t command) {
	hd44780_write(command, false);
}

static void hd44780_write_data(uint8_t data) {
	hd44780_write(data, true);
}

void hd44780_enable(pin_t rs_pin, pin_t rw_pin, pin_t e_pin, port_t data_port) {
	_rs_pin = rs_pin;
	_rw_pin = rw_pin;
	_e_pin = e_pin;
	_data_port = data_port;
	pin_mode(_rs_pin, Output);
	pin_mode(_rw_pin, Output);
	pin_mode(_e_pin, Output);
	port_mode(_data_port, Output);
	hd44780_write_command(DISPLAY_ON_OFF_CONTROL + 4);
	hd44780_write_command(CLEAR_DISPLAY);
}

void hd44780_disable() {
	hd44780_write_command(DISPLAY_ON_OFF_CONTROL);
}

void hd44780_print(uint8_t x, uint8_t y, uint8_t * data) {
	uint8_t addr = (uint8_t[]){0, 64, 20, 84}[y] + x;
	hd44780_write_command(SET_DDRAM_ADDRESS + addr);
	uint8_t i = 0;
	while (data[i] != 0)
		hd44780_write_data(data[i++]);
}
