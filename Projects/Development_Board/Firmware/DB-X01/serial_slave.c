#include "serial_slave.h"

static struct State_Machine_Struct state_machine;

/*
* @Brief: Function: startup_initialization() is used to initialize the NRF52, TMP117, MAX30003, and CY15B108QI
*/
void startup_initialization(void)
{
    uint8_t nrf52_initialization_command[3] = {0x00, SERIAL_SLAVE_MODULE, NRF52_INITIALIZATION_COMMAND};       
    state_handler(nrf52_initialization_command); // Initialize NRF52 Module
    
    #if TMP117
    uint8_t tmp117_initialization_command[3] = {0x00, SERIAL_SLAVE_MODULE, TMP117_INITIALIZATION_COMMAND};       
    state_handler(tmp117_initialization_command); // Initialize TMP117 Module
    #endif

    #if MAX30003
    uint8_t max30003_initialization_command[3] = {0x00, SERIAL_SLAVE_MODULE, MAX30003_INITIALIZATION_COMMAND};       
    state_handler(max30003_initialization_command); // Initialize MAX30003 Module
    #endif

//    #if CY15B108QI
//    uint8_t cy15b108qi_initialization_command[3] = {0x00, SERIAL_SLAVE_MODULE, CY15B108QI_INITIALIZATION_COMMAND};       
//    state_handler(cy15b108qi_initialization_command); // Initialize CY15B108QI Module
//    #endif

    #if FDC1004
    uint8_t fdc1004_initialization_command[3] = {0x00, SERIAL_SLAVE_MODULE, FDC1004_INITIALIZATION_COMMAND};       
    state_handler(fdc1004_initialization_command); // Initialize FDC1004 Module
    #endif
}

/*
*   @Brief: Function: _nrf52_initialization() is used to initialize common functionalities. 
*/
static void _nrf52_initialization(void)
{
    uint8_t log_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LOG_INIT};       
    state_handler(log_command); // Enable LOG Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t nrfx_clock_init_command[4] = {0x00, NRF52_MODULE, NRF52_CLOCK_COMMAND, NRF52_NRFX_CLOCK_DRIVER_INIT};   
    state_handler(nrfx_clock_init_command); // Init and Enable NRFX Clock 

    uint8_t hf_clock_start_command[4] = {0x00, NRF52_MODULE, NRF52_HF_CLOCK_COMMAND, NRF52_HF_CLOCK_START};   
    state_handler(hf_clock_start_command); // Start HF Clock

    uint8_t lf_clock_start_command[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_START};   
    state_handler(lf_clock_start_command); // Start LF Clock 

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t set_pin_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_INPUT_OUTPUT_INIT};       
    state_handler(set_pin_command); // Initialize Input and Output Pins

    uint8_t ldo_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LDO_INIT};       
    state_handler(ldo_command); // Enable LDO Drivers

    uint8_t vcc_en_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_VCC_LDO_EN};   
    state_handler(vcc_en_command); // Enable VCC LDO  

    uint8_t power_manager_command[4] = {0x00, NRF52_MODULE, NRF52_POWER_COMMAND, NRF52_POWER_MANAGER_INIT};  
    state_handler(power_manager_command); // Enable Power Manager

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    #if SPI
    uint8_t spim_setup_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SETUP};
    state_handler(spim_setup_command); // Setup SPIM Module
    #endif

    #if I2C
    uint8_t twim_setup_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_SETUP};  
    state_handler(twim_setup_command); // Setup TWIM Module
    #endif

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t led_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_INIT};  
    state_handler(led_command); // Enable LED Driver

    uint8_t medium_blink_led_command[5] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_IND_BLINK, NRF52_LED_IND_MEDIUM_BLINK};  
    state_handler(medium_blink_led_command); // IND LED Medium Blink

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t rtc_init_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_SENSOR_INIT};
    state_handler(rtc_init_command); // Initialize the RTC for the available sensors
}

/*
*   @Brief: Function: _tmp117_initialization() is used to initialize the TMP117 module
*/
#if TMP117
static void _tmp117_initialization(void)
{
    NRF_LOG_INFO("_tmp117_initialization");

    uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_command); // Init TWIM Driver

    uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_command); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t tmp117_shutdown_command[5] = {0x00, TMP117_MODULE, TMP117_INIT_COMMAND};  
    state_handler(tmp117_shutdown_command); // Initializing the TMP117

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_command); // Disable TWIM Driver

    uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_command); // Uninit TWIM Driver
}
#endif

/*
*   @Brief: Function: _max30003_initialization() is used to initialize the MAX30003 module
*/
#if MAX30003
static void _max30003_initialization(void)
{
    NRF_LOG_INFO("_max30003_initialization");

    uint8_t max30003_power_ldo_enable_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_MAX30003_POWER_LDO_EN};
    state_handler(max30003_power_ldo_enable_command); // Enable 1.8V Supply for MAX30003

    uint8_t nrf52_gpiote_init_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_GPIOTE_INIT};
    state_handler(nrf52_gpiote_init_command); // Initialize the GPIOTE Interrupt to detect the interupt from the MAX30003

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_init_cs_pin_command); // Initialize the Chip Select Pin for the MAX30003

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Select the Chip Select Pin for the MAX30003 for the SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module
 
    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t max30003_init_command[3] = {0x00, MAX30003_MODULE, MAX30003_INIT_COMMAND}; 
    state_handler(max30003_init_command); // MAX30003: Init Command 

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module 
}
#endif

