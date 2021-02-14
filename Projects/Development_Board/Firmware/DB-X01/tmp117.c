#include "tmp117.h"

#if TMP117

static struct TMP117_control_struct control;
struct FDC1004_Temperature_Register_Struct temperature;
struct FDC1004_Configuration_Register_Struct configuration;

void tmp117_init()
{
    NRF_LOG_INFO("tmp117_init");

    control_struct.interrupt = 0;   // Initializing the interrupt to 0 to start out

    control.operation_mode = TMP117_SHUTDOWN_MODE;    // Initializing the operation mode to shutdown mode

    control.averaging_mode = TMP117_NO_AVERAGING_MODE;    // Initializing the averaging mode to not take an average of the data

    control.device_address = TMP117_SLAVE_ADDRESS;        // Setting the device address to the slave I2C address


    control_struct.external_memory_write_start_address = 0x000000;    // Start address of the external memory to store temperature data
    control_struct.external_memory_write_end_address = 0x0001F4;      // End address of the external memory to store temperature data
    control_struct.external_memory_write_current_address = control_struct.external_memory_write_start_address;    // Initializing the current write address to the write start address

    control_struct.external_memory_transmit_start_address = control_struct.external_memory_write_start_address;           // The transmitting start address is set the write start address
    control_struct.external_memory_transmit_end_address = control_struct.external_memory_write_end_address;               // The transmitting end address is set to the write end address
    control_struct.external_memory_transmit_current_address = control_struct.external_memory_transmit_start_address;      // The transmitting current address is set to the transmit start address

    control_struct.bytes_per_sample = 2;    // Number of bytes per sample; uint16_t data type used to store temperature value

//    control_struct.samples_per_minute = rtc_tmp117_get_sampling_frequency();
//    NRF_LOG_INFO("data_flow.max30003_samples_per_second: %u", data_flow.max30003_samples_per_second);
//    data_flow.samples_per_recording_session = 3600 * data_flow.max30003_samples_per_second;         // Each recording session is 1 hour
//    NRF_LOG_INFO("data_flow.samples_per_recording_session: %u", data_flow.samples_per_recording_session);
//
//    data_flow.bytes_per_sample = max30003_get_bytes_per_sample();
//    NRF_LOG_INFO("data_flow.bytes_per_sample: %u", data_flow.bytes_per_sample);
//    data_flow.bytes_per_recording_session = data_flow.samples_per_recording_session * data_flow.bytes_per_sample;
//    NRF_LOG_INFO("data_flow.bytes_per_recording_session: %u", data_flow.bytes_per_recording_session);
//
//    data_flow.current_sample_count = 0;    
//
//    data_flow.ecg_interrupt = 0;    // Disabling the interrupt flag for ECG Data
//
//    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
//
//    data_flow.bytes_per_bluetooth_transmission = bluetooth_get_bytes_per_transmission();
//    data_flow.samples_per_bluetooth_transmission = data_flow.bytes_per_bluetooth_transmission/data_flow.bytes_per_sample;
//    NRF_LOG_INFO("data_flow.samples_per_bluetooth_transmission: %u", data_flow.samples_per_bluetooth_transmission);
//
//    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
//
//    data_flow.recording_session_start_address = cy15b108qi_get_current_write_address() + TEMPERATURE_DATA_REGISTER_SIZE;
//    NRF_LOG_INFO("data_flow.recording_session_start_address: %X", data_flow.recording_session_start_address);
//
//    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _tmp117_unlock_eeprom();
    tmp117_set_operating_mode(configuration_mode, averaging_mode);
    _tmp117_general_call_reset();

}

