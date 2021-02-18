#include "i2c.h"

#if I2C

static struct TWIM_Control_Struct control;

static uint8_t _twim_timeout_handler()
{
    if(control.timeout == 0)
    {
        NRF_LOG_INFO("_twim_timeout_handler()");
        control.timeout_flag = 1;
        control.timeout = TWIM_TIMEOUT;
        return 1;
    }
    --control.timeout;
    return 0;
}

uint8_t get_i2c_timeout()
{
    uint8_t temp_timeout_flag = 1;
    control.timeout_flag = 0;
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
    control.timeout = TWIM_TIMEOUT;
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
            control.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_ADDRESS_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_ADDRESS_NACK");
            control.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_DATA_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_DATA_NACK");
            control.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_BUS_ERROR:
            NRF_LOG_INFO("NRFX_TWIM_EVT_BUS_ERROR");
            control.twim_xfer_done = true;
            break;

        case NRFX_TWIM_EVT_OVERRUN:
            NRF_LOG_INFO("NRFX_TWIM_EVT_OVERRUN");
            control.twim_xfer_done = true;
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
    NRF_LOG_INFO("twim_setup");
    control.twim_config.scl = I2C_SCL_PIN;
    control.twim_config.sda = I2C_SDA_PIN;
    control.twim_config.frequency = NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY;
    control.twim_config.interrupt_priority = APP_IRQ_PRIORITY_HIGH;
    control.twim_config.hold_bus_uninit = NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT;

    const nrfx_twim_t m_twim_0 = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);
    control.nrfx_twim = m_twim_0;
    control.initialization = 0;
}

void twim_disable(void)
{
    NRF_LOG_INFO("twim_disable");

    nrfx_twim_disable(&control.nrfx_twim);
    NRF_LOG_INFO("nrfx_twim has been disabled");
}

void twim_enable(void)
{
    NRF_LOG_INFO("twim_enable");

    nrfx_twim_enable(&control.nrfx_twim);
    NRF_LOG_INFO("nrfx_twim has been enabled");
}

/*
* @Brief: Function: twim_init() is used to initialize the TWIM pheripheral
*/
void twim_init(void)
{
    NRF_LOG_INFO("twim_init");
    if(control.initialization == 0)
    {
        control.error_code = nrfx_twim_init(&control.nrfx_twim, &control.twim_config, twim_handler, NULL);
        APP_ERROR_CHECK(control.error_code);

        control.timeout = TWIM_TIMEOUT;
        control.twim_xfer_done = false;   /* Indicates if operation on TWIM has ended. */
        control.initialization = 1;   // Set flag to state that TWIM has been initialized
        NRF_LOG_INFO("twim has been initialized.");
    }
}

/*
* @Brief: Function: twim_init() is used to uninitialize the TWIM pheripheral
*/
void twim_uninit(void)
{
    NRF_LOG_INFO("twim_uninit");
    if(control.initialization == 1)
    {
        nrfx_twim_uninit(&control.nrfx_twim);
        control.initialization = 0;   // Set flag to state that TWIM has been uninitialized
        NRF_LOG_INFO("twim has been uninitialized.");
    }
}

void i2c_write_registers(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size) 
{
    NRF_LOG_INFO("I2C_write_registers");
    control.twim_xfer_done = false;
    ret_code_t error_code = nrfx_twim_tx(&control.nrfx_twim, slave_address, array_data, array_size, false);

    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
}

void i2c_write_single_register(uint8_t slave_address, uint8_t register_address, uint8_t *register_value, uint8_t register_byte_count)
{
    NRF_LOG_INFO("i2c_write_single_register");
    control.twim_xfer_done = false;
    uint8_t temp_register_byte_count = register_byte_count;
    uint8_t temp_i2c_data[temp_register_byte_count+1];
    temp_i2c_data[0] = register_address;
    for(uint8_t i = 0; i < register_byte_count; i++)
    {
        temp_i2c_data[i+1] = register_value[i];
    }
    
    ret_code_t error_code = nrfx_twim_tx(&control.nrfx_twim, slave_address, temp_i2c_data, register_byte_count, false);

    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
    NRF_LOG_INFO("Register address: 0x%X", register_address);
    NRF_LOG_INFO("HEX values written to address:");
    NRF_LOG_HEXDUMP_INFO(register_value, register_byte_count);
}

void i2c_read_single_register(uint8_t slave_address, uint8_t register_address, uint8_t *register_data, uint8_t register_byte_count)
{
    NRF_LOG_INFO("i2c_read_single_register");
    control.twim_xfer_done = false;
    ret_code_t error_code = nrfx_twim_tx(&control.nrfx_twim, slave_address, &register_address, 1, false);
    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    if(NRF_SUCCESS == error_code)
    {
      error_code = nrfx_twim_rx(&control.nrfx_twim, slave_address, register_data, register_byte_count);
    }
    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("Register address: 0x%X", register_address);
    NRF_LOG_INFO("Hex values stored at address:");
    NRF_LOG_HEXDUMP_INFO(register_data, register_byte_count);
}

