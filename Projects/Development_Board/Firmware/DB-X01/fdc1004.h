#ifndef FDC1004_H_
#define FDC1004_H_

#include "serial_slave.h"
#include "i2c.h"

#if FDC1004

enum FDC1004_CONSTANTS
{
    FDC1004_SLAVE_ADDRESS = 0X50,
    FDC1004_MEAS1_MSB = 0X00,
    FDC1004_MEAS1_LSB = 0X01,
    FDC1004_MEAS2_MSB = 0X02,
    FDC1004_MEAS2_LSB = 0X03,
    FDC1004_MEAS3_MSB = 0X04,
    FDC1004_MEAS3_LSB = 0X05,
    FDC1004_MEAS4_MSB = 0X06,
    FDC1004_MEAS4_LSB = 0X07,
    FDC1004_CONF_MEAS1 = 0X08,
    FDC1004_CONF_MEAS2 = 0X09,
    FDC1004_CONF_MEAS3 = 0X0A,
    FDC1004_CONF_MEAS4 = 0X0B,
    FDC1004_FDC_CONF = 0X0C, 
    FDC1004_OFFSET_CAL_CIN1 = 0X0D,
    FDC1004_OFFSET_CAL_CIN2 = 0X0E,
    FDC1004_OFFSET_CAL_CIN3 = 0X0F,
    FDC1004_OFFSET_CAL_CIN4 = 0X10,
    FDC1004_GAIN_CAL_CIN1 = 0X11,
    FDC1004_GAIN_CAL_CIN2 = 0X12,
    FDC1004_GAIN_CAL_CIN3 = 0X13,
    FDC1004_GAIN_CAL_CIN4 = 0X14,
    FDC1004_MANUFACTURER_ID = 0XFE,
    FDC1004_DEVICE_ID = 0XFF,
    FDC1004_TRIGGER_MEASUREMENT_MODE = 0X20,
    FDC1004_READ_MEASUREMENT_MODE = 0X21,
};

/**@brief MSB Measurement Register Structure. This structure contains all values read from the MSB Measurement Register.
* Records the most significant 16 bits of measurement from channel 'n'. Stores values in an uint8_t array. 
* Register values: 0x00, 0x02, 0x04, 0x06
* Values are stored in bits: [15:0]
*/
struct FDC1004_MSB_Measurement_Register_Struct
{
    uint8_t register_pointer[4];                    /**< Array to store register pointers */
    uint8_t msb_measurement[4][2];                  /**< Temporary variable to record the MSB of measurement for Channel #1-4 */
};

/**@brief LSB Measurement Register Structure. This structure contains all values read from the LSB Measurement Register.
* Records the least significant 16 bits of measurement from channel 'n'. Stores values in an uint8_t array
* [15:8] least significant 8 bits of Measurement 'n'; [7:0] Reserved
* Register values: 0x01, 0x03, 0x05, 0x07
* Values are stored in bits: [15:8]
* Reserved, always 0 (read only): [7:0]
*/
struct FDC1004_LSB_Measurement_Register_Struct
{
    uint8_t register_pointer[4];                    /**< Array to store register pointers */
    uint8_t lsb_measurement[4];                  /**< Temporary variable to record the LSB of measurement for Channel #1-4 */
};


/**@brief Measurement Configuration Register Structure. This structure contains all values to configure the input channels and
* and CAPDAC settings for a measurement.
* Register values: 0x08, 0x09, 0x0A, 0x0B
*/
struct FDC1004_Measurement_Configuration_Register_Struct
{
    uint8_t register_pointer[4];                    /**< Array to store register pointers */
    uint8_t cha[4];                                 /**< Positive Input Channel for Capacitive to Digital Converter for Channel #1-4 */
    uint8_t chb[4];                                 /**< Negative Input Channel for Capacitive to Digital Converter for Channel #1-4 */
    uint8_t capdac[4];                              /**< Offset Capacitance for Channel #1-4 */
};

/**@brief FDC Configuration Register Structure. This structure configures measurement triggering and reports measurement completion.
* FDC Configuration register = 0x0C
* 0 = Measurement n disabled | Measurement n not completed
* 1 = Measurement n enabled | Measurement n completed
*/
struct FDC1004_FDC_Configuration_Register_Struct
{
    uint8_t register_pointer;                       /**< Pointer Address to Register */
    uint8_t rst;                                    /**< Reset */
    uint8_t rate;                                   /**< Measurement Rate */
    uint8_t repeat;                                 /**< Repeat Measurements */
    uint8_t meas[4];                                /**< Initiate Measurements for Channel #1-4 */
    uint8_t done[4];                                /**< Initiate Measurements for Channel #1-4 */
};