/* 
* Read/Write Configuration register. This register is used to write/read all configuration settings for the TMP117
* Register address: 0x01
* D[15], HIGH_ALERT = High alert flag
* D[14], LOW_ALERT = Low alert flag
* D[13], DATA_READY = Data ready flag
* D[12], EEPROM_BUSY = EEPROM busy flag
* D[11:10], MOD = Set converison mode
* D[9:7], CONV = Conversion cycle bit
* D[6:5], AVG = Conversion averaging modes
* D[4], T/nA = Therm/alert mode select
* D[3], POL = Alert pin polarity bit
* D[2], DR/Alert = Alert pin select bit
* D[1], SOFT_RESET = Software reset bit 
* D[0], N/A = Not used
*/
static void _tmp117_read_configuration_register(void)
{
    NRF_LOG_INFO("_tmp117_read_configuration_register");

    i2c_read_registers(control.slave_address[0], configuration.register_pointer[0], control.data_array, control.register_byte_count[0]);

    configuration.high_alert = (control.data_array[0] & 0b10000000) >> 7;
    configuration.low_alert = (control.data_array[0] & 0b01000000) >> 6;
    configuration.data_ready = (control.data_array[0] & 0b00100000) >> 5;
    configuration.eeprom_busy = (control.data_array[0] & 0b00010000) >> 4;
    configuration.mod = (control.data_array[0] & 0b00001100) >> 2;
    configuration.conv = ((control.data_array[0] & 0b00000011) << 1) | (control.data_array[1] >> 7);
    
    configuration.avg = (control.data_array[
    offset_calibration_register.integer[i][1] = (control.data_array[0] & 0b11111000) >> 3;
   
    offset_calibration_register.decimal[i][0] = (control.data_array[0] & 0b00000111);
    offset_calibration_register.decimal[i][1] = control.data_array[1];
}

static void _tmp117_write_configuration_register(void)
{
    NRF_LOG_INFO("tmp117_write_configuration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(offset_calibration_register.register_pointer); i++)
    {
        control.data_array[0] = (offset_calibration_register.integer[i][1] << 3) | offset_calibration_register.decimal[i][0];
        control.data_array[1] = offset_calibration_register.decimal[i][1];

        i2c_write_registers(control.slave_address[0], control.data_array, control.register_byte_count[0]);
    }
}

void tmp117_read_device_id(uint8_t *temp_device_id)
{
    NRF_LOG_INFO("tmp117_read_chip_id");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_DEVICE_ID_REGISTER;       // TMP117 Device ID stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(control.device_address, register_address, register_data, register_byte_count);
    control.device_id =  ((register_data[0] << 8) & 0b00001111) | register_data[1];
    temp_device_id[0] = control.device_id[0];
    temp_device_id[1] = control.device_id[1];
}

void tmp117_get_chip_id(uint8_t *temp_device_id)

void tmp117_read_revision_number(void)
{
    NRF_LOG_INFO("tmp117_read_chip_id");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_DEVICE_ID_REGISTER;       // TMP117 Revision Number stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    control_struct.revision_number =  register_data[0] & 0b11110000;   // Reading revision number bits
}

float tmp117_get_celsius(void)
{
    NRF_LOG_INFO("tmp117_get_celsius");
    float reg_cels_conv = 7812.5; //1 Register unit = 0.0078125 Celcius
    tmp117_get_uint16_t();
    float celsius_counts = (float) control_struct.temp_raw_value;
    float tmp117_celsius = reg_cels_conv*celsius_counts/1000000;
    NRF_LOG_INFO("Temperature Value: " NRF_LOG_FLOAT_MARKER " C.", NRF_LOG_FLOAT(tmp117_celsius));
    return tmp117_celsius;
}

void tmp117_get_uint16_t(void)
{    
    NRF_LOG_INFO("tmp117_get_uint16_t");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_TEMP_RESULT_REGISTER;     // TMP117 temperature data stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    control_struct.temp_raw_value =  (register_data[0] << 8) | register_data[1];   // Reading temperature data bits
    control_struct.temp_raw_value_array[0] = register_data[0];
    control_struct.temp_raw_value_array[1] = register_data[1];
}

void tmp117_string_celsius(char *tmp117_temperature)
{
    float int_temp = tmp117_get_celsius();
    sprintf(tmp117_temperature, "%.4f", int_temp);    
}

void tmp117_get_uint8_t(void)
{
    char tmp117_char[5]; //Initializes the Character Array
    tmp117_string_celsius(tmp117_char);

    for(uint8_t i = 0; i<strlen(tmp117_char); i++)
    {
//        control_struct.temp_uint8_t[i] = (uint8_t) tmp117_char[i];
    }
}

void _tmp117_unlock_eeprom(void)
{
    NRF_LOG_INFO("unlock_eeprom_tmp117");
    uint8_t register_byte_count = 3;                                //Slave Address + Register Write Value

    /*tmp117 unlock EEPROM address for writing to configuration register. */
    uint8_t register_address = TMP117_EEPROM_UNLOCK_REGISTER;       // Register for EEPROM Unlock Address
    uint8_t tmp117_eun_value_1 = 0x80;               // EUN Value to Unlock EEPROM 
    uint8_t tmp117_eun_value_2 = 0x00;               // EUN Value to Unlock EEPROM 
    
    uint8_t register_data[register_byte_count];
    register_data[0] = register_address;
    register_data[1] = tmp117_eun_value_1;
    register_data[2] = tmp117_eun_value_2;
    
    i2c_write_registers(control.device_address, register_data, register_byte_count);
}

void tmp117_set_operating_mode(uint8_t operating_mode)
{
    uint8_t tmp117_config_value_1;
    // MOD[1:0] = 11, ~ Single Shot Mode
    // MOD[1:0] = 00, ~ Continous Conversion Mode
    // MOD[1:0] = 01, ~ Shutdown Mode
    
    if(conversion_mode == TMP117_SHUTDOWN_MODE)
    {
        tmp117_config_value_1 = 0x04;                //Configuration Value to set TMP117 for Shutdown Mode, 0X04
    }
    else if(conversion_mode == TMP117_CONTINUOUS_CONVERSION_MODE)
    {
        tmp117_config_value_1 = 0x00;                //Configuration Value to set TMP117 for Continous Conversion, 0X00
    }
    else
    {
        tmp117_config_value_1 = 0x04;     // Default Mode is Shutdown Mode for the TMP117, 0x04
    }

    uint8_t tmp117_config_value_2;

    // AVG[1:0] = 00, ~ No Averaging, 15.5 ms Active Conversion Time
    // AVG[1:0] = 01, ~ 8 Averaged conversions, 15.5 ms Active Conversion Time
    // AVG[1:0] = 10, ~ 32 Averaged conversions, 15.5 ms Active Conversion Time
    // AVG[1:0] = 11, ~ 64 Averaged conversions, 15.5 ms Active Conversion Time

    // CONV[2:0] = 000, ~Conversion Cycle Time in CC Mode
 
    switch(averaging_mode)
    {
        case TMP117_NO_AVERAGING_MODE:
            tmp117_config_value_2 = 0x00;                // No Averaging, 00
            return;

        case TMP117_8_AVERAGED_MODE:
            tmp117_config_value_2 = 0x20;                // 8 Averaged, 01
            return;

        case TMP117_32_AVERAGED_MODE:
            tmp117_config_value_2 = 0x40;                // 32 Averaged, 10
            return;

        case TMP117_64_AVERAGED_MODE:
            tmp117_config_value_2 = 0x60;                // 64 Averaged, 11
            return;

        default:
            tmp117_config_value_2 = 0x00;                // No Averaging, 00
            return;
    }

    uint8_t register_byte_count = 3;        // Slave Address + Register Write Value
    uint8_t device_address = TMP117_SLAVE_ADDRESS;                   // Slave Address for tmp117, ADD0 = GND
    
    /*TMP117 Internal register write to Configuration Register. */
    uint8_t register_address = TMP117_CONFIGURATION_REGISTER;                //Register for Configuration Register

    uint8_t register_data[register_byte_count];
    register_data[0] = register_address;
    register_data[1] = tmp117_config_value_1;
    register_data[2] = tmp117_config_value_2;

    i2c_write_registers(device_address, register_data, register_byte_count);
    nrf_delay_ms(10);   // Wait 10 ms to allow the register to be written properly 
}

void tmp117_set_averaging_mode(uint8_t averaging_mode)
{



void tmp117_general_call_reset(void)
{
    NRF_LOG_INFO("TMP117: General Call Reset");
    
    uint8_t register_byte_count = 1;
    uint8_t register_data[register_byte_count];    
    register_data[0] = TMP117_GENERAL_CALL_RESET; 
    i2c_write_registers(GENERAL_CALL_ADDRESS, register_data, register_byte_count);
}

void tmp117_interrupt_handler(void)
{
    NRF_LOG_INFO("tmp117_interrupt_handler");

    if(control_struct.interrupt == 1)
    {
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
        state_handler(i2c_init_array_data); // Init TWIM Driver

        uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
        state_handler(i2c_enable_array_data); // Enable TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t tmp117_wakeup_array_data[5] = {0x00, TMP117_MODULE, TMP117_INIT_COMMAND, TMP117_CONTINUOUS_CONVERSION_MODE, TMP117_64_AVERAGED_MODE};  
        state_handler(tmp117_wakeup_array_data); // Set the TMP117 to continous conversion mode, 64 averaging mode

        uint8_t tmp117_read_temperature_value_array_data[3] = {0x00, TMP117_MODULE, TMP117_TEMP_UINT16_COMMAND};  
        state_handler(tmp117_read_temperature_value_array_data); // Read the uint16_t temperature value of the TMP117 

        uint8_t tmp117_shutdown_array_data[5] = {0x00, TMP117_MODULE, TMP117_INIT_COMMAND, TMP117_SHUTDOWN_MODE, TMP117_NO_AVERAGING_MODE};  
        state_handler(tmp117_shutdown_array_data); // Set the TMP117 to shutdown mode, no averaging mode

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
        state_handler(i2c_disable_array_data); // Disable TWIM Driver

        uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
        state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
        state_handler(spim_enable_array_data); // Enable SPIM Module

        uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
        state_handler(spim_init_array_data); // Initialize SPIM Module
    
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
        state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to CY15B108QI for the SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_exit_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_exit_deep_power_down_mode_array_data); // Exit the Deep power down mode

        uint8_t cy15b108qi_set_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_SET_WRITE_ENABLE_LATCH_COMMAND};
        state_handler(cy15b108qi_set_write_enable_latch_array_data); // Set the write enable latch

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

//        if(control_struct.external_memory_current_address > control_struct.external_memory_end_address)
//        {
//            control_struct.external_memory_current_address = control_struct.external_memory_start_address;
//        }

//        cy15b108qi_write_data_command(control_struct.temp_raw_value_array, ARRAY_LENGTH(control_struct.temp_raw_value_array), control_struct.external_memory_current_address);
//        control_struct.external_memory_current_address += ARRAY_LENGTH(control_struct.temp_raw_value_array);

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_reset_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_RESET_WRITE_ENABLE_LATCH_COMMAND};
        state_handler(cy15b108qi_reset_write_enable_latch_array_data); // Reset the write enable latch

        uint8_t cy15b108qi_enter_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_enter_deep_power_down_mode_array_data); // Enter the Deep power down mode

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
        state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

        uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
        state_handler(spim_disable_array_data); // Disable SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    }
}

void tmp117_stop_recording_session(void)
{
    NRF_LOG_INFO("tmp117_stop_recording_session");

    control_struct.interrupt = 0;    // Disabling the interrupt

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t rtc_tmp117_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_TMP117_STOP};  
    state_handler(rtc_tmp117_uninit_array_data); // Stop the RTC sampling interrupt for the TMP117

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t tmp117_shutdown_array_data[5] = {0x00, TMP117_MODULE, TMP117_INIT_COMMAND, TMP117_SHUTDOWN_MODE, TMP117_NO_AVERAGING_MODE};  
    state_handler(tmp117_shutdown_array_data); // Set the TMP117 to shutdown mode, no averaging mode

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    #if !MAX30003
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
        state_handler(spim_enable_array_data); // Enable SPIM Module

        uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
        state_handler(spim_init_array_data); // Initialize SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
        state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to CY15B108QI for the SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_enter_hibernation_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND};
        state_handler(cy15b108qi_enter_hibernation_array_data);

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
        state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

        uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
        state_handler(spim_disable_array_data); // Disable SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    #endif
}


void tmp117_start_recording_session(void)
{
    NRF_LOG_INFO("tmp117_start_recording_session");

    control_struct.interrupt = 1;    // Enabling the sampling interrupt

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t nrfx_rtc_start_tmp117_array_data[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_TMP117_START};
    state_handler(nrfx_rtc_start_tmp117_array_data); // NRFX Start TMP117 RTC

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void tmp117_set_external_memory_write_start_address(uint32_t external_memory_write_start_address)
{
    NRF_LOG_INFO("tmp117_set_external_memory_write_start_address");
    control_struct.external_memory_write_start_address;
}

void tmp117_set_external_memory_write_end_address(uint32_t external_memory_write_end_address)
{
    NRF_LOG_INFO("tmp117_set_external_memory_write_end_address");
    control_struct.external_memory_write_end_address;
}
 
#endif