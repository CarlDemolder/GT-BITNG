#include "serial_slave.h"

void enable_serial_slave_handler(void)
{
    uint8_t log_array_data[4] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_LOG_INIT};       // Enable LOG Driver
    _nrf52_handler(log_array_data);

    uint8_t ldo_array_data[4] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_LDO_INIT};       // Enable LDO Drivers
    _nrf52_handler(ldo_array_data);

    uint8_t vcc_en_array_data[4] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_VCC_LDO_EN};   // Enable VCC LDO
    _nrf52_handler(vcc_en_array_data);  

    uint8_t power_manager_array_data[4] = {0x00, 0x00, NRF52_POWER_COMMAND, NRF52_POWER_MANAGER_INIT};  // Enable Power Manager
    _nrf52_handler(power_manager_array_data);

    uint8_t i2c_init_array_data[4] = {0x00, 0x00, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_INIT};  // Enable TWIM Driver
    _nrf52_handler(i2c_init_array_data);

    uint8_t i2c_start_array_data[4] = {0x00, 0x00, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_START};  // Start TWIM Driver
    _nrf52_handler(i2c_start_array_data);

    uint8_t led_array_data[4] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_LED_INIT};  // Enable LED Driver
    _nrf52_handler(led_array_data);

    uint8_t long_blink_led_array_data[5] = {0x00, 0x00, NRF52_COMMON_COMMAND, NRF52_LED_IND_BLINK, NRF52_LED_IND_LONG_BLINK};  // IND LED Long Blink
    _nrf52_handler(long_blink_led_array_data);
}

static void _disable_serial_slave_manager_handler(void)
{
    uint8_t i2c_stop_array_data[4] = {0x00, 0x00, NRF52_I2C_COMMAND, NRF52_I2C_TWIM_STOP};  // Stop TWIM Driver
    _nrf52_handler(i2c_stop_array_data);
}

void serial_slave_manager_handler(void)
{
    while(FT201X_USB_NORMAL == ft201x_check_usb_state())
    {
        uint8_t ft201x_data_bytes_available = ft201x_available();
        if(ft201x_data_bytes_available > 2)
        {
              uint8_t serial_array_data[ft201x_data_bytes_available];
              ft201x_read_buffer(serial_array_data, ft201x_data_bytes_available);

              if(serial_array_data[0] == USB_COMMAND_HEADER && serial_array_data[LAST_ARRAY_ELEMENT(serial_array_data)] == USB_COMMAND_FOOTER)
              {
                  switch(serial_array_data[1])
                  {
                      case NRF52_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: NRF52_MODULE");
                          _nrf52_handler(serial_array_data);
                          break;

                      case FT201X_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: FT201X_MODULE");
                          _ft201x_handler(serial_array_data);
                          break;

                      case BMI160_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: BMI160_MODULE");
                          _bmi160_handler(serial_array_data);
                          break;

                      case MAX30003_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: MAX30003_MODULE");
                          _max30003_handler(serial_array_data);
                          break;    

                      case TMP117_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: TMP117_MODULE");
                          _tmp117_handler(serial_array_data);
                          break;

                      case BLE_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: BLE_MODULE");
                          _ble_handler(serial_array_data);
                          break;

                      default:
                          ft201x_flush_buffers();
                          break;
                  }
              }
              else
              {
                  ft201x_flush_buffers();
              }
        }
        else
        {
            power_manager_handler();
        }
    }
    _disable_serial_slave_manager_handler();
}


void bluetooth_manager_handler(void)
{
    

}