/*
*   @Brief: Function: _cy15b108qi_initialization() is used to initialize the CY15B108QI module 
*/
#if CY15B108QI
static void _cy15b108qi_initialization(void)
{
    NRF_LOG_INFO("_cy15b108qi_initialization");

    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_init_cs_pin_command); // Initialize the Chip Select Pin for the CY15B108QI

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Select the Chip Select Pin for the MAX30003 for the SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t cy15b108qi_init_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_INIT_COMMAND}; 
    state_handler(cy15b108qi_init_command); // CY15B108QI: Init Command    

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module 
}
#endif

/*
*   @Brief: Function: _fdc1004_initialization() is used to initialize the FDC1004 module
*/
#if FDC1004
static void _fdc1004_initialization(void)
{
    NRF_LOG_INFO("_fdc1004_initialization");

    uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_command); // Init TWIM Driver

    uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_command); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t fdc1004_init_command[5] = {0x00, FDC1004_MODULE, FDC1004_INIT_COMMAND};  
    state_handler(fdc1004_init_command); // Initialize the FDC1004

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_command); // Disable TWIM Driver

    uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_command); // Uninit TWIM Driver
}
#endif

/*
*   @Brief: Function: enable_bluetooth_handler() is used to enable the bluetooth handler 
*   purposes.
*/
void enable_bluetooth_handler(void)
{
    NRF_LOG_INFO("enable_bluetooth_handler");

    state_machine.ble_response_char[0] = 0x00;
    state_machine.ble_response_char[1] = BLUETOOTH_MODULE;
    state_machine.ble_response_char[2] = BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND;
    state_machine.ble_response_char[3] = 0x00;
    state_machine.ble_response_char[4] = 0x00;
    state_machine.ble_response_char[5] = 0x00;
    state_machine.ble_response_char[6] = 0x00;

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t bluetooth_ble_stack_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_BLE_STACK_COMMAND};
    state_handler(bluetooth_ble_stack_init_command); // Initialize the BLE stack for Bluetooth

    uint8_t bluetooth_gap_params_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_GAP_PARAMS_COMMAND};
    state_handler(bluetooth_gap_params_init_command); // Initialize the GAP Parameters for Bluetooth communication

    uint8_t bluetooth_gatt_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_GATT_COMMAND};
    state_handler(bluetooth_gatt_init_command); // Initialize the BLE GATT Stack

    uint8_t bluetooth_services_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_SERVICES_COMMAND};
    state_handler(bluetooth_services_init_command); // Initialize the services inside the BLE stack

    uint8_t bluetooth_advertising_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_ADVERTISING_COMMAND};
    state_handler(bluetooth_advertising_init_command); // Initialize the Advertising Module for Bluetooth

    uint8_t bluetooth_set_advertising_power_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_SET_ADVERTISING_POWER_COMMAND};
    state_handler(bluetooth_set_advertising_power_command); // Set the Advertising Power for the Bluetooth Module

    uint8_t bluetooth_conn_params_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_CONN_PARAMS_COMMAND};
    state_handler(bluetooth_conn_params_init_command); // Initialize the Connection Parameters Between the GATT Server and Client

    uint8_t bluetooth_peer_manager_init_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_INIT_PEER_MANAGER_COMMAND};
    state_handler(bluetooth_peer_manager_init_command); // Initialize the Peer Manager to control the interaction between the GATT Server and Client

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t enable_dcdc_converter_command[4] = {0x00, NRF52_MODULE, NRF52_POWER_COMMAND, NRF52_POWER_DCDC_CONVERTER_ENABLE};
    state_handler(enable_dcdc_converter_command); // Enable the DCDC Converter to reduce power consumption during BLE transmission

    uint8_t bluetooth_advertising_start_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_START_ADVERTISING_COMMAND};
    state_handler(bluetooth_advertising_start_command); // Start advertising data between the GATT Server and Client
}

void bluetooth_handler(uint8_t *settings_char_command)
{
    NRF_LOG_INFO("bluetooth_handler");
    if(settings_char_command[0] == BLUETOOTH_COMMAND_HEADER && settings_char_command[CONFIGURATION_SERVICE_SETTINGS_CHAR_LENGTH-1] == BLUETOOTH_COMMAND_FOOTER)
    {
        NRF_LOG_INFO("Command Header and Footer are correct");
        state_machine.ble_response_char[6] = BLUETOOTH_RESPONSE_CHAR_MESSAGE_RECEIVED;
        state_handler(state_machine.ble_response_char);
        _reset_response_char();
        state_handler(settings_char_command);
    }
    else
    {
        NRF_LOG_INFO("Command Header and Footer are incorrect");
        NRF_LOG_INFO("Header: %X", settings_char_command[0]);
        NRF_LOG_INFO("Footer: %X", settings_char_command[CONFIGURATION_SERVICE_SETTINGS_CHAR_LENGTH-1]);

        state_machine.ble_response_char[6] = BLUETOOTH_RESPONSE_CHAR_HEADER_FOOTER_INCORRECT;
        state_handler(state_machine.ble_response_char);
        _reset_response_char();
    }
}

