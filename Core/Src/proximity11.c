#include "proximity11.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32l4xx_hal.h"
static uint8_t als_data0_gain;
static uint8_t als_data1_gain;
static uint16_t als_measure_time;
uint8_t hal_i2c_master_write_then_read(I2C_HandleTypeDef *handle, uint8_t *write_data_buf, size_t len_write_data,
                                     uint8_t *read_data_buf, size_t len_read_data) {
    HAL_StatusTypeDef status;
    I2C_HandleTypeDef *hi2c = handle;

    if (!hi2c) {
        return HAL_I2C_STATE_ERROR;
    }

    if (len_read_data <= 0 || len_write_data <= 0) {
        return HAL_I2C_STATE_ERROR;
    }

    if (!read_data_buf || !write_data_buf) {
        return HAL_I2C_STATE_ERROR;
    }

    status = HAL_I2C_Master_Transmit(hi2c, (uint16_t)PROXIMITY11_DEVICE_ADDRESS << 1, write_data_buf, len_write_data, HAL_MAX_DELAY);

    if (status != HAL_OK) {
        return HAL_I2C_STATE_ERROR;
    }

    status = HAL_I2C_Master_Receive(hi2c, (uint16_t)PROXIMITY11_DEVICE_ADDRESS << 1, read_data_buf, len_read_data, HAL_MAX_DELAY);

    if (status != HAL_OK) {
        return HAL_I2C_STATE_ERROR;
    }

    return HAL_I2C_STATE_TIMEOUT;
}


uint8_t proximity11_get ( I2C_HandleTypeDef *handle, uint8_t register_address, uint8_t * output_buffer, uint8_t n_bytes )
{
    if ( n_bytes > 22 )
    {
        return 1;
    }

    hal_i2c_master_write_then_read( handle, &register_address, 1,  output_buffer, n_bytes );

    return 0;
}

