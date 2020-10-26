#include "i2c.h"

/* TWIM instance ID. */
#define TWIM_INSTANCE_ID     0

/* Indicates if operation on TWI has ended. */
volatile bool m_xfer_done = false;

/* TWIM instance. */
const nrfx_twim_t m_twim_0 = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);

/**
 * @brief TWI events handler.
 * The point of the TWI event handler is that it is used for current consumption
 * When the MCU is transferring data over I2C, it can go into sleep mode
 */
void twim_handler(nrfx_twim_evt_t const * p_event, void * p_context)
{
    NRF_LOG_INFO("TWI_HANDLER Event Occured");
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
            m_xfer_done = true;
            break;
        case NRFX_TWIM_EVT_ADDRESS_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_ADDRESS_NACK");
            m_xfer_done = true;
            break;
        case NRFX_TWIM_EVT_DATA_NACK:
            NRF_LOG_INFO("NRFX_TWIM_EVT_DATA_NACK");
            m_xfer_done = true;
            break;
        case NRFX_TWIM_EVT_BUS_ERROR:
            NRF_LOG_INFO("NRFX_TWIM_EVT_BUS_ERROR");
            m_xfer_done = true;
            break;
        case NRFX_TWIM_EVT_OVERRUN:
            NRF_LOG_INFO("NRFX_TWIM_EVT_OVERRUN");
            m_xfer_done = true;
            break;
        default:
            NRF_LOG_INFO("Default case reached");
            break;
    }
}

/*
 * Initialize I2C module, called Two Wire Interface (TWI)
 * Setting Pin P0.30 to SDA
 * Setting Pin P0.28 to SCL
 * Setting I2C frequency to 100 kHz
 * Giving it a low priority to not disturb BLE and Power Interrupts
 */
void twim_init(void)
{
    NRF_LOG_INFO("twi_init");
    ret_code_t err_code;

    const nrfx_twim_config_t twim_config = 
    {
        .scl                = I2C_SCL_PIN,
        .sda                = I2C_SDA_PIN,
        .frequency          = NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .hold_bus_uninit    = NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT
    };

    err_code = nrfx_twim_init(&m_twim_0, &twim_config, twim_handler, NULL);
    APP_ERROR_CHECK(err_code);
}

void i2c_write_registers(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size) 
{
    m_xfer_done = false;
    NRF_LOG_INFO("I2C_write");
    ret_code_t error_code = nrfx_twim_tx(&m_twim_0, slave_address, array_data, array_size, false);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);
}


void i2c_write_single_register(uint8_t slave_address, uint8_t register_address, uint8_t register_value)
{
    uint8_t register_byte_count = 2;              
    
    /*BMI160 Register values to initialize sensor*/
    uint8_t register_array[2]= {0};       // Register Address + Register Write Value
    uint8_t device_address = slave_address;

    register_array[0] = register_address;
    register_array[1] = register_value;
    uint8_t *register_array_pointer = register_array;
    
    i2c_write_registers(device_address, register_array_pointer, register_byte_count);
 
    nrf_delay_ms(1); // required delay between I2C write commands
    NRF_LOG_INFO("Register at address 0x%X changed to 0x%X. \r\n", register_address, register_value);
}

void i2c_read_registers(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size)
{
    m_xfer_done = false;
    NRF_LOG_INFO("I2C_read");
    ret_code_t error_code = nrfx_twim_tx(&m_twim_0, slave_address, &register_address, 1, false);
    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    if(NRF_SUCCESS == error_code)
    {
      error_code = nrfx_twim_rx(&m_twim_0, slave_address, array_data, array_size);
    }
    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);

    NRF_LOG_INFO("I2C Register HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

void i2c_read_device(uint8_t slave_address, uint8_t* array_data, uint8_t array_size)
{
    m_xfer_done = false;

    ret_code_t error_code = nrfx_twim_rx(&m_twim_0, slave_address, array_data, array_size);
    
    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(array_data, array_size);
}

uint8_t i2c_read_single_register(uint8_t device_address, uint8_t register_address)
{
    uint8_t register_data;
    i2c_read_registers(device_address, register_address, &register_data, 1);

    return register_data;
}

void i2c_stop(void)
{
    if(NRFX_TWIM0_ENABLED)
    {
        nrfx_twim_disable(&m_twim_0);
    }
}

void i2c_start(void)
{
    if(NRFX_TWIM0_ENABLED)
    {
        nrfx_twim_enable(&m_twim_0);
    }
}

void i2c_write_write_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size)   
{
    m_xfer_done = false;
    
    nrfx_twim_xfer_desc_t twim_xfer_desc = 
    {
        .type = NRFX_TWIM_XFER_TXTX,                  ///< Type of transfer.
        .address = device_address,                    ///< Slave address.
        .primary_length = first_array_data_size,      ///< Number of bytes transferred.
        .secondary_length = second_array_data_size,   ///< Number of bytes transferred.
        .p_primary_buf = first_array_data,            ///< Pointer to transferred data.
        .p_secondary_buf = second_array_data          ///< Pointer to transferred data.
    };

    uint32_t nrfx_twim_xfer_flag = NULL;
    ret_code_t error_code = nrfx_twim_xfer(&m_twim_0, &twim_xfer_desc, nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);
}