void i2c_read_registers(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size)
{
    NRF_LOG_INFO("i2c_read_registers");
    control.twim_xfer_done = false;
    ret_code_t error_code = nrfx_twim_tx(&control.nrfx_twim, slave_address, &register_address, 1, false);
    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    if(NRF_SUCCESS == error_code)
    {
      error_code = nrfx_twim_rx(&control.nrfx_twim, slave_address, array_data, array_size);
    }
    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Register HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

void i2c_read_device(uint8_t slave_address, uint8_t* array_data, uint8_t array_size)
{
    control.twim_xfer_done = false;

    ret_code_t error_code = nrfx_twim_rx(&control.nrfx_twim, slave_address, array_data, array_size);
    
    APP_ERROR_CHECK(error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

void i2c_write_write_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size)   
{
    control.twim_xfer_done = false;

    control.twim_xfer_desc.type = NRFX_TWIM_XFER_TXTX;                   // Type of transfer
    control.twim_xfer_desc.address = device_address;                     // Slave address
    control.twim_xfer_desc.primary_length = first_array_data_size;       // Number of bytes transferred
    control.twim_xfer_desc.secondary_length = second_array_data_size;    // Number of bytes transferred
    control.twim_xfer_desc.p_primary_buf = first_array_data;             // Pointer to transferred data
    control.twim_xfer_desc.p_secondary_buf = second_array_data;          // Pointer to transferred data

    control.nrfx_twim_xfer_flag = NULL;
    control.error_code = nrfx_twim_xfer(&control.nrfx_twim, &control.twim_xfer_desc, control.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(control.error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
}


void i2c_write_read_register(uint8_t slave_address, uint8_t *write_data, uint8_t write_data_length, uint8_t *read_data, uint8_t read_data_length) 
{
    control.twim_xfer_done = false;
    
    control.twim_xfer_desc.type = NRFX_TWIM_XFER_TXRX;                    // Type of transfer
    control.twim_xfer_desc.address = slave_address;                       // Slave address
    control.twim_xfer_desc.primary_length = write_data_length;            // Number of bytes transferred
    control.twim_xfer_desc.secondary_length = read_data_length;           // Number of bytes transferred
    control.twim_xfer_desc.p_primary_buf = write_data;                    // Pointer to transferred data
    control.twim_xfer_desc.p_secondary_buf = read_data;                   // Pointer to transferred data

    control.nrfx_twim_xfer_flag = NULL;
    control.error_code = nrfx_twim_xfer(&control.nrfx_twim, &control.twim_xfer_desc, control.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(control.error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
    NRF_LOG_INFO("Register address: 0x%X", write_data[0]);
    NRF_LOG_INFO("HEX values read from address:");
    NRF_LOG_HEXDUMP_INFO(read_data, read_data_length);
}

void i2c_separate_write_read_register(uint8_t slave_address, uint8_t *write_data, uint8_t write_data_length, uint8_t *read_data, uint8_t read_data_length) 
{
    NRF_LOG_INFO("i2c_separate_write_read_registers");
    control.twim_xfer_done = false;
    
    control.twim_xfer_desc.type = NRFX_TWIM_XFER_TX;                      // Type of transfer
    control.twim_xfer_desc.address = slave_address;                       // Slave address
    control.twim_xfer_desc.primary_length = write_data_length;            // Number of bytes transferred
    control.twim_xfer_desc.secondary_length = 0;                          // Number of bytes transferred
    control.twim_xfer_desc.p_primary_buf = write_data;                    // Pointer to transferred data
    control.twim_xfer_desc.p_secondary_buf = NULL;                        // Pointer to transferred data

    control.nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    control.error_code = nrfx_twim_xfer(&control.nrfx_twim, &control.twim_xfer_desc, control.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(control.error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }
    
    control.twim_xfer_done = false;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    control.twim_xfer_desc.type = NRFX_TWIM_XFER_RX;                      // Type of transfer
    control.twim_xfer_desc.address = slave_address;                       // Slave address
    control.twim_xfer_desc.primary_length = read_data_length;             // Number of bytes transferred
    control.twim_xfer_desc.secondary_length = 0;                          // Number of bytes transferred
    control.twim_xfer_desc.p_primary_buf = read_data;                     // Pointer to transferred data
    control.twim_xfer_desc.p_secondary_buf = NULL;                        // Pointer to transferred data

    control.nrfx_twim_xfer_flag = NULL;
    control.error_code = nrfx_twim_xfer(&control.nrfx_twim, &control.twim_xfer_desc, control.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(control.error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        NRF_LOG_INFO("control.timeout_flag == 0x01");
        return;
    }
    nrf_delay_ms(TWIM_DELAY);
    NRF_LOG_INFO("Register address: 0x%X", write_data[0]);
    NRF_LOG_INFO("HEX values read from address:");
    NRF_LOG_HEXDUMP_INFO(read_data, read_data_length);
}

void i2c_no_stop_write_register(uint8_t slave_address, uint8_t *write_data, uint8_t write_data_length)
{
    control.twim_xfer_done = false;
    
    control.twim_xfer_desc.type = NRFX_TWIM_XFER_TX;                      // Type of transfer
    control.twim_xfer_desc.address = slave_address;                       // Slave address
    control.twim_xfer_desc.primary_length = write_data_length;            // Number of bytes transferred
    control.twim_xfer_desc.secondary_length = 0;                          // Number of bytes transferred
    control.twim_xfer_desc.p_primary_buf = write_data;                    // Pointer to transferred data
    control.twim_xfer_desc.p_secondary_buf = NULL;                        // Pointer to transferred data

    control.nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    control.error_code = nrfx_twim_xfer(&control.nrfx_twim, &control.twim_xfer_desc, control.nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(control.error_code);
    while((control.twim_xfer_done == false) && (_twim_timeout_handler() == false));
    if(control.timeout_flag == 0x01)
    {
        return;
    }

    nrf_delay_ms(TWIM_DELAY);
    NRF_LOG_INFO("Register address: 0x%X", write_data[0]);
    NRF_LOG_INFO("HEX values written to address:");
    NRF_LOG_HEXDUMP_INFO(write_data, write_data_length);
}

#endif