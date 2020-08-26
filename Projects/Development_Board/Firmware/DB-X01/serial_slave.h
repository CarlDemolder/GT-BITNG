#ifndef __SERIAL_SLAVE_H__
#define __SERIAL_SLAVE_H__

#include "common.h"
#include "ft201x.h"

#define USB_COMMAND_HEADER                        0XAA
#define USB_COMMAND_FOOTER                        0XAB
#define USB_LOG_HEADER                            0XAC
#define USB_LOG_FOOTER                            0XAD

#define NRF52_MODULE                              0X01
#define TMP116_MODULE                             0X02
#define BMI160_MODULE                             0X03
#define MAX30003_MODULE                           0X04
#define FT201X_MODULE                             0X05
#define BLE_MODULE                                0x06

#define NRF52_HF_CLOCK_COMMAND                    0x10
#define NRF52_LF_CLOCK_COMMAND                    0x11
#define NRF52_RTC_CLOCK_COMMAND                   0X12
#define NRF52_POWER_COMMAND                       0X13
#define NRF52_LED_COMMAND                         0X14
#define NRF52_LOG_COMMAND                         0X15
#define NRF52_I2C_COMMAND                         0X16
#define NRF52_SPI_COMMAND                         0X17

#define FT201X_READ_CHIP_ID_COMMAND               0X20
#define FT201X_SET_VCP_COMMAND                    0X21
#define FT201X_WRITE_EEPROM_COMMAND               0X22
#define FT201X_READ_EEPROM_COMMAND                0X23

#define BMI160_READ_CHIP_ID_COMMAND               0X30
#define BMI160_INIT_COMMAND                       0X31
#define BMI160_READ_ACCEL_GYRO_COMMAND            0X32

#define TMP116_READ_CHIP_ID_COMMAND               0X40
#define TMP116_INIT_COMMAND                       0X41
#define TMP116_SET_OPERATING_MODE_COMMAND         0X42
#define TMP116_TEMP_UINT16_COMMAND                0X43
#define TMP116_TEMP_UINT8_COMMAND                 0X44
#define TMP116_TEMP_CHAR_ARRAY_COMMAND            0X45

#define MAX30003_READ_CHIP_ID_COMMAND             0X50
#define MAX30003_INIT_COMMAND                     0X51

#define BLE_INIT_GAP_PARAMS_COMMAND               0X60
#define BLE_UPDATE_GAP_PARAMS_COMMAND             0X61
#define BLE_INIT_GATT_COMMAND                     0X62
#define BLE_INIT_SERVICES_COMMAND                 0X63
#define BLE_INIT_CONN_PARAMS_COMMAND              0X64
#define BLE_INIT_BLE_STACK_COMMAND                0X65
#define BLE_INIT_PEER_MANAGER_COMMAND             0X66
#define BLE_INIT_ADVERTISING_COMMAND              0X67



void serial_slave_handler(void);
static void _nrf52_handler(uint8_t *serial_array_data);
static void _ft201x_handler(uint8_t *serial_array_data);
static void _bmi160_handler(uint8_t *serial_array_data);
static void _tmp116_handler(uint8_t *serial_array_data);
static void _max30003_handler(uint8_t *serial_array_data);
static void _ble_handler(uint8_t *serial_array_data);

#endif /*__SERIAL_SLAVE_H__*/
