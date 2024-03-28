#ifndef mpu6050_h
#define mpu6050_h

#include "i2c.h"

typedef enum {
	mpu6050AccelerometerRange2G = 0,
	mpu6050AccelerometerRange4G = 1,
	mpu6050AccelerometerRange8G = 2,
	mpu6050AccelerometerRange16G = 3
} mpu6050_accelerometer_range_t;

typedef enum {
	mpu6050GyroscopeRange250Deg = 0,
	mpu6050GyroscopeRange500Deg = 1,
	mpu6050GyroscopeRange1000Deg = 2,
	mpu6050GyroscopeRange2000Deg = 3
} mpu6050_gyroscope_range_t;

typedef enum {
	mpu6050FSyncDisabled = 0,
	mpu6050FSyncTemp = 1,
	mpu6050FSyncGyroX = 2,
	mpu6050FSyncGyroY = 3,
	mpu6050FSyncGyroZ = 4,
	mpu6050FSyncAccelX = 5,
	mpu6050FSyncAccelY = 6,
	mpu6050FSyncAccelZ = 7
} mpu6050_fsync_t;

typedef enum {
	mpu6050Bandwith260Hz = 0,
	mpu6050Bandwith184Hz = 1,
	mpu6050Bandwith94Hz = 2,
	mpu6050Bandwith44Hz = 3,
	mpu6050Bandwith21Hz = 4,
	mpu6050Bandwith10Hz = 5,
	mpu6050Bandwith5Hz = 6
} mpu6050_lpf_bandwith_t;

typedef enum {
	mpu6050WakeupFrequency1_25HZ = 0,
	mpu6050WakeupFrequency5HZ = 1,
	mpu6050WakeupFrequency20HZ = 2,
	mpu6050WakeupFrequency40HZ = 3
} mpu6050_wakeup_frequency_t;


typedef enum {
	mpu6050HighpassDisable,
	mpu6050Highpass5Hz,
	mpu6050Highpass2_5HZ,
	mpu6050Highpass1_25HZ,
	mpu6050HighpassUnused,
	mpu6050HighpassHold,
} mpu6050_highpass_filter_t;

typedef enum {
	mpu6050Internal8MHz = 0,
	mpu6050PLLGyroscopeX = 1,
	mpu6050PLLGyroscopeY = 2,
	mpu6050PLLGyroscopeZ = 3,
	mpu6050PLLExternal_32K = 4,
	mpu6050PLLExternal_19MHz = 5,
	mpu6050ClockStopped = 7
} mpu6050_clock_source_t;
#ifdef __cplusplus
extern "C" {
#endif
void mpu6050_init();
void mpu6050_reset();
void mpu6050_set_sample_rate_divider (uint8_t val);
void mpu6050_set_config (mpu6050_lpf_bandwith_t bandwith, mpu6050_fsync_t fsync);
void mpu6050_set_accelerometer_config (mpu6050_accelerometer_range_t);
void mpu6050_set_gyroscope_config (mpu6050_gyroscope_range_t);

void mpu6050_enable(mpu6050_clock_source_t clock);
void mpu6050_enable_cycle(mpu6050_clock_source_t clock, mpu6050_wakeup_frequency_t frequency);
void mpu6050_disable();

void mpu6050_read_acceleration(volatile int16_t * values);
void mpu6050_read_angular_velocity(volatile int16_t * values);
void mpu6050_read_temperature(volatile float * dest);
#ifdef __cplusplus
}
#endif
#endif
