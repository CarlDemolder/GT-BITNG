#include "spi.h"

/* SPIM instance ID. */
#define SPIM_INSTANCE_ID     2

/* Indicates if operation on SPI has ended. */
static volatile bool spim_xfer_done = false;


/* SPIM instance. */
const nrfx_spim_t m_spim_2 = NRFX_SPIM_INSTANCE(SPIM_INSTANCE_ID);

void spim_event_handler(nrfx_spim_evt_t const *p_event, void *p_context) 
{
    NRF_LOG_INFO("SPI_HANDLER Event Occured");
    switch (p_event->type)
    {
        case NRFX_SPIM_EVENT_DONE:
            NRF_LOG_INFO("NRFX SPIM EVENT DONE");
            spim_xfer_done = true;
            break;
        default:
            break;
    }
}

void spim_init(void) 
{
    nrfx_spim_config_t spim_config = NRFX_SPIM_DEFAULT_CONFIG;
    spim_config.mode = NRF_SPIM_MODE_0;
    spim_config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    spim_config.frequency = NRF_SPIM_FREQ_1M;
    spim_config.irq_priority = APP_IRQ_PRIORITY_HIGHEST;
    spim_config.miso_pin = SPI_MISO_PIN;
    spim_config.sck_pin = SPI_CLK_PIN;
    spim_config.mosi_pin = SPI_MOSI_PIN;
    spim_config.ss_pin = MAX30003_CS_PIN;
    spim_config.orc = 0x55;

    ret_code_t error_code = nrfx_spim_init(&m_spim_2, &spim_config, spim_event_handler, NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_INFO("SPIM initialized \r\n");
}

void spim_uninit(void) 
{
    nrfx_spim_uninit(&m_spim_2);
    NRF_LOG_INFO("SPIM uninitialized \r\n");
}

void spim_enable(void)
{
    if(NRF_SPI2->ENABLE == 0)
    {
        NRF_LOG_INFO("SPIM_ENABLE");
        NRF_SPIM2->ENABLE = 1;
    }
}

void spim_disable(void)
{
    if(NRF_SPI2->ENABLE == 1)
    {
        NRF_LOG_INFO("SPIM_DISABLE");
        NRF_SPIM2->ENABLE = 0;
    }
}

void spim_read_single_register(uint8_t register_address, uint8_t *data_array) 
{
    uint8_t address_array[1] = {SPIM_READ_ADDRESS(register_address)};
    uint8_t register_data[4] = {0, 0, 0, 0};
    spim_xfer_done = false;
    
    nrfx_spim_xfer_desc_t transfer_buffer; 
    transfer_buffer.p_tx_buffer = address_array;
    transfer_buffer.tx_length = sizeof(address_array);
    transfer_buffer.p_rx_buffer = register_data;
    transfer_buffer.rx_length = sizeof(register_data);

    ret_code_t error_code = nrfx_spim_xfer(&m_spim_2, &transfer_buffer, 0);
    APP_ERROR_CHECK(error_code);
    while (spim_xfer_done == false); 

    NRF_LOG_INFO("REGISTER DATA @ Address 0x%x: \r\n", register_address);
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data)); // Hex Dump Output (temp)

    memcpy_fast(data_array, &register_data[1], 3);
}

void spim_read_registers(uint8_t register_address, uint8_t *data_array, uint8_t array_size) 
{
    uint8_t address_array[1] = {SPIM_READ_ADDRESS(register_address)};
    spim_xfer_done = false;

    nrfx_spim_xfer_desc_t transfer_buffer; 
    transfer_buffer.p_tx_buffer = address_array;
    transfer_buffer.tx_length = sizeof(address_array);
    transfer_buffer.p_rx_buffer = data_array;
    transfer_buffer.rx_length = array_size;

    ret_code_t error_code = nrfx_spim_xfer(&m_spim_2, &transfer_buffer, 0);
    APP_ERROR_CHECK(error_code);
    while (spim_xfer_done == false); 

    NRF_LOG_INFO("REGISTER DATA @ Address 0x%x: \r\n", register_address);
    NRF_LOG_HEXDUMP_INFO(data_array, array_size); // Hex Dump Output (temp)
}

void spim_write_single_register(uint8_t register_address, uint8_t byte_0, uint8_t byte_1, uint8_t byte_2)
{
    uint8_t register_data[4];
    register_data[0] = SPIM_WRITE_ADDRESS(register_address);
    register_data[1] = byte_0;
    register_data[2] = byte_1;
    register_data[3] = byte_2;
    NRF_LOG_INFO("SPIM Writing 0x[%X|%X|%X] to Address 0x%X\r\n", register_data[1], register_data[2], register_data[3], register_address);
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data));
    // SPI Command
    spim_xfer_done = false;

    nrfx_spim_xfer_desc_t transfer_buffer; 
    transfer_buffer.p_tx_buffer = register_data;
    transfer_buffer.tx_length = sizeof(register_data);
    transfer_buffer.p_rx_buffer = NULL;
    transfer_buffer.rx_length = NULL;

    ret_code_t error_code = nrfx_spim_xfer(&m_spim_2, &transfer_buffer, 0);
    APP_ERROR_CHECK(error_code);

    while (spim_xfer_done == false); 

    nrf_delay_ms(5);
}