/*
*   @Brief: Function: enable_serial_slave_handler() is used to initialize the serial debugger to debug and communicate with the NRF52 
*   using an I2C to USB Converter (FT201X). It creates a RTC so that an interrupt occurs every 1 second. When an RTC
*/
#if FT201X
void enable_usb_handler(void)
{
    NRF_LOG_INFO("_enable_usb_handler");

    uint8_t lf_clock_start_command[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_START};   
    state_handler(lf_clock_start_command); // Start LF Clock 

    uint8_t rtc_ft201x_init_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_INIT};  
    state_handler(rtc_ft201x_init_command); // Initialize the RTC for the FT201X

    uint8_t rtc_ft201x_set_counter_command[6] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_SET_COUNTER, 0x00, 0x08};  
    state_handler(rtc_ft201x_set_counter_command); // Setting the counter for the RTC for the FT201X to 1 second

    uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_command); // Init TWIM Driver

    uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_command); // Enable TWIM Driver

    uint8_t rtc_ft201x_start_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_START};  
    state_handler(rtc_ft201x_start_command); // Start the RTC for the FT201X
}

/*
*   @Brief: Function: _disable_serial_slave_handler() is used to uninitialize the serial debugger since it is not available for debugging 
*   purposes.
*/
static void _disable_usb_handler(void)
{
    NRF_LOG_INFO("_disable_usb_handler");

    uint8_t rtc_ft201x_stop_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_STOP};       
    state_handler(rtc_ft201x_stop_command); // Stop the RTC for the FT201X

    uint8_t rtc_ft201x_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_FT201X_UNINIT};   
    state_handler(rtc_ft201x_uninit_command); // Uninitialize the RTC for the FT201X

    uint8_t lf_clock_stop_command[4] = {0x00, NRF52_MODULE, NRF52_LF_CLOCK_COMMAND, NRF52_LF_CLOCK_STOP};            
    state_handler(lf_clock_stop_command); // Stop LF Clock 
     
    uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_command); // Disable TWIM Driver

    uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_command); // Uninit TWIM Driver
}

/*
*   @Brief: Function: usb_handler() is used to handle the incoming messages over USB for debugging
*   purposes.
*/
void usb_handler(void)
{
    NRF_LOG_INFO("_usb_handler");

    uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_command); // Init TWIM Driver

    uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_command); // Enable TWIM Driver

    if(FT201X_USB_NORMAL == ft201x_check_usb_state())
    {
        uint8_t ft201x_data_bytes_available = ft201x_available();
        if(ft201x_data_bytes_available > 2)
        {
              uint8_t serial_command[ft201x_data_bytes_available];
              ft201x_read_buffer(serial_command, ft201x_data_bytes_available);

              if(serial_command[0] == USB_COMMAND_HEADER && serial_command[LAST_ARRAY_ELEMENT(serial_command)] == USB_COMMAND_FOOTER)
              {
                  state_handler(serial_command);
              }
              else
              {
                  ft201x_flush_buffers();
              }
        }
    }
    else
    {
        uint8_t disable_usb_handler_command[3] = {0x00, SERIAL_SLAVE_MODULE, DISABLE_USB_HANDLER_COMMAND};
        state_handler(disable_usb_handler_command);  // Disable USB handler

        uint8_t enable_bluetooth_handler_command[3] = {0x00, SERIAL_SLAVE_MODULE, ENABLE_BLUETOOTH_HANDLER_COMMAND};
        state_handler(enable_bluetooth_handler_command);  // Enable Bluetooth Handler

        return;
    }

    uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_command); // Disable TWIM Driver

    uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_command); // Uninit TWIM Driver
}
#endif

/*
*   @Brief: Function: state_handler() is used to handle the different possible states that a command can use to control something.
*/
void state_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_state_handler");

    switch(serial_command[1])
    {
        case NRF52_MODULE:
            NRF_LOG_INFO("STATE HANDLER: NRF52_MODULE");
            _nrf52_handler(serial_command);
            break;
        
        #if FT201X
        case FT201X_MODULE:
            NRF_LOG_INFO("STATE HANDLER: FT201X_MODULE");
            _ft201x_handler(serial_command);
            break;
        #endif

        #if BMI160
        case BMI160_MODULE:
            NRF_LOG_INFO("STATE HANDLER: BMI160_MODULE");
            _bmi160_handler(serial_command);
            break;
        #endif

        #if MAX30003
        case MAX30003_MODULE:
            NRF_LOG_INFO("STATE HANDLER: MAX30003_MODULE");
            _max30003_handler(serial_command);
            break;
        #endif        

        #if TMP117
        case TMP117_MODULE:
            NRF_LOG_INFO("STATE HANDLER: TMP117_MODULE");
            _tmp117_handler(serial_command);
            break;
        #endif

        case BLUETOOTH_MODULE:
            NRF_LOG_INFO("STATE HANDLER: BLUETOOTH_MODULE");
            _bluetooth_handler(serial_command);
            break; 

        #if CY15B108QI
        case CY15B108QI_MODULE:
            NRF_LOG_INFO("STATE HANDLER: CY15B108QI_MODULE");
            _cy15b108qi_handler(serial_command);
            break;
        #endif

        #if FDC1004
        case FDC1004_MODULE:
            NRF_LOG_INFO("STATE HANDLER: FDC1004_MODULE");
            _fdc1004_handler(serial_command);
            break;
        #endif

        case SERIAL_SLAVE_MODULE:
            NRF_LOG_INFO("STATE HANDLER: SERIAL_SLAVE_MODULE");
            _serial_slave_handler(serial_command);
            break;

        default:
            NRF_LOG_INFO("default");
            #if FT201X
            ft201x_flush_buffers();
            #endif
            break;
    }
}

