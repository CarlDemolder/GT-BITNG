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


uint8_t nrf_drv_bmi_read_chip_id(void)
{
    m_xfer_done = false;
    uint8_t tx_data[1];
    tx_data[0] = BMI160_RA_CHIP_ID; // Register address 0x00, CHIP_ID
    uint8_t rx_data[1] = {0x00}; // empty response buffer
    // Write + Address:
    ret_code_t err_code = nrf_drv_twi_tx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, tx_data, sizeof(tx_data), false);
    APP_ERROR_CHECK(err_code);
    while (!m_xfer_done) 
    {
      __WFE();
    }
    m_xfer_done = false;
    // If ACK: send a read signal, 
    err_code = nrf_drv_twi_rx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, rx_data, sizeof(rx_data));
    APP_ERROR_CHECK(err_code);
    while (!m_xfer_done) 
    {
      __WFE();
    }
  // Expected vs actual output:
  NRF_LOG_INFO("[BMI160] WHO_AM_I: 0x%X (expected: 0xD1)\r\n", rx_data[0]);
  return rx_data[0];
}

uint8_t nrf_drv_bmi_read_single_register(uint8_t reg_addr) {
  m_xfer_done = false;
  uint8_t tx_data[1];
  tx_data[0] = reg_addr; // Register address 0x00, CHIP_ID
  uint8_t rx_data[1] = {0x00}; // empty response buffer
  // Write + Address:
  ret_code_t err_code = nrf_drv_twi_tx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);
  while (!m_xfer_done) {
    __WFE();
  }
  m_xfer_done = false;
  // If ACK: send a read signal, 
  err_code = nrf_drv_twi_rx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, rx_data, sizeof(rx_data));
  APP_ERROR_CHECK(err_code);
  while (!m_xfer_done) {
    __WFE();
  }
  // Log data:
//  NRF_LOG_INFO("[BMI160] Address: 0x%X, Output: 0x%X\r\n", reg_addr, rx_data[0]);
  return rx_data[0];
}

void bmi160_write_single_register(uint8_t register_address, uint8_t register_value) 
{
    uint8_t register_byte_count = 2;              // Register Address + Register Write Value
    
    /*BMI160 Register values to initialize sensor*/
    uint8_t bmi160_register_value[2]= {0};
    bmi160_register_value[0] = register_address;
    bmi160_register_value[1] = register_value;
    uint8_t *bmi160_register_value_pointer = bmi160_register_value;
    
    i2c_write(BMI160_I2C_ADDRESS_DEFAULT, bmi160_register_value_pointer, register_byte_count);
 
    nrf_delay_ms(1); // required delay between write commands.
    NRF_LOG_INFO("[BMI160] Register at address 0x%X adjusted to 0x%X. \r\n", register_address, register_value);
}

void bmi160_init_registers(void) 
{
    bmi160_write_single_register(0x40, 0x26); // 0x40 - ACC_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
    
    bmi160_write_single_register(0x41, 0x0C); // 0x41 - ACC_RANGE, 0b0000|1100 (RESERVED|+/-16g range)
    
    bmi160_write_single_register(0x42, 0x26); // 0x42 - GYR_CONF, 0b0|010|0110 (no undersampling, bandwidth for filter, output data rate Hz=25)
    
    bmi160_write_single_register(0x43, 0x00); // 0x43 - GYR_RANGE, 0b00000|000 (RESERVED|+/-2000deg range)

    // 0x44 - MAG_CONF

    bmi160_write_single_register(0x45, 0x88); // 0x45 - FIFO_DOWNS //downsampling
    
    bmi160_write_single_register(0x46, 0x80); // 0x46 - FIFO_CONFIG[0], fifo_water_mark
    
    bmi160_write_single_register(0x47, 0xC0); // 0x47 - FIFO_CONFIG[1], enable gyr, accel, disable header
    
    bmi160_write_single_register(0x50, 0x00); // 0x50 - INT_EN[0]
    
    bmi160_write_single_register(0x51, 0x10); // 0x51 - INT_EN[1], INT1 = DATA_READY 
    
    bmi160_write_single_register(0x52, 0x00); // 0x52 - INT_EN[2]
    
    bmi160_write_single_register(0x53, 0x0C); // 0x53 - INT_OUT_CTRL, disable int2, use only int1
    
    bmi160_write_single_register(0x54, 0x00); // 0x54 - INT_LATCH
     
    bmi160_write_single_register(0x55, 0x00); // 0x55 - INT_MAP[0]
    
    bmi160_write_single_register(0x56, 0x80); // 0x56 - INT_MAP[1] 
    
    bmi160_write_single_register(0x57, 0x00); // 0x57 - INT_MAP[2]
}

void bmi160_read_accel_gyro(uint8_t *accel_gyro_array_pointer) 
{

    NRF_LOG_INFO("BMI160 read the accelerometer and gyroscope");
    uint8_t tmp116_gnd_address = 0x48;                    //Slave Address for TMP116, ADD0 = GND
    uint8_t tmp116_temp_address = 0x00;                   //Temperature values stored in this Address of TMP116
    uint8_t i2c_temp_counts[2];
    uint8_t temp_register_byte_count = 2;                 //Number of bytes of buffer to read

    i2c_read(tmp116_gnd_address, tmp116_temp_address, i2c_temp_counts, temp_register_byte_count);
    uint16_t uint16_i2c_counts =  (i2c_temp_counts[0] << 8) | i2c_temp_counts[1];
    return uint16_i2c_counts;

    m_xfer_done = false;
    // Read 12 bytes:
    uint8_t start_address[1];
    start_address[0] = 0x0C; // Gyro data starts at 0x0C and ends at 0x11
    uint8_t rx_data[12];
    ret_code_t err_code = nrf_drv_twi_tx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, start_address, sizeof(start_address), false);
    APP_ERROR_CHECK(err_code);
    while (!m_xfer_done) {
      __WFE();
    }
    m_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, BMI160_I2C_ADDRESS_DEFAULT, &rx_data[0], sizeof(rx_data));
    APP_ERROR_CHECK(err_code);
    while (!m_xfer_done) {
      __WFE();
    }
  //  NRF_LOG_INFO("[BMI160] Data HexDump: \r\n");
  //  NRF_LOG_HEXDUMP_INFO(rx_data, sizeof(rx_data));
    memcpy_fast(&p_mpu->mpu_buffer[p_mpu->mpu_count], rx_data, sizeof(rx_data));
    p_mpu->mpu_count+=12;
}