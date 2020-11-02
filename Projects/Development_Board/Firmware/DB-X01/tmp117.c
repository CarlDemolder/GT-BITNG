#include "tmp117.h"


uint16_t tmp117_read_chip_id(void)
{
    NRF_LOG_INFO("tmp117_read_chip_id");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_DEVICE_ID_REGISTER;       // TMP117 Device ID stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    uint16_t device_id =  ((register_data[0] << 8) & 0b00001111) | register_data[1];
    return device_id;
}

uint8_t tmp117_read_revision_number(void)
{
    NRF_LOG_INFO("tmp117_read_chip_id");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_DEVICE_ID_REGISTER;       // TMP117 Revision Number stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    uint8_t revision_number =  register_data[0] & 0b11110000;   // Reading revision number bits
    return revision_number;
}

float tmp117_get_celsius(void)
{
    NRF_LOG_INFO("tmp117_get_celsius");
    float reg_cels_conv = 7812.5; //1 Register unit = 0.0078125 Celcius
    float celsius_counts = (float) tmp117_get_uint16_t();
    float tmp117_celsius = reg_cels_conv*celsius_counts/1000000;
    NRF_LOG_INFO("Temperature Value: " NRF_LOG_FLOAT_MARKER " C.", NRF_LOG_FLOAT(tmp117_celsius));
    return tmp117_celsius;
}

uint16_t tmp117_get_uint16_t(void)
{    
    NRF_LOG_INFO("tmp117_get_uint16_t");
    uint8_t register_byte_count = 2;                            // Number of bytes of buffer to read
    uint8_t device_address = TMP117_SLAVE_ADDRESS;              // Slave Address for TMP117
    uint8_t register_address = TMP117_TEMP_RESULT_REGISTER;     // TMP117 temperature data stored in register
    uint8_t register_data[register_byte_count];                 // Store data in array
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    uint16_t uint16_temp_results =  (register_data[0] << 8) | register_data[1];   // Reading temperature data bits
    return uint16_temp_results;
}

void tmp117_string_celsius(char *tmp117_temperature)
{
    float int_temp = tmp117_get_celsius();
    sprintf(tmp117_temperature, "%.4f", int_temp);    
}

void tmp117_get_uint8_t(uint8_t *tmp117_uint8_t)
{
    char tmp117_char[5]; //Initializes the Character Array
    tmp117_string_celsius(tmp117_char);

    for(uint8_t i = 0; i<strlen(tmp117_char); i++)
    {
        tmp117_uint8_t[i] = (uint8_t) tmp117_char[i];
    }
}

void tmp117_init(uint8_t configuration_mode, uint8_t averaging_mode)
{
    tmp117_unlock_eeprom();
    tmp117_set_operating_mode(configuration_mode, averaging_mode);
    tmp117_general_call_reset();
}

void tmp117_unlock_eeprom(void)
{
    NRF_LOG_INFO("unlock_eeprom_tmp117");
    uint8_t register_byte_count = 3;                                //Slave Address + Register Write Value
    uint8_t device_address = TMP117_SLAVE_ADDRESS;                  // Slave Address for TMP117, ADD0 = GND
    
    /*tmp117 unlock EEPROM address for writing to configuration register. */
    uint8_t register_address = TMP117_EEPROM_UNLOCK_REGISTER;       // Register for EEPROM Unlock Address
    uint8_t tmp117_eun_value_1 = 0x80;               // EUN Value to Unlock EEPROM 
    uint8_t tmp117_eun_value_2 = 0x00;               // EUN Value to Unlock EEPROM 
    
    uint8_t register_data[register_byte_count];
    register_data[0] = register_address;
    register_data[1] = tmp117_eun_value_1;
    register_data[2] = tmp117_eun_value_2;
    
    i2c_write_registers(device_address, register_data, register_byte_count);
}

void tmp117_set_operating_mode(uint8_t conversion_mode, uint8_t averaging_mode)
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
}

void tmp117_general_call_reset(void)
{
    NRF_LOG_INFO("TMP117: General Call Reset");
    
    uint8_t register_byte_count = 1;    
    uint8_t register_data = TMP117_GENERAL_CALL_RESET; 
    i2c_write_registers(GENERAL_CALL_ADDRESS, &register_data, register_byte_count);
}
