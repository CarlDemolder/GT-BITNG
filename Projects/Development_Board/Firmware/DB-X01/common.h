#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "fds.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "app_error.h"
#include "boards.h"
#include "nordic_common.h"
#include "sdk_common.h"
#include "app_util_platform.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"



#define BLE_LED_PIN                     11                                      /** Pinout for BLE LED for DB-X01 */
#define IND_LED_PIN                     12                                      /** Pinout for IND LED for DB-X01 */

#define I2C_SDA_PIN                     30                                      /** Pinout for LED for DB-X01 */
#define I2C_SCL_PIN                     28                                      /** Pinout for LED for DB-X01 */

#define EN_VCC_LDO_PIN                  8                                       /** Pinout for LDO Enable for DB-X01 */
#define EN_MAX30102_LED_LDO_PIN         6                                       /** Pinout for MAX30102 LED ENABLE for DB-X01 */
#define EN_MAX30102_POWER_LDO_PIN       7                                       /** Pinout for MAX30102 POWER ENABLE for DB-X01 */
#define EN_MAX30003_POWER_LDO_PIN       13                                      /** Pinout for MAX30003 for DB-X01 */


#define I2C_SCL_PIN                     28                                      /** Pinout for LED for DB-X01 */
#define I2C_SCL_PIN                     28                                      /** Pinout for LED for DB-X01 */


#define HARDWARE_VERSION_NUMBER         "DB-X01"

void log_init(void);
void ldo_init(void);

void enable_vcc_ldo(void);
void disable_vcc_ldo(void);

void enable_max30102_led_ldo(void);
void disable_max30102_led_ldo(void);

void enable_max30102_power_ldo(void);
void disable_max30102_power_ldo(void);

void enable_max30003_power_ldo(void);
void disable_max30003_power_ldo(void);

void led_init(void);

void ind_led_off(void);
void ind_led_on(void);
void ind_led_blink(uint16_t on_ms, uint16_t off_ms);


void ble_led_off(void);
void ble_led_on(void);
void ble_led_blink(uint16_t on_ms, uint16_t off_ms);


#endif // __COMMON_H