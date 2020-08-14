#include "bmi160.h"

// Array of Default Registers:
uint8_t bmi160_default_registers[] = 
{
    DEFVAL_BMI160_ACC_RANGE,
    DEFVAL_BMI160_GYR_CONF,
    DEFVAL_BMI160_GYR_RANGE,
    DEFVAL_BMI160_MAG_CONF,
    DEFVAL_BMI160_FIFO_DOWNS,
    DEFVAL_BMI160_FIFO_CONFIG_0,
    DEFVAL_BMI160_FIFO_CONFIG_1,
    DEFVAL_BMI160_MAG_IF_0,
    DEFVAL_BMI160_MAG_IF_1,
    DEFVAL_BMI160_MAG_IF_2,
    DEFVAL_BMI160_MAG_IF_3,
    DEFVAL_BMI160_MAG_IF_4,
    DEFVAL_BMI160_INT_EN_0,
    DEFVAL_BMI160_INT_EN_1,
    DEFVAL_BMI160_INT_EN_2,
    DEFVAL_BMI160_INT_OUT_CTRL,
    DEFVAL_BMI160_INT_LATCH,
    DEFVAL_BMI160_INT_MAP_0,
    DEFVAL_BMI160_INT_MAP_1,
    DEFVAL_BMI160_INT_MAP_2,
    DEFVAL_BMI160_INT_DATA_0,
    DEFVAL_BMI160_INT_DATA_1,
    DEFVAL_BMI160_INT_LOWHIGH_0,
    DEFVAL_BMI160_INT_LOWHIGH_1,
    DEFVAL_BMI160_INT_LOWHIGH_2,
    DEFVAL_BMI160_INT_LOWHIGH_3,
    DEFVAL_BMI160_INT_LOWHIGH_4,
    DEFVAL_BMI160_INT_MOTION_0,
    DEFVAL_BMI160_INT_MOTION_1,
    DEFVAL_BMI160_INT_MOTION_2,
    DEFVAL_BMI160_INT_MOTION_3
};


uint8_t bmi160_read_chip_id(void)
{
    uint8_t bmi160_chip_id = i2c_read_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CHIP_ID);    // Register address 0x00, CHIP_ID

    NRF_LOG_INFO("BMI160 Device ID: 0x%X (expected: 0xD1)\r\n", bmi160_chip_id);
    return bmi160_chip_id;
}

void bmi160_init_registers(void) 
{
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x40, 0x26); // 0x40 - ACC_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x41, 0x0C); // 0x41 - ACC_RANGE, 0b0000|1100 (RESERVED|+/-16g range)
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x42, 0x26); // 0x42 - GYR_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x43, 0x00); // 0x43 - GYR_RANGE, 0b00000|000 (RESERVED|+/-2000deg range)

    // 0x44 - MAG_CONF

    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x45, 0x88); // 0x45 - FIFO_DOWNS //downsampling
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x46, 0x80); // 0x46 - FIFO_CONFIG[0], fifo_water_mark
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x47, 0xC0); // 0x47 - FIFO_CONFIG[1], enable gyr, accel, disable header
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x50, 0x00); // 0x50 - INT_EN[0]
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x51, 0x10); // 0x51 - INT_EN[1], INT1 = DATA_READY 
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x52, 0x00); // 0x52 - INT_EN[2]
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x53, 0x0C); // 0x53 - INT_OUT_CTRL, disable int2, use only int1
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x54, 0x00); // 0x54 - INT_LATCH
     
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x55, 0x00); // 0x55 - INT_MAP[0]
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x56, 0x80); // 0x56 - INT_MAP[1] 
    
    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, 0x57, 0x00); // 0x57 - INT_MAP[2]
}

void bmi160_read_accel_gyro(uint8_t *data_array_pointer) 
{
    NRF_LOG_INFO("BMI160 read the accelerometer and gyroscope");
    uint8_t device_address = BMI160_I2C_ADDRESS_DEFAULT;      // I2C Device Slave Address for BMI160
    uint8_t register_address = 0x0C;                          // Register address to read data from the BMI160 chip, gyro data starts at 0x0C, ends at 0x11
    uint8_t register_byte_count = 12;                          // Number of bytes to read from registers
    uint8_t register_data[register_byte_count];

    i2c_read(device_address, register_address, register_data, register_byte_count);

    NRF_LOG_INFO("BMI160 Hex Data Dump: \r\n");
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data));

//    for(uint8_t i = 0; i < register_byte_count; i++)
//    {
//        data_array_pointer[i] = register_data[i];
//    }
}