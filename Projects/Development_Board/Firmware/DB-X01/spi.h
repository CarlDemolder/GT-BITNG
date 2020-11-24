#ifndef SPI_H_
#define SPI_H_

#include "common.h"
#include "nrfx_spim.h"

#define SPIM_READ_ADDRESS(x) ((x << 1) | 1) // x is the register address  
#define SPIM_WRITE_ADDRESS(x) ((x << 1) | 0) // x is the register address  

/* Indicates if operation on SPI has ended. */
volatile bool spim_xfer_done = false;


/**@brief SPIM Configuration Structure. This structure contains all values for the SPIM Configuration.*/
struct SPIM_Configuration
{
    uint32_t timeout;
    volatile bool spim_xfer_done;
    nrfx_spim_t nrfx_spim;
    nrfx_spim_config_t spim_config;

    uint8_t timeout_flag;
    nrfx_twim_t nrfx_twim;
    nrfx_twim_config_t twim_config;
    nrfx_twim_xfer_desc_t twim_xfer_desc;
    uint32_t nrfx_twim_xfer_flag;
    ret_code_t error_code;
};

static struct SPIM_Configuration spim_configuration;


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