#include "fdc1004.h"

#if FDC1004

static struct FDC1004_Control_Struct control;
static struct FDC1004_MSB_Measurement_Register_Struct msb_measurement_register;
static struct FDC1004_LSB_Measurement_Register_Struct lsb_measurement_register;
static struct FDC1004_Measurement_Configuration_Register_Struct measurement_configuration_register;
static struct FDC1004_FDC_Configuration_Register_Struct fdc_configuration_register;
static struct FDC1004_Offset_Calibration_Register_Struct offset_calibration_register;
static struct FDC1004_Gain_Calibration_Register_Struct gain_calibration_register;

void fdc1004_init(void)
{
    NRF_LOG_INFO("fdc1004_init");

    // Writing register addresses to register array constants
    control.slave_address[0] = FDC1004_SLAVE_ADDRESS;
    msb_measurement_register.register_pointer[0] = FDC1004_MEAS1_MSB;
    msb_measurement_register.register_pointer[1] = FDC1004_MEAS2_MSB;
    msb_measurement_register.register_pointer[2] = FDC1004_MEAS3_MSB;
    msb_measurement_register.register_pointer[3] = FDC1004_MEAS4_MSB;

    lsb_measurement_register.register_pointer[0] = FDC1004_MEAS1_LSB;
    lsb_measurement_register.register_pointer[1] = FDC1004_MEAS2_LSB;
    lsb_measurement_register.register_pointer[2] = FDC1004_MEAS3_LSB;
    lsb_measurement_register.register_pointer[3] = FDC1004_MEAS4_LSB;

    measurement_configuration_register.register_pointer[0] = FDC1004_CONF_MEAS1;
    measurement_configuration_register.register_pointer[1] = FDC1004_CONF_MEAS2;
    measurement_configuration_register.register_pointer[2] = FDC1004_CONF_MEAS3;
    measurement_configuration_register.register_pointer[3] = FDC1004_CONF_MEAS4;
    
    fdc_configuration_register.register_pointer[0] = FDC1004_FDC_CONF; 

    offset_calibration_register.register_pointer[0] = FDC1004_OFFESET_CAL_CIN1;
    offset_calibration_register.register_pointer[1] = FDC1004_OFFESET_CAL_CIN2;
    offset_calibration_register.register_pointer[2] = FDC1004_OFFESET_CAL_CIN3;
    offset_calibration_register.register_pointer[3] = FDC1004_OFFESET_CAL_CIN4;

    gain_calibration_register.register_pointer[0] = FDC1004_GAIN_CAL_CIN1;
    gain_calibration_register.register_pointer[1] = FDC1004_GAIN_CAL_CIN2;
    gain_calibration_register.register_pointer[2] = FDC1004_GAIN_CAL_CIN3;
    gain_calibration_register.register_pointer[3] = FDC1004_GAIN_CAL_CIN4;

    control.manufacturer_id_register_pointer[0] = FDC1004_MANUFACTURER_ID;
    control.device_id_register_pointer[0] = FDC1004_DEVICE_ID;

    control.register_byte_count[0] = 2;   // Each register counts 2 bytes

    fdc1004_soft_reset();              // Soft Reset
    
    // MSB Measurement Configuration Register
    fdc1004_read_measurement_configuration_register();
    measurement_configuration_register.cha[0] = 0;        // Assign Positive Channel CDC of Measurement Channel #1 to CIN1
    measurement_configuration_register.cha[1] = 1;        // Assign Positive Channel CDC of Measurement Channel #2 to CIN2
    measurement_configuration_register.cha[2] = 2;        // Assign Positive Channel CDC of Measurement Channel #3 to CIN3
    measurement_configuration_register.cha[3] = 3;        // Assign Positive Channel CDC of Measurement Channel #4 to CIN4
    measurement_configuration_register.chb[0] = 5;        // Disable Negative Channel CDC of Measurement Channel #1
    measurement_configuration_register.chb[1] = 5;        // Disable Negative Channel CDC of Measurement Channel #2
    measurement_configuration_register.chb[2] = 5;        // Disable Negative Channel CDC of Measurement Channel #3
    measurement_configuration_register.chb[3] = 5;        // Disable Negative Channel CDC of Measurement Channel #4
    measurement_configuration_register.capdac[0] = 0;     // Setting the Offset Capacitance to 0 pF for Measurement Channel #1
    measurement_configuration_register.capdac[1] = 0;     // Setting the Offset Capacitance to 0 pF for Measurement Channel #2
    measurement_configuration_register.capdac[2] = 0;     // Setting the Offset Capacitance to 0 pF for Measurement Channel #3
    measurement_configuration_register.capdac[3] = 0;     // Setting the Offset Capacitance to 0 pF for Measurement Channel #4
    fdc1004_write_measurement_configuration_register();
     fdc1004_read_measurement_configuration_register();

    // FDC Configuration Register
    fdc1004_read_fdc_configuration_register();
    fdc_configuration_register.rst = 0;                   // Normal Operation
    fdc_configuration_register.rate = 1;                  // Measurement Rate = 100 Samples per Second 
    fdc_configuration_register.repeat = 0;                // Repeat disabled
    fdc_configuration_register.meas[0] = 0;               // Measurement Channel #1 Disabled
    fdc_configuration_register.meas[1] = 0;               // Measurement Channel #2 Disabled
    fdc_configuration_register.meas[2] = 0;               // Measurement Channel #3 Disabled
    fdc_configuration_register.meas[3] = 0;               // Measurement Channel #4 Disabled
    fdc1004_write_fdc_configuration_register();
    fdc1004_read_fdc_configuration_register();


    // Offset Calibration Register
    fdc1004_read_offset_calibration_register();
    offset_calibration_register.integer[0][1] = 0;        // Integer part set to 0 pF for Measurement Channel #1
    offset_calibration_register.integer[1][1] = 0;        // Integer part set to 0 pF for Measurement Channel #2
    offset_calibration_register.integer[2][1] = 0;        // Integer part set to 0 pF for Measurement Channel #3
    offset_calibration_register.integer[3][1] = 0;        // Integer part set to 0 pF for Measurement Channel #4
    offset_calibration_register.decimal[0][0] = 0;        // Decimal part set to 0 pF for Measurement Channel #1
    offset_calibration_register.decimal[0][1] = 0;        // Decimal part set to 0 pF for Measurement Channel #1
    offset_calibration_register.decimal[1][0] = 0;        // Decimal part set to 0 pF for Measurement Channel #2
    offset_calibration_register.decimal[1][1] = 0;        // Decimal part set to 0 pF for Measurement Channel #2
    offset_calibration_register.decimal[2][0] = 0;        // Decimal part set to 0 pF for Measurement Channel #3
    offset_calibration_register.decimal[2][1] = 0;        // Decimal part set to 0 pF for Measurement Channel #3
    offset_calibration_register.decimal[3][0] = 0;        // Decimal part set to 0 pF for Measurement Channel #4
    offset_calibration_register.decimal[3][1] = 0;        // Decimal part set to 0 pF for Measurement Channel #4
    fdc1004_write_offset_calibration_register();
    fdc1004_read_offset_calibration_register();


    // Gain Calibration Register
    fdc1004_read_gain_calibration_register();
    gain_calibration_register.integer[0][1] = 0;          // Integer part of the gain set to 0 for Measurement Channel #1
    gain_calibration_register.integer[1][1] = 0;          // Integer part of the gain set to 0 for Measurement Channel #2
    gain_calibration_register.integer[2][1] = 0;          // Integer part of the gain set to 0 for Measurement Channel #3
    gain_calibration_register.integer[3][1] = 0;          // Integer part of the gain set to 0 for Measurement Channel #4
    gain_calibration_register.decimal[0][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #1
    gain_calibration_register.decimal[0][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #1
    gain_calibration_register.decimal[1][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #2
    gain_calibration_register.decimal[1][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #2
    gain_calibration_register.decimal[2][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #3
    gain_calibration_register.decimal[2][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #3
    gain_calibration_register.decimal[3][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #4
    gain_calibration_register.decimal[3][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #4
    fdc1004_write_gain_calibration_register();
    fdc1004_read_gain_calibration_register();

    control.interrupt = 0;    // Initializing the interrupt to 0 to start out
    control.status = 1;       // Setting the status of this sensor to enabled
}

void fdc1004_uninit(void)
{
    NRF_LOG_INFO("fdc1004_uninit");
    control.status = 0;
}

uint8_t fdc1004_check_status(void)
{
    NRF_LOG_INFO("fdc1004_check_status");
    return control.status;
}

void fdc1004_soft_reset(void)
{
    NRF_LOG_INFO("fdc1004_soft_reset");
    fdc1004_read_measurement_configuration_register();
    fdc_configuration_register.rst = 1;                   // Software reset to initiate a device reset. After reset, value will return to 0
    fdc1004_write_measurement_configuration_register();
    fdc1004_read_measurement_configuration_register();
}


/* 
* Read MSB Measurement Register to read the following values:
* Register address: 0x00, 0x02, 0x04, 0x06
* D[15:0], MSB_MEASn = Most significant 16 bits of Measurement n (read only) 
*/
static void fdc1004_read_msb_measurement_register(void)
{
    NRF_LOG_INFO("fdc1004_read_msb_measurement_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(msb_measurement_register.register_pointer); i++)
    {
        i2c_read_registers(control.slave_address[0], msb_measurement_register.register_pointer[i], control.data_array, control.register_byte_count[0]);
        msb_measurement_register.msb_measurement[i][0] = control.data_array[0];
        msb_measurement_register.msb_measurement[i][1] = control.data_array[1];
    }
}

/* 
* Read LSB Measurement Register to read the following values:
* Register address: 0x01, 0x03, 0x05, 0x07
* D[15:8], LSB_MEASn = Least significant 8 bits of Measurement n (read only)
* D[7:0], RESERVED, always 0 
*/
static void fdc1004_read_lsb_measurement_register(void)
{
    NRF_LOG_INFO("fdc1004_read_lsb_measurement_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(lsb_measurement_register.register_pointer); i++)
    {
        i2c_read_registers(control.slave_address[0], lsb_measurement_register.register_pointer[i], control.data_array, control.register_byte_count[0]);
        lsb_measurement_register.lsb_measurement[i][0] = control.data_array[0];
    }
}

/**@brief Measurement Configuration Register Structure. This structure contains all values to configure the input channels and
* and CAPDAC settings for a measurement.
* Register address: 0x08, 0x09, 0x0A, 0x0B
* D[15:13], CHA = positive input channel capacitance to digital converter
* D[12:10], CHB = negative input channel capacitance to digital converter
* D[9:5], CAPDAC = Offset Capacitance; Configure the single-ended measurement capacitince offset: C_offset = CAPDAC x 3.125 pF
* D[4:0], RESERVED, Always 0 (read only)
*/
static void fdc1004_read_measurement_configuration_register(void)
{
    NRF_LOG_INFO("fdc1004_read_measurement_configuration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(measurement_configuration_register.register_pointer); i++)
    {
        i2c_read_registers(control.slave_address[0], measurement_configuration_register.register_pointer[i], control.data_array, control.register_byte_count[0]);
        measurement_configuration_register.cha[i] = (control.data_array[0] & 0b11100000) >> 5;
        measurement_configuration_register.chb[i] = (control.data_array[0] & 0b00011100) >> 2;
        measurement_configuration_register.capdac[i] = ((control.data_array[0] & 0b00000011) << 3) | ((control.data_array[1] & 0b11100000) >> 5);
    }
}

static void fdc1004_write_measurement_configuration_register(void)
{
    NRF_LOG_INFO("fdc1004_write_measurement_configuration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(measurement_configuration_register.register_pointer); i++)
    {
        control.data_array[0] = (measurement_configuration_register.cha[i] << 5) | (measurement_configuration_register.chb[i] << 2);
        control.data_array[0] = ((measurement_configuration_register.capdac[i] & 0b00011000) >> 3) | control.data_array[0];
        control.data_array[1] = (measurement_configuration_register.capdac[i] & 0b00000111) << 5;

        i2c_write_registers(control.slave_address[0], control.data_array, control.register_byte_count[0]);
    }
}


/**@brief FDC Configuration Register Structure. This register configures measurement triggering and reports measurement completion
* and CAPDAC settings for a measurement.
* Register address: 0x0C
* D[15], RST = positive input channel capacitance to digital converter
* D[14:12], RESERVED, Always 0 (read only)
* D[11:10], RATE = Offset Capacitance; Configure the single-ended measurement capacitince offset: C_offset = CAPDAC x 3.125 pF
* D[9], RESERVED, Always 0 (read only)
* D[8], REPEAT = Repeat Measurement
* D[7], MEAS_1 = Initiate Measurement
* D[6], MEAS_2 = Initiate Measurement
* D[5], MEAS_3 = Initiate Measurement
* D[4], MEAS_4 = Initiate Measurement
* D[3], DONE_1 = Measurement Complete
* D[2], DONE_2 = Measurement Complete
* D[1], DONE_3 = Measurement Complete
* D[0], DONE_4 = Measurement Complete
*/
static void fdc1004_read_fdc_configuration_register(void)
{
    NRF_LOG_INFO("fdc1004_read_fdc_configuration_register");

    i2c_read_registers(control.slave_address[0], fdc_configuration_register.register_pointer[0], control.data_array, control.register_byte_count[0]);
    
    fdc_configuration_register.rst = (control.data_array[0] & 0b10000000) && 0b10000000;
    fdc_configuration_register.rate = (control.data_array[0] & 0b00001100) >> 2;
    fdc_configuration_register.repeat = (control.data_array[0] & 0b00000001) && 0b00000001;
    
    fdc_configuration_register.meas[0] = (control.data_array[1] & 0b10000000) && 0b10000000;
    fdc_configuration_register.meas[1] = (control.data_array[1] & 0b01000000) && 0b01000000;
    fdc_configuration_register.meas[2] = (control.data_array[1] & 0b00100000) && 0b00100000;
    fdc_configuration_register.meas[3] = (control.data_array[1] & 0b00010000) && 0b00010000;

    fdc_configuration_register.done[0] = (control.data_array[1] & 0b00001000) && 0b00001000;
    fdc_configuration_register.done[1] = (control.data_array[1] & 0b00000100) && 0b00000100;
    fdc_configuration_register.done[2] = (control.data_array[1] & 0b00000010) && 0b00000010;
    fdc_configuration_register.done[3] = (control.data_array[1] & 0b00000001) && 0b00000001;
}

static void fdc1004_write_fdc_configuration_register(void)
{
    NRF_LOG_INFO("fdc1004_write_configuration_register");

    control.data_array[0] = (fdc_configuration_register.rst << 7) | (fdc_configuration_register.rate << 2) | fdc_configuration_register.repeat;
    
    for(uint8_t i = 0; i < ARRAY_LENGTH(fdc_configuration_register.done); i++)
    {
        control.data_array[1] = (fdc_configuration_register.meas[i] << (7-i)) | control.data_array[1];
        control.data_array[1] = (fdc_configuration_register.done[i] << (3-i)) | control.data_array[1];
    }
    i2c_write_registers(control.slave_address[0], control.data_array, control.register_byte_count[0]);
}

/* 
* Read Offset Calibration Register to read the digitized capacitance value in the range of -16 pF to 16 pF that can be added to each channel
* in order to remove parasitic capacitance due to external circuitry. It is possible to support a larger offset by using the CAPDAC
* up to a value of 100 pF. These are formatted as a fixed point number, where the first 5 bits represent the integer portion of the
* capacitance in Two's complement format, and the remaining 11 bits represent the fractional portion of the capacitance. 
* Register address: 0x0D, 0x0E, 0x0F, 0x10
* D[15:11], OFFSET_CALn (Integer Part) = Integer portion of the Offset Calibration of Channel CINn
* D[10:0], OFFSET_CALn (Decimal Part) = Decimal portion of the Offset Calibration of Channel CINn
*/
static void fdc1004_read_offset_calibration_register(void)
{
    NRF_LOG_INFO("fdc1004_read_offset_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(offset_calibration_register.register_pointer); i++)
    {
        i2c_read_registers(control.slave_address[0], offset_calibration_register.register_pointer[i], control.data_array, control.register_byte_count[0]);
        offset_calibration_register.integer[i][0] = 0;
        offset_calibration_register.integer[i][1] = (control.data_array[0] & 0b11111000) >> 3;
       
        offset_calibration_register.decimal[i][0] = (control.data_array[0] & 0b00000111);
        offset_calibration_register.decimal[i][1] = control.data_array[1];
    }
}

static void fdc1004_write_offset_calibration_register(void)
{
    NRF_LOG_INFO("fdc1004_write_offset_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(offset_calibration_register.register_pointer); i++)
    {
        control.data_array[0] = (offset_calibration_register.integer[i][1] << 3) | offset_calibration_register.decimal[i][0];
        control.data_array[1] = offset_calibration_register.decimal[i][1];

        i2c_write_registers(control.slave_address[0], control.data_array, control.register_byte_count[0]);
    }
}

/* 
* Read Gain Calibration Register to read the gain factor correction in the range of 0 to 4 that can be applied to each channel in order to
* remove gain mismatch due to the external circuitry. This 16-bit register is formatted as a fixed point number, where the 2 MSBs of the
* GAIN_CALn register correspond to an integer portion of the gain correction, and the remaining 14 bits represent the fractional portion of
* gain correction. Gain = GAIN_CAL[15:0]/2^14
* Register address: 0x11, 0x12, 0x13, 0x14
* D[15:14], GAIN_CALn (Integer Part) = Integer portion of the GAIN Calibration of Channel CINn
* D[13:0], GAIN_CALn (Decimal Part) = Decimal portion of the GAIN Calibration of Channel CINn
*/
static void fdc1004_read_gain_calibration_register(void)
{
    NRF_LOG_INFO("fdc1004_read_gain_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(gain_calibration_register.register_pointer); i++)
    {
        i2c_read_registers(control.slave_address[0], gain_calibration_register.register_pointer[i], control.data_array, control.register_byte_count[0]);
        gain_calibration_register.integer[i][0] = 0;
        gain_calibration_register.integer[i][1] = (control.data_array[0] & 0b11000000) >> 6;
       
        gain_calibration_register.decimal[i][0] = (control.data_array[0] & 0b00111111);
        gain_calibration_register.decimal[i][1] = control.data_array[1];
    }
}

static void fdc1004_write_gain_calibration_register(void)
{
    NRF_LOG_INFO("fdc1004_write_gain_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(gain_calibration_register.register_pointer); i++)
    {
        control.data_array[0] = (gain_calibration_register.integer[i][1] << 6) | gain_calibration_register.decimal[i][0];
        control.data_array[1] = gain_calibration_register.decimal[i][1];

        i2c_write_registers(control.slave_address[0], control.data_array, control.register_byte_count[0]);
    }
}


/* 
* Read Manufacturer ID Register. Factory-programmable identification value that identifies this device. The manufacturer ID reads 0x5449 
* Register address: 0xFE
* D[15:0], Manufacturer ID = Texas Instruments ID (read only)
*/
static void fdc1004_read_manufacturer_id_register(void)
{
    NRF_LOG_INFO("fdc1004_read_manufacturer_id_register");

    i2c_read_registers(control.slave_address[0], control.manufacturer_id_register_pointer[0], control.data_array, control.register_byte_count[0]);
    control.manufacturer_id[0] = control.data_array[0];
    control.manufacturer_id[1] = control.data_array[1];
}

/* 
* Read Device ID Register. Factory-programmable identification value that identifies this device as a FDC1004. The Device ID reads 0x1004 
* Register address: 0xFF
* D[15:0], Device ID = FDC1004 Device ID (read only)
*/
static void fdc1004_read_device_id_register(void)
{
    NRF_LOG_INFO("fdc1004_read_device_id_register");

    uint8_t temp_register_value[2];
    i2c_read_registers(control.slave_address[0], control.device_id_register_pointer[0], control.data_array, control.register_byte_count[0]);
    control.device_id[0] = control.data_array[0];
    control.device_id[1] = control.data_array[1];
}

void fdc1004_set_offset_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2)
{
    NRF_LOG_INFO("fdc1004_set_offset_calibration");
}

void fdc1004_set_gain_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2)
{
    NRF_LOG_INFO("fdc1004_set_gain_calibration");
}

void fdc1004_set_measurement_rate(uint8_t measurement_rate)
{
    NRF_LOG_INFO("fdc1004_set_measurement_rate");
}

void fdc1004_set_repeat_measurement(uint8_t repeat_measurement)
{
    NRF_LOG_INFO("fdc1004_set_repeat_measurement");
}

void fdc1004_get_manufacturer_id(void)
{
    NRF_LOG_INFO("fdc1004_get_manufacturer_id");
    fdc1004_read_manufacturer_id_register();
}

void fdc1004_get_device_id(void)
{
    NRF_LOG_INFO("fdc1004_get_device_id");
}

void fdc1004_set_capdac(uint8_t channel, uint8_t capdac)
{
    NRF_LOG_INFO("fdc1004_set_capdac");
}

void fdc1004_trigger_measurement(uint8_t channel)
{
    NRF_LOG_INFO("fdc1004_trigger_measurement");
}

void fdc1004_get_measurement(uint8_t channel)
{
    NRF_LOG_INFO("fdc1004_get_measurement");
}

#endif