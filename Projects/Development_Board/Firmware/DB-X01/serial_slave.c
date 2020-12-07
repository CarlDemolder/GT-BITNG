#include "serial_slave.h"

/*
* @Brief: Function: startup_initialization() is used to initialize the NRF52, TMP117, MAX30003, and CY15B108QI
*/
void startup_initialization(void)
{
    uint8_t nrf52_initialization_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, NRF52_INITIALIZATION_COMMAND};       
    state_handler(nrf52_initialization_array_data); // Initialize NRF52 Module

//    uint8_t tmp117_initialization_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, TMP117_INITIALIZATION_COMMAND};       
//    state_handler(tmp117_initialization_array_data); // Initialize TMP117 Module
//
//    uint8_t max30003_initialization_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, MAX30003_INITIALIZATION_COMMAND};       
//    state_handler(max30003_initialization_array_data); // Initialize MAX30003 Module
//
//    uint8_t cy15b108qi_initialization_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, CY15B108QI_INITIALIZATION_COMMAND};       
//    state_handler(max30003_initialization_array_data); // Initialize CY15B108QI Module
}

/*
*   @Brief: Function: _nrf52_initialization() is used to initialize common functionalities. 
*/
static void _nrf52_initialization(void)
{
    uint8_t log_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LOG_INIT};       
    state_handler(log_array_data); // Enable LOG Driver

    uint8_t nrfx_clock_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_CLOCK_COMMAND, NRF52_NRFX_CLOCK_DRIVER_INIT};   
    state_handler(nrfx_clock_init_array_data); // Init and Enable NRFX Clock 

    uint8_t set_pin_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_INPUT_OUTPUT_INIT};       
    state_handler(set_pin_array_data); // Initialize Input and Output Pins

    uint8_t ldo_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LDO_INIT};       
    state_handler(ldo_array_data); // Enable LDO Drivers

    uint8_t vcc_en_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_VCC_LDO_EN};   
    state_handler(vcc_en_array_data); // Enable VCC LDO  

    uint8_t power_manager_array_data[4] = {0x00, NRF52_MODULE, NRF52_POWER_COMMAND, NRF52_POWER_MANAGER_INIT};  
    state_handler(power_manager_array_data); // Enable Power Manager

    uint8_t hf_clock_start_array_data[4] = {0x00, NRF52_MODULE, NRF52_HF_CLOCK_COMMAND, NRF52_HF_CLOCK_START};   
    state_handler(hf_clock_start_array_data); // Start HF Clock

    uint8_t spim_setup_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SETUP};
    state_handler(spim_setup_array_data); // Setup SPIM Module

    uint8_t twim_setup_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_SETUP};  
    state_handler(twim_setup_array_data); // Setup TWIM Module

    uint8_t led_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_INIT};  
    state_handler(led_array_data); // Enable LED Driver

    uint8_t medium_blink_led_array_data[5] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_IND_BLINK, NRF52_LED_IND_MEDIUM_BLINK};  
    state_handler(medium_blink_led_array_data); // IND LED Medium Blink
}

/*
*   @Brief: Function: _tmp117_initialization() is used to initialize the TMP117 module
*/
static void _tmp117_initialization(void)
{
    NRF_LOG_INFO("_tmp117_initialization");

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    uint8_t tmp117_shutdown_array_data[5] = {0x00, TMP117_MODULE, TMP117_INIT_COMMAND, TMP117_SHUTDOWN_MODE, TMP117_64_AVERAGED_MODE};  
    state_handler(tmp117_shutdown_array_data); // Set the TMP117 to shutdown mode, no averaging mode

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver
}

/*
*   @Brief: Function: _max30003_initialization() is used to initialize the MAX30003 module
*/
static void _max30003_initialization(void)
{
    NRF_LOG_INFO("_max30003_initialization");

    uint8_t max30003_power_ldo_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_MAX30003_POWER_LDO_EN};
    state_handler(max30003_power_ldo_enable_array_data); // Enable 1.8V Supply for MAX30003

    uint8_t nrf52_gpiote_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_GPIOTE_INIT};
    state_handler(nrf52_gpiote_init_array_data); // Initialize the GPIOTE Interrupt to detect the interupt from the MAX30003

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_init_cs_pin_array_data); // Initialize the Chip Select Pin for the MAX30003

    uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_select_cs_pin_array_data); // Select the Chip Select Pin for the MAX30003 for the SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_array_data); // Initialize SPIM Module
 
    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t max30003_init_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_INIT_COMMAND}; 
    state_handler(max30003_init_array_data); // MAX30003: Init Command 

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_array_data); // Disable SPIM Module 
}

