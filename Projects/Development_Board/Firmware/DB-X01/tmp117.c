#include "tmp117.h"

#if TMP117

static struct TMP117_control_struct control;
static struct TMP117_Temperature_Register_Struct temperature;
static struct TMP117_Configuration_Register_Struct configuration;
static struct TMP117_High_Limit_Register_Struct high_limit;
static struct TMP117_Low_Limit_Register_Struct low_limit;
static struct TMP117_EEPROM_Unlock_Register_Struct eeprom_unlock;
static struct TMP117_Temperature_Offset_Register_Struct temperature_offset;
static struct TMP117_Device_ID_Register_Struct device_id;

/* Public Functions */

void tmp117_init()
{
    NRF_LOG_INFO("tmp117_init");

    control.interrupt = 0;                                  // Initializing the interrupt to 0 to start out
    control.slave_address = TMP117_SLAVE_ADDRESS;           // Setting the slave address to the slave address of the TMP117
    control.register_byte_count = 2;                        // Number of bytes per register
    control.data_ready_for_transmit = 0;                    // Data ready for transmit

    temperature.register_pointer = TMP117_TEMP_RESULT;
    configuration.register_pointer = TMP117_CONFIGURATION;
    high_limit.register_pointer = TMP117_THIGH_LIMIT;
    low_limit.register_pointer = TMP117_TLOW_LIMIT;
    eeprom_unlock.register_pointer = TMP117_EEPROM_UL;
    temperature_offset.register_pointer = TMP117_TEMP_OFFSET;
    device_id.register_pointer = TMP117_DEVICE_ID;

    // Software reset to clear previous settings
    _tpm117_soft_reset();

    // Read the configuration register
    _tmp117_read_configuration_register();
    configuration.mod = TMP117_SHUTDOWN_MODE;               // Initializing the operation mode to shutdown mode
    configuration.avg = TMP117_NO_AVERAGING_MODE;           // Initializing the averaging mode to not take an average of the data
    configuration.conv = TMP117_SHORTEST_TIME_MODE;         // Initializing the conversion cycle time to the short time amount
    _tmp117_unlock_eeprom();
    _tmp117_write_configuration_register();

    // General call to reset the chip
    _tmp117_general_call_reset();
    _tmp117_read_configuration_register();

    // Reading the temperature register. After a reset, the first data saved to the register is trash. This is to flush that data out. 
    tmp117_set_conversion_mode(TMP117_ONE_SHOT_CONVERSION);
    _tmp117_read_temperature_register();
    tmp117_set_conversion_mode(TMP117_SHUTDOWN_MODE);

    control.external_memory_start_address = TMP117_EXTERNAL_MEMORY_START_ADDRESS;     // Start address of the external memory to store temperature data
    control.external_memory_end_address = TMP117_EXTERNAL_MEMORY_END_ADDRESS;         // End address of the external memory to store temperature data
    
    control.external_memory_write_start_address = control.external_memory_start_address;            // Initializing the start address to the memory start address
    control.external_memory_write_current_address = control.external_memory_write_start_address;    // Initializing the current write address to the write start address

    control.external_memory_transmit_start_address = 0;             // The transmitting start address is set the write start address
    control.external_memory_transmit_end_address = 0;               // The transmitting end address is set to the write end address
    control.external_memory_transmit_current_address = 0;           // The transmitting current address is set to the transmit start address

    control.bytes_per_sample = 2;    // Number of bytes per sample; uint16_t data type used to store temperature value

    control.samples_per_minute = rtc_sensor_get_sampling_frequency();
    control.samples_per_recording_session = 60 * control.samples_per_minute;         // Each recording session is 1 hour
    NRF_LOG_INFO("control.samples_per_recording_session: %u", control.samples_per_recording_session);

    control.bytes_per_recording_session = control.samples_per_recording_session * control.bytes_per_sample;
    NRF_LOG_INFO("control.bytes_per_recording_session: %u", control.bytes_per_recording_session);

    control.current_sample_count = 0;    

    control.bytes_per_bluetooth_transmission = bluetooth_get_bytes_per_transmission();
    control.samples_per_bluetooth_transmission = control.bytes_per_bluetooth_transmission/control.bytes_per_sample;
    NRF_LOG_INFO("control.samples_per_bluetooth_transmission: %u", control.samples_per_bluetooth_transmission);
 
    control.status = 1;                 // By calling the init function, you are enabling the TMP117
    control.long_term_storage = 0;      // Setting the default to instant temperature data transmission
    control.interrupt = 0;              // Disabling the sampling interrupt to begin
}

