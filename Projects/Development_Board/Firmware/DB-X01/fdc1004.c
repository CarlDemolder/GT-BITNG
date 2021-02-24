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
    control.slave_address = FDC1004_SLAVE_ADDRESS;
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
    
    fdc_configuration_register.register_pointer = FDC1004_FDC_CONF; 

    offset_calibration_register.register_pointer[0] = FDC1004_OFFSET_CAL_CIN1;
    offset_calibration_register.register_pointer[1] = FDC1004_OFFSET_CAL_CIN2;
    offset_calibration_register.register_pointer[2] = FDC1004_OFFSET_CAL_CIN3;
    offset_calibration_register.register_pointer[3] = FDC1004_OFFSET_CAL_CIN4;

    gain_calibration_register.register_pointer[0] = FDC1004_GAIN_CAL_CIN1;
    gain_calibration_register.register_pointer[1] = FDC1004_GAIN_CAL_CIN2;
    gain_calibration_register.register_pointer[2] = FDC1004_GAIN_CAL_CIN3;
    gain_calibration_register.register_pointer[3] = FDC1004_GAIN_CAL_CIN4;

    control.manufacturer_id_register_pointer = FDC1004_MANUFACTURER_ID;
    control.device_id_register_pointer = FDC1004_DEVICE_ID;

    control.cin_status[0] = 0;     // CIN1 is disabled
    control.cin_status[1] = 0;     // CIN2 is disabled
    control.cin_status[2] = 0;     // CIN3 is disabled
    control.cin_status[3] = 0;     // CIN4 is disabled

    control.register_byte_count = 2;   // Each register counts 2 bytes

    fdc1004_soft_reset();              // Soft Reset
    
    // MSB Measurement Configuration Register
    _fdc1004_read_measurement_configuration_register();
    measurement_configuration_register.cha[0] = 0x00;         // Assign Positive Channel CDC of Measurement Channel #1 to CIN1
    measurement_configuration_register.cha[1] = 0x01;         // Assign Positive Channel CDC of Measurement Channel #2 to CIN2
    measurement_configuration_register.cha[2] = 0x02;         // Assign Positive Channel CDC of Measurement Channel #3 to CIN3
    measurement_configuration_register.cha[3] = 0x03;         // Assign Positive Channel CDC of Measurement Channel #4 to CIN4
    measurement_configuration_register.chb[0] = 0x04;         // Disable Negative Channel CDC of Measurement Channel #1
    measurement_configuration_register.chb[1] = 0x04;         // Disable Negative Channel CDC of Measurement Channel #2
    measurement_configuration_register.chb[2] = 0x04;         // Disable Negative Channel CDC of Measurement Channel #3
    measurement_configuration_register.chb[3] = 0x04;         // Disable Negative Channel CDC of Measurement Channel #4
    measurement_configuration_register.capdac[0] = 0x00;      // Setting the Offset Capacitance to 0 pF for Measurement Channel #1
    measurement_configuration_register.capdac[1] = 0x00;      // Setting the Offset Capacitance to 0 pF for Measurement Channel #2
    measurement_configuration_register.capdac[2] = 0x00;      // Setting the Offset Capacitance to 0 pF for Measurement Channel #3
    measurement_configuration_register.capdac[3] = 0x00;      // Setting the Offset Capacitance to 0 pF for Measurement Channel #4
    _fdc1004_write_measurement_configuration_register();
    _fdc1004_read_measurement_configuration_register();

    // FDC Configuration Register
    _fdc1004_read_fdc_configuration_register();
    fdc_configuration_register.rst = 0x00;                    // Normal Operation
    fdc_configuration_register.rate = 0x03;                   // Measurement Rate = 400 Samples per Second; 100 samples per second per sensor if there are 4 sensors 
    fdc_configuration_register.repeat = 0x00;                 // Repeat disabled
    fdc_configuration_register.meas[0] = 0x00;                // Measurement Channel #1 Disabled
    fdc_configuration_register.meas[1] = 0x00;                // Measurement Channel #2 Disabled
    fdc_configuration_register.meas[2] = 0x00;                // Measurement Channel #3 Disabled
    fdc_configuration_register.meas[3] = 0x00;                // Measurement Channel #4 Disabled
    _fdc1004_write_fdc_configuration_register();
    _fdc1004_read_fdc_configuration_register();


    // Offset Calibration Register
    _fdc1004_read_offset_calibration_register();
    offset_calibration_register.integer[0] = 0x00;            // Integer part set to 0 pF for Measurement Channel #1
    offset_calibration_register.integer[1] = 0x00;            // Integer part set to 0 pF for Measurement Channel #2
    offset_calibration_register.integer[2] = 0x00;            // Integer part set to 0 pF for Measurement Channel #3
    offset_calibration_register.integer[3] = 0x00;            // Integer part set to 0 pF for Measurement Channel #4

    offset_calibration_register.decimal[0][0] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #1
    offset_calibration_register.decimal[0][1] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #1

    offset_calibration_register.decimal[1][0] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #2
    offset_calibration_register.decimal[1][1] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #2

    offset_calibration_register.decimal[2][0] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #3
    offset_calibration_register.decimal[2][1] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #3

    offset_calibration_register.decimal[3][0] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #4
    offset_calibration_register.decimal[3][1] = 0x00;         // Decimal part set to 0 pF for Measurement Channel #4
    _fdc1004_write_offset_calibration_register();
    _fdc1004_read_offset_calibration_register();


    // Gain Calibration Register
    _fdc1004_read_gain_calibration_register();
    gain_calibration_register.integer[0] = 0X01;          // Integer part of the gain set to 1 for Measurement Channel #1
    gain_calibration_register.integer[1] = 0X01;          // Integer part of the gain set to 1 for Measurement Channel #2
    gain_calibration_register.integer[2] = 0X01;          // Integer part of the gain set to 1 for Measurement Channel #3
    gain_calibration_register.integer[3] = 0X01;          // Integer part of the gain set to 1 for Measurement Channel #4

    gain_calibration_register.decimal[0][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #1
    gain_calibration_register.decimal[0][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #1

    gain_calibration_register.decimal[1][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #2
    gain_calibration_register.decimal[1][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #2

    gain_calibration_register.decimal[2][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #3
    gain_calibration_register.decimal[2][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #3

    gain_calibration_register.decimal[3][0] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #4
    gain_calibration_register.decimal[3][1] = 0;          // Decimal part of the gain set to 0 for Measurement Channel #4
    _fdc1004_write_gain_calibration_register();
    _fdc1004_read_gain_calibration_register();

    control.interrupt = 0;    // Disabling the interrupt to begin
    control.status = 1;       // Setting the status of this sensor to enabled
}

void fdc1004_uninit(void)
{
    NRF_LOG_INFO("fdc1004_uninit");
    control.status = 0;       // Disable the sensor on the board
    control.interrupt = 0;    // Disabling the interrupt and setting it to 0
}

/*
* Used to check if this sensor is enabled on the firmware. That way, the GUI knows what sensors are available on the hardware board. 
*/
uint8_t fdc1004_is_enabled(void)
{
    NRF_LOG_INFO("fdc1004_is_enabled");
    return control.status;
}

void fdc1004_soft_reset(void)
{
    NRF_LOG_INFO("fdc1004_soft_reset");
    _fdc1004_read_fdc_configuration_register();
    fdc_configuration_register.rst = 1;                   // Software reset to initiate a device reset. After reset, value will return to 0
    _fdc1004_write_fdc_configuration_register();
    _fdc1004_read_fdc_configuration_register();
}

void fdc1004_set_offset_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2)
{
    NRF_LOG_INFO("fdc1004_set_offset_calibration");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_offset_calibration_register();
    offset_calibration_register.integer[channel-1] = integer;
    offset_calibration_register.decimal[channel-1][0] = decimal_1;
    offset_calibration_register.decimal[channel-1][1] = decimal_2;
    _fdc1004_write_offset_calibration_register();
    _fdc1004_read_offset_calibration_register();

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_set_gain_calibration(uint8_t channel, uint8_t integer, uint8_t decimal_1, uint8_t decimal_2)
{
    NRF_LOG_INFO("fdc1004_set_gain_calibration");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_gain_calibration_register();
    gain_calibration_register.integer[channel-1] = integer;
    gain_calibration_register.decimal[channel-1][0] = decimal_1;
    gain_calibration_register.decimal[channel-1][1] = decimal_2;
    _fdc1004_write_gain_calibration_register();
    _fdc1004_read_gain_calibration_register();

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_set_measurement_rate(uint8_t measurement_rate)
{
    NRF_LOG_INFO("fdc1004_set_measurement_rate");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_measurement_configuration_register();
    fdc_configuration_register.rate = measurement_rate;                  // 1 = 100 S/s, 2 = 200 S/s, 3 = 400 S/s 
    _fdc1004_write_measurement_configuration_register();
    _fdc1004_read_measurement_configuration_register();

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_set_repeat_measurement(uint8_t repeat_measurement)
{
    NRF_LOG_INFO("fdc1004_set_repeat_measurement");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_measurement_configuration_register();
    fdc_configuration_register.repeat = repeat_measurement;               // 1 = repeat enabled, 0 = repeat disabled
    _fdc1004_write_measurement_configuration_register();
    _fdc1004_read_measurement_configuration_register();

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_get_manufacturer_id(uint8_t *manufacturer_id)
{
    NRF_LOG_INFO("fdc1004_get_manufacturer_id");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_manufacturer_id_register();
    manufacturer_id[0] = control.manufacturer_id[0];
    manufacturer_id[1] = control.manufacturer_id[1];

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_get_device_id(uint8_t *device_id)
{
    NRF_LOG_INFO("fdc1004_get_device_id");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_device_id_register();
    device_id[0] = control.device_id[0];
    device_id[1] = control.device_id[1];

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_set_capdac(uint8_t channel, uint8_t capdac)
{
    NRF_LOG_INFO("fdc1004_set_capdac");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    _fdc1004_read_measurement_configuration_register();
    measurement_configuration_register.capdac[channel-1] = capdac; // C_offset = CAPDAC x 3.125 pF; Single channel capacitance offset
    _fdc1004_write_measurement_configuration_register();
    _fdc1004_read_measurement_configuration_register();

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_enable_channel(uint8_t channel)
{
    NRF_LOG_INFO("fdc1004_enable_channel");

    control.cin_status[channel-1] = 1;
}

void fdc1004_disable_channel(uint8_t channel)
{
    NRF_LOG_INFO("fdc1004_disable_channel");

    control.cin_status[channel-1] = 0;
}

void fdc1004_get_measurement(uint8_t channel, uint8_t *measurement)
{
    NRF_LOG_INFO("fdc1004_get_measurement");

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
    state_handler(i2c_init_array_data); // Init TWIM Driver

    uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
    state_handler(i2c_enable_array_data); // Enable TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
    if(control.cin_status[channel-1] == 1)
    {       
        _fdc1004_trigger_single_measurement(channel);  // Trigger channel measurement
        nrf_delay_us(2500);
        _fdc1004_read_measurement(channel);
        measurement[0] = control.cin_data[channel-1][0];
        measurement[1] = control.cin_data[channel-1][1];
        measurement[2] = control.cin_data[channel-1][2];
    }
    else
    {
        NRF_LOG_INFO("FDC1004 channel is not enabled.");
    }

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
    state_handler(i2c_disable_array_data); // Disable TWIM Driver

    uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
    state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void fdc1004_interrupt_handler(void)
{
    NRF_LOG_INFO("fdc1004_interrupt_handler");
    
    if(control.interrupt == 1)
    {
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
        uint8_t i2c_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  
        state_handler(i2c_init_array_data); // Init TWIM Driver

        uint8_t i2c_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_ENABLE};  
        state_handler(i2c_enable_array_data); // Enable TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        for(uint8_t i = 0; i < ARRAY_LENGTH(control.cin_status); i++)
        {
            if(control.cin_status[i] == 1)
            {
                NRF_LOG_INFO("channel reading: %u", i);
                _fdc1004_trigger_single_measurement(i+1);  // Trigger channel measurement
                nrf_delay_us(2500);
                _fdc1004_read_measurement(i+1);     // Read Channel measurement
            }
            else
            {
                control.cin_data[i][0] = 0;
                control.cin_data[i][1] = 0;
            }
        }

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t i2c_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_DISABLE};  
        state_handler(i2c_disable_array_data); // Disable TWIM Driver

        uint8_t i2c_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_UNINIT};  
        state_handler(i2c_uninit_array_data); // Uninit TWIM Driver

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t ble_fdc1004_array_data[15] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_INSTANT_PRESSURE_CHAR_COMMAND, control.cin_data[0][0],
        control.cin_data[0][1], control.cin_data[0][2], control.cin_data[1][0], control.cin_data[1][1], control.cin_data[1][2],
        control.cin_data[2][0], control.cin_data[2][1], control.cin_data[2][2], control.cin_data[3][0], control.cin_data[3][1], control.cin_data[3][2]};  
        state_handler(ble_fdc1004_array_data); // Write the instant pressure data to GATT database
    }
}

void fdc1004_start_data_collection(void)
{
    NRF_LOG_INFO("fdc1004_start_data_collection");
    control.interrupt = 1;    // Enabling the interrupt

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t response_char_pressure_data_collection_started_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_PRESSURE_DATA_COLLECTION_STARTED};
    state_handler(response_char_pressure_data_collection_started_command); // Sending message to client that data collection has started
}

void fdc1004_stop_data_collection(void)
{
    NRF_LOG_INFO("fdc1004_stop_data_collection");
    control.interrupt = 0;    // Disabling the interrupt

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t response_char_pressure_data_collection_finished_command[7] = {0X00, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 0x00, 
    0x00, 0x00, BLUETOOTH_RESPONSE_CHAR_PRESSURE_DATA_COLLECTION_FINISHED};
    state_handler(response_char_pressure_data_collection_finished_command); // Sending message to client that data collection is finished
}

/* Static Functions */

/* 
* Read MSB Measurement Register to read the following values:
* Register address: 0x00, 0x02, 0x04, 0x06
* D[15:0], MSB_MEASn = Most significant 16 bits of Measurement n (read only) 
*/
static void _fdc1004_read_msb_measurement_register(uint8_t channel)
{
    NRF_LOG_INFO("_fdc1004_read_msb_measurement_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &msb_measurement_register.register_pointer[channel-1], 1, control.i2c_data, 2);

    msb_measurement_register.msb_measurement[channel-1][0] = control.i2c_data[0];
    msb_measurement_register.msb_measurement[channel-1][1] = control.i2c_data[1];
}

/* 
* Read LSB Measurement Register to read the following values:
* Register address: 0x01, 0x03, 0x05, 0x07
* D[15:8], LSB_MEASn = Least significant 8 bits of Measurement n (read only)
* D[7:0], RESERVED, always 0 
*/
static void _fdc1004_read_lsb_measurement_register(uint8_t channel)
{
    NRF_LOG_INFO("_fdc1004_read_lsb_measurement_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &lsb_measurement_register.register_pointer[channel-1], 1, control.i2c_data, 2);

    lsb_measurement_register.lsb_measurement[channel-1] = control.i2c_data[0];
}

/**@brief Measurement Configuration Register Structure. This structure contains all values to configure the input channels and
* and CAPDAC settings for a measurement.
* Register address: 0x08, 0x09, 0x0A, 0x0B
* D[15:13], CHA = positive input channel capacitance to digital converter
* D[12:10], CHB = negative input channel capacitance to digital converter
* D[9:5], CAPDAC = Offset Capacitance; Configure the single-ended measurement capacitince offset: C_offset = CAPDAC x 3.125 pF
* D[4:0], RESERVED, Always 0 (read only)
*/
static void _fdc1004_read_measurement_configuration_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_measurement_configuration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(measurement_configuration_register.register_pointer); i++)
    {
        control.i2c_data[0] = 0;
        control.i2c_data[1] = 0;

        i2c_separate_write_read_register(control.slave_address, &measurement_configuration_register.register_pointer[i], 1, control.i2c_data, 2);    

        measurement_configuration_register.cha[i] = (control.i2c_data[0] & 0b11100000) >> 5;
        measurement_configuration_register.chb[i] = (control.i2c_data[0] & 0b00011100) >> 2;
        measurement_configuration_register.capdac[i] = ((control.i2c_data[0] & 0b00000011) << 3) | ((control.i2c_data[1] & 0b11100000) >> 5);
    }
}

static void _fdc1004_write_measurement_configuration_register(void)
{
    NRF_LOG_INFO("_fdc1004_write_measurement_configuration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(measurement_configuration_register.register_pointer); i++)
    {
        control.i2c_data[0] = (measurement_configuration_register.cha[i] << 5) | (measurement_configuration_register.chb[i] << 2);
        control.i2c_data[0] = ((measurement_configuration_register.capdac[i] & 0b00011000) >> 3) | control.i2c_data[0];
        control.i2c_data[1] = (measurement_configuration_register.capdac[i] & 0b00000111) << 5;

        uint8_t temp_data[3] = {measurement_configuration_register.register_pointer[i], control.i2c_data[0], control.i2c_data[1]};
        i2c_no_stop_write_register(control.slave_address, temp_data, 3);
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
static void _fdc1004_read_fdc_configuration_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_fdc_configuration_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &fdc_configuration_register.register_pointer, 1, control.i2c_data, 2);    

    fdc_configuration_register.rst = (control.i2c_data[0] & 0b10000000) && 0b10000000;
    fdc_configuration_register.rate = (control.i2c_data[0] & 0b00001100) >> 2;
    fdc_configuration_register.repeat = (control.i2c_data[0] & 0b00000001) && 0b00000001;
    
    fdc_configuration_register.meas[0] = (control.i2c_data[1] & 0b10000000) && 0b10000000;
    fdc_configuration_register.meas[1] = (control.i2c_data[1] & 0b01000000) && 0b01000000;
    fdc_configuration_register.meas[2] = (control.i2c_data[1] & 0b00100000) && 0b00100000;
    fdc_configuration_register.meas[3] = (control.i2c_data[1] & 0b00010000) && 0b00010000;

    fdc_configuration_register.done[0] = (control.i2c_data[1] & 0b00001000) && 0b00001000;
    fdc_configuration_register.done[1] = (control.i2c_data[1] & 0b00000100) && 0b00000100;
    fdc_configuration_register.done[2] = (control.i2c_data[1] & 0b00000010) && 0b00000010;
    fdc_configuration_register.done[3] = (control.i2c_data[1] & 0b00000001) && 0b00000001;
}

static void _fdc1004_write_fdc_configuration_register(void)
{
    NRF_LOG_INFO("_fdc1004_write_configuration_register");

    control.i2c_data[0] = (fdc_configuration_register.rst << 7) | (fdc_configuration_register.rate << 2) | fdc_configuration_register.repeat;
    
    for(uint8_t i = 0; i < ARRAY_LENGTH(fdc_configuration_register.done); i++)
    {
        control.i2c_data[1] = (fdc_configuration_register.meas[i] << (7-i)) | control.i2c_data[1];
        control.i2c_data[1] = (fdc_configuration_register.done[i] << (3-i)) | control.i2c_data[1];
    }
    uint8_t temp_data[3] = {fdc_configuration_register.register_pointer, control.i2c_data[0], control.i2c_data[1]};
    i2c_no_stop_write_register(control.slave_address, temp_data, 3);
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
static void _fdc1004_read_offset_calibration_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_offset_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(offset_calibration_register.register_pointer); i++)
    {
        control.i2c_data[0] = 0;
        control.i2c_data[1] = 0;

        i2c_separate_write_read_register(control.slave_address, &offset_calibration_register.register_pointer[i], 1, control.i2c_data, 2);

        offset_calibration_register.integer[i] = (control.i2c_data[0] & 0b11111000) >> 3;
       
        offset_calibration_register.decimal[i][0] = (control.i2c_data[0] & 0b00000111);
        offset_calibration_register.decimal[i][1] = control.i2c_data[1];
    }
}

static void _fdc1004_write_offset_calibration_register(void)
{
    NRF_LOG_INFO("_fdc1004_write_offset_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(offset_calibration_register.register_pointer); i++)
    {
        control.i2c_data[0] = (offset_calibration_register.integer[i] << 3) | offset_calibration_register.decimal[i][0];
        control.i2c_data[1] = offset_calibration_register.decimal[i][1];

        uint8_t temp_data[3] = {offset_calibration_register.register_pointer[i], control.i2c_data[0], control.i2c_data[1]};
        i2c_no_stop_write_register(control.slave_address, temp_data, 3);
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
static void _fdc1004_read_gain_calibration_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_gain_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(gain_calibration_register.register_pointer); i++)
    {
        control.i2c_data[0] = 0;
        control.i2c_data[1] = 0;

        i2c_separate_write_read_register(control.slave_address, &gain_calibration_register.register_pointer[i], 1, control.i2c_data, 2);

        gain_calibration_register.integer[i] = (control.i2c_data[0] & 0b11000000) >> 6;
       
        gain_calibration_register.decimal[i][0] = (control.i2c_data[0] & 0b00111111);
        gain_calibration_register.decimal[i][1] = control.i2c_data[1];
    }
}

static void _fdc1004_write_gain_calibration_register(void)
{
    NRF_LOG_INFO("_fdc1004_write_gain_calibration_register");

    for(uint8_t i = 0; i < ARRAY_LENGTH(gain_calibration_register.register_pointer); i++)
    {
        control.i2c_data[0] = (gain_calibration_register.integer[i] << 6) | gain_calibration_register.decimal[i][0];
        control.i2c_data[1] = gain_calibration_register.decimal[i][1];

        uint8_t temp_data[3] = {gain_calibration_register.register_pointer[i], control.i2c_data[0], control.i2c_data[1]};
        i2c_no_stop_write_register(control.slave_address, temp_data, 3);
    }
}

/* 
* Read Manufacturer ID Register. Factory-programmable identification value that identifies this device. The manufacturer ID reads 0x5449 
* Register address: 0xFE
* D[15:0], Manufacturer ID = Texas Instruments ID (read only)
*/
static void _fdc1004_read_manufacturer_id_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_manufacturer_id_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &control.manufacturer_id_register_pointer, 1, control.i2c_data, 2);

    control.manufacturer_id[0] = control.i2c_data[0];
    control.manufacturer_id[1] = control.i2c_data[1];
}

/* 
* Read Device ID Register. Factory-programmable identification value that identifies this device as a FDC1004. The Device ID reads 0x1004 
* Register address: 0xFF
* D[15:0], Device ID = FDC1004 Device ID (read only)
*/
static void _fdc1004_read_device_id_register(void)
{
    NRF_LOG_INFO("_fdc1004_read_device_id_register");

    control.i2c_data[0] = 0;
    control.i2c_data[1] = 0;

    i2c_separate_write_read_register(control.slave_address, &control.device_id_register_pointer, 1, control.i2c_data, 2);

    control.device_id[0] = control.i2c_data[0];
    control.device_id[1] = control.i2c_data[1];
}


static void _fdc1004_read_measurement(uint8_t channel)
{
    NRF_LOG_INFO("_fdc1004_read_measurement");
    _fdc1004_read_fdc_configuration_register();
    if(fdc_configuration_register.done[channel-1] == 1)
    {
        NRF_LOG_INFO("Channel: %u measurement is done", channel);
        _fdc1004_read_msb_measurement_register(channel);
        _fdc1004_read_lsb_measurement_register(channel);
        control.cin_data[channel-1][0] = msb_measurement_register.msb_measurement[channel-1][0];
        control.cin_data[channel-1][1] = msb_measurement_register.msb_measurement[channel-1][1];
        control.cin_data[channel-1][2] = lsb_measurement_register.lsb_measurement[channel-1];

        _fdc1004_read_fdc_configuration_register();
        if(fdc_configuration_register.done[channel-1] == 0)
        {
            NRF_LOG_INFO("Channel: %u measurement was properly read", channel);
        }
    }
    else
    {
        NRF_LOG_INFO("Channel: %u measurement is not complete", channel);
    }
}

static void _fdc1004_trigger_single_measurement(uint8_t channel)
{
    NRF_LOG_INFO("fdc1004_trigger_single_measurement");
    _fdc1004_read_fdc_configuration_register();
    /* 
    * 1. Set repeat = 0
    * 2. Sett the corresponding MEAS_x field to 1
    */
    for(uint8_t i = 0; i < ARRAY_LENGTH(fdc_configuration_register.meas); i++)
    {
        if(i == (channel-1))
        {
            fdc_configuration_register.meas[i] = 1;   // Enabling the measurement channel to initiate measurement
        }
        else
        {
            fdc_configuration_register.meas[i] = 0;   // Disabling all other measurement channels
        }
    }
    _fdc1004_write_fdc_configuration_register();
    _fdc1004_read_fdc_configuration_register();
}

#endif