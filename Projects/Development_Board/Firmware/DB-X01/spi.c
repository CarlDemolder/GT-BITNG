#include "spi.h"

static uint8_t _spim_timeout_handler()
{
    if(spim_configuration.timeout == 0)
    {
        NRF_LOG_INFO("_spim_timeout_handler()");
        spim_configuration.timeout_flag = 1;
        spim_configuration.timeout = SPIM_TIMEOUT;
        return 1;
    }
    --spim_configuration.timeout;
    return 0;
}

uint8_t get_spi_timeout()
{
    NRF_LOG_INFO("get_spi_timeout");    
    uint8_t temp_timeout_flag = 1;
    spim_configuration.timeout_flag = 0;
    return temp_timeout_flag;
}

void spim_event_handler(nrfx_spim_evt_t const *p_event, void *p_context) 
{
    NRF_LOG_INFO("SPI_HANDLER Event Occured");
    spim_configuration.timeout = SPIM_TIMEOUT;
    switch (p_event->type)
    {
        case NRFX_SPIM_EVENT_DONE:
            NRF_LOG_INFO("NRFX SPIM EVENT DONE");
            spim_configuration.spim_xfer_done = true;
            break;
        default:
            break;
    }
}

void spim_init(void) 
{
    NRF_LOG_INFO("spim_init");

    spim_configuration.nrfx_spim_config = (nrfx_spim_config_t) NRFX_SPIM_DEFAULT_CONFIG;

    spim_configuration.nrfx_spim_config.mode = NRF_SPIM_MODE_0;
    spim_configuration.nrfx_spim_config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    spim_configuration.nrfx_spim_config.frequency = NRF_SPIM_FREQ_1M;
    spim_configuration.nrfx_spim_config.irq_priority = APP_IRQ_PRIORITY_HIGHEST;
    spim_configuration.nrfx_spim_config.miso_pin = SPI_MISO_PIN;
    spim_configuration.nrfx_spim_config.sck_pin = SPI_CLK_PIN;
    spim_configuration.nrfx_spim_config.mosi_pin = SPI_MOSI_PIN;
    spim_configuration.nrfx_spim_config.ss_pin = MAX30003_CS_PIN;
    spim_configuration.nrfx_spim_config.orc = 0x55;

    const nrfx_spim_t m_spim_2 = NRFX_SPIM_INSTANCE(SPIM_INSTANCE_ID);
    spim_configuration.nrfx_spim = m_spim_2;

    spim_configuration.error_code = nrfx_spim_init(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_config, spim_event_handler, NULL);
    APP_ERROR_CHECK(spim_configuration.error_code);
    NRF_LOG_INFO("SPIM initialized \r\n");

    spim_configuration.timeout = SPIM_TIMEOUT;
    spim_configuration.spim_xfer_done = false; /* Indicates if operation on SPIM has ended. */
}

void spim_uninit(void) 
{
    NRF_LOG_INFO("spim_uninit");
    nrfx_spim_uninit(&spim_configuration.nrfx_spim);
}

void spim_enable(void)
{
    NRF_LOG_INFO("spim_enable");
    if(NRF_SPI2->ENABLE == 0)
    {
        NRF_LOG_INFO("SPIM_ENABLE");
        NRF_SPIM2->ENABLE = 1;
    }
}

void spim_disable(void)
{
    NRF_LOG_INFO("spim_disable");
    if(NRF_SPI2->ENABLE == 1)
    {
        NRF_LOG_INFO("SPIM_DISABLE");
        NRF_SPIM2->ENABLE = 0;
    }
}

void spim_read_registers(uint8_t start_register_address, uint8_t *data_array, uint8_t data_array_size) 
{
    NRF_LOG_INFO("spim_read_registers");
    uint8_t address_array[1] = {SPIM_READ_ADDRESS(start_register_address)};
    spim_configuration.spim_xfer_done = false;
 
    spim_configuration.nrfx_spim_xfer_desc.p_tx_buffer = address_array;
    spim_configuration.nrfx_spim_xfer_desc.tx_length = sizeof(address_array);
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = data_array;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = data_array_size;

    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);
    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false)); 

    NRF_LOG_INFO("REGISTER DATA @ Address 0x%x: \r\n", start_register_address);
    NRF_LOG_HEXDUMP_INFO(data_array, data_array_size); // Hex Dump Output (temp)
}

void spim_write_registers(uint8_t start_register_address, uint8_t *data_array, uint8_t data_array_size)
{
    NRF_LOG_INFO("spim_write_single_register");
    uint8_t register_data[data_array_size + 1];
    register_data[0] = SPIM_WRITE_ADDRESS(start_register_address);
    for(int i = 1; i < (data_array_size + 1); i++)
    {
        register_data[i] = data_array[i];
    }
    NRF_LOG_HEXDUMP_INFO(register_data, data_array_size);

    // SPI Command
    spim_configuration.spim_xfer_done = false;

    spim_configuration.nrfx_spim_xfer_desc.p_tx_buffer = data_array;
    spim_configuration.nrfx_spim_xfer_desc.tx_length = sizeof(data_array);
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = NULL;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = NULL;

    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);

    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false)); 
    nrf_delay_ms(SPIM_DELAY);
}