static void _serial_slave_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_serial_slave_handler");

    switch(serial_command[2])
    {
        case NRF52_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: NRF52_INITIALIZATION_COMMAND");
            _nrf52_initialization();
            break;
        
        #if TMP117
        case TMP117_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: TMP117_INITIALIZATION_COMMAND");
            _tmp117_initialization();
            break;
        #endif

        #if CY15B108QI
        case CY15B108QI_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: CY15B108QI_INITIALIZATION_COMMAND");
            _cy15b108qi_initialization();
            break;
        #endif

        #if MAX30003
        case MAX30003_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: MAX30003_INITIALIZATION_COMMAND");
            _max30003_initialization();
            break;
        #endif

        #if FDC1004
        case FDC1004_INITIALIZATION_COMMAND:
            NRF_LOG_INFO("SERIAL_SLAVE_MODULE: FDC1004_INITIALIZATION_COMMAND");
            _fdc1004_initialization();
            break;
        #endif

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

static void _nrf52_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_nrf52_handler");

    switch(serial_command[2])
    {
        case NRF52_HF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_HF_CLOCK_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_HF_CLOCK_START:
                    NRF_LOG_INFO("NRF52_HF_CLOCK_START");
                    hfclock_start();
                    break;

                case NRF52_HF_CLOCK_STOP:
                    NRF_LOG_INFO("NRF52_HF_CLOCK_STOP");
                    hfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_LF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LF_CLOCK_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_LF_CLOCK_START:
                    NRF_LOG_INFO("NRF52_LF_CLOCK_START");
                    lfclock_start();
                    break;

                case NRF52_LF_CLOCK_STOP:
                    NRF_LOG_INFO("NRF52_LF_CLOCK_STOP");
                    lfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_CLOCK_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_NRFX_CLOCK_DRIVER_INIT:
                    NRF_LOG_INFO("NRF52_NRFX_CLOCK_DRIVER_INIT");
                    nrf52_nrfx_clock_init();
                    break;

                case NRF52_NRFX_CLOCK_DRIVER_UNINIT:
                    NRF_LOG_INFO("NRF52_NRFX_CLOCK_DRIVER_UNINIT");
                    nrf52_nrfx_clock_uninit();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_RTC_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_RTC_CLOCK_COMMAND");

            switch(serial_command[3])
            {
                #if FT201X
                case NRF52_RTC_FT201X_INIT:
                    NRF_LOG_INFO("NRF52_RTC_FT201X_INIT");
                    rtc_ft201x_init();
                    break;

                case NRF52_RTC_FT201X_UNINIT:
                    NRF_LOG_INFO("NRF52_RTC_FT201X_UNINIT");
                    nrfx_rtc_ft201x_uninit();
                    break;

                case NRF52_RTC_FT201X_START:
                    NRF_LOG_INFO("NRF52_RTC_FT201X_START");  
                    rtc_ft201x_start();
                    break;

                case NRF52_RTC_FT201X_STOP:
                    NRF_LOG_INFO("NRF52_RTC_FT201X_STOP");
                    rtc_ft201x_stop();
                    break;

                case NRF52_RTC_FT201X_RESTART:
                    NRF_LOG_INFO("NRF52_RTC_FT201X_RESTART");
                    rtc_ft201x_restart();
                    break;

                case NRF52_RTC_FT201X_SET_COUNTER:
                {
                    NRF_LOG_INFO("NRF52_RTC_FT201X_SET_COUNTER");
                    uint32_t temp_counter = serial_command[4] << 8 | serial_command[5];  
                    rtc_ft201x_set_counter(temp_counter);
                    break;
                }
                #endif

                case NRF52_RTC_SENSOR_INIT:
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_INIT");
                    rtc_sensor_init();
                    break;

                case NRF52_RTC_SENSOR_UNINIT:
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_UNINIT");
                    nrfx_rtc_sensor_uninit();
                    break;

                case NRF52_RTC_SENSOR_START:
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_START");
                    rtc_sensor_start();
                    break;

                case NRF52_RTC_SENSOR_STOP:
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_STOP");
                    rtc_sensor_stop();
                    break;

                case NRF52_RTC_SENSOR_RESTART:
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_RESTART");
                    rtc_sensor_restart();
                    break;

                case NRF52_RTC_SENSOR_SET_COUNTER:
                {
                    NRF_LOG_INFO("NRF52_RTC_SENSOR_SET_COUNTER");
                    uint32_t sensor_counter = serial_command[4] << 8 | serial_command[5];  
                    rtc_sensor_set_counter(sensor_counter);
                    break;
                }

                default:
                    break;
            }
            break;

        case NRF52_POWER_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_POWER_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_POWER_DCDC_CONVERTER_ENABLE:
                    NRF_LOG_INFO("NRF52_POWER_DCDC_CONVERTER_ENABLE");
                    enable_dcdc_converter();
                    break;

                case NRF52_POWER_DCDC_CONVERTER_DISABLE:
                    NRF_LOG_INFO("NRF52_POWER_DCDC_CONVERTER_DISABLE");
                    disable_dcdc_converter();
                    break;

                case NRF52_POWER_SLEEP_MODE_ENTER:
                    NRF_LOG_INFO("NRF52_POWER_SLEEP_MODE_ENTER");
                    sleep_mode_enter();
                    break;

                case NRF52_POWER_DEEP_SLEEP_MODE_ENTER:
                    NRF_LOG_INFO("NRF52_POWER_DEEP_SLEEP_MODE_ENTER");
                    deep_sleep_mode_enter();
                    break;

                case NRF52_POWER_MANAGER_HANDLER:
                    NRF_LOG_INFO("NRF52_POWER_MANAGER_HANDLER");
                    power_manager_handler();
                    break;

                case NRF52_POWER_MANAGER_INIT:
                    NRF_LOG_INFO("NRF52_POWER_MANAGER_INIT");
                    power_manager_init();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_COMMON_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_COMMON_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_LED_INIT:
                    NRF_LOG_INFO("NRF52_LED_INIT");
                    init_leds();
                    break;

                case NRF52_LED_IND_LED_ON:
                    NRF_LOG_INFO("NRF52_LED_IND_LED_ON");
                    ind_led_on();
                    break;

                case NRF52_LED_IND_LED_OFF:
                    NRF_LOG_INFO("NRF52_LED_IND_LED_OFF");
                    ind_led_off();
                    break;

                case NRF52_LED_IND_BLINK:
                    NRF_LOG_INFO("NRF52_LED_IND_BLINK");
                    switch(serial_command[4])
                    {
                        case NRF52_LED_IND_CUSTOM_BLINK:
                        {
                            NRF_LOG_INFO("NRF52_LED_IND_CUSTOM_BLINK");
                            uint16_t ind_led_on_ms = (uint16_t)(((uint16_t)serial_command[5] << 8) | serial_command[6]);
                            uint16_t ind_led_off_ms = (uint16_t)(((uint16_t)serial_command[7] << 8) | serial_command[8]);
                            ind_led_blink(ind_led_on_ms, ind_led_off_ms);
                            break;
                        }
                                 
                        case NRF52_LED_IND_SHORT_BLINK:
                            NRF_LOG_INFO("NRF52_LED_IND_SHORT_BLINK");
                            ind_led_short_blink();
                            break;
                               
                        case NRF52_LED_IND_MEDIUM_BLINK:
                            NRF_LOG_INFO("NRF52_LED_IND_MEDIUM_BLINK");
                            ind_led_medium_blink();
                            break;

                        case NRF52_LED_IND_LONG_BLINK:
                            NRF_LOG_INFO("NRF52_LED_IND_LONG_BLINK");
                            ind_led_long_blink();
                            break;

                        default:
                            break;
                    }                                                                                       
                    break;

                case NRF52_LED_BLE_LED_ON:
                    NRF_LOG_INFO("NRF52_LED_BLE_LED_ON");
                    ble_led_on();
                    break;

                case NRF52_LED_BLE_LED_OFF:
                    NRF_LOG_INFO("NRF52_LED_BLE_LED_OFF");
                    ble_led_off();
                    break;

                case NRF52_LED_BLE_BLINK:
                {
                    NRF_LOG_INFO("NRF52_LED_BLE_BLINK");
                    uint16_t ble_led_on_ms = (uint16_t)(((uint16_t)serial_command[4] << 8) | serial_command[5]);
                    uint16_t ble_led_off_ms = (uint16_t)(((uint16_t)serial_command[6] << 8) | serial_command[7]);
                    ble_led_blink(ble_led_on_ms, ble_led_off_ms);
                    break;
                }

                case NRF52_LOG_INIT:
                    NRF_LOG_INFO("NRF52_LOG_INIT");
                    log_init();
                    break;

                case NRF52_LDO_INIT:
                    NRF_LOG_INFO("NRF52_LDO_INIT");
                    ldo_init();
                    break;

                case NRF52_INPUT_OUTPUT_INIT:
                    NRF_LOG_INFO("NRF52_INPUT_OUTPUT_INIT");
                    input_output_init();
                    break;

                case NRF52_VCC_LDO_EN:
                    NRF_LOG_INFO("NRF52_VCC_LDO_EN");
                    enable_vcc_ldo();
                    break;
                
                #if MAX30003
                case NRF52_MAX30003_POWER_LDO_EN:
                    NRF_LOG_INFO("NRF52_MAX30003_POWER_LDO_EN");
                    enable_max30003_power_ldo();
                    break;
                #endif

                case NRF52_GPIOTE_INIT:
                    NRF_LOG_INFO("NRF52_GPIOTE_INIT");
                    gpiote_init();
                    break;

                default:
                    break;
            }
            break;
        
        #if I2C
        case NRF52_I2C_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_I2C_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_I2C_TWIM_SETUP:
                    NRF_LOG_INFO("NRF52_I2C_TWIM_SETUP");
                    twim_setup();
                    break;
              
                case NRF52_I2C_TWIM_INIT:
                    NRF_LOG_INFO("NRF52_I2C_TWIM_INIT");
                    twim_init();
                    break;

                case NRF52_I2C_TWIM_UNINIT:
                    NRF_LOG_INFO("NRF52_I2C_TWIM_UNINIT");
                    twim_uninit();
                    break;

                case NRF52_I2C_TWIM_ENABLE:
                    NRF_LOG_INFO("NRF52_I2C_TWIM_ENABLE");
                    twim_enable();
                    break;

                case NRF52_I2C_TWIM_DISABLE:
                    NRF_LOG_INFO("NRF52_I2C_TWIM_DISABLE");
                    twim_disable();
                    break;

                default:
                    break;
            }
            break;
        #endif

        #if SPI
        case NRF52_SPI_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_SPIM_COMMAND");

            switch(serial_command[3])
            {
                case NRF52_SPI_SPIM_INIT:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_INIT");
                    spim_init();
                    break;

                case NRF52_SPI_SPIM_ENABLE:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_ENABLE");
                    spim_enable();
                    break;

                case NRF52_SPI_SPIM_DISABLE:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_DISABLE");
                    spim_disable();
                    break;
                
                case NRF52_SPI_SPIM_UNINIT:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_UNINIT");
                    spim_uninit();
                    break;

                case NRF52_SPI_SPIM_SELECT_CS_PIN:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_SELECT_CS_PIN");
                    spim_select_cs_pin(serial_command[4]);
                    break;

                case NRF52_SPI_SPIM_INIT_CS_PIN:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_INIT_CS_PIN");
                    spim_init_cs_pin(serial_command[4]);
                    break;                    

                case NRF52_SPI_SPIM_SETUP:
                    NRF_LOG_INFO("NRF52_SPI_SPIM_SETUP");
                    spim_setup();
                    break;

                default:
                    break;
            }
            break;
        #endif

        default:
            break;
    }
}