/*
*   @Brief: Function: _cy15b108qi_initialization() is used to initialize the CY15B108QI module 
*/
static void _cy15b108qi_initialization(void)
{
    NRF_LOG_INFO("_cy15b108qi_initialization");

    uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_init_cs_pin_array_data); // Initialize the Chip Select Pin for the CY15B108QI

    uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_array_data); // Select the Chip Select Pin for the MAX30003 for the SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_array_data); // Initialize SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t cy15b108qi_init_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_INIT_COMMAND}; 
    state_handler(cy15b108qi_init_array_data); // CY15B108QI: Init Command    

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_array_data); // Disable SPIM Module 
}

/*
*   @Brief: Function: enable_bluetooth_handler() is used to enable the bluetooth handler 
*   purposes.
*/
void enable_bluetooth_handler(void)
{
    NRF_LOG_INFO("enable_bluetooth_handler");

    uint8_t lf_clock_start_array_data[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_START};   
    state_handler(lf_clock_start_array_data); // Start LF Clock 

    uint8_t bluetooth_ble_stack_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_BLE_STACK_COMMAND};
    state_handler(bluetooth_ble_stack_init_array_data); // Initialize the BLE stack for Bluetooth

    uint8_t bluetooth_gap_params_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_GAP_PARAMS_COMMAND};
    state_handler(bluetooth_gap_params_init_array_data); // Initialize the GAP Parameters for Bluetooth communication

    uint8_t bluetooth_gatt_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_GATT_COMMAND};
    state_handler(bluetooth_gatt_init_array_data); // Initialize the BLE GATT Stack

    uint8_t bluetooth_services_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_SERVICES_COMMAND};
    state_handler(bluetooth_services_init_array_data); // Initialize the services inside the BLE stack

    uint8_t bluetooth_advertising_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_ADVERTISING_COMMAND};
    state_handler(bluetooth_advertising_init_array_data); // Initialize the Advertising Module for Bluetooth

    uint8_t bluetooth_set_advertising_power_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_SET_ADVERTISING_POWER_COMMAND};
    state_handler(bluetooth_set_advertising_power_array_data); // Set the Advertising Power for the Bluetooth Module

    uint8_t bluetooth_conn_params_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_CONN_PARAMS_COMMAND};
    state_handler(bluetooth_conn_params_init_array_data); // Initialize the Connection Parameters Between the GATT Server and Client

    uint8_t bluetooth_peer_manager_init_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_PEER_MANAGER_COMMAND};
    state_handler(bluetooth_peer_manager_init_array_data); // Initialize the Peer Manager to control the interaction between the GATT Server and Client

    uint8_t enable_dcdc_converter_array_data[4] = {0x00, NRF52_MODULE, NRF52_POWER_COMMAND, NRF52_POWER_DCDC_CONVERTER_ENABLE};
    state_handler(enable_dcdc_converter_array_data); // Enable the DCDC Converter to reduce power consumption during BLE transmission

    uint8_t bluetooth_advertising_start_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_START_ADVERTISING_COMMAND};
    state_handler(bluetooth_advertising_start_array_data); // Start advertising data between the GATT Server and Client

//    uint8_t ecg_start_data_recording_array_data[3] = {0x00, ECG_MODULE, ECG_START_DATA_RECORDING};  
//    state_handler(ecg_start_data_recording_array_data); // Start Data Recording
}

/*
*   @Brief: Function: enable_serial_slave_handler() is used to initialize the serial debugger to debug and communicate with the NRF52 
*   using an I2C to USB Converter (FT201X). It creates a RTC so that an interrupt occurs every 1 second. When an RTC
*/
void enable_usb_handler(void)
{
    NRF_LOG_INFO("_enable_usb_handler");

    uint8_t lf_clock_start_array_data[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_START};   
    state_handler(lf_clock_start_array_data); // Start LF Clock 

    uint8_t rtc_ft201x_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_INIT};  
    state_handler(rtc_ft201x_init_array_data); // Initialize the RTC for the FT201X

    uint8_t rtc_ft201x_set_counter_array_data[6] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_SET_COUNTER, 0x00, 0x08};  
    state_handler(rtc_ft201x_set_counter_array_data); // Setting the counter for the RTC for the FT201X to 1 second

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    uint8_t rtc_ft201x_start_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_START};  
    state_handler(rtc_ft201x_start_array_data); // Start the RTC for the FT201X
}

