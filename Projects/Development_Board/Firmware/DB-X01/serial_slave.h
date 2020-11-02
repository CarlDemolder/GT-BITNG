#ifndef __SERIAL_SLAVE_H__
#define __SERIAL_SLAVE_H__

#include "common.h"
#include "ft201x.h"
#include "tmp117.h"                   
#include "bmi160.h"
#include "clocks.h"
#include "power.h"  

#define USB_COMMAND_HEADER                        0XAA
#define USB_COMMAND_FOOTER                        0XAB
#define USB_LOG_HEADER                            0XAC
#define USB_LOG_FOOTER                            0XAD

#define NRF52_MODULE                              0X01
#define TMP117_MODULE                             0X02
#define BMI160_MODULE                             0X03
#define MAX30003_MODULE                           0X04
#define FT201X_MODULE                             0X05
#define BLE_MODULE                                0x06

#define NRF52_HF_CLOCK_COMMAND                    0x10
#define NRF52_LF_CLOCK_COMMAND                    0x11
#define NRF52_CLOCK_COMMAND                       0X12
#define NRF52_RTC_CLOCK_COMMAND                   0X13
#define NRF52_POWER_COMMAND                       0X14
#define NRF52_COMMON_COMMAND                      0X15
#define NRF52_I2C_COMMAND                         0X17
#define NRF52_SPI_COMMAND                         0X18

#define FT201X_READ_CHIP_ID_COMMAND               0X20
#define FT201X_SET_VCP_COMMAND                    0X21
#define FT201X_WRITE_EEPROM_COMMAND               0X22
#define FT201X_READ_EEPROM_COMMAND                0X23
#define FT201X_WRITE_DATA_COMMAND                 0X24
#define FT201X_READ_DATA_COMMAND                  0X25

#define BMI160_READ_CHIP_ID_COMMAND               0X30
#define BMI160_INIT_COMMAND                       0X31
#define BMI160_READ_ACCEL_GYRO_COMMAND            0X32

#define TMP117_READ_CHIP_ID_COMMAND               0X40
#define TMP117_INIT_COMMAND                       0X41
#define TMP117_SET_OPERATING_MODE_COMMAND         0X42
#define TMP117_TEMP_UINT16_COMMAND                0X43
#define TMP117_TEMP_CHAR_ARRAY_COMMAND            0X44
#define TMP117_READ_REVISION_NUMBER_COMMAND       0X45
#define TMP117_UNLOCK_EEPROM_COMMAND              0X46
#define TMP117_GENERAL_CALL_RESET_COMMAND         0X47

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

#define NRF52_HF_CLOCK_START                      0X70
#define NRF52_HF_CLOCK_STOP                       0X71
#define NRF52_NRFX_CLOCK_DRIVER_INIT              0X72
#define NRF52_NRFX_CLOCK_DRIVER_UNINIT            0X73
#define NRF52_LF_CLOCK_START                      0X74
#define NRF52_LF_CLOCK_STOP                       0X75

#define NRF52_RTC_CONFIG                          0X80
#define NRF52_RTC_SET_COUNTER                     0X81
#define NRF52_RTC_START                           0X82
#define NRF52_RTC_STOP                            0X83
#define NRF52_RTC_RESTART                         0X84

#define NRF52_POWER_DCDC_CONVERTER_ENABLE         0X90
#define NRF52_POWER_DCDC_CONVERTER_DISABLE        0X91
#define NRF52_POWER_SLEEP_MODE_ENTER              0X92
#define NRF52_POWER_DEEP_SLEEP_MODE_ENTER         0X93
#define NRF52_POWER_MANAGER_HANDLER               0X94
#define NRF52_POWER_MANAGER_INIT                  0X95

#define NRF52_LED_INIT                            0X99
#define NRF52_LED_IND_LED_ON                      0X9A
#define NRF52_LED_IND_LED_OFF                     0X9B
#define NRF52_LED_IND_BLINK                       0X9C
#define NRF52_LED_BLE_LED_ON                      0X9D
#define NRF52_LED_BLE_LED_OFF                     0X9E
#define NRF52_LED_BLE_BLINK                       0X9F

#define NRF52_I2C_TWIM_INIT                       0XA0
#define NRF52_I2C_TWIM_START                      0XA1
#define NRF52_I2C_TWIM_STOP                       0xA2
#define NRF52_LOG_INIT                            0XA3
#define NRF52_GPIOTE_INIT                         0XA4
#define NRF52_LDO_INIT                            0XA5
#define NRF52_VCC_LDO_EN                          0XA6


#define NRF52_LED_IND_CUSTOM_BLINK                0XB0
#define NRF52_LED_IND_SHORT_BLINK                 0XB1
#define NRF52_LED_IND_MEDIUM_BLINK                0XB2
#define NRF52_LED_IND_LONG_BLINK                  0XB3


void enable_serial_slave_handler(void);
void serial_slave_manager_handler(void);
void bluetooth_manager_handler(void);

static void _nrf52_handler(uint8_t *serial_array_data);
static void _ft201x_handler(uint8_t *serial_array_data);
static void _bmi160_handler(uint8_t *serial_array_data);
static void _tmp117_handler(uint8_t *serial_array_data);
static void _max30003_handler(uint8_t *serial_array_data);
static void _ble_handler(uint8_t *serial_array_data);

#endif /*__SERIAL_SLAVE_H__*/
