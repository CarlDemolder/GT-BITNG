#ifndef __TMP117_H
#define __TMP117_H

#include "serial_slave.h"
#include "i2c.h"

#if TMP117

/* String literals stored in enums */

enum TMP117_REGISTER_CONSTANTS
{
    TMP117_SLAVE_ADDRESS = 0X48,
    TMP117_TEMP_RESULT = 0X00,
    TMP117_CONFIGURATION = 0X01,
    TMP117_THIGH_LIMIT = 0X02,
    TMP117_TLOW_LIMIT = 0X03,
    TMP117_EEPROM_UL = 0X04,
    TMP117_EEPROM1 = 0X05,
    TMP117_EEPROM2 = 0X06,
    TMP117_TEMP_OFFSET = 0X07,
    TMP117_EEPROM3 = 0X08,
    TMP117_DEVICE_ID = 0X0F
};

enum TMP117_OPERATION_CONSTANTS
{
    TMP117_SHUTDOWN_MODE = 0X04,
    TMP117_CONTINUOUS_CONVERSION_MODE = 0X01
};

enum TMP117_AVERAGING_CONSTANTS
{
    TMP117_NO_AVERAGING_MODE = 0X00,
    TMP117_8_AVERAGED_MODE = 0X20,
    TMP117_32_AVERAGED_MODE = 0X40,
    TMP117_64_AVERAGED_MODE = 0X60
};

enum TMP117_FUNCTION_CONSTANTS
{
    TMP117_GENERAL_CALL_RESET = 0X06
};

/* Control Struct */

struct TMP117_control_struct
{
    ret_code_t error_code;                                  /**< Variable to track errors */
    uint8_t conversion_mode;                                /**< Variable to record the conversion mode */
    uint8_t averaging_mode;                                 /**< Variable to record the averaging mode */
    uint8_t slave_address[0];                               /**< Variable to record the I2C slave address */
     
    uint8_t data_array[2];                                  /**< Variable to record the data communicated through I2C */
    uint8_t register_byte_count[1];                         /**< Variable to record the number of bytes communicated through I2C */
    uint16_t temp_raw_value;                                /**< Variable to record the temperature raw value in an uint16_t format */
    uint8_t temp_raw_value_array[2];                        /**< Variable to record the temperature raw value in an uint8_t array format */
    uint8_t temp_uint8_t[5];                                /**< Variable to record temperature uint8_t */
    uint8_t device_id[2];                                   /**< Variable to record Device ID */
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

/* Register Structs */

/**@brief Temperature Register Structure. This structure contains all values read from this register. This register is a 16-bit, read-only 
* register that stores the output of the most recent conversion. One LSB equals 7.8125 m°C. Data are represented in binary two's complement format. 
* Following a reset, the temperature register reads –256°C until the first conversion, including averaging, is complete.
* Register value: 0x00
* Values are stored in bits: [15:0]
*/
struct FDC1004_Temperature_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address to this register */
    uint8_t temperature_measurement[2];           /**< Array to store temperature measurement value */
};

/**@brief Configuration Register Structure. This register contains value to configure the settings of the
* TMP117. 
*/
struct FDC1004_Configuration_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address to this register */
    uint8_t high_alert;                           /**< High alert flag */
    uint8_t low_alert;                            /**< Low alert flag */
    uint8_t data_ready;                           /**< Data ready flag */
    uint8_t eeprom_busy;                          /**< EEPROM busy flag */
    uint8_t mod;                                  /**< Conversion mode */
    uint8_t conv;                                 /**< Conversion cycle bit */
    uint8_t avg;                                  /**< Conversion averaging mode */
    uint8_t t_na;                                 /**< Therm/alert mode select */
    uint8_t pol;                                  /**< Alert pin polarity bit */
    uint8_t dr_alert;                             /**< Alert pin select bit */
    uint8_t soft_reset;                           /**< Software reset bit */
};

/* Static Functions */

static void _tmp117_unlock_eeprom(void);

static void _tmp117_read_chip_id(void);

static void _tmp117_read_revision_number(void);

void tmp117_set_operating_mode(uint8_t conversion_mode, uint8_t averaging_mode);

static void _tmp117_general_call_reset(void);

float tmp117_get_celsius(void);

void tmp117_get_uint16_t(void);

void tmp117_string_celsius(char *tmp117_temperature);

void tmp117_get_uint8_t(void);


/* Public Functions */

void tmp117_init();

void tmp117_get_chip_id(uint8_t *temp_device_id);

void tmp117_get_revision_number(uint8_t *temp_revision_number);

void tmp117_interrupt_handler(void);

void tmp117_stop_recording_session(void);

void tmp117_set_external_memory_write_start_address(uint32_t external_memory_write_start_address);

void tmp117_set_external_memory_write_end_address(uint32_t external_memory_write_end_address);

#endif

#endif // __tmp117_H