/*
*   @Brief: Function: _disable_serial_slave_handler() is used to uninitialize the serial debugger since it is not available for debugging 
*   purposes.
*/
static void _disable_usb_handler(void)
{
    NRF_LOG_INFO("_disable_usb_handler");

    uint8_t rtc_ft201x_stop_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_STOP};       
    state_handler(rtc_ft201x_stop_array_data); // Stop the RTC for the FT201X

    uint8_t rtc_ft201x_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_UNINIT};   
    state_handler(rtc_ft201x_uninit_array_data); // Uninitialize the RTC for the FT201X

    uint8_t lf_clock_stop_array_data[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_STOP};            
    state_handler(lf_clock_stop_array_data); // Stop LF Clock 
     
    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver
}

/*
*   @Brief: Function: usb_handler() is used to handle the incoming messages over USB for debugging
*   purposes.
*/
void usb_handler(void)
{
    NRF_LOG_INFO("_usb_handler");

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    if(FT201X_USB_NORMAL == ft201x_check_usb_state())
    {
        uint8_t ft201x_data_bytes_available = ft201x_available();
        if(ft201x_data_bytes_available > 2)
        {
              uint8_t serial_array_data[ft201x_data_bytes_available];
              ft201x_read_buffer(serial_array_data, ft201x_data_bytes_available);

              if(serial_array_data[0] == USB_COMMAND_HEADER && serial_array_data[LAST_ARRAY_ELEMENT(serial_array_data)] == USB_COMMAND_FOOTER)
              {
                  state_handler(serial_array_data);
              }
              else
              {
                  ft201x_flush_buffers();
              }
        }
    }
    else
    {
        uint8_t disable_usb_handler_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, DISABLE_USB_HANDLER_COMMAND};
        state_handler(disable_usb_handler_array_data);  // Disable USB handler

        uint8_t enable_bluetooth_handler_array_data[3] = {0x00, SERIAL_SLAVE_MODULE, ENABLE_BLUETOOTH_HANDLER_COMMAND};
        state_handler(enable_bluetooth_handler_array_data);  // Enable Bluetooth Handler

        return;
    }

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver
}

/*
*   @Brief: Function: state_handler() is used to handle the different possible states that a command can use to control something.
*/
void state_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_state_handler");

    switch(serial_array_data[1])
    {
        case NRF52_MODULE:
            NRF_LOG_INFO("STATE HANDLER: NRF52_MODULE");
            _nrf52_handler(serial_array_data);
            break;

        case FT201X_MODULE:
            NRF_LOG_INFO("STATE HANDLER: FT201X_MODULE");
            _ft201x_handler(serial_array_data);
            break;

        case BMI160_MODULE:
            NRF_LOG_INFO("STATE HANDLER: BMI160_MODULE");
            _bmi160_handler(serial_array_data);
            break;

        case MAX30003_MODULE:
            NRF_LOG_INFO("STATE HANDLER: MAX30003_MODULE");
            _max30003_handler(serial_array_data);
            break;    

        case TMP117_MODULE:
            NRF_LOG_INFO("STATE HANDLER: TMP117_MODULE");
            _tmp117_handler(serial_array_data);
            break;

        case BLUETOOTH_MODULE:
            NRF_LOG_INFO("STATE HANDLER: BLUETOOTH_MODULE");
            _bluetooth_handler(serial_array_data);
            break;

        case ECG_MODULE:
            NRF_LOG_INFO("STATE HANDLER: ECG_MODULE");
            _ecg_handler(serial_array_data);
            break;

        case CY15B108QI_MODULE:
            NRF_LOG_INFO("STATE HANDLER: CY15B108QI_MODULE");
            _cy15b108qi_handler(serial_array_data);
            break;

        case SERIAL_SLAVE_MODULE:
            NRF_LOG_INFO("STATE HANDLER: SERIAL_SLAVE_MODULE");
            _serial_slave_handler(serial_array_data);
            break;

        default:
            NRF_LOG_INFO("default");
            ft201x_flush_buffers();
            break;
    }

}

