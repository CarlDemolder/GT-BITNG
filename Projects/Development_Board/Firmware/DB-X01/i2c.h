#ifndef I2C_H
#define I2C_H

#include "common.h"
#include "nrfx_twim.h"

#define GENERAL_CALL_ADDRESS            0X00                                    /** General Call Address **/

void twim_init(void);
void i2c_write_registers(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size);
void i2c_write_single_register(uint8_t device_address, uint8_t register_address, uint8_t register_value);


void i2c_write_write_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size);
void i2c_write_read_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size); 
void i2c_separate_write_read_registers(uint8_t device_command, uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size); 
void i2c_no_stop_write_registers(uint8_t device_command, uint8_t* first_array_data, uint8_t first_array_data_size);


void i2c_read_registers(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size);
void i2c_read_device(uint8_t slave_address, uint8_t* array_data, uint8_t array_size);
uint8_t i2c_read_single_register(uint8_t device_address, uint8_t register_address); 
void i2c_stop(void);
void i2c_start(void);


#endif // __I2C_H