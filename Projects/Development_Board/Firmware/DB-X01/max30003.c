#include "max30003.h"

uint8_t max30003_default_registers[] = {};

uint8_t max30003_interrupt;

void max30003_read_device_info(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Information \r\n");
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_INFO_ADDRESS, data_array);

    NRF_LOG_INFO("Device Information: \r\n");
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

// Control functions>
void max30003_init(void) 
{
    _max30003_init_interrupt();   // Initializing the interrupt pin

    // Interrupts and Dynamic Modes
    spim_write_single_register(MAX30003_EN_INT_ADDRESS, 0x80, 0x00, 0x03);   //0x02
    spim_write_single_register(MAX30003_EN_INT2_ADDRESS, 0x00, 0x00, 0x03);  //0x03
    spim_write_single_register(MAX30003_MNGR_INT_ADDRESS, 0x78, 0x00, 0x04); //0x04
    spim_write_single_register(MAX30003_MNGR_DYN_ADDRESS, 0x3f, 0x00, 0x00); //0x05

    // Set up ECG Configurations:
    spim_write_single_register(MAX30003_CNFG_GEN_ADDRESS, 0x08, 0x10, 0x07);   //0x10
    spim_write_single_register(MAX30003_CNFG_CAL_ADDRESS, 0x72, 0x00, 0x00);   //0x12
    spim_write_single_register(MAX30003_CNFG_EMUX_ADDRESS, 0x0B, 0x00, 0x00);  //0x14
    spim_write_single_register(MAX30003_CNFG_ECG_ADDRESS, 0x80, 0x50, 0x00);   //0x15
    spim_write_single_register(MAX30003_CNFG_RTOR1_ADDRESS, 0x3f, 0x35, 0x00); //0x1D - RR DETECT DISABLED

    max30003_sync();
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
    NRF_LOG_INFO("[MAX30003] Running software reset \r\n");
    spim_write_single_register(MAX30003_SW_RST_ADDRESS, 0x00, 0x00, 0x00);
    nrf_delay_ms(5); // Give time to start up.
}

void max30003_sync(void) 
{
    NRF_LOG_INFO("[MAX30003] Running Sync/Start \r\n");
    spim_write_single_register(MAX30003_SYNCH_ADDRESS, 0x00, 0x00, 0x00);
}

void max30003_read_fifo_data(uint8_t* data_array) 
{
    uint8_t register_address = MAX30003_ECG_FIFO_BURST_ADDRESS;
    uint8_t register_data[51+1];

    spim_read_registers(register_address, register_data, sizeof(register_data)); 

    NRF_LOG_INFO("[MAX30003] FIFO READ DATA \r\n");
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data)); // Hex Dump Output (temp)
}

static void _max30003_init_interrupt(void)
{
    nrfx_gpiote_in_config_t max30003_in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    max30003_in_config.is_watcher = true;
    max30003_in_config.pull = NRF_GPIO_PIN_NOPULL;

    ret_code_t error_code = nrfx_gpiote_in_init(MAX30003_INT1_PIN, &max30003_in_config, max30003_interrupt_handler);
    
    NRF_LOG_INFO(" nrfx_gpiote_in_init: %d: \r\n", error_code);
    APP_ERROR_CHECK(error_code);

//    nrfx_gpiote_in_event_enable(MAX30003_INT2_PIN, true);
}

void max30003_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    UNUSED_PARAMETER(pin);
    UNUSED_PARAMETER(action);
    NRF_LOG_INFO("MAX30003 Interrupt Handler Triggered");
    max30003_interrupt = 1;
}

uint8_t max30003_read_interrupt()
{
    uint8_t return_value = max30003_interrupt;
    max30003_interrupt = 0;
    return return_value;
}