static void _serial_slave_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_serial_slave_handler");

    switch(serial_array_data[2])
    {
        case NRF52_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: NRF52_INITIALIZATION_COMMAND");
            _nrf52_initialization();
            break;

        case TMP117_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: TMP117_INITIALIZATION_COMMAND");
            _tmp117_initialization();
            break;

        case CY15B108QI_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: CY15B108QI_INITIALIZATION_COMMAND");
            _cy15b108qi_initialization();
            break;

        case STARTUP_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: STARTUP_INITIALIZATION_COMMAND");
            startup_initialization();
            break;

        case ENABLE_BLUETOOTH_HANDLER_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: ENABLE_BLUETOOTH_HANDLER");
            enable_bluetooth_handler();
            break;

        default:
            break;
    }
}

static void _nrf52_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_nrf52_handler");

    switch(serial_array_data[2])
    {
        case NRF52_HF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_HF_CLOCK_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_HF_CLOCK_START:  
                    hfclock_start();
                    break;

                case NRF52_HF_CLOCK_STOP:
                    hfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_LF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LF_CLOCK_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_LF_CLOCK_START:  
                    lfclock_start();
                    break;

                case NRF52_LF_CLOCK_STOP:
                    lfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_CLOCK_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_NRFX_CLOCK_DRIVER_INIT:  
                    nrf52_nrfx_clock_init();
                    break;

                case NRF52_NRFX_CLOCK_DRIVER_UNINIT:
                    nrf52_nrfx_clock_uninit();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_RTC_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_RTC_CLOCK_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_RTC_FT201X_INIT:
                    rtc_ft201x_init();
                    break;

                case NRF52_RTC_FT201X_UNINIT:
                    nrfx_rtc_ft201x_uninit();
                    break;

                case NRF52_RTC_FT201X_START:  
                    rtc_ft201x_start();
                    break;

                case NRF52_RTC_FT201X_STOP:
                    rtc_ft201x_stop();
                    break;

                case NRF52_RTC_FT201X_RESTART:  
                    rtc_ft201x_restart();
                    break;

                case NRF52_RTC_FT201X_SET_COUNTER:
                {
                    uint32_t temp_counter = serial_array_data[4] << 8 | serial_array_data[5];  
                    rtc_ft201x_set_counter(temp_counter);
                    break;
                }

                case NRF52_RTC_TMP117_INIT:
                    rtc_tmp117_init();
                    break;

                case NRF52_RTC_TMP117_UNINIT:
                    nrfx_rtc_tmp117_uninit();
                    break;

                case NRF52_RTC_TMP117_START:  
                    rtc_tmp117_start();
                    break;

                case NRF52_RTC_TMP117_STOP:
                    rtc_tmp117_stop();
                    break;

                case NRF52_RTC_TMP117_RESTART:  
                    rtc_tmp117_restart();
                    break;

                case NRF52_RTC_TMP117_SET_COUNTER:
                {
                    uint32_t temp_counter = serial_array_data[4] << 8 | serial_array_data[5];  
                    rtc_tmp117_set_counter(temp_counter);
                    break;
                }
                default:
                    break;
            }
            break;

        case NRF52_POWER_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_POWER_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_POWER_DCDC_CONVERTER_ENABLE:
                    enable_dcdc_converter();
                    break;

                case NRF52_POWER_DCDC_CONVERTER_DISABLE:  
                    disable_dcdc_converter();
                    break;

                case NRF52_POWER_SLEEP_MODE_ENTER:
                    sleep_mode_enter();
                    break;

                case NRF52_POWER_DEEP_SLEEP_MODE_ENTER:  
                    deep_sleep_mode_enter();
                    break;

                case NRF52_POWER_MANAGER_HANDLER:  
                    power_manager_handler();
                    break;

                case NRF52_POWER_MANAGER_INIT:  
                    power_manager_init();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_COMMON_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_COMMON_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_LED_INIT:
                    init_leds();
                    break;

                case NRF52_LED_IND_LED_ON:  
                    ind_led_on();
                    break;

                case NRF52_LED_IND_LED_OFF:
                    ind_led_off();
                    break;

                case NRF52_LED_IND_BLINK: 
                    switch(serial_array_data[4])
                    {
                        case NRF52_LED_IND_CUSTOM_BLINK:
                        {
                            uint16_t ind_led_on_ms = (uint16_t)(((uint16_t)serial_array_data[5] << 8) | serial_array_data[6]);
                            uint16_t ind_led_off_ms = (uint16_t)(((uint16_t)serial_array_data[7] << 8) | serial_array_data[8]);
                            ind_led_blink(ind_led_on_ms, ind_led_off_ms);
                            break;
                        }         
                        case NRF52_LED_IND_SHORT_BLINK:
                            ind_led_short_blink();
                            break;
                               
                        case NRF52_LED_IND_MEDIUM_BLINK:
                            ind_led_medium_blink();
                            break;

                        case NRF52_LED_IND_LONG_BLINK:
                            ind_led_long_blink();
                            break;

                        default:
                            break;
                    }                                                                                       
                    break;

                case NRF52_LED_BLE_LED_ON:  
                    ble_led_on();
                    break;

                case NRF52_LED_BLE_LED_OFF:
                    ble_led_off();
                    break;

                case NRF52_LED_BLE_BLINK:
                {
                    uint16_t ble_led_on_ms = (uint16_t)(((uint16_t)serial_array_data[4] << 8) | serial_array_data[5]);
                    uint16_t ble_led_off_ms = (uint16_t)(((uint16_t)serial_array_data[6] << 8) | serial_array_data[7]);
                    ble_led_blink(ble_led_on_ms, ble_led_off_ms);
                    break;
                }
                case NRF52_LOG_INIT:
                    log_init();
                    break;

                case NRF52_LDO_INIT:
                    ldo_init();
                    break;

                case NRF52_INPUT_OUTPUT_INIT:
                    input_output_init();
                    break;

                case NRF52_VCC_LDO_EN:
                    enable_vcc_ldo();
                    break;

                case NRF52_MAX30003_POWER_LDO_EN:
                    enable_max30003_power_ldo();
                    break;

                case NRF52_GPIOTE_INIT:
                    gpiote_init();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_I2C_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_I2C_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_I2C_TWIM_INIT:
                    twim_init();
                    break;

                case NRF52_I2C_TWIM_UNINIT:
                    twim_uninit();
                    break;

                case NRF52_I2C_TWIM_ENABLE:
                    twim_enable();
                    break;

                case NRF52_I2C_TWIM_DISABLE:
                    twim_disable();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_SPI_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_SPIM_COMMAND");

            switch(serial_array_data[3])
            {
                case NRF52_SPI_SPIM_INIT:
                    spim_init();
                    break;

                case NRF52_SPI_SPIM_ENABLE:
                    spim_enable();
                    break;

                case NRF52_SPI_SPIM_DISABLE:
                    spim_disable();
                    break;
                
                case NRF52_SPI_SPIM_UNINIT:
                    spim_uninit();
                    break;

                case NRF52_SPI_SPIM_SELECT_CS_PIN:
                    spim_select_cs_pin(serial_array_data[4]);
                    break;

                case NRF52_SPI_SPIM_INIT_CS_PIN:
                    spim_init_cs_pin(serial_array_data[4]);
                    break;                    

                case NRF52_SPI_SPIM_SETUP:
                    spim_setup();
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

static void _ft201x_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_ft201x_handler");

    switch(serial_array_data[2])
    {
        case FT201X_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_CHIP_ID_COMMAND");
            break;

        case FT201X_SET_VCP_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_SET_VCP_COMMAND");
            break;

        case FT201X_READ_EEPROM_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_EEPROM_COMMAND");
            break;

        case FT201X_WRITE_EEPROM_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_WRITE_EEPROM_COMMAND");
            break;

        case FT201X_WRITE_DATA_ARRAY_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_WRITE_DATA_ARRAY_COMMAND");
//            ft201x_write_buffer(&serial_array_data[3], serial_array_data[4]);   // Pointer to Data Array, Number of elements in Data Array
            break;
        
        case FT201X_READ_DATA_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_DATA_COMMAND");
            break;

        default:
            break;
    }
}

