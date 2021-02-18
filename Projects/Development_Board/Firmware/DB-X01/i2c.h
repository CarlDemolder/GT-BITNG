#ifndef I2C_H
#define I2C_H

#include "common.h"
#include "nrfx_twim.h"

#if I2C

/* TWIM instance ID. */
#define TWIM_INSTANCE_ID     0

enum TWIM_CONSTANTS
{
    TWIM_DELAY = 1, 
    GENERAL_CALL_ADDRESS = 0X00,                                    /** General Call Address **/
    TWIM_TIMEOUT = 10000
};

/**@brief TWIM Configuration Structure. This structure contains all values for the TWIM Configuration.*/
struct TWIM_Control_Struct
{
    uint32_t timeout;
    volatile bool twim_xfer_done;
    uint8_t timeout_flag;
    nrfx_twim_t nrfx_twim;
    nrfx_twim_config_t twim_config;
    nrfx_twim_xfer_desc_t twim_xfer_desc;
    uint32_t nrfx_twim_xfer_flag;
    ret_code_t error_code;
    uint8_t initialization;
};

uint8_t get_i2c_timeout();
void twim_uninit(void);
void twim_setup(void);
void twim_init(void);
void twim_enable(void);
void twim_disable(void);
void i2c_write_registers(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size);

void i2c_write_single_register(uint8_t device_address, uint8_t register_address, uint8_t *register_value, uint8_t register_byte_count);
void i2c_read_single_register(uint8_t slave_address, uint8_t register_address, uint8_t *register_data, uint8_t register_byte_count);

void i2c_write_write_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size);
void i2c_write_read_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size); 

void i2c_separate_write_read_register(uint8_t slave_address, uint8_t *write_data, uint8_t write_data_length, uint8_t *read_data, uint8_t read_data_length);
void i2c_no_stop_write_register(uint8_t slave_address, uint8_t *primary_data, uint8_t primary_data_length);


void i2c_read_registers(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size);
void i2c_read_device(uint8_t slave_address, uint8_t* array_data, uint8_t array_size);

#endif

#endif // __I2C_H