#if FT201X
static void _ft201x_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_ft201x_handler");

    switch(serial_command[2])
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
//            ft201x_write_buffer(&serial_command[3], serial_command[4]);   // Pointer to Data Array, Number of elements in Data Array
            break;
        
        case FT201X_READ_DATA_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_DATA_COMMAND");
            break;

        default:
            break;
    }
}
#endif

#if BMI160
static void _bmi160_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_bmi160_handler");

    switch(serial_command[2])
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
#endif

#if TMP117
static void _tmp117_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_tmp117_handler");

    switch(serial_command[2])
    {
        case TMP117_GET_DEVICE_ID_COMMAND:
        {
            NRF_LOG_INFO("tmp117_MODULE: TMP117_GET_DEVICE_ID_COMMAND");
            uint8_t temp_device_id[2] = {0};
            tmp117_get_device_id(temp_device_id);
            state_machine.ble_response_char[5] = temp_device_id[0];
            state_machine.ble_response_char[6] = temp_device_id[1];
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case TMP117_GET_REVISION_NUMBER_COMMAND:
        {
            NRF_LOG_INFO("TMP117_MODULE: TMP117_GET_REVISION_NUMBER_COMMAND");
            uint8_t temp_revision_number = 0;
            tmp117_get_revision_number(&temp_revision_number);
            state_machine.ble_response_char[6] = temp_revision_number;
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case TMP117_INIT_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_INIT_COMMAND");
            tmp117_init();
            break;

        case TMP117_SET_AVERAGING_MODE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_SET_AVERAGING_MODE_COMMAND");
            tmp117_set_averaging_mode(serial_command[3]);
            break;

        case TMP117_SET_CONVERSION_MODE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_SET_CONVERSION_MODE_COMMAND");   
            tmp117_set_conversion_mode(serial_command[3]);
            break;

        case TMP117_GET_TEMPERATURE_COMMAND:
        {
            NRF_LOG_INFO("TMP117_MODULE: TMP117_GET_TEMPERATURE_COMMAND");
            uint8_t temp_temperature[2] = {0};
            tmp117_get_temperature(temp_temperature);
            state_machine.ble_response_char[5] = temp_temperature[0];
            state_machine.ble_response_char[6] = temp_temperature[1];
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case TMP117_UNINIT_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_UNINIT_COMMAND");
            tmp117_uninit();
            break;

        case TMP117_ENABLE_LONG_TERM_STORAGE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_ENABLE_LONG_TERM_STORAGE_COMMAND");
            tmp117_enable_long_term_storage();
            break;

        case TMP117_DISABLE_LONG_TERM_STORAGE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_DISABLE_LONG_TERM_STORAGE_COMMAND");
            tmp117_disable_long_term_storage();
            break;

        case TMP117_TRANSMIT_TEMPERATURE_RECORDING_SESSION_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TRANSMIT_TEMPERATURE_RECORDING_SESSION");
            tmp117_transmit_temperature_recording_session();
            break;

        case TMP117_START_DATA_COLLECTION_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: START_DATA_COLLECTION_COMMAND");
            tmp117_start_data_collection();
            break;

        case TMP117_STOP_DATA_COLLECTION_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: STOP_DATA_COLLECTION_COMMAND");
            tmp117_stop_data_collection();
            break;

        default:
            break;
    }
}
#endif

#if CY15B108QI
static void _cy15b108qi_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_cy15b108qi_handler");

    switch(serial_command[2])
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
            cy15b108qi_enter_deep_power_down_mode();
            break;

        case CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: EXIT_DEEP_POWER_DOWN_MODE");
            cy15b108qi_exit_deep_power_down_mode();
            break;

        case CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: ENTER_HIBERNATION_MODE");
            cy15b108qi_enter_hibernation_mode();
            break;

        case CY15B108QI_EXIT_HIBERNATION_MODE_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: EXIT_HIBERNATION_MODE");
            cy15b108qi_exit_hibernation_mode();
            break;

        case CY15B108QI_GET_MANUFACTURER_ID_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: GET_MANUFACTURER_ID");
            uint8_t temp_manufacturer_id[7] = {0};
            cy15b108qi_get_manufacturer_id(temp_manufacturer_id);
            break;

        case CY15B108QI_GET_UNIQUE_ID_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: GET_UNIQUE_ID");
            uint8_t temp_unique_id[8] = {0};
            cy15b108qi_get_unique_id(temp_unique_id);
            break;

        case CY15B108QI_WRITE_REGISTERS_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: WRITE_REGISTERS");
            uint32_t start_write_address = (serial_command[4] << 16) | (serial_command[5] << 8) | (serial_command[6]);
            cy15b108qi_write_registers(&serial_command[3], 1, start_write_address);
            break;

        case CY15B108QI_READ_SINGLE_REGISTER_COMMAND:
            NRF_LOG_INFO("CY15B108QI_MODULE: READ_UNIQUE_ID");
            uint32_t temp_start_write_address = (serial_command[3] << 16) | (serial_command[4] << 8) | (serial_command[5]);
            uint8_t temp_data_read = cy15b108qi_read_single_register(temp_start_write_address);
            state_machine.ble_response_char[6] = temp_data_read;
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;

        default:
            break;
    }
}
#endif