static void _bmi160_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_bmi160_handler");

    switch(serial_array_data[2])
    {
        case BMI160_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_READ_CHIP_ID_COMMAND");
            break;

        case BMI160_INIT_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_INIT_COMMAND");
            break;

        case BMI160_READ_ACCEL_GYRO_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_READ_ACCEL_GYRO_COMMAND");
            break;

        default:
            break;
    }
}

static void _tmp117_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_tmp117_handler");

    switch(serial_array_data[2])
    {
        case TMP117_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("tmp117_MODULE: TMP117_READ_CHIP_ID_COMMAND");
            uint16_t tmp117_chip_id = tmp117_read_chip_id();
            uint8_t tmp117_chip_id_array_data[5] = {0x00, FT201X_MODULE, FT201X_WRITE_DATA_ARRAY_COMMAND, (tmp117_chip_id & 0xF0), (tmp117_chip_id & 0x0F)};  // Write TMP117 Chip ID
            state_handler(tmp117_chip_id_array_data);
            break;

        case TMP117_READ_REVISION_NUMBER_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_READ_CHIP_ID_COMMAND");
            uint8_t tmp117_revision_number = tmp117_read_revision_number();
            uint8_t tmp117_revision_number_array_data[4] = {0x00, FT201X_MODULE, FT201X_WRITE_DATA_ARRAY_COMMAND, tmp117_chip_id};  // Write TMP117 Chip ID
            state_handler(tmp117_revision_number_array_data);
            break;

        case TMP117_UNLOCK_EEPROM_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_UNLOCK_EEPROM_COMMAND");
            tmp117_unlock_eeprom();
            break;

        case TMP117_INIT_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_INIT_COMMAND");
            tmp117_init(serial_array_data[3], serial_array_data[4]);
            break;

        case TMP117_SET_OPERATING_MODE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_SET_OPERATING_MODE_COMMAND");
            tmp117_set_operating_mode(serial_array_data[3], serial_array_data[4]);
            break;

        case TMP117_GENERAL_CALL_RESET_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_GENERAL_CALL_RESET_COMMAND");   
            tmp117_general_call_reset();
            break;

        case TMP117_TEMP_UINT16_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_UINT16_COMMAND");
            uint16_t tmp117_temp_value = tmp117_get_uint16_t();
            uint8_t tmp117_temp_value_array_data[5] = {0x00, FT201X_MODULE, FT201X_WRITE_DATA_ARRAY_COMMAND, (tmp117_temp_value & 0xF0), (tmp117_temp_value & 0x0F)};  // Write TMP117 Temperature Value
            state_handler(tmp117_temp_value_array_data);             
            break;

        case TMP117_TEMP_CHAR_ARRAY_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_TEMP_CHAR_ARRAY_COMMAND");    
            uint8_t tmp117_uint8_t[5];
            tmp117_get_uint8_t(tmp117_uint8_t);
            uint8_t tmp117_uint8_t_array_data[8] = {0x00, FT201X_MODULE, FT201X_WRITE_DATA_ARRAY_COMMAND, tmp117_uint8_t[4], tmp117_uint8_t[3],
            tmp117_uint8_t[2], tmp117_uint8_t[1], tmp117_uint8_t[0]};  // Write TMP117 Temperature Value
            state_handler(tmp117_uint8_t_array_data);           
            break;

        default:
            break;
    }
}

