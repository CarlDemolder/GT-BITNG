#ifndef SPI_H_
#define SPI_H_

#include "common.h"
#include "nrfx_spim.h"

#define SPIM_READ_ADDRESS(x) ((x << 1) | 1) // x is the register address  
#define SPIM_WRITE_ADDRESS(x) ((x << 1) | 0) // x is the register address  

/* SPIM instance ID. */
#define SPIM_INSTANCE_ID     2

enum SPIM_CONSTANTS
{
    SPIM_DELAY = 1,
    SPIM_TIMEOUT = 1000
};

/**@brief SPIM Configuration Structure. This structure contains all values for the SPIM Configuration.*/
struct SPIM_Configuration
{
    uint32_t timeout;
    uint8_t timeout_flag;
    volatile bool spim_xfer_done;
    nrfx_spim_t nrfx_spim;
    nrfx_spim_config_t nrfx_spim_config;
    nrfx_spim_xfer_desc_t nrfx_spim_xfer_desc;
    ret_code_t error_code;
};

static struct SPIM_Configuration spim_configuration;

uint8_t get_spi_timeout();

void spim_init(void);
void spim_uninit(void);

void spim_write_registers(uint8_t start_register_address, uint8_t *data_array, uint8_t data_array_size);
void spim_read_registers(uint8_t start_register_address, uint8_t *data_array, uint8_t data_array_size);
  
void spim_stop(void);
void spim_start(void);

void spim_enable(void);
void spim_disable(void);

#endif // __SPI_H