#if MAX30003
static void _max30003_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_max30003_handler");

    switch(serial_command[2])
    {
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
        {
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_GET_ECG_VOLTAGE_COMMAND");
            uint8_t max30003_bytes_per_interrupt = max30003_get_bytes_per_interrupt();
            uint8_t max30003_ecg_voltage[max30003_bytes_per_interrupt];
            max30003_get_ecg_voltage(max30003_ecg_voltage, max30003_bytes_per_interrupt); 
            break;
        }
        
        case MAX30003_INIT_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_INIT_COMMAND");
            max30003_init();
            break;

        case MAX30003_START_RECORDING_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: START_RECORDING_COMMAND");
            max30003_start_recording();
            break;
        
        case MAX30003_START_DATA_COLLECTION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: START_DATA_COLLECTION_COMMAND");
            max30003_start_data_collection();
            break;

        case MAX30003_STOP_DATA_COLLECTION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: START_DATA_COLLECTION_COMMAND");
            max30003_stop_data_collection();
            break;

        case MAX30003_FIFO_RESET_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: FIFO_RESET_COMMAND");
            max30003_fifo_reset();
            break;

        case MAX30003_READ_ECG_FIFO_MEMORY_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: READ_FIFO_COMMAND");
            max30003_read_ecg_fifo_memory();
            break;

        case MAX30003_ENABLE_LONG_TERM_STORAGE_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_ENABLE_LONG_TERM_STORAGE_COMMAND");
            max30003_enable_long_term_storage();
            break;

        case MAX30003_DISABLE_LONG_TERM_STORAGE_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_DISABLE_LONG_TERM_STORAGE_COMMAND");
            max30003_disable_long_term_storage();
            break;

        case MAX30003_TRANSMIT_ECG_RECORDING_SESSION_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: TRANSMIT_ECG_RECORDING_SESSION");
            max30003_transmit_ecg_recording_session();
            break;

        default:
            break;
    }
}
#endif