void tmp117_uninit(void)
{
    NRF_LOG_INFO("tmp117_uninit");
    control.status = 0;
    control.interrupt = 0;
}

uint8_t tmp117_is_enabled(void)
{
    NRF_LOG_INFO("tmp117_is_enabled: %u", control.status);
    return control.status;
}

void tmp117_enable_long_term_storage(void)
{
    NRF_LOG_INFO("tmp117_enable_long_term_storage");
    control.long_term_storage = 1;
}

void tmp117_disable_long_term_storage(void)
{
    NRF_LOG_INFO("tmp117_disable_long_term_storage");
    control.long_term_storage = 0;
}

void tmp117_get_device_id(uint8_t *temp_device_id)
{
    NRF_LOG_INFO("tmp117_get_device_id");
    _tmp117_read_device_id_register();
    temp_device_id[0] = device_id.device_id[0];
    temp_device_id[1] = device_id.device_id[1];
}

void tmp117_get_revision_number(uint8_t *temp_revision_number)
{
    NRF_LOG_INFO("tmp117_get_revision_number");
    _tmp117_read_device_id_register();
    *temp_revision_number = device_id.rev;
}

void tmp117_get_temperature(uint8_t *temp_temperature)
{    
    NRF_LOG_INFO("tmp117_get_temperature");
    _tmp117_read_temperature_register();
    temp_temperature[0] = temperature.t[0];
    temp_temperature[1] = temperature.t[1];
}

void tmp117_set_averaging_mode(uint8_t averaging_mode)
{
    NRF_LOG_INFO("tmp117_set_averaging_mode");
    _tmp117_unlock_eeprom();
    configuration.avg = averaging_mode;             // 0 = No Averaging; (1 = 8, 2 = 32, 3 = 64) Averaged Conversions
    _tmp117_write_configuration_register();
    _tmp117_general_call_reset();
}

void tmp117_set_conversion_mode(uint8_t conversion_mode)
{
    NRF_LOG_INFO("tmp117_set_averaging_mode");
    _tmp117_unlock_eeprom();
    configuration.mod = conversion_mode;             // 0 & 2 = Continous conversion; 1 = Shutdown; 2 = One-Shot conversion
    _tmp117_write_configuration_register();
    _tmp117_general_call_reset();
}

