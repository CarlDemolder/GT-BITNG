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
    float celsius_counts = (float) tmp117_get_uint16();
    float tmp117_celsius = reg_cels_conv*celsius_counts/1000000;
    NRF_LOG_INFO("Temperature Value: " NRF_LOG_FLOAT_MARKER " C.", NRF_LOG_FLOAT(tmp117_celsius));
    return tmp117_celsius;
}

uint16_t tmp117_get_uint16(void)
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

void configure_tmp117(uint8_t configuration_mode)
{
    _unlock_eeprom_tmp117();
    _set_operating_mode_tmp117(configuration_mode);
    _general_call_reset_tmp117();
}

static void _unlock_eeprom_tmp117(void)
{
    uint8_t register_byte_count = 3;                  //Slave Address + Register Write Value
    uint8_t device_address = 0x48;                   // Slave Address for tmp117, ADD0 = GND
    
    /*tmp117 unlock EEPROM address for writing to configuration register. */
    uint8_t register_address = 0x04;                 // Register for EEPROM Unlock Address
    uint8_t tmp117_eun_value_1 = 0x80;               // EUN Value to Unlock EEPROM 
    uint8_t tmp117_eun_value_2 = 0x00;               // EUN Value to Unlock EEPROM 
    
    uint8_t register_data[register_byte_count];
    register_data[0] = register_address;
    register_data[1] = tmp117_eun_value_1;
    register_data[2] = tmp117_eun_value_2;
    
    i2c_write_registers(device_address, register_data, register_byte_count);

    NRF_LOG_INFO("Unlock EPROM");
}

static void _set_operating_mode_tmp117(uint8_t configuration_mode)
{
    uint8_t shutdown_mode = 0x00;
    uint8_t continous_mode = 0x01;
    uint8_t tmp117_config_value_1;
    
    if(configuration_mode == shutdown_mode)
    {
        tmp117_config_value_1 = 0x04;                //Configuration Value to set tmp117 for Shutdown Mode, 0x04
    }
    else if(configuration_mode == continous_mode)
    {
        tmp117_config_value_1 = 0x00;                //Configuration Value to set tmp117 for Continous Conversion, 0x00
    }
    else
    {
        tmp117_config_value_1 = 0x04;     // Default Mode is Shutdown Mode for the tmp117
    }
    
    /*tmp117 Internal register write to Configuration Register. */
    uint8_t register_address = 0x01;                //Register for Configuration Register

    uint8_t tmp117_config_value_2 = 0x00;                //Configuration Value to set tmp117
    // MOD[1:0] = 11, ~ Single Shot Mode
    // MOD[1:0] = 00, ~ Continous Conversion Mode
    // MOD[1:0] = 01, ~ Shutdown Mode
    // AVG[1:0] = 00, ~ No Averaging, 15.5 ms Active Conversion Time
    // CONV[2:0] = 000, ~Conversion Cycle Time in CC Mode
 
    uint8_t register_byte_count = 3; //Slave Address + Register Write Value
    uint8_t device_address = 0x48;                   // Slave Address for tmp117, ADD0 = GND

    uint8_t register_data[register_byte_count];
    register_data[0] = register_address;
    register_data[1] = tmp117_config_value_1;
    register_data[2] = tmp117_config_value_2;

    i2c_write_registers(device_address, register_data, register_byte_count);
}

static void _general_call_reset_tmp117(void)
{
    NRF_LOG_INFO("Configure Register Function");
    
    uint8_t device_address = 0x48;                   // Slave Address for tmp117, ADD0 = GND

    /*General-Call Reset Function to reset tmp117 internal registers to power-up values. */
    uint8_t register_address = 0x00;                  // General Call Address, ADD0 = GND
    uint8_t register_value = 0x06;                    // tmp117 General Call Reset Value    

    i2c_write_single_register(device_address, register_address, register_value);
      
    NRF_LOG_INFO("General-Call Reset Function");

    /*Read back the configuration register */

    register_address = 0x01;                // Register for Configuration Register
    uint8_t register_byte_count = 2;                // Number of bytes of buffer to read
    uint8_t register_data[register_byte_count];
    
    i2c_read_registers(device_address, register_address, register_data, register_byte_count);
    nrf_delay_ms(100);    
}