void _nrf52_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case NRF52_HF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_HF_CLOCK_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_HF_CLOCK_START:  
                    hfclock_start();
                    break;

                case NRF52_HF_CLOCK_STOP:
                    hfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_LF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LF_CLOCK_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_LF_CLOCK_START:  
                    lfclock_start();
                    break;

                case NRF52_LF_CLOCK_STOP:
                    lfclock_stop();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_CLOCK_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_NRFX_CLOCK_DRIVER_INIT:  
                    nrf52_nrfx_clock_init();
                    break;

                case NRF52_NRFX_CLOCK_DRIVER_UNINIT:
                    nrf52_nrfx_clock_uninit();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_RTC_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_RTC_CLOCK_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_RTC_CONFIG:
                    rtc_config();
                    break;

                case NRF52_RTC_START:  
                    rtc_start();
                    break;

                case NRF52_RTC_STOP:
                    rtc_stop();
                    break;

                case NRF52_RTC_RESTART:  
                    rtc_restart();
                    break;

                case NRF52_RTC_SET_COUNTER:
                    rtc_set_counter(serial_array_data[4]);
                    break;

                default:
                    break;
            }
            break;

        case NRF52_POWER_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_POWER_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_POWER_DCDC_CONVERTER_ENABLE:
                    enable_dcdc_converter();
                    break;

                case NRF52_POWER_DCDC_CONVERTER_DISABLE:  
                    disable_dcdc_converter();
                    break;

                case NRF52_POWER_SLEEP_MODE_ENTER:
                    sleep_mode_enter();
                    break;

                case NRF52_POWER_DEEP_SLEEP_MODE_ENTER:  
                    deep_sleep_mode_enter();
                    break;

                case NRF52_POWER_MANAGER_HANDLER:  
                    power_manager_handler();
                    break;

                case NRF52_POWER_MANAGER_INIT:  
                    power_manager_init();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_COMMON_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_COMMON_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_LED_INIT:
                    init_leds();
                    break;

                case NRF52_LED_IND_LED_ON:  
                    ind_led_on();
                    break;

                case NRF52_LED_IND_LED_OFF:
                    ind_led_off();
                    break;

                case NRF52_LED_IND_BLINK: 
                    switch(serial_array_data[4])
                    {
                        case NRF52_LED_IND_CUSTOM_BLINK:
                        {
                            uint16_t ind_led_on_ms = (uint16_t)(((uint16_t)serial_array_data[5] << 8) | serial_array_data[6]);
                            uint16_t ind_led_off_ms = (uint16_t)(((uint16_t)serial_array_data[7] << 8) | serial_array_data[8]);
                            ind_led_blink(ind_led_on_ms, ind_led_off_ms);
                            break;
                        }         
                        case NRF52_LED_IND_SHORT_BLINK:
                            ind_led_short_blink();
                            break;
                               
                        case NRF52_LED_IND_MEDIUM_BLINK:
                            ind_led_medium_blink();
                            break;

                        case NRF52_LED_IND_LONG_BLINK:
                            ind_led_long_blink();
                            break;

                        default:
                            break;
                    }                                                                                       
                    break;

                case NRF52_LED_BLE_LED_ON:  
                    ble_led_on();
                    break;

                case NRF52_LED_BLE_LED_OFF:
                    ble_led_off();
                    break;

                case NRF52_LED_BLE_BLINK:
                {
                    uint16_t ble_led_on_ms = (uint16_t)(((uint16_t)serial_array_data[4] << 8) | serial_array_data[5]);
                    uint16_t ble_led_off_ms = (uint16_t)(((uint16_t)serial_array_data[6] << 8) | serial_array_data[7]);
                    ble_led_blink(ble_led_on_ms, ble_led_off_ms);
                    break;
                }
                case NRF52_LOG_INIT:
                    log_init();
                    break;

                case NRF52_LDO_INIT:
                    ldo_init();
                    break;

                case NRF52_VCC_LDO_EN:
                    enable_vcc_ldo();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_I2C_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_I2C_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_I2C_TWIM_INIT:
                    twim_init();
                    break;

                case NRF52_I2C_TWIM_STOP:
                    i2c_stop();
                    break;

                case NRF52_I2C_TWIM_START:
                    i2c_start();
                    break;

                default:
                    break;
            }
            break;

        case NRF52_SPI_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_SPIM_COMMAND");
            switch(serial_array_data[3])
            {
                case NRF52_SPI_SPIM_INIT:
                    spim_init();
                    break;

                case NRF52_SPI_SPIM_ENABLE:
                    spim_enable();
                    break;

                case NRF52_SPI_SPIM_DISABLE:
                    spim_disable();
                    break;
                
                case NRF52_SPI_SPIM_UNINIT:
                    spim_uninit();
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

static void _ft201x_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case FT201X_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_CHIP_ID_COMMAND");
            break;

        case FT201X_SET_VCP_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_SET_VCP_COMMAND");
            break;

        case FT201X_READ_EEPROM_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_EEPROM_COMMAND");
            break;

        case FT201X_WRITE_EEPROM_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_WRITE_EEPROM_COMMAND");
            break;

        case FT201X_WRITE_DATA_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_WRITE_DATA_COMMAND");
            break;
        
        case FT201X_READ_DATA_COMMAND:
            NRF_LOG_INFO("FT201X_MODULE: FT201X_READ_DATA_COMMAND");
            break;

        default:
            break;
    }
}

