#include "serial_slave.h"


void serial_slave_handler(void)
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
}


static void _nrf52_handler(uint8_t *serial_array_data)
{
    switch(serial_array_data[2])
    {
        case NRF52_HF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_HF_CLOCK_COMMAND");
            break;

        case NRF52_LF_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LF_CLOCK_COMMAND");
            break;

        case NRF52_RTC_CLOCK_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_RTC_CLOCK_COMMAND");
            break;

        case NRF52_POWER_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_POWER_COMMAND");
            break;

        case NRF52_LED_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LED_COMMAND");
            break;

        case NRF52_LOG_COMMAND:
            NRF_LOG_INFO("NRF52_MODULE: NRF52_LOG_COMMAND");
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


