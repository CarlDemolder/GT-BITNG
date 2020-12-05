#include "storage.h"


void storage_init(void)
{
//    uint8_t max30003_power_ldo_enable_array_data[4] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_MAX30003_POWER_LDO_EN};
//    nrf52_handler(max30003_power_ldo_enable_array_data); // Enable 1.8V Supply for MAX30003

    uint8_t spim_enable_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    nrf52_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_set_cs_pin_array_data[5] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SET_CS_PIN, CY15B108QI_CS_PIN};
    nrf52_handler(spim_set_cs_pin_array_data); // Set Chip Select Pin to MAX30003 for the SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    nrf52_handler(spim_init_array_data); // Initialize SPIM Module
 
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t cy15b108qi_init_array_data[4] = {0x00, 0x00, CY15B108QI_INIT_COMMAND}; // CY15B108QI: Init Command
    cy15b108qi_handler(cy15b108qi_init_array_data);     
           
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    nrf52_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    nrf52_handler(spim_disable_array_data); // Disable SPIM Module

//    uint8_t power_deep_sleep_array_data[4] = {0x00, 0x00, NRF52_POWER_COMMAND, NRF52_POWER_DEEP_SLEEP_MODE_ENTER};
//    nrf52_handler(power_deep_sleep_array_data); // Enter System Off Mode (Deep Sleep)
}

void storage_interrupt_handler(void)
{
    uint8_t spim_enable_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    nrf52_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    nrf52_handler(spim_init_array_data); // Initialize SPIM Module
 
    uint8_t read_fifo_memory_array_data[3] = {0x00, 0x00, MAX30003_READ_ECG_FIFO_MEMORY_COMMAND};
    max30003_handler(read_fifo_memory_array_data);

//    uint8_t max30003_data_array[MAX30003_MAX_FIFO_WORDS];
//    max30003_get_ecg_voltage(max30003_data_array, ARRAY_LENGTH(max30003_data_array));
//    ft201x_write_buffer(max30003_data_array, ARRAY_LENGTH(max30003_data_array));

    uint8_t spim_uninit_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    nrf52_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, 0x00, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    nrf52_handler(spim_disable_array_data); // Disable SPIM Module

//    uint8_t power_deep_sleep_array_data[4] = {0x00, 0x00, NRF52_POWER_COMMAND, NRF52_POWER_DEEP_SLEEP_MODE_ENTER};
//    nrf52_handler(power_deep_sleep_array_data); // Enter System Off Mode (Deep Sleep)

}