USB_COMMAND_HEADER = 0XAA
USB_COMMAND_FOOTER = 0XAB
USB_LOG_HEADER = 0XAC
USB_LOG_FOOTER = 0XAD

NRF52_MODULE = 0X01
TMP117_MODULE = 0X02
BMI160_MODULE = 0X03
MAX30003_MODULE = 0X04
FT201X_MODULE = 0X05
BLE_MODULE = 0x06

NRF52_HF_CLOCK_COMMAND = 0x10
NRF52_LF_CLOCK_COMMAND = 0x11
NRF52_CLOCK_COMMAND = 0X12
NRF52_RTC_CLOCK_COMMAND = 0X13
NRF52_POWER_COMMAND = 0X14
NRF52_COMMON_COMMAND = 0X15
NRF52_I2C_COMMAND = 0X17
NRF52_SPI_COMMAND = 0X18

FT201X_READ_CHIP_ID_COMMAND = 0X20
FT201X_SET_VCP_COMMAND = 0X21
FT201X_WRITE_EEPROM_COMMAND = 0X22
FT201X_READ_EEPROM_COMMAND = 0X23
FT201X_WRITE_DATA_COMMAND = 0X24
FT201X_READ_DATA_COMMAND = 0X25

BMI160_READ_CHIP_ID_COMMAND = 0X30
BMI160_INIT_COMMAND = 0X31
BMI160_READ_ACCEL_GYRO_COMMAND = 0X32

TMP117_READ_CHIP_ID_COMMAND = 0X40
TMP117_INIT_COMMAND = 0X41
TMP117_SET_OPERATING_MODE_COMMAND = 0X42
TMP117_TEMP_UINT16_COMMAND = 0X43
TMP117_TEMP_CHAR_ARRAY_COMMAND = 0X44
TMP117_READ_REVISION_NUMBER_COMMAND = 0X45
TMP117_UNLOCK_EEPROM_COMMAND = 0X46
TMP117_GENERAL_CALL_RESET_COMMAND = 0X47

MAX30003_READ_CHIP_ID_COMMAND = 0X50
MAX30003_INIT_COMMAND = 0X51

BLE_INIT_GAP_PARAMS_COMMAND = 0X60
BLE_UPDATE_GAP_PARAMS_COMMAND = 0X61
BLE_INIT_GATT_COMMAND = 0X62
BLE_INIT_SERVICES_COMMAND = 0X63
BLE_INIT_CONN_PARAMS_COMMAND = 0X64
BLE_INIT_BLE_STACK_COMMAND = 0X65
BLE_INIT_PEER_MANAGER_COMMAND = 0X66
BLE_INIT_ADVERTISING_COMMAND = 0X67

NRF52_HF_CLOCK_START = 0X70
NRF52_HF_CLOCK_STOP = 0X71
NRF52_NRFX_CLOCK_DRIVER_INIT = 0X72
NRF52_NRFX_CLOCK_DRIVER_UNINIT = 0X73
NRF52_LF_CLOCK_START = 0X74
NRF52_LF_CLOCK_STOP = 0X75

NRF52_RTC_CONFIG = 0X80
NRF52_RTC_SET_COUNTER = 0X81
NRF52_RTC_START = 0X82
NRF52_RTC_STOP = 0X83
NRF52_RTC_RESTART = 0X84

NRF52_POWER_DCDC_CONVERTER_ENABLE = 0X90
NRF52_POWER_DCDC_CONVERTER_DISABLE = 0X91
NRF52_POWER_SLEEP_MODE_ENTER = 0X92
NRF52_POWER_DEEP_SLEEP_MODE_ENTER = 0X93
NRF52_POWER_MANAGER_HANDLER = 0X94
NRF52_POWER_MANAGER_INIT = 0X95

NRF52_LED_INIT = 0X99
NRF52_LED_IND_LED_ON = 0X9A
NRF52_LED_IND_LED_OFF = 0X9B
NRF52_LED_IND_BLINK = 0X9C
NRF52_LED_BLE_LED_ON = 0X9D
NRF52_LED_BLE_LED_OFF = 0X9E
NRF52_LED_BLE_BLINK = 0X9F

NRF52_I2C_TWIM_INIT = 0XA0
NRF52_I2C_TWIM_START = 0XA1
NRF52_I2C_TWIM_STOP = 0xA2
NRF52_LOG_INIT = 0XA3
NRF52_GPIOTE_INIT = 0XA4
NRF52_LDO_INIT = 0XA5
NRF52_VCC_LDO_EN = 0XA6


NRF52_LED_IND_CUSTOM_BLINK = 0XB0
NRF52_LED_IND_SHORT_BLINK = 0XB1
NRF52_LED_IND_MEDIUM_BLINK = 0XB2
NRF52_LED_IND_LONG_BLINK = 0XB3