uint8_t proximity11_set ( I2C_HandleTypeDef *handle, uint8_t register_address, uint8_t * input_buffer, uint8_t n_bytes )
{
    if ( n_bytes > 22 )
    {
        return 1;
    }
    I2C_HandleTypeDef *hi2c = handle;
    uint8_t tx_buf[ 256 ];
    uint8_t cnt;

	tx_buf[ 0 ] = register_address;

	for ( cnt = 1; cnt <= n_bytes; cnt++ )
	{
		tx_buf[ cnt ] = input_buffer[ cnt - 1 ];
	}
	HAL_I2C_Master_Transmit(hi2c, (uint16_t)PROXIMITY11_DEVICE_ADDRESS << 1, tx_buf, n_bytes + 1, HAL_MAX_DELAY);

    return 0;
}
float proximity11_convert_lx ( I2C_HandleTypeDef *handle, uint16_t *input_data )
{
    float lx;
    float d0;
    float d1;
    float d1_d0;

    if ( als_data0_gain == 0 )
    {
        return ( -1.0 );
    }

    if ( als_data1_gain == 0 )
    {
        return ( -1.0 );
    }

    if ( als_measure_time == 0 )
    {
        return ( -1.0 );
    }
    else if ( als_measure_time == 50 )
    {
        if ( ( input_data[ 0 ] & 0x8000 ) == 0x8000 )
        {
            input_data[ 0 ] = 0x7FFF;
        }
        if ( ( input_data[ 1 ] & 0x8000 ) == 0x8000 )
        {
            input_data[ 1 ] = 0x7FFF;
        }
    }

    d0  = ( float )( input_data[ 0 ] );
    d0 *= ( float )( 100.0 / als_measure_time  );
    d0 /= ( float )( als_data0_gain);

    d1  = ( float )( input_data[ 1 ] );
    d1 *= ( float )( 100.0 / als_measure_time );
    d1 /= ( float )( als_data1_gain );


    if ( d0 == 0.0 )
    {
        lx = 0.0;
        return ( lx );
    }

    d1_d0 = d1 / d0;

    if ( d1_d0 < 0.595 )
    {
        lx = ( 1.682 * d0 - 1.877 * d1 );
    }
    else if ( d1_d0 < 1.015 )
    {
        lx = ( 0.644 * d0 - 0.132 * d1 );
    }
    else if ( d1_d0 < 1.352 )
    {
        lx = ( 0.756 * d0 - 0.243 * d1 );
    }
    else if ( d1_d0 < 3.053 )
    {
        lx = ( 0.766 * d0 - 0.25 * d1 );
    }
    else
    {
        lx = 0.0;
    }

    return ( lx );
}
void proximity11_update ( I2C_HandleTypeDef *handle )
{
    uint8_t aux_reg_addr;
    uint8_t aux_reg_val;

    uint8_t index;
    uint8_t als_gain_table[ 5 ] = { 1, 2, 64, 128 };
    uint16_t als_meas_time_table[ 17 ] = { 0, 0, 0, 0, 0, 100, 100, 100, 400, 400, 400, 400, 50, 0, 0, 0 };

    aux_reg_addr = PROXIMITY11_MODE_CTRL;
    proximity11_get( handle, aux_reg_addr, &aux_reg_val, 1 );

    index = aux_reg_val & 0x0F;
    als_measure_time = als_meas_time_table[ index ];

    aux_reg_addr = PROXIMITY11_ALS_PS_CTRL;
    proximity11_get( handle, aux_reg_addr, &aux_reg_val, 1 );

    index = ( aux_reg_val >> 4 ) & 0x03;
    als_data0_gain = als_gain_table[ index ];

    index = ( aux_reg_val >> 2 ) & 0x03;
    als_data1_gain = als_gain_table[ index ];
}
uint8_t proximity11_default_cfg ( I2C_HandleTypeDef *handle )
{
    uint8_t aux_reg_addr;
    uint8_t aux_reg_val;

    aux_reg_addr = PROXIMITY11_SYS_CTRL;
    proximity11_get( handle, aux_reg_addr, &aux_reg_val, 1 );

    if ( ( aux_reg_val & 0x3F ) != PROXIMITY11_PART_ID_VAL )
    {
        return 1;
    }
    aux_reg_addr = PROXIMITY11_MANUFACT_ID;
    proximity11_get( handle, aux_reg_addr, &aux_reg_val, 1 );

    if ( aux_reg_val != PROXIMITY11_MANUFACT_ID_VAL)
    {
        return 1;
    }



    aux_reg_addr = PROXIMITY11_ALS_PS_CTRL;
	aux_reg_val = PROXIMITY11_ALS_PS_CONTROL_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	aux_reg_addr = PROXIMITY11_PS_CTRL;
	aux_reg_val |= PROXIMITY11_PS_CONTROL_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

    aux_reg_addr = PROXIMITY11_MODE_CTRL;
    aux_reg_val = PROXIMITY11_MODE_CONTROL_VAL;
    proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

    aux_reg_addr = PROXIMITY11_INTERRUPT;
	aux_reg_val = PROXIMITY11_INTERRUPT_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	// CONFIGURE PS THRESHHOLD VALUES
	aux_reg_addr = PROXIMITY11_PS_TH_LSB;
	aux_reg_val = PROXIMITY11_PS_TH_LSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	aux_reg_addr = PROXIMITY11_PS_TH_MSB;
	aux_reg_val = PROXIMITY11_PS_TH_MSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	// CONFIGURE ALS THRESHHOLD VALUES
	aux_reg_addr = PROXIMITY11_ALS_DATA0_TH_LSB;
	aux_reg_val = PROXIMITY11_ALS_DATA0_TH_LSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	aux_reg_addr = PROXIMITY11_ALS_DATA0_TH_MSB;
	aux_reg_val = PROXIMITY11_ALS_DATA0_TH_MSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	aux_reg_addr = PROXIMITY11_ALS_DATA0_TL_LSB;
	aux_reg_val = PROXIMITY11_ALS_DATA0_TL_LSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );

	aux_reg_addr = PROXIMITY11_ALS_DATA0_TL_MSB;
	aux_reg_val = PROXIMITY11_ALS_DATA0_TL_MSB_VAL;
	proximity11_set( handle, aux_reg_addr, &aux_reg_val, 1 );


    proximity11_update( handle );

    return 0;
}
void proximity11_get_raw_ps_als_values ( I2C_HandleTypeDef *handle, uint8_t *data_buffer )
{
    uint8_t aux_reg_addr;

    aux_reg_addr = PROXIMITY11_PS_DATA_LSB;
    proximity11_get( handle, aux_reg_addr, data_buffer, 6 );
}
void proximity11_get_ps_als_values ( I2C_HandleTypeDef *handle, uint16_t *ps_value, float *als_value )
{
    uint8_t ps_als_values[ 6 ];
    uint16_t raw_ps;
    uint16_t raw_als[ 2 ];

    proximity11_get_raw_ps_als_values( handle, ps_als_values );

    raw_ps     = ( ( uint16_t )(ps_als_values[ 1 ]) << 8) | ps_als_values[ 0 ];
    raw_als[ 0 ] = ( ( uint16_t )ps_als_values[ 3 ] << 8 ) | ps_als_values[ 2 ];
    raw_als[ 1 ] = ( ( uint16_t )ps_als_values[ 5 ] << 8 ) | ps_als_values[ 4 ];

    *ps_value  = raw_ps;
    *als_value = proximity11_convert_lx( handle, raw_als );
}



