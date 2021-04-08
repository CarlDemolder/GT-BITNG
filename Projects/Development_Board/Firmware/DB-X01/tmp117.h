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
    TMP117_DEVICE_ID = 0X0F,
    
    TMP117_EXTERNAL_MEMORY_START_ADDRESS = 0x000000,
    TMP117_EXTERNAL_MEMORY_END_ADDRESS = 0x000118,
};

enum TMP117_OPERATION_CONSTANTS
{
    TMP117_SHUTDOWN_MODE = 0X01,
    TMP117_CONTINUOUS_CONVERSION_MODE = 0X00,
    TMP117_ONE_SHOT_CONVERSION = 0X03,
};

enum TMP117_CONVERSION_CYCLE_TIME_CONSTANTS
{
    TMP117_SHORTEST_TIME_MODE = 0X00,
    TMP117_LONGEST_TIME_MODE = 0X07,

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

/* Control Struct */

struct TMP117_Control_Struct
{
    ret_code_t error_code;                                  /**< Variable to track errors */
    uint8_t slave_address;                                  /**< Variable to record the I2C slave address */
    uint8_t status;                                         /**< Variable to record if the TMP117 is enabled or disabled */
    uint8_t interrupt;                                      /**< Variable to record when the data interrupt flag */
    uint8_t data_ready_for_transmit;                        /**< Variable to record when the data is ready for transmit */
         
    uint8_t i2c_data[2];                                    /**< Variable to record the data communicated through I2C */
    uint8_t register_byte_count;                            /**< Variable to record the number of bytes communicated through I2C */

    uint8_t long_term_storage;                              /**< Variable to record a flag to determine if the data is going to be stored long term */

    uint32_t external_memory_write_start_address;           /**< Variable to record the start address to store values in the external memory */
    uint32_t external_memory_write_current_address;         /**< Variable to record the current address to store values in the external memory */
    
    uint32_t external_memory_start_address;                 /**< Variable to record the start address of the external memory */
    uint32_t external_memory_end_address;                   /**< Variable to record the end address of the external memory */

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
};

/* Register Structs */

/**@brief Temperature Register Structure. This structure contains all values read from this register. This register is a 16-bit, read-only 
* register that stores the output of the most recent conversion. One LSB equals 7.8125 m°C. Data are represented in binary two's complement format. 
* Following a reset, the temperature register reads –256°C until the first conversion, including averaging, is complete.
* Register address: 0x00
* Values are stored in bits: [15:0]
*/
struct TMP117_Temperature_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address to this register */
    uint8_t t[2];                                 /**< Array to store temperature measurement value */
};

/**@brief Configuration Register Structure. This register contains value to configure the settings of the
* TMP117. 
*/
struct TMP117_Configuration_Register_Struct
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

/* @brief High Limit Register Structure. This register is a 16-bit, read/write register that stores the high limit for comparison with the 
* temperature result. One LSB equals 7.8125m°C. The range of the register is ±256°C. Negative numbers are represented in binary two's 
* complement format. Following power-up or a general-call reset,the high-limit register is loaded with the stored value from the EEPROM.
* Register address: 0x02
* Values are stored in bits: [15:0]
*/
struct TMP117_High_Limit_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t h[2];                                 /**< Array to store the high limit register value */
};


/* @brief Low Limit Register Structure. This register is a 16-bit, read/write register that stores the low limit for comparison with the 
* temperature result. One LSB equals 7.8125m°C. The range of the register is ±256°C. Negative numbers are represented in binary two's 
* complement format. Following power-up or a general-call reset,the low-limit register is loaded with the stored value from the EEPROM.
* Register address: 0x03
* Values are stored in bits: [15:0]
*/
struct TMP117_Low_Limit_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t l[2];                                 /**< Array to store the low limit register value */
};


/* @brief EEPROM Unlock Register Structure. The EEPROM1 register is a 16-bit register that be used as a scratchpad by the customer to store
* general-purpose data. This register has a corresponding EEPROM location. Writes to this address when the EEPROM is locked write data into 
* the register and not to the EEPROM. Writes to this register when the EEPROM is unlocked causes the corresponding EEPROM location to be 
* programmed.
* Register address: 0x04
* D[15]: EUN = EEPROM unlock. 1 = unlocked; 0 = locked
* D[14]: EEPROM_BUSY = EEPROM BUSY FLAG
* D[13:0]: N/A = NOT USED
*/
struct TMP117_EEPROM_Unlock_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t eun;                                  /**< Value to store the command to lock/unlock the EEPROM */
    uint8_t eeprom_busy;                          /**< Value to store the flag that indicates that the EEPROM is busy */
};

/* @brief Temperature Offset Register Structure. This 16-bit register is to be used as a user-defined temperature offset register during 
* system calibration.The offset will be added to the temperature result after linearization. It has a same resolution of 7.8125 m°C and 
* same range of ±256°C as the temperature result register. The data format is the same as the temperature register. If the added result is
* out of boundary, then the temperature result will show as the maximum or minimum value.
* Register address: 0x07
* Values are stored in bits: [15:0]
*/
struct TMP117_Temperature_Offset_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t offset[2];                            /**< Array to store the temperature offset register value */
};

/* @brief Device ID Register. This read-only register contains the device ID.
* Register address: 0x0F
* D[15:12] Rev = Indicates the revision number
* D[11:0] DID = Indicates the Device ID
*/
struct TMP117_Device_ID_Register_Struct
{
    uint8_t register_pointer;                     /**< Value to store the address pointing to this register */
    uint8_t rev;                                  /**< Value to store the revision number */
    uint8_t device_id[2];                         /**< Array to store the Device ID */
};


/* Public Functions */

void tmp117_init(void);

void tmp117_uninit(void);

uint8_t tmp117_is_enabled(void);

void tmp117_enable_long_term_storage(void);

void tmp117_disable_long_term_storage(void);

void tmp117_get_device_id(uint8_t *temp_device_id);

void tmp117_get_revision_number(uint8_t *temp_revision_number);

void tmp117_get_temperature(uint8_t *temp_temperature);

void tmp117_set_averaging_mode(uint8_t averaging_mode);

void tmp117_set_conversion_mode(uint8_t conversion_mode);

void tmp117_interrupt_handler(void);

void tmp117_start_data_collection(void);

void tmp117_stop_data_collection(void);

void tmp117_transmit_temperature_recording_session(void);

void tmp117_set_external_memory_start_address(uint32_t external_memory_start_address);

void tmp117_set_external_memory_end_address(uint32_t external_memory_end_address);

void tmp117_take_measurement(void);

/* Static Functions */

static void _tmp117_read_temperature_register(void);

static void _tmp117_read_configuration_register(void);

static void _tmp117_write_configuration_register(void);

static void _tmp117_read_high_limit_register(void);

static void _tmp117_write_high_limit_register(void);

static void _tmp117_read_low_limit_register(void);

static void _tmp117_write_low_limit_register(void);

static void _tmp117_read_eeprom_unlock_register(void);

static void _tmp117_write_eeprom_unlock_register(void);

static void _tmp117_read_temperature_offset_register(void);

static void _tmp117_write_temperature_offset_register(void);

static void _tmp117_read_device_id_register(void);

static void _tmp117_unlock_eeprom(void);

static void _tmp117_lock_eeprom(void);

static void _tpm117_soft_reset(void);

static void _tmp117_general_call_reset(void);

#endif

#endif // __tmp117_H