#ifndef qmc5883l_h
#define qmc5883l_h

#include <stdint.h>
#include "i2c.h"

typedef enum {
	qmc5883lModeStandby	= 0,
	qmc5883lModeContinous	= 1
} qmc5883l_mode_t;

typedef enum {
	qmc5883lRange2G = 0,
	qmc5883lRange8G = 1
} qmc5883l_range_t;

typedef enum {
	qmc5883lOutputDataRate10HZ	= 0,
	qmc5883lOutputDataRate50HZ	= 1,
	qmc5883lOutputDataRate100HZ	= 2,
	qmc5883lOutputDataRate200HZ	= 3
} qmc5883l_output_data_rate_t;

typedef enum {
	qmc5883lOversampleRatio512	= 0,
	qmc5883lOversampleRatio256	= 1,
	qmc5883lOversampleRatio128	= 2,
	qmc5883lOversampleRatio64		= 3
} qmc5883l_over_sample_ratio_t;

#ifdef __cplusplus
extern "C" {
#endif
void qmc5883l_init();
//DoubleVector qmc5883l_read();
//int16_t qmc5883l_read_temperature();
void qmc5883l_enable_interrupts();
void qmc5883l_disable_interrupts();
void qmc5883l_set_range(qmc5883l_range_t range);
//void qmc5883l_set_output_data_rate(qmc5883l_output_data_rate_t rate);
//void qmc5883l_set_mode(qmc5883l_mode_t mode);
//void qmc5883l_set_over_sample_ratio(qmc5883l_over_sample_ratio_t ratio);
void qmc5883l_set_config(qmc5883l_mode_t mode,
		qmc5883l_output_data_rate_t odr,
		qmc5883l_range_t range,
		qmc5883l_over_sample_ratio_t osr);
bool qmc5883l_data_ready();
bool qmc5883l_data_overflow();
bool qmc5883l_data_skipped();
void qmc5883l_soft_reset();
#ifdef __cplusplus
}
#endif
#endif
