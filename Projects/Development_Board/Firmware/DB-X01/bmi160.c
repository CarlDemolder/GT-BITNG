#include "bmi160.h"

#if BMI160

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

void bmi160_init(void)
{
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CMD, BMI160_CMD_SOFT_RESET); // Soft Reset
//
//    bmi160_init_registers();    // Configuring the registers on the BMI160: Accelerometer, Gyroscope, Magnetometer
//
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CMD, BMI160_CMD_START_FOC); // Start FOC
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CMD, BMI160_CMD_ACC_MODE_LOW_POWER); // Setting the ACC PMU for the accelerometer; 0x11 = Normal mode, 0x12 = low power mode
//    // Full operation mode: Accelerometer and Gyroscope; Low Power mode: Accelerometer
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CMD, BMI160_CMD_GYR_MODE_NORMAL); // Setting the ACC PMU for the gyroscope;
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CMD, BMI160_CMD_INT_RESET); // Reset interrupt engine
}

uint8_t bmi160_read_chip_id(void)
{
//    uint8_t bmi160_chip_id = i2c_read_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_CHIP_ID);    // Register address 0x00, CHIP_ID
//  
//    NRF_LOG_INFO("BMI160 Device ID: 0x%X (expected: 0xD1)\r\n", bmi160_chip_id);
//    return bmi160_chip_id;
}

void bmi160_init_registers(void) 
{
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_ACCEL_CONF, 0x26); // 0x40 - ACC_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_ACCEL_RANGE, 0x0C); // 0x41 - ACC_RANGE, 0b0000|1100 (RESERVED|+/-16g range)
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_GYRO_CONF, 0x26); // 0x42 - GYR_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_GYRO_RANGE, 0x00); // 0x43 - GYR_RANGE, 0b00000|000 (RESERVED|+/-2000deg range)
//
//    // 0x44 - MAG_CONF
//
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_FIFO_DOWNS, 0x88); // 0x45 - FIFO_DOWNS //downsampling
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_FIFO_CONFIG_0, 0x80); // 0x46 - FIFO_CONFIG[0], fifo_water_mark
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_FIFO_CONFIG_1, 0xC0); // 0x47 - FIFO_CONFIG[1], enable gyr, accel, disable header
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_EN_0, 0x00); // 0x50 - INT_EN[0]
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_EN_1, 0x10); // 0x51 - INT_EN[1], INT1 = DATA_READY 
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_EN_2, 0x00); // 0x52 - INT_EN[2]
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_OUT_CTRL, 0x0C); // 0x53 - INT_OUT_CTRL, disable int2, use only int1
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_LATCH, 0x00); // 0x54 - INT_LATCH
//     
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_MAP_0, 0x00); // 0x55 - INT_MAP[0]
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_MAP_1, 0x80); // 0x56 - INT_MAP[1] 
//    
//    i2c_write_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_INT_MAP_2, 0x00); // 0x57 - INT_MAP[2]
}

void bmi160_read_accel_gyro(uint8_t *data_array_pointer) 
{
//    uint8_t bmi160_flag = i2c_read_single_register(BMI160_I2C_ADDRESS_DEFAULT, BMI160_RA_STATUS);    // Register address: 0x1B, BMI160_RA_STATUS)
//    // Check if bits 7, 6 are set:
//    if ((bmi160_flag & 0xC0) == 0xC0) 
//    {
//        NRF_LOG_INFO("BMI160 read the accelerometer and gyroscope");
//        uint8_t device_address = BMI160_I2C_ADDRESS_DEFAULT;                    // I2C Device Slave Address for BMI160
//        uint8_t register_address = BMI160_RA_GYRO_X_L;                          // Register address to read data from the BMI160 chip, gyro data starts at 0x0C, ends at 0x11
//        uint8_t register_byte_count = 12;                                       // Number of bytes to read from registers
//        uint8_t register_data[12];
//        
//        /*
//        BMI160_RA_GYRO_X_L          0x0C
//        BMI160_RA_GYRO_X_H          0x0D
//        BMI160_RA_GYRO_Y_L          0x0E
//        BMI160_RA_GYRO_Y_H          0x0F
//        BMI160_RA_GYRO_Z_L          0x10
//        BMI160_RA_GYRO_Z_H          0x11
//        BMI160_RA_ACCEL_X_L         0x12
//        BMI160_RA_ACCEL_X_H         0x13
//        BMI160_RA_ACCEL_Y_L         0x14
//        BMI160_RA_ACCEL_Y_H         0x15
//        BMI160_RA_ACCEL_Z_L         0x16
//        BMI160_RA_ACCEL_Z_H         0x17
//        */
//        
//
//        i2c_read_registers(device_address, register_address, register_data, register_byte_count);
//
//    //    for(uint8_t i = 0; i < register_byte_count; i++)
//    //    {
//    //        data_array_pointer[i] = register_data[i];
//    //    }
//    }
}

#endif