#ifndef __TMP117_H
#define __TMP117_H

#include "common.h"
#include "i2c.h"

#if TMP117

enum TMP117_REGISTER_CONSTANTS
{
    TMP117_SLAVE_ADDRESS = 0X48,
    TMP117_TEMP_RESULT_REGISTER = 0X00,
    TMP117_CONFIGURATION_REGISTER = 0X01,
    TMP117_TEMP_HIGH_LIMIT_REGISTER = 0X02,
    TMP117_TEMP_LOW_LIMIT_REGISTER = 0X03,
    TMP117_EEPROM_UNLOCK_REGISTER = 0X04,
    TMP117_EEPROM_1_REGISTER = 0X05,
    TMP117_EEPROM_2_REGISTER = 0X06,
    TMP117_TEMP_OFFSET_REGISTER = 0X07,
    TMP117_EEPROM_3_REGISTER = 0X08,
    TMP117_DEVICE_ID_REGISTER = 0X0F
};

enum TMP117_OPERATION_CONSTANTS
{
    TMP117_SHUTDOWN_MODE = 0X00,
    TMP117_CONTINUOUS_CONVERSION_MODE = 0X01
};

enum TMP117_AVERAGING_CONSTANTS
{
    TMP117_NO_AVERAGING_MODE = 0X00,
    TMP117_8_AVERAGED_MODE = 0X01,
    TMP117_32_AVERAGED_MODE = 0X02,
    TMP117_64_AVERAGED_MODE = 0X03
};

enum TMP117_FUNCTION_CONSTANTS
{
    TMP117_GENERAL_CALL_RESET = 0X06
};


void tmp117_unlock_eeprom(void);
void tmp117_set_operating_mode(uint8_t conversion_mode, uint8_t averaging_mode);
void tmp117_general_call_reset(void);

float tmp117_get_celsius(void);
uint16_t tmp117_get_uint16_t(void);
void tmp117_string_celsius(char *tmp117_temperature);
void tmp117_get_uint8_t(uint8_t *tmp117_uint8);
void tmp117_init(uint8_t configuration_mode, uint8_t averaging_mode);

uint16_t tmp117_read_chip_id(void);
uint8_t tmp117_read_revision_number(void);

#endif

#endif // __tmp117_H