static void _ecg_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_ecg_handler");

    switch(serial_array_data[2])
    {
        case ECG_INIT:
            NRF_LOG_INFO("ECG_MODULE: ECG_INIT");
            ecg_init();
            break;

        case ECG_START_DATA_RECORDING:
            NRF_LOG_INFO("ECG_MODULE: ECG_START_DATA_RECORDING");
            ecg_start_data_recording();
        default:
            break;
    }

}

static void _cy15b108qi_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_cy15b108qi_handler");

    switch(serial_array_data[2])
    {
        case CY15B108QI_INIT_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: CY15B108QI_INIT");
            cy15b108qi_init();
            break;
        
        case CY15B108QI_UNINIT_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: CY15B108QI_UNINIT");
            cy15b108qi_uninit();
            break;

        case CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: ENTER_DEEP_POWER_DOWN_MODE");
            cy15b108qi_enter_deep_power_down_mode_command();
            break;

        case CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: EXIT_DEEP_POWER_DOWN_MODE");
            cy15b108qi_exit_deep_power_down_mode_command();
            break;

        case CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: ENTER_HIBERNATION_MODE");
            cy15b108qi_enter_hibernation_mode_command();
            break;

        case CY15B108QI_EXIT_HIBERNATION_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: EXIT_HIBERNATION_MODE");
            cy15b108qi_exit_hibernation_mode_command();
            break;

        case CY15B108QI_SET_WRITE_ENABLE_LATCH_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: SET_WRITE_ENABLE_LATCH");
            cy15b108qi_set_write_enable_latch_command();
            break;

        case CY15B108QI_RESET_WRITE_ENABLE_LATCH_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: RESET_WRITE_ENABLE_LATCH");
            cy15b108qi_reset_write_enable_latch_command();
            break;

        case CY15B108QI_READ_DEVICE_ID_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: READ_DEVICE_ID");
            cy15b108qi_read_device_id_command();
            break;

        case CY15B108QI_READ_UNIQUE_ID_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: READ_UNIQUE_ID");
            cy15b108qi_read_unique_id_command();
            break;

        case CY15B108QI_WRITE_STATUS_REGISTER_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: WRITE_STATUS_REGISTER");
            cy15b108qi_write_status_register();
            break;

        case CY15B108QI_READ_STATUS_REGISTER_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: READ_STATUS_REGISTER");
            cy15b108qi_read_status_register();
            break;

        default:
            break;
    }
}

