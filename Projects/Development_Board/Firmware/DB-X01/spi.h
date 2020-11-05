#ifndef SPI_H_
#define SPI_H_

#include "common.h"
#include "nrfx_spim.h"

#define SPIM_READ_ADDRESS(x) ((x << 1) | 1) // x is the register address  
#define SPIM_WRITE_ADDRESS(x) ((x << 1) | 0) // x is the register address  

void spim_init(void);
void spim_uninit(void);
void spim_write_single_register(uint8_t register_address, uint8_t byte_0, uint8_t byte_1, uint8_t byte_2);
void spim_read_single_register(uint8_t register_address, uint8_t *data_array);
void spim_read_registers(uint8_t register_address, uint8_t *data_array, uint8_t array_size);  
void spim_stop(void);
void spim_start(void);

void spim_enable(void);
void spim_disable(void);

#endif // __SPI_H