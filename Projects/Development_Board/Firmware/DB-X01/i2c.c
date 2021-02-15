#include "i2c.h"

#if I2C

static uint8_t _twim_timeout_handler()
{
    if(twim_configuration.timeout == 0)
    {
        NRF_LOG_INFO("_twim_timeout_handler()");
        twim_configuration.timeout_flag = 1;
        twim_configuration.timeout = TWIM_TIMEOUT;
        return 1;
    }
    --twim_configuration.timeout;
    return 0;
}

uint8_t get_i2c_timeout()
{
    uint8_t temp_timeout_flag = 1;
    twim_configuration.timeout_flag = 0;
    return temp_timeout_flag;
}

/**
 * @brief TWI events handler.
 * The point of the TWI event handler is that it is used for current consumption
 * When the MCU is transferring data over I2C, it can go into sleep mode
 */
void twim_handler(nrfx_twim_evt_t const * p_event, void * p_context)
{
    NRF_LOG_INFO("TWI_HANDLER Event Occured");
    twim_configuration.timeout = TWIM_TIMEOUT;
    switch (p_event->type)
    {
        case NRFX_TWIM_EVT_DONE:
            if(p_event->xfer_desc.type == NRFX_TWIM_XFER_RX)
            {
                NRF_LOG_INFO("I2C Receiving Data");
            }
            else if(p_event->xfer_desc.type == NRFX_TWIM_XFER_TX)
            {
                NRF_LOG_INFO("I2C Sending Data");
            }
            else
            {
                NRF_LOG_INFO("I2C No Event type");
            }
            twim_configuration.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_ADDRESS_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_ADDRESS_NACK");
            twim_configuration.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_DATA_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_DATA_NACK");
            twim_configuration.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_BUS_ERROR:
            NRF_LOG_INFO("NRFX_TWIM_EVT_BUS_ERROR");
            twim_configuration.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_OVERRUN:
            NRF_LOG_INFO("NRFX_TWIM_EVT_OVERRUN");
            twim_configuration.twim_xfer_done = true;
            break;
        
        default:
            NRF_LOG_INFO("Default case reached");
            break;
    }
}

/*
* @Brief: Function: twim_setup() is used to setup the variables used for the TWIM
* Initialize I2C module, called Two Wire Interface (TWI)
* Setting Pin P0.30 to SDA
* Setting Pin P0.28 to SCL
* Setting I2C frequency to 100 kHz
* Giving it a low priority to not disturb BLE and Power Interrupts 
*/
void twim_setup(void)
{
    twim_configuration.twim_config.scl = I2C_SCL_PIN;
    twim_configuration.twim_config.sda = I2C_SDA_PIN;
    twim_configuration.twim_config.frequency = NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY;
    twim_configuration.twim_config.interrupt_priority = APP_IRQ_PRIORITY_HIGH;
    twim_configuration.twim_config.hold_bus_uninit = NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT;

    const nrfx_twim_t m_twim_0 = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);
    twim_configuration.nrfx_twim = m_twim_0;
    twim_configuration.initialization = 0;
}

void twim_disable(void)
{
    NRF_LOG_INFO("twim_disable");
    if(NRFX_TWIM0_ENABLED)
    {
        nrfx_twim_disable(&twim_configuration.nrfx_twim);
        NRF_LOG_INFO("nrfx_twim has been disabled");
    }
}

void twim_enable(void)
{
    NRF_LOG_INFO("twim_enable");
    if(!NRFX_TWIM0_ENABLED)
    {
        nrfx_twim_enable(&twim_configuration.nrfx_twim);
        NRF_LOG_INFO("nrfx_twim has been enabled");
    }
}

/*
* @Brief: Function: twim_init() is used to initialize the TWIM pheripheral
*/
void twim_init(void)
{
    NRF_LOG_INFO("twim_init");
    if(!twim_configuration.initialization)
    {
        twim_configuration.error_code = nrfx_twim_init(&twim_configuration.nrfx_twim, &twim_configuration.twim_config, twim_handler, NULL);
        APP_ERROR_CHECK(twim_configuration.error_code);

        twim_configuration.timeout = TWIM_TIMEOUT;
        twim_configuration.twim_xfer_done = false; /* Indicates if operation on TWIM has ended. */
        NRF_LOG_INFO("twim has been initialized.");
    }
}