static void _max30003_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_max30003_handler");

    switch(serial_array_data[2])
    {
        case MAX30003_READ_INFO_REGISTER_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_INFO_REGISTER_COMMAND");
            max30003_read_info_register();
            break;

        case MAX30003_READ_STATUS_REGISTER_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_STATUS_REGISTER_COMMAND");
            max30003_read_status_register();
            break;

        case MAX30003_READ_GENERAL_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_GENERAL_CONFIGURATION_COMMAND");
            max30003_read_general_configuration_register();
            break;

        case MAX30003_WRITE_GENERAL_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_GENERAL_CONFIGURATION_COMMAND");
            max30003_write_general_configuration_register();
            break;

        case MAX30003_READ_CALIBRATION_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_CALIBRATION_CONFIGURATION_COMMAND");
            max30003_read_calibration_configuration_register();
            break;

        case MAX30003_WRITE_CALIBRATION_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_CALIBRATION_CONFIGURATION_COMMAND");
            max30003_write_calibration_configuration_register();
            break;

        case MAX30003_READ_INPUT_MULTIPLEXER_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_INPUT_MULTIPLEXER_CONFIGURATION_COMMAND");
            max30003_read_input_multiplexer_configuration_register();
            break;

        case MAX30003_WRITE_INPUT_MULTIPLEXER_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_INPUT_MULTIPLEXER_CONFIGURATION_COMMAND");
            max30003_write_input_multiplexer_configuration_register();
            break;

        case MAX30003_READ_ECG_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_ECG_CONFIGURATION_COMMAND");
            max30003_read_ecg_configuration_register();
            break;

        case MAX30003_WRITE_ECG_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_ECG_CONFIGURATION_COMMAND");
            max30003_write_ecg_configuration_register();
            break;

        case MAX30003_READ_RTOR1_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_RTOR1_CONFIGURATION_COMMAND");
            max30003_read_rtor1_configuration_register();
            break;

        case MAX30003_WRITE_RTOR1_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_RTOR1_CONFIGURATION_COMMAND");
            max30003_write_rtor1_configuration_register();
            break;

        case MAX30003_READ_RTOR2_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_RTOR2_CONFIGURATION_COMMAND");
            max30003_read_rtor2_configuration_register();
            break;

        case MAX30003_WRITE_RTOR2_CONFIGURATION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_RTOR2_CONFIGURATION_COMMAND");
            max30003_write_rtor2_configuration_register();
            break;

        case MAX30003_READ_ECG_FIFO_MEMORY_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_ECG_FIFO_MEMORY_COMMAND");
            max30003_read_ecg_fifo_memory_register();
            break;

        case MAX30003_SOFT_RESET_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_SOFT_RESET_COMMAND");
            max30003_soft_reset();
            break;

        case MAX30003_SYNC_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_SYNC_COMMAND");
            max30003_sync(); 
            break;

        case MAX30003_INIT_INTERRUPT_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_INIT_INTERRUPT_COMMAND");
            max30003_init_pin_interrupt(); 
            break;
            
        case MAX30003_ENABLE_PIN_INTERRUPT_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_ENABLE_PIN_INTERRUPT_COMMAND");
            max30003_enable_pin_interrupt(); 
            break;

        case MAX30003_DISABLE_PIN_INTERRUPT_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_DISABLE_PIN_INTERRUPT_COMMAND");
            max30003_disable_pin_interrupt(); 
            break;

        case MAX30003_INTERRUPT1_DISABLE_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_INTERRUPT1_DISABLE_COMMAND");
            max30003_interrupt1_disable();
            break;

        case MAX30003_GET_ECG_VOLTAGE_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_GET_ECG_VOLTAGE_COMMAND");
//            uint16_t max30003_data_array[MAX30003_MAX_FIFO_WORDS];
//            max30003_get_ecg_voltage(max30003_data_array, ARRAY_LENGTH(max30003_data_array)); 
            break;

      case MAX30003_READ_INTERRUPT1_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_INTERRUPT1_REGISTER_COMMAND");
          max30003_read_interrupt1_register();
          break;

      case MAX30003_WRITE_INTERRUPT1_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_INTERRUPT1_REGISTER_COMMAND");
          max30003_write_interrupt1_register();
          break;

      case MAX30003_READ_INTERRUPT2_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_INTERRUPT2_REGISTER_COMMAND");
          max30003_read_interrupt2_register();
          break;

      case MAX30003_WRITE_INTERRUPT2_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_INTERRUPT2_REGISTER_COMMAND");
          max30003_write_interrupt2_register();
          break;
          
      case MAX30003_READ_INTERRUPT_MANAGER_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_INTERRUPT_MANAGER_REGISTER_COMMAND");
          max30003_read_interrupt_manager_register();
          break;

      case MAX30003_WRITE_INTERRUPT_MANAGER_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_WRITE_INTERRUPT_MANAGER_REGISTER_COMMAND");
          max30003_write_interrupt_manager_register();
          break;
          
      case MAX30003_INIT_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: MAX30003_INIT_COMMAND");
          max30003_init();
          break;

      case MAX30003_START_RECORDING_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: START_RECORDING");
          max30003_start_recording();
          break;

      case MAX30003_WRITE_FIFO_RESET_REGISTER_COMMAND:
          NRF_LOG_INFO("MAX30003_MODULE: WRITE_FIFO_RESET_REGISTER");
          max30003_write_fifo_reset_register();
          break;

        default:
            break;
    }
}

