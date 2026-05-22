#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

/** DEVICE ADDRESS */
#define PROXIMITY11_DEVICE_ADDRESS     0x38


/** REGISTERS */
#define PROXIMITY11_SYS_CTRL           0x40		//RW
#define PROXIMITY11_MODE_CTRL          0x41		//RW
#define PROXIMITY11_ALS_PS_CTRL        0x42		//RW
#define PROXIMITY11_PS_CTRL            0x43		//RW
#define PROXIMITY11_PS_DATA_LSB        0x44		//R
#define PROXIMITY11_PS_DATA_MSB        0x45		//R
#define PROXIMITY11_ALS_DATA0_LSB      0x46		//R
#define PROXIMITY11_ALS_DATA0_MSB      0x47		//R
#define PROXIMITY11_ALS_DATA1_LSB      0x48		//R
#define PROXIMITY11_ALS_DATA1_MSB      0x49		//R
#define PROXIMITY11_INTERRUPT          0x4A		//RW
#define PROXIMITY11_PS_TH_LSB          0x4B		//RW
#define PROXIMITY11_PS_TH_MSB          0x4C		//RW
#define PROXIMITY11_PS_TL_LSB          0x4D		//RW
#define PROXIMITY11_PS_TL_MSB          0x4E		//RW
#define PROXIMITY11_ALS_DATA0_TH_LSB   0x4F		//RW
#define PROXIMITY11_ALS_DATA0_TH_MSB   0x50		//RW
#define PROXIMITY11_ALS_DATA0_TL_LSB   0x51		//RW
#define PROXIMITY11_ALS_DATA0_TL_MSB   0x52		//RW
#define PROXIMITY11_PS_OFFSET_LSB      0x53		//RW
#define PROXIMITY11_PS_OFFSET_MSB      0x54		//RW
#define PROXIMITY11_MANUFACT_ID        0x92		//R
#define PROXIMITY11_PART_ID_VAL		   0x0A
#define PROXIMITY11_MANUFACT_ID_VAL    0xE0


// DEFAULT SETUP OF SENSOR
#define PROXIMITY11_MODE_CONTROL_MEASTIME_100_100MS  (6 << 0)
#define PROXIMITY11_MODE_CONTROL_MEASTIME_400_100MS  (9 << 0)
//#define PROXIMITY11_MODE_CONTROL_PS_EN               (1 << 6)
//#define PROXIMITY11_MODE_CONTROL_ALS_EN              (1 << 7)

//#define PROXIMITY11_ALS_PS_CONTROL_LED_CURRENT_100MA (2 << 0)
//#define PROXIMITY11_ALS_PS_CONTROL_DATA1_GAIN_X1     (0 << 2)
//#define PROXIMITY11_ALS_PS_CONTROL_DATA0_GAIN_X1     (0 << 4)

//#define PROXIMITY11_PS_CONTROL_PS_GAINX1             (0 << 4)

//#define PROXIMITY11_MODE_CONTROL_VAL (PROXIMITY11_MODE_CONTROL_MEASTIME_100_100MS | PROXIMITY11_MODE_CONTROL_PS_EN | PROXIMITY11_MODE_CONTROL_ALS_EN)
//#define PROXIMITY11_ALS_PS_CONTROL_VAL (PROXIMITY11_ALS_PS_CONTROL_DATA0_GAIN_X1 | PROXIMITY11_ALS_PS_CONTROL_DATA1_GAIN_X1 | PROXIMITY11_ALS_PS_CONTROL_LED_CURRENT_100MA)
//#define PROXIMITY11_PS_CONTROL_VAL (PROXIMITY11_PS_CONTROL_PS_GAINX1)

// ADVANCED SETUP OF SENSOR
#define PROXIMITY11_MODE_CONTROL_MEASTIME_400_400MS  (11 << 0)
#define PROXIMITY11_MODE_CONTROL_PS_EN               (1 << 6)
#define PROXIMITY11_MODE_CONTROL_ALS_EN              (1 << 7)
#define PROXIMITY11_MODE_CONTROL_VAL (PROXIMITY11_MODE_CONTROL_MEASTIME_400_100MS | PROXIMITY11_MODE_CONTROL_PS_EN | PROXIMITY11_MODE_CONTROL_ALS_EN)

#define PROXIMITY11_ALS_PS_CONTROL_LED_CURRENT_200MA (3 << 0)
#define PROXIMITY11_ALS_PS_CONTROL_DATA1_GAIN_X1     (0 << 2)
#define PROXIMITY11_ALS_PS_CONTROL_DATA0_GAIN_X1     (0 << 4)
#define PROXIMITY11_ALS_PS_CONTROL_VAL (PROXIMITY11_ALS_PS_CONTROL_DATA0_GAIN_X1 | PROXIMITY11_ALS_PS_CONTROL_DATA1_GAIN_X1 | PROXIMITY11_ALS_PS_CONTROL_LED_CURRENT_200MA)

#define PROXIMITY11_PS_CONTROL_PS_GAINX1             (0 << 4)
#define PROXIMITY11_PS_CONTROL_VAL (PROXIMITY11_PS_CONTROL_PS_GAINX1)


#define PROXIMITY11_PS_INT_STATUS	(1 << 7)	//R
#define PROXIMITY11_ALS_INT_STATUS	(1 << 6)	//R
#define PROXIMITY11_INT_MODE	(0 << 4)
#define PROXIMITY11_INT_ASSERT	(0 << 3)
#define PROXIMITY11_INT_LATCH	(1 << 2)
#define PROXIMITY11_INT_TRIG	(3 << 0)
#define PROXIMITY11_INTERRUPT_VAL (PROXIMITY11_INT_MODE | PROXIMITY11_INT_ASSERT | PROXIMITY11_INT_LATCH | PROXIMITY11_INT_TRIG)

#define PROXIMITY11_PS_TH_MSB_VAL	0x00
#define PROXIMITY11_PS_TH_LSB_VAL	0x08

#define PROXIMITY11_ALS_DATA0_TH_LSB_VAL 0x32
#define PROXIMITY11_ALS_DATA0_TH_MSB_VAL 0x00
#define PROXIMITY11_ALS_DATA0_TL_LSB_VAL 0x00
#define PROXIMITY11_ALS_DATA0_TL_MSB_VAL 0x00
uint8_t hal_i2c_master_write_then_read(I2C_HandleTypeDef *handle, uint8_t *write_data_buf, size_t len_write_data,
                                     uint8_t *read_data_buf, size_t len_read_data);


uint8_t proximity11_get ( I2C_HandleTypeDef *handle, uint8_t register_address, uint8_t * output_buffer, uint8_t n_bytes );

uint8_t proximity11_set ( I2C_HandleTypeDef *handle, uint8_t register_address, uint8_t * input_buffer, uint8_t n_bytes );

float proximity11_convert_lx ( I2C_HandleTypeDef *handle, uint16_t *input_data );

void proximity11_update ( I2C_HandleTypeDef *handle );

uint8_t proximity11_default_cfg ( I2C_HandleTypeDef *handle );

void proximity11_get_raw_ps_als_values ( I2C_HandleTypeDef *handle, uint8_t *data_buffer );

void proximity11_get_ps_als_values ( I2C_HandleTypeDef *handle, uint16_t *ps_value, float *als_value );