/*
* @Brief: Function: twim_init() is used to uninitialize the TWIM pheripheral
*/
void twim_uninit(void)
{
    NRF_LOG_INFO("twim_uninit");
    if(twim_configuration.initialization)
    {
        nrfx_twim_uninit(&twim_configuration.nrfx_twim);
        NRF_LOG_INFO("twim has been initialized.");
    }
}

void i2c_write_registers(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size) 
{
    twim_configuration.twim_xfer_done = false;
    NRF_LOG_INFO("I2C_write");
    ret_code_t error_code = nrfx_twim_tx(&twim_configuration.nrfx_twim, slave_address, array_data, array_size, false);

    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
}


void i2c_write_single_register(uint8_t slave_address, uint8_t register_address, uint8_t *register_value, uint8_t register_byte_count)
{
    NRF_LOG_INFO("i2c_write_single_register");
    twim_configuration.twim_xfer_done = false;
    uint8_t temp_register_byte_count = register_byte_count;
    uint8_t temp_i2c_data[temp_register_byte_count+1];
    temp_i2c_data[0] = register_address;
    for(uint8_t i = 0; i < register_byte_count; i++)
    {
        temp_i2c_data[i+1] = register_value[i];
    }
    
    ret_code_t error_code = nrfx_twim_tx(&twim_configuration.nrfx_twim, slave_address, temp_i2c_data, register_byte_count, false);

    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
    NRF_LOG_INFO("Register at address 0x%X changed to 0x%X. \r\n", register_address, register_value);
}

void i2c_read_single_register(uint8_t slave_address, uint8_t register_address, uint8_t *register_data, uint8_t register_byte_count)
{
    twim_configuration.twim_xfer_done = false;
    NRF_LOG_INFO("I2C_read");
    ret_code_t error_code = nrfx_twim_tx(&twim_configuration.nrfx_twim, slave_address, &register_address, 1, false);
    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    if(NRF_SUCCESS == error_code)
    {
      error_code = nrfx_twim_rx(&twim_configuration.nrfx_twim, slave_address, register_data, register_byte_count);
    }
    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Register HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(register_data, register_byte_count);
}

