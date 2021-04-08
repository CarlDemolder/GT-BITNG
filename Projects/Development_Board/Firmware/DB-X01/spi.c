#include "spi.h"

#if SPI

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
//    NRF_LOG_INFO("SPI_HANDLER Event Occured");
    spim_configuration.timeout = SPIM_TIMEOUT;
    switch (p_event->type)
    {
        case NRFX_SPIM_EVENT_DONE:
        {
//            NRF_LOG_INFO("NRFX SPIM EVENT DONE");
            nrf_gpio_pin_write(spim_configuration.cs_pin, 1);   // Setting the Chip Select Pin high to deactivate
            spim_configuration.spim_xfer_done = true;
            break;
        }

        default:
            break;
    }
}

void spim_init_cs_pin(uint8_t spim_cs_pin)
{
    NRF_LOG_INFO("spim_init_cs_pin");

    nrf_gpio_cfg_output(spim_cs_pin);                  
    nrf_gpio_pin_write(spim_cs_pin, 1);
}

void spim_select_cs_pin(uint8_t cs_pin)
{
    NRF_LOG_INFO("spim_select_cs_pin");

    spim_configuration.cs_pin = cs_pin;
    NRF_LOG_INFO("SPIM Select Chip Select: %u", spim_configuration.cs_pin);
}

void spim_setup(void)
{
    NRF_LOG_INFO("spim_setup");

    spim_configuration.nrfx_spim_config = (nrfx_spim_config_t) NRFX_SPIM_DEFAULT_CONFIG;

    spim_configuration.nrfx_spim_config.mode = NRF_SPIM_MODE_0;
    spim_configuration.nrfx_spim_config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    spim_configuration.nrfx_spim_config.frequency = NRF_SPIM_FREQ_1M;
    spim_configuration.nrfx_spim_config.irq_priority = APP_IRQ_PRIORITY_HIGHEST;
    spim_configuration.nrfx_spim_config.miso_pin = SPI_MISO_PIN;
    spim_configuration.nrfx_spim_config.sck_pin = SPI_CLK_PIN;
    spim_configuration.nrfx_spim_config.mosi_pin = SPI_MOSI_PIN;
    spim_configuration.nrfx_spim_config.ss_pin = NRFX_SPIM_PIN_NOT_USED;
    spim_configuration.nrfx_spim_config.orc = 0x55;

    const nrfx_spim_t m_spim_2 = NRFX_SPIM_INSTANCE(SPIM_INSTANCE_ID);
    spim_configuration.nrfx_spim = m_spim_2;

    spim_configuration.initialization = 0;
}

void spim_init(void) 
{
    if(spim_configuration.initialization == 0)
    {       
        NRF_LOG_INFO("spim_init");

        spim_configuration.error_code = nrfx_spim_init(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_config, spim_event_handler, NULL);
        APP_ERROR_CHECK(spim_configuration.error_code);
        nrf_delay_ms(SPIM_DELAY);

        NRF_LOG_INFO("SPIM initialized \r\n");

        spim_configuration.timeout = SPIM_TIMEOUT;
        spim_configuration.spim_xfer_done = false; /* Indicates if operation on SPIM has ended. */
        spim_configuration.initialization = 1;
    }
    else
    {
        NRF_LOG_INFO("spim is already initialized");
    }
}

void spim_uninit(void) 
{
    if(spim_configuration.initialization == 1)
    {
        NRF_LOG_INFO("spim_uninit");
        nrfx_spim_uninit(&spim_configuration.nrfx_spim);
        nrf_delay_ms(SPIM_DELAY);
        spim_configuration.initialization = 0;    // Flag used to indicate whether SPIM has been initialized
    }
    else
    {
        NRF_LOG_INFO("spim is already uninitialized");
    }
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
    spim_configuration.nrfx_spim_xfer_desc.tx_length = ARRAY_LENGTH(address_array);
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = data_array;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = data_array_size;

    nrf_gpio_pin_write(spim_configuration.cs_pin, 0);   // Setting the Chip Select Pin low to activate
    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);

    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false));
    nrf_delay_ms(SPIM_DELAY);
}

void spim_write_registers(uint8_t start_register_address, uint8_t *data_array, uint8_t data_array_size)
{
    NRF_LOG_INFO("spim_write_registers");
    uint8_t register_data[data_array_size+1];
    register_data[0] = SPIM_WRITE_ADDRESS(start_register_address);
    for(uint8_t i = 1; i <= data_array_size; i++)
    {
        register_data[i] = data_array[i-1];
    }

    // SPI Command
    spim_configuration.spim_xfer_done = false;

    spim_configuration.nrfx_spim_xfer_desc.p_tx_buffer = register_data;
    spim_configuration.nrfx_spim_xfer_desc.tx_length = ARRAY_LENGTH(register_data);
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = NULL;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = NULL;

    nrf_gpio_pin_write(spim_configuration.cs_pin, 0);   // Setting the Chip Select Pin low to activate
    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);

    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false)); 
    nrf_delay_ms(SPIM_DELAY);
}

void spim_write_data_array(uint8_t *data_array, uint8_t data_array_size)
{
    NRF_LOG_INFO("spim_write_data_array");

    // SPI Command
    spim_configuration.spim_xfer_done = false;

    spim_configuration.nrfx_spim_xfer_desc.p_tx_buffer = data_array;
    spim_configuration.nrfx_spim_xfer_desc.tx_length = data_array_size;
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = NULL;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = NULL;

    nrf_gpio_pin_write(spim_configuration.cs_pin, 0);   // Setting the Chip Select Pin low to activate
    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);
    
    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false)); 
    nrf_delay_ms(SPIM_DELAY);
}

void spim_read_data_array(uint8_t *write_data_array, uint8_t write_data_array_size, uint8_t *read_data_array, uint8_t read_data_array_size)
{
    NRF_LOG_INFO("spim_read_data_array");

    // SPI Command
    spim_configuration.spim_xfer_done = false;

    spim_configuration.nrfx_spim_xfer_desc.p_tx_buffer = write_data_array;
    spim_configuration.nrfx_spim_xfer_desc.tx_length = write_data_array_size;
    spim_configuration.nrfx_spim_xfer_desc.p_rx_buffer = read_data_array;
    spim_configuration.nrfx_spim_xfer_desc.rx_length = read_data_array_size;

    nrf_gpio_pin_write(spim_configuration.cs_pin, 0);   // Setting the Chip Select Pin low to activate
    spim_configuration.error_code = nrfx_spim_xfer(&spim_configuration.nrfx_spim, &spim_configuration.nrfx_spim_xfer_desc, 0);
    APP_ERROR_CHECK(spim_configuration.error_code);
    
    while ((spim_configuration.spim_xfer_done == false) && (_spim_timeout_handler() == false)); 
    nrf_delay_ms(SPIM_DELAY);
}

#endif