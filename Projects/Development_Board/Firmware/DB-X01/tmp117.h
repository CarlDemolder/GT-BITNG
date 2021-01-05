#ifndef __TMP117_H
#define __TMP117_H

#include "serial_slave.h"
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

struct TMP117_CONTROL_STRUCT
{
    ret_code_t error_code;                                  /**< Variable to track errors */
    uint16_t temp_raw_value;                                /**< Variable to record the temperature raw value in an uint16_t format */
    uint8_t temp_raw_value_array[2];                        /**< Variable to record the temperature raw value in an uint8_t array format */
    uint8_t temp_uint8_t[5];                                /**< Variable to record temperature uint8_t */
    uint16_t device_id;                                     /**< Variable to record Device ID */
    uint8_t revision_number;                                /**< Variable to record Revision Number */
    uint32_t external_memory_write_start_address;           /**< Variable to record the start address to store values in the external memory */
    uint32_t external_memory_write_end_address;             /**< Variable to record the end address to store values in the external memory */
    uint32_t external_memory_write_current_address;         /**< Variable to record the current address to store values in the external memory */
    uint32_t external_memory_transmit_start_address;        /**< Variable to record the start address to transmit stored values of the external memory over BLE/USB */
    uint32_t external_memory_transmit_end_address;          /**< Variable to record the end address to transmit stored values of the external memory over BLE/USB */
    uint32_t external_memory_transmit_current_address;      /**< Variable to record the current address to transmit stored values of the external memory over BLE/USB */
    uint8_t samples_per_bluetooth_transmission;             /**< Variable to record the number of samples per bluetooth transmission */
    uint8_t bytes_per_bluetooth_transmission;               /**< Variable to record the number of bytes per bluetooth transmission */
    uint32_t samples_per_recording_session;                 /**< Variable to record the number of samples per recording session */
    uint32_t bytes_per_recording_session;                   /**< Variable to record the number of bytes per recording session */
    uint32_t current_sample_count;                          /**< Variable to record the current sample count */         
    uint16_t samples_per_minute;                            /**< Variable to record the number of samples per minute */
    uint32_t bytes_left_to_transmit;                        /**< Variable to record the number of bytes left to transmit over BLE/USB */
    uint8_t bytes_per_sample;                               /**< Variable to record the number of bytes per samples */
    uint8_t interrupt;                                      /**< Variable to record when the data interrupt flag */
};

void tmp117_unlock_eeprom(void);
void tmp117_set_operating_mode(uint8_t conversion_mode, uint8_t averaging_mode);
void tmp117_general_call_reset(void);

float tmp117_get_celsius(void);

void tmp117_get_uint16_t(void);

void tmp117_string_celsius(char *tmp117_temperature);

void tmp117_get_uint8_t(void);

void tmp117_init(uint8_t configuration_mode, uint8_t averaging_mode);

void tmp117_read_chip_id(void);

void tmp117_read_revision_number(void);

void tmp117_interrupt_handler(void);

void tmp117_stop_recording_session(void);

void tmp117_set_external_memory_write_start_address(uint32_t external_memory_write_start_address);

void tmp117_set_external_memory_write_end_address(uint32_t external_memory_write_end_address);

#endif

#endif // __tmp117_H