void i2c_read_registers(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size)
{
    twim_configuration.twim_xfer_done = false;
    NRF_LOG_INFO("I2C_read");
    ret_code_t error_code = nrfx_twim_tx(&twim_configuration.nrfx_twim, slave_address, &register_address, 1, false);
    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    if(NRF_SUCCESS == error_code)
    {
      error_code = nrfx_twim_rx(&twim_configuration.nrfx_twim, slave_address, array_data, array_size);
    }
    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Register HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

void i2c_read_device(uint8_t slave_address, uint8_t* array_data, uint8_t array_size)
{
    twim_configuration.twim_xfer_done = false;

    ret_code_t error_code = nrfx_twim_rx(&twim_configuration.nrfx_twim, slave_address, array_data, array_size);
    
    APP_ERROR_CHECK(error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

void i2c_write_write_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size)   
{
    twim_configuration.twim_xfer_done = false;

    twim_configuration.twim_xfer_desc.type = NRFX_TWIM_XFER_TXTX;                   // Type of transfer
    twim_configuration.twim_xfer_desc.address = device_address;                     // Slave address
    twim_configuration.twim_xfer_desc.primary_length = first_array_data_size;       // Number of bytes transferred
    twim_configuration.twim_xfer_desc.secondary_length = second_array_data_size;    // Number of bytes transferred
    twim_configuration.twim_xfer_desc.p_primary_buf = first_array_data;             // Pointer to transferred data
    twim_configuration.twim_xfer_desc.p_secondary_buf = second_array_data;          // Pointer to transferred data

    twim_configuration.nrfx_twim_xfer_flag = NULL;
    twim_configuration.error_code = nrfx_twim_xfer(&twim_configuration.nrfx_twim, &twim_configuration.twim_xfer_desc, twim_configuration.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(twim_configuration.error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
}


void i2c_write_read_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size) 
{
    twim_configuration.twim_xfer_done = false;
    
    twim_configuration.twim_xfer_desc.type = NRFX_TWIM_XFER_TXRX;                   // Type of transfer
    twim_configuration.twim_xfer_desc.address = device_address;                     // Slave address
    twim_configuration.twim_xfer_desc.primary_length = first_array_data_size;       // Number of bytes transferred
    twim_configuration.twim_xfer_desc.secondary_length = second_array_data_size;    // Number of bytes transferred
    twim_configuration.twim_xfer_desc.p_primary_buf = first_array_data;             // Pointer to transferred data
    twim_configuration.twim_xfer_desc.p_secondary_buf = second_array_data;          // Pointer to transferred data

    twim_configuration.nrfx_twim_xfer_flag = NULL;
    twim_configuration.error_code = nrfx_twim_xfer(&twim_configuration.nrfx_twim, &twim_configuration.twim_xfer_desc, twim_configuration.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(twim_configuration.error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(second_array_data, second_array_data_size);
}

void i2c_separate_write_read_registers(uint8_t device_command, uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size) 
{
    NRF_LOG_INFO("i2c_separate_write_read_registers");
    twim_configuration.twim_xfer_done = false;
    
    twim_configuration.twim_xfer_desc.type = NRFX_TWIM_XFER_TX;                     // Type of transfer
    twim_configuration.twim_xfer_desc.address = device_command;                     // Slave address
    twim_configuration.twim_xfer_desc.primary_length = first_array_data_size;       // Number of bytes transferred
    twim_configuration.twim_xfer_desc.secondary_length = 0;                         // Number of bytes transferred
    twim_configuration.twim_xfer_desc.p_primary_buf = first_array_data;             // Pointer to transferred data
    twim_configuration.twim_xfer_desc.p_secondary_buf = NULL;                       // Pointer to transferred data

    twim_configuration.nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    twim_configuration.error_code = nrfx_twim_xfer(&twim_configuration.nrfx_twim, &twim_configuration.twim_xfer_desc, twim_configuration.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(twim_configuration.error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    
    twim_configuration.twim_xfer_done = false;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    twim_configuration.twim_xfer_desc.type = NRFX_TWIM_XFER_RX;                     // Type of transfer
    twim_configuration.twim_xfer_desc.address = device_address;                     // Slave address
    twim_configuration.twim_xfer_desc.primary_length = second_array_data_size;      // Number of bytes transferred
    twim_configuration.twim_xfer_desc.secondary_length = 0;                         // Number of bytes transferred
    twim_configuration.twim_xfer_desc.p_primary_buf = second_array_data;            // Pointer to transferred data
    twim_configuration.twim_xfer_desc.p_secondary_buf = NULL;                       // Pointer to transferred data

    twim_configuration.nrfx_twim_xfer_flag = NULL;
    twim_configuration.error_code = nrfx_twim_xfer(&twim_configuration.nrfx_twim, &twim_configuration.twim_xfer_desc, twim_configuration.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(twim_configuration.error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(second_array_data, second_array_data_size);
}

void i2c_no_stop_write_registers(uint8_t device_command, uint8_t* first_array_data, uint8_t first_array_data_size)
{
    twim_configuration.twim_xfer_done = false;
    
    twim_configuration.twim_xfer_desc.type = NRFX_TWIM_XFER_TX;                     // Type of transfer
    twim_configuration.twim_xfer_desc.address = device_command;                     // Slave address
    twim_configuration.twim_xfer_desc.primary_length = first_array_data_size;       // Number of bytes transferred
    twim_configuration.twim_xfer_desc.secondary_length = 0;                         // Number of bytes transferred
    twim_configuration.twim_xfer_desc.p_primary_buf = first_array_data;             // Pointer to transferred data
    twim_configuration.twim_xfer_desc.p_secondary_buf = NULL;                       // Pointer to transferred data

    twim_configuration.nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    twim_configuration.error_code = nrfx_twim_xfer(&twim_configuration.nrfx_twim, &twim_configuration.twim_xfer_desc, twim_configuration.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(twim_configuration.error_code);
    while((twim_configuration.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(twim_configuration.timeout_flag == 0x01)
    {
        return;
    }
}

#endif