void tmp117_interrupt_handler(void)
{
    NRF_LOG_INFO("tmp117_interrupt_handler");

    if(control.interrupt == 1)
    {
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
        state_handler(i2c_init_command); // Init TWIM Driver

        uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
        state_handler(i2c_enable_command); // Enable TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        tmp117_set_conversion_mode(TMP117_ONE_SHOT_CONVERSION);

        _tmp117_read_temperature_register();

        tmp117_set_conversion_mode(TMP117_SHUTDOWN_MODE);

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
        state_handler(i2c_disable_command); // Disable TWIM Driver

        uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
        state_handler(i2c_uninit_command); // Uninit TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        if(control.long_term_storage == 1)
        {
            #if CY15B108QI
            uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
            state_handler(spim_enable_command); // Enable SPIM Module

            uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
            state_handler(spim_init_command); // Initialize SPIM Module
    
            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
            uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
            state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            uint8_t cy15b108qi_exit_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
            state_handler(cy15b108qi_exit_deep_power_down_mode_command); // Exit the Deep power down mode

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            if(control.external_memory_write_current_address > (control.external_memory_end_address - control.bytes_per_sample))
            {
                control.external_memory_write_current_address = control.external_memory_start_address;
            }

            cy15b108qi_write_registers(temperature.t, ARRAY_LENGTH(temperature.t), control.external_memory_write_current_address);
            control.external_memory_write_current_address += ARRAY_LENGTH(temperature.t);

            control.current_sample_count++;

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            uint8_t cy15b108qi_enter_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
            state_handler(cy15b108qi_enter_deep_power_down_mode_command); // Enter the Deep power down mode

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
            state_handler(spim_uninit_command); // Uninitialize SPIM Module

            uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
            state_handler(spim_disable_command); // Disable SPIM Module

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
            
            if(control.current_sample_count >= control.samples_per_recording_session)
            {   
                // The transmitting start address is set the write start address      
                control.external_memory_transmit_start_address = control.external_memory_write_start_address;           
                NRF_LOG_INFO("control.external_memory_transmit_start_address: %X", control.external_memory_transmit_start_address);  

                // The transmitting end address is set to the last write address
                control.external_memory_transmit_end_address = control.external_memory_write_current_address - control.bytes_per_sample;           
                NRF_LOG_INFO("control.external_memory_transmit_end_address: %X", control.external_memory_transmit_end_address); 

                // The external memory write start address is set to the external memory write_current address
                control.external_memory_write_start_address = control.external_memory_write_current_address;
                NRF_LOG_INFO("control.external_memory_write_start_address: %X", control.external_memory_write_start_address); 
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                // Reset sampling count
                control.current_sample_count = 0;

                // Flag to state that the data is ready to be transmitted via BLE
                control.data_ready_for_transmit = 1;  
                
                // Setting the number of bytes left to transmit to the number of bytes per recording session  
                control.bytes_left_to_transmit = control.bytes_per_recording_session;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit); 
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
                
                #if !MAX30003
                uint8_t bluetooth_restart_advertising_command[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_RESTART_ADVERTISING_COMMAND};
                state_handler(bluetooth_restart_advertising_command); // Start to transmit the Recording Session of the ECG Data
                #endif
                control.interrupt = 0;
            }
            #endif
        }
        else
        {
            uint8_t ble_tmp117_data[5] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_INSTANT_TEMP_CHAR_COMMAND, temperature.t[0], temperature.t[1]};
            state_handler(ble_tmp117_data); // Write the instant temperature data to GATT database
        }
    }
}

void tmp117_start_data_collection(void)
{
    NRF_LOG_INFO("tmp117_start_data_collection");
    control.interrupt = 1;    // Enabling the interrupt

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    #if !MAX30003 && CY15B108QI
    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Select Chip Select Pin to CY15B108QI for the SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    if(control.long_term_storage == 1)
    {
        #if CY15B108QI
        uint8_t cy15b108qi_exit_hibernation_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_HIBERNATION_MODE_COMMAND};
        state_handler(cy15b108qi_exit_hibernation_command); // Exit the Hibernation power state

        uint8_t cy15b108qi_enter_deep_power_down_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_enter_deep_power_down_command); // Enter the deep-power-down power state
        #endif
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module
    #endif

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t response_char_temp_data_collection_started_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TEMP_DATA_COLLECTION_STARTED};
    state_handler(response_char_temp_data_collection_started_command); // Sending message to phone that the recording session has started
}

void tmp117_stop_data_collection(void)
{
    NRF_LOG_INFO("tmp117_stop_data_collection");

    control.interrupt = 0;            // Disabling the interrupt flag
    control.long_term_storage = 0;    // Setting the long term storage flag to 0

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    control.external_memory_start_address = TMP117_EXTERNAL_MEMORY_START_ADDRESS;     // Start address of the external memory to store temperature data
    control.external_memory_end_address = TMP117_EXTERNAL_MEMORY_END_ADDRESS;         // End address of the external memory to store temperature data
    
    control.external_memory_write_start_address = control.external_memory_start_address;            // Initializing the start address to the memory start address
    control.external_memory_write_current_address = control.external_memory_write_start_address;    // Initializing the current write address to the write start address

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_command); // Init TWIM Driver

    uint8_t i2c_enable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_command); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    tmp117_set_averaging_mode(TMP117_NO_AVERAGING_MODE);
    tmp117_set_conversion_mode(TMP117_SHUTDOWN_MODE);

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_command); // Disable TWIM Driver

    uint8_t i2c_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_command); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    #if !MAX30003 && CY15B108QI

    uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_command); // Enable SPIM Module

    uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_command); // Initialize SPIM Module

    uint8_t spim_select_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
    if(control.long_term_storage == 1)
    {
        #if CY15B108QI
        spim_select_cs_pin_command[4] = CY15B108QI_CS_PIN;
        state_handler(spim_select_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

        uint8_t cy15b108qi_enter_hibernation_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND};
        state_handler(cy15b108qi_enter_hibernation_command);
        #endif
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_command); // Uninitialize SPIM Module

    uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_command); // Disable SPIM Module
    #endif

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t response_char_temp_data_collection_finished_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TEMP_DATA_COLLECTION_FINISHED};
    state_handler(response_char_temp_data_collection_finished_command); // Sending message to phone that the recording session has started
}

