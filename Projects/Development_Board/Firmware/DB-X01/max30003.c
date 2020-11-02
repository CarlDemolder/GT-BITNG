#include "max30003.h"

void max30003_read_device_info(void) 
{
    NRF_LOG_INFO("max30003_read_device_info");
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_INFO_ADDRESS, data_array);

    NRF_LOG_INFO("Device Information:");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

void max30003_read_device_status(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Status \r\n");
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_STAT_ADDRESS, data_array);

    NRF_LOG_INFO("Device Status: \r\n");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

void max30003_init(void) 
{
    max30003_soft_reset();        // Soft Reset
    max30003_init_interrupt();    // Initializing the interrupt pin

    // Interrupts and Dynamic Modes
    spim_write_single_register(MAX30003_EN_INT_ADDRESS, 0x80, 0x00, 0x03);      // 0x02
    spim_write_single_register(MAX30003_EN_INT2_ADDRESS, 0x00, 0x00, 0x03);     // 0x03
    spim_write_single_register(MAX30003_MNGR_INT_ADDRESS, 0x78, 0x00, 0x04);    // 0x04
    spim_write_single_register(MAX30003_MNGR_DYN_ADDRESS, 0x3f, 0x00, 0x00);    // 0x05

    // Set up ECG Configurations:
    spim_write_single_register(MAX30003_CNFG_GEN_ADDRESS, 0x08, 0x10, 0x07);    // 0x10
    spim_write_single_register(MAX30003_CNFG_CAL_ADDRESS, 0x72, 0x00, 0x00);    // 0x12
    spim_write_single_register(MAX30003_CNFG_EMUX_ADDRESS, 0x0B, 0x00, 0x00);   // 0x14
    spim_write_single_register(MAX30003_CNFG_ECG_ADDRESS, 0x80, 0x50, 0x00);    // 0x15
    spim_write_single_register(MAX30003_CNFG_RTOR1_ADDRESS, 0x3f, 0x35, 0x00);  // 0x1D - RR DETECT DISABLED

    max30003_enable_interrupt();    // Enable the interrupt of the NRF52
    max30003_sync();            // Synchronize the clock frequency of the MAX30003 with the NRF52
}

void max30003_readback_registers(void) 
{
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_EN_INT_ADDRESS, data_array);
    spim_read_single_register(MAX30003_EN_INT2_ADDRESS, data_array);
    spim_read_single_register(MAX30003_MNGR_INT_ADDRESS, data_array);

    spim_read_single_register(MAX30003_CNFG_GEN_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_CAL_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_EMUX_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_ECG_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_RTOR1_ADDRESS, data_array);
}

void max30003_soft_reset(void) 
{
    NRF_LOG_INFO("max_30003_soft_reset");
    spim_write_single_register(MAX30003_SW_RST_ADDRESS, 0x00, 0x00, 0x00);
    nrf_delay_ms(5); // Delay the MCU to allow the MAX30003 to go through a soft reset
}

void max30003_sync(void) 
{
    NRF_LOG_INFO("max30003_sync");
    spim_write_single_register(MAX30003_SYNCH_ADDRESS, 0x00, 0x00, 0x00);
}

void max30003_read_fifo_data(uint8_t* data_array) 
{
    NRF_LOG_INFO("max30003_read_fifo_data");
    uint8_t register_address = MAX30003_ECG_FIFO_BURST_ADDRESS;
    uint8_t register_data[52];

    spim_read_registers(register_address, register_data, sizeof(register_data)); 

    NRF_LOG_INFO("[MAX30003] FIFO READ DATA: \r\n");
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data)); // Hex Dump Output (temp)
}

void max30003_init_interrupt(void)
{
    NRF_LOG_INFO("max30003_init_interrupt");
    nrf_gpio_pin_dir_set(MAX30003_INT1_PIN, NRF_GPIO_PIN_DIR_INPUT);
    nrfx_gpiote_in_config_t max30003_interrupt_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    ret_code_t error_code = nrfx_gpiote_in_init(MAX30003_INT1_PIN, &max30003_interrupt_config, max30003_interrupt_handler);
    
    NRF_LOG_INFO(" nrfx_gpiote_in_init: %d: \r\n", error_code);
    APP_ERROR_CHECK(error_code);
}

void max30003_enable_interrupt(void)
{
    NRF_LOG_INFO("max30003_enable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, true);
}

void max30003_disable_interrupt(void)
{
    NRF_LOG_INFO("max30003_disable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, true);
}

void max30003_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("max30003_interrupt_handler");
    UNUSED_PARAMETER(pin);
    UNUSED_PARAMETER(action);
}