void i2c_write_read_registers(uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size) 
{
    m_xfer_done = false;
    
    nrfx_twim_xfer_desc_t twim_xfer_desc = 
    {
        .type = NRFX_TWIM_XFER_TXRX,                  ///< Type of transfer.
        .address = device_address,                    ///< Slave address.
        .primary_length = first_array_data_size,      ///< Number of bytes transferred.
        .secondary_length = second_array_data_size,   ///< Number of bytes transferred.
        .p_primary_buf = first_array_data,            ///< Pointer to transferred data.
        .p_secondary_buf = second_array_data          ///< Pointer to transferred data.
    };

    uint32_t nrfx_twim_xfer_flag = NULL;
    ret_code_t error_code = nrfx_twim_xfer(&m_twim_0, &twim_xfer_desc, nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(second_array_data, second_array_data_size);
}

void i2c_separate_write_read_registers(uint8_t device_command, uint8_t device_address, uint8_t* first_array_data, uint8_t first_array_data_size, uint8_t* second_array_data, uint8_t second_array_data_size) 
{
    m_xfer_done = false;
    
    nrfx_twim_xfer_desc_t twim_xfer_desc_tx = 
    {
        .type = NRFX_TWIM_XFER_TX,                    ///< Type of transfer.
        .address = device_command,                    ///< Slave address.
        .primary_length = first_array_data_size,      ///< Number of bytes transferred.
        .secondary_length = 0,                        ///< Number of bytes transferred.
        .p_primary_buf = first_array_data,            ///< Pointer to transferred data.
        .p_secondary_buf = NULL                       ///< Pointer to transferred data.
    };

    uint32_t nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    ret_code_t error_code = nrfx_twim_xfer(&m_twim_0, &twim_xfer_desc_tx, nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);

    m_xfer_done = false;

    nrfx_twim_xfer_desc_t twim_xfer_desc_rx = 
    {
        .type = NRFX_TWIM_XFER_RX,                    ///< Type of transfer.
        .address = device_address,                    ///< Slave address.
        .primary_length = second_array_data_size,      ///< Number of bytes transferred.
        .secondary_length = 0,                        ///< Number of bytes transferred.
        .p_primary_buf = second_array_data,            ///< Pointer to transferred data.
        .p_secondary_buf = NULL                       ///< Pointer to transferred data.
    };

    nrfx_twim_xfer_flag = NULL;
    error_code = nrfx_twim_xfer(&m_twim_0, &twim_xfer_desc_rx, nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
    nrf_delay_ms(10);

    NRF_LOG_INFO("I2C Device HEX Data Dump:");
    NRF_LOG_HEXDUMP_INFO(second_array_data, second_array_data_size);
}

void i2c_no_stop_write_registers(uint8_t device_command, uint8_t* first_array_data, uint8_t first_array_data_size)
{
    m_xfer_done = false;
    
    nrfx_twim_xfer_desc_t twim_xfer_desc_tx = 
    {
        .type = NRFX_TWIM_XFER_TX,                    ///< Type of transfer.
        .address = device_command,                    ///< Slave address.
        .primary_length = first_array_data_size,      ///< Number of bytes transferred.
        .secondary_length = 0,                        ///< Number of bytes transferred.
        .p_primary_buf = first_array_data,            ///< Pointer to transferred data.
        .p_secondary_buf = NULL                       ///< Pointer to transferred data.
    };

    uint32_t nrfx_twim_xfer_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
    ret_code_t error_code = nrfx_twim_xfer(&m_twim_0, &twim_xfer_desc_tx, nrfx_twim_xfer_flag);

    APP_ERROR_CHECK(error_code);
    while(m_xfer_done == false);
}