#if FDC1004
static void _fdc1004_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_fdc1004_handler");

    switch(serial_command[2])
    {
        case FDC1004_INIT_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_INIT_COMMAND");
            fdc1004_init();
            break;

        case FDC1004_UNINIT_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_UNINIT_COMMAND");
            fdc1004_uninit();
            break;

        case FDC1004_SOFT_RESET_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SOFT_RESET_COMMAND");
            fdc1004_soft_reset();
            break;

        case FDC1004_SET_OFFSET_CALIBRATION_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SET_OFFSET_CALIBRATION_COMMAND");
            fdc1004_set_offset_calibration(serial_command[3], serial_command[4], serial_command[5], serial_command[6]);
            break;

        case FDC1004_SET_GAIN_CALIBRATION_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SET_GAIN_CALIBRATION_COMMAND");
            fdc1004_set_gain_calibration(serial_command[3], serial_command[4], serial_command[5], serial_command[6]);
            break;

        case FDC1004_SET_MEASUREMENT_RATE_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SET_MEASUREMENT_RATE_COMMAND");
            fdc1004_set_measurement_rate(serial_command[3]);
            break;

        case FDC1004_SET_REPEAT_MEASUREMENT_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SET_REPEAT_MEASUREMENT_COMMAND");
            fdc1004_set_repeat_measurement(serial_command[3]);
            break;

        case FDC1004_GET_MANUFACTURER_ID_COMMAND:
        {
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_GET_MANUFACTURER_ID_COMMAND");
            uint8_t manufacturer_id[2] = {0};
            fdc1004_get_manufacturer_id(manufacturer_id);
            state_machine.ble_response_char[5] = manufacturer_id[0];
            state_machine.ble_response_char[6] = manufacturer_id[1];
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case FDC1004_GET_DEVICE_ID_COMMAND:
        {
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_GET_DEVICE_ID_COMMAND");
            uint8_t temp_device_id[2] = {0};
            fdc1004_get_device_id(temp_device_id);
            state_machine.ble_response_char[5] = temp_device_id[0];
            state_machine.ble_response_char[6] = temp_device_id[1];
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case FDC1004_SET_CAPDAC_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_SET_CAPDAC_COMMAND");
            fdc1004_set_capdac(serial_command[3], serial_command[4]);
            break;

        case FDC1004_TRIGGER_SINGLE_MEASUREMENT_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_TRIGGER_SINGLE_MEASUREMENT_COMMAND");
            fdc1004_trigger_single_measurement(serial_command[3]);
            break;

        case FDC1004_GET_MEASUREMENT_COMMAND:
        {
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_GET_MEASUREMENT_COMMAND");
            uint8_t measurement[3] = {0};
            fdc1004_get_measurement(serial_command[3], measurement);
            state_machine.ble_response_char[4] = measurement[0];
            state_machine.ble_response_char[5] = measurement[1];
            state_machine.ble_response_char[6] = measurement[2];
            state_handler(state_machine.ble_response_char);
            _reset_response_char();
            break;
        }

        case FDC1004_ENABLE_CHANNEL_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_ENABLE_CHANNEL_COMMAND");
            fdc1004_enable_channel(serial_command[3]);
            break;

        case FDC1004_DISABLE_CHANNEL_COMMAND:
            NRF_LOG_INFO("FDC1004_MODULE: FDC1004_DISABLE_CHANNEL_COMMAND");
            fdc1004_disable_channel(serial_command[3]);
            break;

        default:
            break;
    }
}
#endif

static void _bluetooth_handler(uint8_t *serial_command)
{
    NRF_LOG_INFO("_bluetooth_handler");

    switch(serial_command[2])
    {
        case BLUETOOTH_INIT_GAP_PARAMS_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_GAP_PARAMS");
            bluetooth_gap_params_init();
            break;

        case BLUETOOTH_INIT_GATT_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_GATT");
            bluetooth_gatt_init();
            break;

        case BLUETOOTH_INIT_BLE_STACK_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_BLE_STACK");
            bluetooth_ble_stack_init();
            break;

        case BLUETOOTH_INIT_SERVICES_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_SERVICES");
            bluetooth_services_init();
            break;

        case BLUETOOTH_INIT_CONN_PARAMS_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_CONN_PARAMS");
            bluetooth_conn_params_init();
            break;

        case BLUETOOTH_INIT_PEER_MANAGER_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_PEER_MANAGER");
            bluetooth_peer_manager_init();
            break;

        case BLUETOOTH_INIT_ADVERTISING_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: INIT_ADVERTISING");
            bluetooth_advertising_init();
            break;

        case BLUETOOTH_SET_ADVERTISING_POWER_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: SET_ADVERTISING_POWER");
            bluetooth_set_advertising_power();
            break;

        case BLUETOOTH_START_ADVERTISING_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: START_ADVERTISING");
            bluetooth_advertising_start();
            break;

        case BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_RESPONSE_CHAR");
            uint8_t response_char_data[4] = {serial_command[3], serial_command[4], serial_command[5], serial_command[6]};
            bluetooth_configuration_service_response_char_write(response_char_data);
            break;
        }

        case BLUETOOTH_WRITE_CRC_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_CRC_CHAR");
            uint8_t crc_char_data[2] = {serial_command[3], serial_command[4]};
            bluetooth_configuration_service_response_char_write(crc_char_data);            
            break;
        }

        case BLUETOOTH_TRANSMIT_FIRMWARE_VERSION_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: TRANSMIT_FIRMWARE_VERSION");
            bluetooth_transmit_firmware_version();
            break;

        case BLUETOOTH_TRANSMIT_HARDWARE_BOARD_VERSION_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: TRANSMIT_HARDWARE_BOARD_VERSION");
            bluetooth_transmit_hardware_board_version();
            break;

        #if TMP117
        case BLUETOOTH_WRITE_TEMP_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_TEMP_CHAR");
            uint8_t temp_char[250] = {0};
            bluetooth_temperature_service_temp_char_write(temp_char);
            break;
        }

        case BLUETOOTH_WRITE_INSTANT_TEMP_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_INSTANT_TEMP_CHAR");
            uint8_t temp_char[2] = {serial_command[3], serial_command[4]};
            bluetooth_temperature_service_instant_temp_char_write(temp_char);
            break;
        }
        #endif

        #if FDC1004
        case BLUETOOTH_WRITE_INSTANT_PRESSURE_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_INSTANT_PRESSURE_CHAR");
            uint8_t pressure_char[8] = {serial_command[3], serial_command[4], serial_command[5], serial_command[6], 
            serial_command[7], serial_command[8], serial_command[9], serial_command[10]};
            bluetooth_pressure_service_instant_pressure_char_write(pressure_char);
            break;
        }
        #endif

        #if MAX30003
        case BLUETOOTH_WRITE_INSTANT_ECG_CHAR_COMMAND:
        {
            NRF_LOG_INFO("BLUETOOTH_MODULE: WRITE_INSTANT_ECG_CHAR");
            uint8_t max30003_bytes_per_interrupt = max30003_get_bytes_per_interrupt();
            uint8_t max30003_ecg_voltage[max30003_bytes_per_interrupt];
            max30003_get_ecg_voltage(max30003_ecg_voltage, max30003_bytes_per_interrupt);
            bluetooth_ecg_service_instant_ecg_char_write(max30003_ecg_voltage);
            break;
        }

        case BLUETOOTH_TRANSMIT_ECG_RECORDING_SESSION_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: TRANSMIT_ECG_RECORDING_SESSION");
            bluetooth_transmit_ecg_recording_session();
            break;
        #endif

        case BLUETOOTH_DISCONNECT_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: DISCONNECT");
            bluetooth_disconnect();
            break;

        case BLUETOOTH_OVERRIDE_REQUEST_RECEIVED_COMMAND:
            NRF_LOG_INFO("BLUETOOTH_MODULE: OVERRIDE_REQUEST_RECEIVED_COMMAND");
            bluetooth_override_request_received();
            break;

        default:
            break;
    }
}

static void _reset_response_char(void)
{
    NRF_LOG_INFO("_reset_response_char");
    for(uint8_t i = 3; i < ARRAY_LENGTH(state_machine.ble_response_char); i++)
    {
        state_machine.ble_response_char[i] = 0;
    }
}
