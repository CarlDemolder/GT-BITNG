#include "i2c.h"

/* TWIM instance ID. */
#define TWIM_INSTANCE_ID     0

/* Indicates if operation on TWI has ended. */
volatile bool m_xfer_done = false;

/* TWI instance. */
const nrfx_twim_t m_twim_0 = NRFX_TWIM_INSTANCE(TWIM_INSTANCE_ID);
/**
 * @brief TWI events handler.
 * The point of the TWI event handler is that it is used for current consumption
 * When the MCU is transferring data over I2C, it can go into sleep mode
 */
void twim_handler(nrfx_twim_evt_t const * p_event, void * p_context)
{
    NRF_LOG_DEBUG("TWI_HANDLER Event Occured");
    switch (p_event->type)
    {
        case NRFX_TWIM_EVT_DONE:
            if (p_event->xfer_desc.type == NRFX_TWIM_XFER_RX)
            {
                NRF_LOG_INFO("I2C Receiving Data");
            }
            m_xfer_done = true;
            break;
        default:
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
void twi_init(void)
{
    NRF_LOG_INFO("twi_init");
    ret_code_t err_code;

    const nrfx_twim_config_t twim_TMP116_config = 
    {
        .scl                = I2C_SCL_PIN,
        .sda                = I2C_SDA_PIN,
        .frequency          = NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .hold_bus_uninit    = NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT
    };

    err_code = nrfx_twim_init(&m_twim_0, &twim_TMP116_config, twim_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_twim_enable(&m_twim_0);
}

void i2c_write(uint8_t slave_address, uint8_t const* array_data, uint8_t array_size) 
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
    
    i2c_write(device_address, register_array_pointer, register_byte_count);
 
    nrf_delay_ms(1); // required delay between I2C write commands
    NRF_LOG_INFO("Register at address 0x%X changed to 0x%X. \r\n", register_address, register_value);
}

void i2c_read(uint8_t slave_address, uint8_t register_address, uint8_t* array_data, uint8_t array_size)
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
}

uint8_t i2c_read_single_register(uint8_t device_address, uint8_t register_address)
{
    uint8_t register_data;
    i2c_read(device_address, register_address, &register_data, 1);

    NRF_LOG_INFO("Register HEX Data Dump: \r\n");
    NRF_LOG_HEXDUMP_INFO(&register_data, sizeof(register_data));

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
