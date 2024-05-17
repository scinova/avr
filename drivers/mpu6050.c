#include "system.h"
#include "mpu6050.h"
#include "i2c.h"
#include <avr/io.h>
#include "uart.h"

#define ADDRESS 0x68

typedef enum {
	SELF_TEST_X =			0x0D,
	SELF_TEST_Y =			0x0E,
	SELF_TEST_Z =			0x0F,
	SELF_TEST_A =			0x10,
	SMPLRT_DIV =			0x19,
	CONFIG =					0x1A,
	GYRO_CONFIG =			0x1B,
	ACCEL_CONFIG =		0x1C,
	MOT_THR							= 0x1F,
	MOT_DUR							= 0x20,
	INT_PIN_CONFIG			= 0x37,
	INT_ENABLE 					= 0x38,
	INT_STATUS					= 0x3A,
	ACCEL_OUT 					= 0x3B,
	ACCEL_XOUT_H 				= 0x3B,
	ACCEL_XOUT_L 				= 0x3C,
	ACCEL_YOUT_H 				= 0x3D,
	ACCEL_YOUT_L 				= 0x3E,
	ACCEL_ZOUT_H 				= 0x3F,
	ACCEL_ZOUT_L 				= 0x40,
	TEMP_OUT_H =		0x41,
	TEMP_OUT_L =		0x42,
	GYRO_OUT =			0x43,
	GYRO_XOUT_H =		0x43,
	GYRO_XOUT_L =		0x44,
	GYRO_YOUT_H =		0x45,
	GYRO_YOUT_L =		0x46,
	GYRO_ZOUT_H =		0x47,
	GYRO_ZOUT_L =		0x48,
	SIGNAL_PATH_RESET		=	0x68, // write only
	USER_CTRL 					= 0x6A,
	PWR_MGMT_1 					= 0x6B,
	PWR_MGMT_2 					= 0x6C,
	WHO_AM_I 						= 0x75 // device id (=0x55)
} register_t;

// GYRO_CONFIG
#define XG_ST						7
#define YG_ST						6
#define ZG_ST						5
#define FS_SEL_1				4
#define FS_SEL_0				3
#define FS_SEL					_BV(FS_SEL_1) | _BV(FS_SEL_0)
// CONFIG
#define EXT_SYNC_SET_2	5
#define EXT_SYNC_SET_1	4
#define EXT_SYNC_SET_0	3
#define DLPF_CFG_2			2
#define DLPF_CFG_1			1
#define DLPF_CFG_0			0
#define EXT_SYNC_SET		_BV(EXT_SYNC_SET_2)) | _BV(EXT_SYNC_SET_1) | _BV(EXT_SYNC_SET_0)
#define DLP_CFG					_BV(DLP_CFG_2)) | _BV(DLP_CFG_1) | _BV(DLP_CFG_0)
// ACCEL_CONFIG
#define XA_ST						7
#define YA_ST						6
#define ZA_ST						5
#define AFS_SEL_1				4
#define AFS_SEL_0				3
#define AFS_SEL					_BV(AFS_SEL_1) | _BV(AFS_SEL_0)
// SIGNAL_PATH_RESET
#define GYRO_RESET			2
#define ACCEL_RESET			1
#define TEMP_RESET			0
// USER_CTL
#define FIFO_EN						6
#define I2C_MST_EN				5
#define I2C_IF_DIS				4 //only on 6000
#define FIFO_RESET				2
#define I2C_MASTER_RESET	1
#define SIG_COND_RESET		0
// PWR_MGMT_1
#define DEVICE_RESET		7
#define	SLEEP						6
#define	CYCLE						5
#define	TEMP_DIS				3
#define	CLKSEL_2				2
#define	CLKSEL_1				1
#define	CLKSEL_0				0
#define CLKSEL					_BV(CLKSEL_2) | _BV(CLKSEL_1) | _BV(CLKSEL_0)
// PWR_MGMT_2
#define LP_WAKE_CTRL_1 7
#define LP_WAKE_CTRL_0 6
#define STBY_XA					5
#define STBY_YA					4
#define STBY_ZA					3
#define STBY_XG					2
#define STBY_YG					1
#define STBY_ZG					0
#define LP_WAKE_CTRL		_BV(LP_WAKE_CTRL_1) | _BV(LP_WAKE_CTRL_0)
// INT_PIN_CFG
#define INT_LEVEL				7
#define INT_OPEN				6
#define LATCH_INT_EN		5
#define INT_RD_CLEAR		4
#define	FSYNC_INT_EN		3
#define I2C_BYPASS_EN		2
// INT_ENABLE
#define MOT_EN			6 //???
#define FIFO_OFLOW_EN		4
#define I2C_MST_INT_EN	3
#define DATA_RDY_EN			2
// INT_STATUS
#define MOT_INT					6 //???
#define FIFO_OFLOW_INT	4
#define I2C_MST_INT			3
#define DATA_RDY_INT		2

