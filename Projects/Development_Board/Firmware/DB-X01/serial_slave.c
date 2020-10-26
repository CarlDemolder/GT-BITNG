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

void disable_serial_slave_handler(void)
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
              if(serial_array_data[0] == USB_COMMAND_HEADER && serial_array_data[-1] == USB_COMMAND_FOOTER)
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

                      case TMP116_MODULE:
                          NRF_LOG_INFO("SERIAL HANDLER: TMP116_MODULE");
                          _tmp116_handler(serial_array_data);
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
}


static void _nrf52_handler(uint8_t *serial_array_data)
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
                            ind_led_blink(serial_array_data[5], serial_array_data[6]);
                            break;     
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
                    ble_led_blink(serial_array_data[4], serial_array_data[5]);
                    break;

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

static void _tmp116_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case TMP116_READ_CHIP_ID_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_READ_CHIP_ID_COMMAND");
            break;

        case TMP116_INIT_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_INIT_COMMAND");
            break;

        case TMP116_SET_OPERATING_MODE_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_SET_OPERATING_MODE_COMMAND");
            break;

        case TMP116_TEMP_UINT16_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_UINT16_COMMAND");    
            break;

        case TMP116_TEMP_UINT8_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_TEMP_UINT8_COMMAND");    
            break;

        case TMP116_TEMP_CHAR_ARRAY_COMMAND:
            NRF_LOG_INFO("TMP116_MODULE: TMP116_TEMP_CHAR_ARRAY_COMMAND");    
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