/**@brief Offset Calibration Register Structure. This structure contains a value in the range of -16 pF to 16 pF that can be added to each 
* channel in order to remove parasitic capacitance due to external circuitry. The CAPDAC is used for larger offset by using the CAPDAC up to
* 100 pF. These 16-bit registers are formatted as a fixed point number, the first 5 bits represent the integer portion of the capacitance in
* Two's complement format, and the remaining 11 bits represent the fractional portion of the capacitance. [0x0D, 0x0E, 0x0F, 0x10]
* [15:11] Integer portion of the Offset Calibration of Channel CINn
* [10:0] Decimal portion of the Offset Calibration of Channel CINn
* Register values: 0x0D, 0x0E, 0x0F, 0x10
* Integer part: [15:11] Integer portion of the Offset Calibration of Channel CINn
* Decimal part: [10:0] Decimal portion of the Offset Calibration of Channel CINn
*/
struct FDC1004_Offset_Calibration_Register_Struct
{
    uint8_t register_pointer[4];                    /**< Array to store register pointers */
    uint8_t integer[4];                          /**< Integer portion: Offset Calibration for Channel #1-4 */
    uint8_t decimal[4][2];                          /**< Decimal portion: Offset Calibration for Channel #1-4 */
};

/**@brief Gain Calibration Register Structure. Gain factor correction in the range of 0 - 4 that can be applied to each channel in order to 
* remove gain mismatch due to the external circuitry. This 16-bit register is formatted as a fixed point number, where the 2 MSBs fo the
* GAIN_CALn register corresponds to an integer portion of the gain correction, and the remaining 14 bits represent the fractional portion of
* the gain correction. 
* Gain = GAIN_CAL[15:0]/2^14 
* [0x11, 0x12, 0x13, 0x14]
* [15:14] Integer portion of the Gain Calibration of Channel CINn
* [13:0] Decimal portion of the Gain Calibration of Channel CINn
*/
struct FDC1004_Gain_Calibration_Register_Struct
{
    uint8_t register_pointer[4];                    /**< Array to store register pointers */
    uint8_t integer[4];                          /**< Integer portion: Gain Calibration for Channel #1-4 */
    uint8_t decimal[4][2];                          /**< Decimal portion: Gain Calibration for Channel #1-4 */
};

/**@brief Control Struct to store values used in between functions and in the class
*/
struct FDC1004_Control_Struct
{
    uint8_t slave_address;                          /**< Variable to store the slave address of the device */
    uint8_t register_pointer;                       /**< Variable to store the pointer to the register */
    uint8_t register_byte_count;                    /**< Variable to number of registers to read from */
    uint8_t i2c_data[2];                            /**< Variable to store data array to read and write */

    uint8_t manufacturer_id_register_pointer;       /**< Variable to record the Manufacturer ID register pointer */
    uint8_t device_id_register_pointer;             /**< Variable to record the Device ID register pointer */
    
    uint8_t manufacturer_id[2];                     /**< Variable to record the manufacturer ID (0x5449) in an uint8_t array format */
    uint8_t device_id[2];                           /**< Variable to record the Device ID (0x1004) in an uint8_t array format */

    uint8_t cin_data[4][3];                         /**< Variable to record the raw capacitance value for Channel #1-4 */
    uint8_t cin_status[4];                          /**< Variable to track which inputs are enabled */
    ret_code_t error_code;                          /**< Variable to track errors */

    uint8_t status;                                 /**< Variable to determine if sensor is enabled or disabled */  
    uint8_t interrupt;                              /**< Variable to record when the data interrupt flag */
};

/* Public Functions */

void fdc1004_init(void);

void fdc1004_uninit(void);

void fdc1004_soft_reset(void);

uint8_t fdc1004_is_enabled(void);

void fdc1004_set_offset_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2);

void fdc1004_set_gain_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2);

void fdc1004_set_measurement_rate(uint8_t measurement_rate);

void fdc1004_set_repeat_measurement(uint8_t repeat_measurement);

void fdc1004_get_manufacturer_id(uint8_t *manufacturer_id);

void fdc1004_get_device_id(uint8_t *device_id);

void fdc1004_set_capdac(uint8_t channel, uint8_t capdac);

void fdc1004_enable_channel(uint8_t channel);

void fdc1004_disable_channel(uint8_t channel);

void fdc1004_get_measurement(uint8_t channel, uint8_t *measurement);

void fdc1004_interrupt_handler(void);

void fdc1004_start_data_collection(void);

void fdc1004_stop_data_collection(void);

/* Static Functions */

static void _fdc1004_read_msb_measurement_register(uint8_t channel);

static void _fdc1004_read_lsb_measurement_register(uint8_t channel);

static void _fdc1004_read_measurement_configuration_register(void);

static void _fdc1004_write_measurement_configuration_register(void);

static void _fdc1004_read_fdc_configuration_register(void);

static void _fdc1004_write_fdc_configuration_register(void);

static void _fdc1004_read_offset_calibration_register(void);

static void _fdc1004_write_offset_calibration_register(void);

static void _fdc1004_read_gain_calibration_register(void);

static void _fdc1004_write_gain_calibration_register(void);

static void _fdc1004_read_manufacturer_id_register(void);

static void _fdc1004_read_device_id_register(void);

static void _fdc1004_read_measurement(uint8_t channel);

static void _fdc1004_trigger_single_measurement(uint8_t channel);

#endif

#endif // __FDC1004_H