void tmp117_set_external_memory_start_address(uint32_t external_memory_start_address)
{
    NRF_LOG_INFO("tmp117_set_external_memory_write_start_address");
    control.external_memory_start_address;
}

void tmp117_set_external_memory_end_address(uint32_t external_memory_end_address)
{
    NRF_LOG_INFO("tmp117_set_external_memory_write_end_address");
    control.external_memory_end_address;
}

void tmp117_transmit_temperature_recording_session(void)
{
    NRF_LOG_INFO("tmp117_transmit_recording_session");
    if(control.data_ready_for_transmit == 1)
    {
        uint8_t empty_values = 0;

        uint8_t bluetooth_temp_data[control.bytes_per_bluetooth_transmission];

        #if CY15B108QI
        uint8_t spim_enable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
        state_handler(spim_enable_command); // Enable SPIM Module

        uint8_t spim_init_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
        state_handler(spim_init_command); // Initialize SPIM Module

        uint8_t spim_set_cs_pin_command[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
        state_handler(spim_set_cs_pin_command); // Set Chip Select Pin to CY15B108QI for the SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_exit_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_exit_deep_power_down_mode_command); // Exit the Deep power down mode

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        while(control.bytes_left_to_transmit > 0)
        {
            for(uint8_t i = 0; i < control.bytes_per_bluetooth_transmission; i++)
            {
                bluetooth_temp_data[i] = 0;
            }

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            if((control.external_memory_transmit_current_address > (control.external_memory_end_address - control.bytes_per_bluetooth_transmission))
            && (control.bytes_left_to_transmit > control.bytes_per_bluetooth_transmission))
            {
                NRF_LOG_INFO("tmp117_transmit_recording_session: case 1");
                // Get difference between end register and current transmit register
                uint8_t external_memory_end_address_difference = control.external_memory_end_address - control.external_memory_transmit_current_address; 
            
                // Creating an array to store the data until the end address
                uint8_t external_memory_end_address_data[external_memory_end_address_difference];

                // Read data from the registers until the last address
                cy15b108qi_fast_read_registers(external_memory_end_address_data, external_memory_end_address_difference, control.external_memory_transmit_current_address);
            
                // Reset the current read address
                control.external_memory_transmit_current_address = control.external_memory_start_address;

                // Calculate the rest of the data that is required to read
                uint8_t external_memory_start_address_difference = control.bytes_per_bluetooth_transmission - external_memory_end_address_difference;
            
                // Creating an array to store the data to fill the rest of the bluetooth buffer
                uint8_t external_memory_start_address_data[external_memory_start_address_difference];

                // Read data from the register until the number of bytes is read
                cy15b108qi_fast_read_registers(external_memory_start_address_data, external_memory_start_address_difference, control.external_memory_transmit_current_address);
            
                // Transfer the data from these registers to the Bluetooth data packet for transfer
            
                for(uint8_t i = 0; i < external_memory_end_address_difference; i++)
                {
                    bluetooth_temp_data[i] = external_memory_end_address_data[i];
                }

                for(uint8_t i = 0; i < external_memory_start_address_difference; i++)
                {
                    bluetooth_temp_data[i+external_memory_end_address_difference] = external_memory_start_address_data[i];
                }

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += external_memory_start_address_difference;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }
        
            else if((control.external_memory_transmit_current_address > (control.external_memory_end_address - control.bytes_left_to_transmit))
            && (control.bytes_left_to_transmit < control.bytes_per_bluetooth_transmission))
            {
                NRF_LOG_INFO("tmp117_transmit_recording_session: case 2");
                // Get difference between end register and current transmit register
                uint8_t external_memory_end_address_difference = control.external_memory_end_address - control.external_memory_transmit_current_address; 
            
                // Creating an array to store the data until the end address
                uint8_t external_memory_end_address_data[external_memory_end_address_difference];

                // Read data from the registers until the last address
                cy15b108qi_fast_read_registers(external_memory_end_address_data, external_memory_end_address_difference, control.external_memory_transmit_current_address);
            
                // Reset the current read address
                control.external_memory_transmit_current_address = control.external_memory_start_address;

                // Calculate the rest of the data that is required to read
                uint8_t external_memory_start_address_difference = control.bytes_left_to_transmit - external_memory_end_address_difference;
            
                // Creating an array to store the data to fill the rest of the bluetooth buffer
                uint8_t external_memory_start_address_data[external_memory_start_address_difference];

                // Read data from the register until the number of bytes is read
                cy15b108qi_fast_read_registers(external_memory_start_address_data, external_memory_start_address_difference, control.external_memory_transmit_current_address);
            
                // Transfer the data from these registers to the Bluetooth data packet for transfer
            
                for(uint8_t i = 0; i < external_memory_end_address_difference; i++)
                {
                    bluetooth_temp_data[i] = external_memory_end_address_data[i];
                }

                for(uint8_t i = 0; i < external_memory_start_address_difference; i++)
                {
                    bluetooth_temp_data[i+external_memory_end_address_difference] = external_memory_start_address_data[i];
                }

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += external_memory_start_address_difference;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }

            else if((control.bytes_left_to_transmit < control.bytes_per_bluetooth_transmission) && 
            ((control.external_memory_transmit_current_address < (control.external_memory_end_address - control.bytes_left_to_transmit))))
            {
                NRF_LOG_INFO("tmp117_transmit_recording_session: case 3");

                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
                NRF_LOG_INFO("control.bytes_per_bluetooth_transmission: %u", control.bytes_per_bluetooth_transmission);
                empty_values = (control.bytes_per_bluetooth_transmission - control.bytes_left_to_transmit);
                NRF_LOG_INFO("empty_value: %u", empty_values);

                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                // Read remaining data from external memory
                cy15b108qi_fast_read_registers(bluetooth_temp_data, control.bytes_left_to_transmit, control.external_memory_transmit_current_address);
            
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_left_to_transmit;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }

            else
            {
                NRF_LOG_INFO("tmp117_transmit_recording_session: default");
                // Read data from external memory
                cy15b108qi_fast_read_registers(bluetooth_temp_data, control.bytes_per_bluetooth_transmission, control.external_memory_transmit_current_address);
            
                /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

                control.external_memory_transmit_current_address += control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.external_memory_transmit_current_address: %X", control.external_memory_transmit_current_address);
                control.bytes_left_to_transmit -= control.bytes_per_bluetooth_transmission;
                NRF_LOG_INFO("control.bytes_left_to_transmit: %u", control.bytes_left_to_transmit);
            }

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            bluetooth_temperature_service_temp_char_write(bluetooth_temp_data);
        
            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            if(control.bytes_left_to_transmit == 0)
            {
                NRF_LOG_INFO("There are no more bytes left to transmit.");

                uint8_t bluetooth_transmit_temp_recording_session_finished_command[7] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND,
                0x00, 0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TRANSMIT_TEMP_DATA_FINISHED};
                state_handler(bluetooth_transmit_temp_recording_session_finished_command); // Recording session finished
                
                control.data_ready_for_transmit = 0;
            }
    
            if(empty_values != 0)
            {
                NRF_LOG_INFO("There are empty values: %u", empty_values);

                uint8_t bluetooth_transmit_temp_data_empty_values_command[7] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND,
                0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_TRANSMIT_TEMP_DATA_EMPTY_VALUES, empty_values};
                state_handler(bluetooth_transmit_temp_data_empty_values_command); // Send the number of empty values
            }
        }

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        control.data_ready_for_transmit = 0;    // Resetting the flag stating that data is not ready for transmit

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t cy15b108qi_enter_deep_power_down_mode_command[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
        state_handler(cy15b108qi_enter_deep_power_down_mode_command); // Enter the Deep power down mode

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t spim_uninit_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
        state_handler(spim_uninit_command); // Uninitialize SPIM Module

        uint8_t spim_disable_command[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
        state_handler(spim_disable_command); // Disable SPIM Module  

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        #endif
    }
}


/* Static Functions */


/* Temperature Register. This structure contains all values read from this register. This register is a 16-bit, read-only 
* register that stores the output of the most recent conversion. One LSB equals 7.8125 m°C. Data are represented in binary two's complement format. 
* Following a reset, the temperature register reads –256°C until the first conversion, including averaging, is complete.
* Register address: 0x00
* Values are stored in bits: [15:0]
*/
static void _tmp117_read_temperature_register(void)
{
    NRF_LOG_INFO("_tmp117_read_temperature_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &temperature.register_pointer, 1, control.i2c_data, 2);

    temperature.t[0] = control.i2c_data[0];
    temperature.t[1] = control.i2c_data[1];
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

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &configuration.register_pointer, 1, control.i2c_data, 2); 

    configuration.high_alert = (control.i2c_data[0] & 0b10000000) >> 7;
    configuration.low_alert = (control.i2c_data[0] & 0b01000000) >> 6;
    configuration.data_ready = (control.i2c_data[0] & 0b00100000) >> 5;
    configuration.eeprom_busy = (control.i2c_data[0] & 0b00010000) >> 4;
    configuration.mod = (control.i2c_data[0] & 0b00001100) >> 2;
    configuration.conv = ((control.i2c_data[0] & 0b00000011) << 1) | ((control.i2c_data[1] & 0b10000000) >> 7);
    
    configuration.avg = (control.i2c_data[1] & 0b01100000) >> 5;
    configuration.t_na = (control.i2c_data[1] & 0b00010000) >> 4;
    configuration.pol = (control.i2c_data[1] & 0b00001000) >> 3;
    configuration.dr_alert = (control.i2c_data[1] & 0b00000100) >> 2;
    configuration.soft_reset = (control.i2c_data[1] & 0b00000010) >> 1;
}


static void _tmp117_write_configuration_register(void)
{
    NRF_LOG_INFO("_tmp117_write_configuration_register");

    control.i2c_data[0] = (configuration.high_alert << 7) | (configuration.low_alert << 6) | (configuration.data_ready << 5);
    control.i2c_data[0] = (configuration.eeprom_busy << 4) | (configuration.mod << 2) | control.i2c_data[0];
    control.i2c_data[0] = ((configuration.conv & 0b00000110) >> 1) | control.i2c_data[0];

    control.i2c_data[1] = ((configuration.conv & 0b00000001) << 7) | (configuration.avg << 5) | (configuration.t_na << 4);
    control.i2c_data[1] = (configuration.pol << 3) | (configuration.dr_alert << 2) | (configuration.soft_reset << 1) | control.i2c_data[1];


    uint8_t temp_data[3] = {configuration.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
}

/* High Limit Register. This register is a 16-bit, read/write register that stores the high limit for comparison with the 
* temperature result. One LSB equals 7.8125 m°C. The range of the register is ±256°C. Negative numbers are represented in binary two's 
* complement format. Following power-up or a general-call reset,the high-limit register is loaded with the stored value from the EEPROM.
* Register address: 0x02
* Values are stored in bits: [15:0]
*/
static void _tmp117_read_high_limit_register(void)
{
    NRF_LOG_INFO("_tmp117_read_high_limit_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &high_limit.register_pointer, 1, control.i2c_data, 2); 

    high_limit.h[0] = control.i2c_data[0];
    high_limit.h[1] = control.i2c_data[1];
}

static void _tmp117_write_high_limit_register(void)
{
    NRF_LOG_INFO("_tmp117_write_high_limit_register");

    control.i2c_data[0] = high_limit.h[0];
    control.i2c_data[1] = high_limit.h[1];

    uint8_t temp_data[3] = {high_limit.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
}

/* Low Limit Register. This register is a 16-bit, read/write register that stores the low limit for comparison with the 
* temperature result. One LSB equals 7.8125 m°C. The range of the register is ±256°C. Negative numbers are represented in binary two's 
* complement format. Following power-up or a general-call reset,the low-limit register is loaded with the stored value from the EEPROM.
* Register address: 0x03
* Values are stored in bits: [15:0]
*/
static void _tmp117_read_low_limit_register(void)
{
    NRF_LOG_INFO("_tmp117_read_low_limit_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &low_limit.register_pointer, 1, control.i2c_data, 2); 

    low_limit.l[0] = control.i2c_data[0];
    low_limit.l[1] = control.i2c_data[1];
}

static void _tmp117_write_low_limit_register(void)
{
    NRF_LOG_INFO("_tmp117_write_low_limit_register");

    control.i2c_data[0] = low_limit.l[0];
    control.i2c_data[1] = low_limit.l[1];

    uint8_t temp_data[3] = {low_limit.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
}


/* EEPROM Unlock Register. The EEPROM1 register is a 16-bit register that be used as a scratchpad by the customer to store
* general-purpose data. This register has a corresponding EEPROM location. Writes to this address when the EEPROM is locked write data into 
* the register and not to the EEPROM. Writes to this register when the EEPROM is unlocked causes the corresponding EEPROM location to be 
* programmed.
* Register address: 0x04
* D[15]: EUN = EEPROM unlock. 1 = unlocked; 0 = locked
* D[14]: EEPROM_BUSY = EEPROM BUSY FLAG
* D[13:0]: N/A = NOT USED
*/
static void _tmp117_read_eeprom_unlock_register(void)
{
    NRF_LOG_INFO("_tmp117_read_eeprom_unlock_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &eeprom_unlock.register_pointer, 1, control.i2c_data, 2); 

    eeprom_unlock.eun = (control.i2c_data[0] & 0b10000000) >> 7;
    eeprom_unlock.eeprom_busy = (control.i2c_data[0] & 0b01000000) >> 6;
}

static void _tmp117_write_eeprom_unlock_register(void)
{
    NRF_LOG_INFO("_tmp117_write_eeprom_unlock_register");

    control.i2c_data[0] = (eeprom_unlock.eun << 7) | (eeprom_unlock.eeprom_busy << 6);
    control.i2c_data[1] = 0;

    uint8_t temp_data[3] = {eeprom_unlock.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
}

/* Temperature Offset Register Structure. This 16-bit register is to be used as a user-defined temperature offset register during 
* system calibration.The offset will be added to the temperature result after linearization. It has a same resolution of 7.8125 m°C and 
* same range of ±256°C as the temperature result register. The data format is the same as the temperature register. If the added result is
* out of boundary, then the temperature result will show as the maximum or minimum value.
* Register address: 0x07
* Values are stored in bits: [15:0]
*/
static void _tmp117_read_temperature_offset_register(void)
{
    NRF_LOG_INFO("_tmp117_read_temperature_offset_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &temperature_offset.register_pointer, 1, control.i2c_data, 2); 

    temperature_offset.offset[0] = control.i2c_data[0];
    temperature_offset.offset[1] = control.i2c_data[1];
}

static void _tmp117_write_temperature_offset_register(void)
{
    NRF_LOG_INFO("_tmp117_write_temperature_offset_register");

    control.i2c_data[0] = temperature_offset.offset[0];
    control.i2c_data[1] = temperature_offset.offset[1];

    uint8_t temp_data[3] = {temperature_offset.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
}

/* Device ID Register. This read-only register contains the device ID.
* Register address: 0x0F
* D[15:12] Rev = Indicates the revision number
* D[11:0] DID = Indicates the Device ID
*/
static void _tmp117_read_device_id_register(void)
{
    NRF_LOG_INFO("_tmp117_read_device_id_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &device_id.register_pointer, 1, control.i2c_data, 2);

    device_id.rev = (control.i2c_data[0] & 0b11110000) >> 4;
    device_id.device_id[0] = control.i2c_data[0] & 0b00001111;
    device_id.device_id[1] = control.i2c_data[1];
}

static void _tmp117_unlock_eeprom(void)
{
    NRF_LOG_INFO("_tmp117_unlock_eeprom");
    eeprom_unlock.eun = 1;
    _tmp117_write_eeprom_unlock_register();
}

static void _tmp117_lock_eeprom(void)
{
    NRF_LOG_INFO("_tmp117_lock_eeprom");
    eeprom_unlock.eun = 0;
    _tmp117_write_eeprom_unlock_register();
}

static void _tpm117_soft_reset(void)
{
    NRF_LOG_INFO("_tmp117_lock_eeprom");
    _tmp117_unlock_eeprom();
    configuration.soft_reset = 1;
    _tmp117_write_configuration_register();
    nrf_delay_ms(3);
}

static void _tmp117_general_call_reset(void)
{
    NRF_LOG_INFO("TMP117: General Call Reset");
    
    uint8_t temp_data[1] = {TMP117_GENERAL_CALL_RESET};
    i2c_no_stop_write_register(control.slave_address, temp_data, 1);
    nrf_delay_ms(2);
}
 
#endif