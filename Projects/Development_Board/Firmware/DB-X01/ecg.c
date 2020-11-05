#include "ecg.h"


void ecg_initialize(void)
{


}

void ecg_interrupt_handler(void)
{
    uint8_t spim_enable_array_data[2] = {NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    _nrf52_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_array_data[2] = {NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    _nrf52_handler(spim_init_array_data); // Initialize SPIM Module

//    _max30003_handler(serial_array_data);
 
    uint8_t max30003_data_array[52];
    max30003_read_fifo_data(max30003_data_array);
//    write_data(max30003_data_array);

    uint8_t spim_uninit_array_data[2] = {NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    _nrf52_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[2] = {NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    _nrf52_handler(spim_disable_array_data); // Disable SPIM Module

    uint8_t power_deep_sleep_array_data[2] = {NRF52_POWER_COMMAND, NRF52_POWER_DEEP_SLEEP_MODE_ENTER};
    _nrf52_handler(power_deep_sleep_array_data); // Enter System Off Mode (Deep Sleep)

}