static void _bmi160_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case BMI160_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_READ_CHIP_ID_COMMAND");
            break;

        case BMI160_INIT_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_INIT_COMMAND");
            break;

        case BMI160_READ_ACCEL_GYRO_COMMAND:
            NRF_LOG_INFO("BMI160_MODULE: BMI160_READ_ACCEL_GYRO_COMMAND");
            break;

        default:
            break;
    }
}

static void _tmp117_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case TMP117_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("tmp117_MODULE: TMP117_READ_CHIP_ID_COMMAND");
            uint16_t tmp117_chip_id = tmp117_read_chip_id();
            uint8_t tmp117_chip_id_array_data[5] = {0x00, 0x00, FT201X_WRITE_DATA_COMMAND, (tmp117_chip_id & 0xF0), (tmp117_chip_id & 0x0F)};  // Write TMP117 Chip ID
            _ft201x_handler(tmp117_chip_id_array_data);
            break;

        case TMP117_READ_REVISION_NUMBER_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_READ_CHIP_ID_COMMAND");
            uint8_t tmp117_revision_number = tmp117_read_revision_number();
            uint8_t tmp117_revision_number_array_data[4] = {0x00, 0x00, FT201X_WRITE_DATA_COMMAND, tmp117_chip_id};  // Write TMP117 Chip ID
            _ft201x_handler(tmp117_revision_number_array_data);
            break;

        case TMP117_UNLOCK_EEPROM_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_UNLOCK_EEPROM_COMMAND");
            tmp117_unlock_eeprom();
            break;

        case TMP117_INIT_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_INIT_COMMAND");
            tmp117_init(serial_array_data[3], serial_array_data[4]);
            break;

        case TMP117_SET_OPERATING_MODE_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_SET_OPERATING_MODE_COMMAND");
            tmp117_set_operating_mode(serial_array_data[3], serial_array_data[4]);
            break;

        case TMP117_GENERAL_CALL_RESET_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_GENERAL_CALL_RESET_COMMAND");   
            tmp117_general_call_reset();
            break;

        case TMP117_TEMP_UINT16_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_UINT16_COMMAND");
            uint16_t tmp117_temp_value = tmp117_get_uint16_t();
            uint8_t tmp117_temp_value_array_data[5] = {0x00, 0x00, FT201X_WRITE_DATA_COMMAND, (tmp117_temp_value & 0xF0), (tmp117_temp_value & 0x0F)};  // Write TMP117 Temperature Value
            _ft201x_handler(tmp117_temp_value_array_data);             
            break;

        case TMP117_TEMP_CHAR_ARRAY_COMMAND:
            NRF_LOG_INFO("TMP117_MODULE: TMP117_TEMP_CHAR_ARRAY_COMMAND");    
            uint8_t tmp117_uint8_t[5];
            tmp117_get_uint8_t(tmp117_uint8_t);
            uint8_t tmp117_uint8_t_array_data[8] = {0x00, 0x00, FT201X_WRITE_DATA_COMMAND, tmp117_uint8_t[4], tmp117_uint8_t[3],
            tmp117_uint8_t[2], tmp117_uint8_t[1], tmp117_uint8_t[0]};  // Write TMP117 Temperature Value
            _ft201x_handler(tmp117_uint8_t_array_data);           
            break;

        default:
            break;
    }
}


static void _max30003_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case MAX30003_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_READ_CHIP_ID_COMMAND");            
            break;

        case MAX30003_INIT_COMMAND:
            NRF_LOG_INFO("MAX30003_MODULE: MAX30003_INIT_COMMAND");
            break;

        default:
            break;
    }
}

static void _ble_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case BLE_INIT_GAP_PARAMS_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_GAP_PARAMS_COMMAND");            
            break;

        case BLE_UPDATE_GAP_PARAMS_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_UPDATE_GAP_PARAMS_COMMAND");
            break;

        case BLE_INIT_GATT_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_GATT_COMMAND");
            break;

        case BLE_INIT_SERVICES_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_SERVICES_COMMAND");
            break;

        case BLE_INIT_CONN_PARAMS_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_CONN_PARAMS_COMMAND");    
            break;

        case BLE_INIT_PEER_MANAGER_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_PEER_MANAGER_COMMAND");    
            break;

        case BLE_INIT_ADVERTISING_COMMAND:
            NRF_LOG_INFO("BLE_MODULE: BLE_INIT_ADVERTISING_COMMAND");    
            break;

        default:
            break;
    }
}