static void _bluetooth_handler(uint8_t *serial_array_data)
{
    NRF_LOG_INFO("_bluetooth_handler");

    switch(serial_array_data[2])
    {
        case BLUETOOTH_INIT_GAP_PARAMS_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_GAP_PARAMS");
            gap_params_init();
            break;

        case BLUETOOTH_INIT_GATT_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_GATT");
            gatt_init();
            break;

        case BLUETOOTH_INIT_BLE_STACK_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_BLE_STACK");
            ble_stack_init();
            break;

        case BLUETOOTH_INIT_SERVICES_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_SERVICES");
            services_init();
            break;

        case BLUETOOTH_INIT_CONN_PARAMS_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_CONN_PARAMS");
            conn_params_init();
            break;

        case BLUETOOTH_INIT_PEER_MANAGER_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_PEER_MANAGER");
            peer_manager_init();
            break;

        case BLUETOOTH_INIT_ADVERTISING_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_ADVERTISING");
            advertising_init();
            break;

        case BLUETOOTH_SET_ADVERTISING_POWER_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: SET_ADVERTISING_POWER");
            set_advertising_power();
            break;

        case BLUETOOTH_START_ADVERTISING_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: START_ADVERTISING");
            advertising_start();
            break;

        case BLUETOOTH_TRANSMIT_RECORDING_SESSION_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: TRANSMIT_RECORDING_SESSION");
            bluetooth_transmit_recording_session();
            break;

        default:
            break;
    }
}