void mpu6050_reset() {
	i2c_write_register(ADDRESS, PWR_MGMT_1, DEVICE_RESET);
	delay_ms(100);
	i2c_write_register(ADDRESS, SIGNAL_PATH_RESET,
			ACCEL_RESET | GYRO_RESET | TEMP_RESET);
	delay_ms(100);
}

void mpu6050_set_sample_rate_divider(uint8_t value) {
	i2c_write_register(ADDRESS, SMPLRT_DIV, value);
}

void mpu6050_set_config(mpu6050_lpf_bandwith_t bandwith, mpu6050_fsync_t fsync) {
	i2c_write_register(ADDRESS, CONFIG, bandwith << DLPF_CFG_0 | fsync << EXT_SYNC_SET_0);
}

void mpu6050_set_accelerometer_config(mpu6050_accelerometer_range_t range) {
	i2c_write_register(ADDRESS, ACCEL_CONFIG, range << AFS_SEL_0);
}

void mpu6050_set_gyroscope_config(mpu6050_gyroscope_range_t range) {
	i2c_write_register(ADDRESS, GYRO_CONFIG, range << FS_SEL_0);
}

void mpu6050_enable(mpu6050_clock_source_t clock) {
	i2c_write_register(ADDRESS, PWR_MGMT_1, clock << CLKSEL_0);
}

void mpu6050_enable_cycle(mpu6050_clock_source_t clock,
		mpu6050_wakeup_frequency_t frequency) {
	i2c_write_register(ADDRESS, PWR_MGMT_1, 1 << CYCLE | clock << CLKSEL_0);
	i2c_write_register(ADDRESS, PWR_MGMT_2,frequency << LP_WAKE_CTRL_0);
}

void mpu6050_disable() {
	i2c_write_register(ADDRESS, PWR_MGMT_1, 1 << SLEEP);
}

void mpu6050_init() {
	mpu6050_reset();
	mpu6050_set_sample_rate_divider(0);
	mpu6050_set_config(mpu6050Bandwith260Hz, mpu6050FSyncGyroX);
	mpu6050_set_accelerometer_config(mpu6050AccelerometerRange2G);
	mpu6050_set_gyroscope_config(mpu6050GyroscopeRange500Deg);
	mpu6050_enable(mpu6050PLLGyroscopeX);
	delay_ms(100);
	}

static volatile uint8_t _tempbuf[2];
static volatile float * _temp;
static volatile uint8_t _accelbuf[6];
static volatile int16_t * _accel;
static volatile uint8_t _gyrobuf[6];
static volatile int16_t * _gyro;

static void _read_temperature_callback() {	
	int16_t rawtemp = _tempbuf[0] << 8 | (uint8_t)_tempbuf[1];
	*_temp = ((float)rawtemp / 340.0) + 36.53;
}

void mpu6050_read_temperature(volatile float * dest) {
	_temp = dest;
	i2c_read_registers_cb(ADDRESS, TEMP_OUT_H, 2, &_tempbuf[0], _read_temperature_callback);
}

static void _read_acceleration_callback() {
	_accel[0] = _accelbuf[0] << 8 | (uint8_t)_accelbuf[1];
	_accel[1] = _accelbuf[2] << 8 | (uint8_t)_accelbuf[3];
	_accel[2] = _accelbuf[4] << 8 | (uint8_t)_accelbuf[5];
}

void mpu6050_read_acceleration(volatile int16_t * dest) {
	_accel = dest;
	i2c_read_registers_cb(ADDRESS, ACCEL_OUT, 6, &_accelbuf[0], _read_acceleration_callback);
}

static void _read_angular_velocity_callback() {
	_gyro[0] = _gyrobuf[0] << 8 | (uint8_t)_gyrobuf[1];
	_gyro[1] = _gyrobuf[2] << 8 | (uint8_t)_gyrobuf[3];
	_gyro[2] = _gyrobuf[4] << 8 | (uint8_t)_gyrobuf[5];
}

void mpu6050_read_angular_velocity(volatile int16_t * dest) {
	_gyro = dest;
	i2c_read_registers_cb(ADDRESS, GYRO_OUT, 6, (uint8_t *)dest, _read_angular_velocity_callback);
}

bool mpu6050_identified() {
	uint8_t chip_id;
	i2c_read_register(ADDRESS, WHO_AM_I, &chip_id);
	while (~!i2c_is_ready());
	return chip_id == 0x68;
}

void mpu6050_set_motion_detection_threshold(uint8_t threshold) {
	i2c_write_register(ADDRESS, MOT_THR, threshold);
}

void mpu6050_set_motion_detection_duration(uint8_t duration) {
	i2c_write_register(ADDRESS, MOT_DUR, duration);
}
