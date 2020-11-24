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
#include "nrfx_gpiote.h"


enum DB_X02_PIN_CONSTANTS
{
    BLE_LED_PIN = 11,                                   /** Pinout for BLE LED for DB-X02 */
    IND_LED_PIN = 12,                                   /** Pinout for IND LED for DB-X02 */

    I2C_SDA_PIN = 30,                                   /** Pinout for I2C SDA for DB-X02 */
    I2C_SCL_PIN = 28,                                   /** Pinout for I2C SCL for DB-X02 */

    EN_VCC_LDO_PIN = 8,                                 /** Pinout for LDO Enable for DB-X02 */
    EN_MAX30102_LED_LDO_PIN = 6,                        /** Pinout for MAX30102 LED ENABLE for DB-X02 */
    EN_MAX30102_POWER_LDO_PIN = 7,                      /** Pinout for MAX30102 POWER ENABLE for DB-X02 */
    EN_MAX30003_POWER_LDO_PIN = 13,                     /** Pinout for MAX30003 for DB-X02 */

    BMI160_INT1_PIN = 24,                               /** Pinout for BMI 160 INT1 pin for DB-X02 */
    BMI160_INT2_PIN = 23,                               /** Pinout for BMI 160 INT2 pin for DB-X02 */

    SPI_MISO_PIN = 16,                                  /** Pinout for SPI SDI pin for DB-X02 */
    SPI_MOSI_PIN = 14,                                  /** Pinout for SPI SDO pin for DB-X02 */
    SPI_CLK_PIN = 15,                                   /** Pinout for SPI CLK pin for DB-X02 */

    MAX30003_INT1_PIN = 27,                             /** Pinout for MAX30003 INT1 pin for DB-X02 */
    MAX30003_INT2_PIN = 26,                             /** Pinout for MAX30003 INT2 pin for DB-X02 */
    MAX30003_CS_PIN = 17,                               /** Pinout for MAX30003 CS pin for DB-X02 */

    MAX30102_INT1_PIN = 25,                             /** Pinout for MAX30102 INT1 pin for DB-X02 */

    FT201X_RESET_PIN = 22,                              /** Pinout for FT201X RESET pin for DB-X02 */  

    TMP117_ALERT_PIN = 19,                              /** Pinout for TMP117 ALERT pin for DB-X02 */

    CY15B108QI_CS_PIN = 18,                             /** Pinout for Chip Select pin of CY15B108QI for DB-X02 */

    NRESET_PIN = 21,                                    /** Pinout for nRESET pin for DB-X02 */ 

    WB_V_OUT = 4,                                       /** Pinout for Wheatstone Bridge Analog pin for DB-X02 */ 
};

#define HARDWARE_VERSION_NUMBER         "DB-X02"



#define LAST_ARRAY_ELEMENT(x)           (sizeof(x)/sizeof(x[0])-1)             /** Array index for the last element of the array */ 

void gpiote_init(void);

void log_init(void);
void ldo_init(void);
void input_output_init(void);

void enable_vcc_ldo(void);
void disable_vcc_ldo(void);

void enable_max30102_led_ldo(void);
void disable_max30102_led_ldo(void);

void enable_max30102_power_ldo(void);
void disable_max30102_power_ldo(void);

void enable_max30003_power_ldo(void);
void disable_max30003_power_ldo(void);

void init_leds(void);

void ind_led_off(void);
void ind_led_on(void);
void ind_led_blink(uint16_t on_ms, uint16_t off_ms);
void ind_led_short_blink(void);
void ind_led_medium_blink(void);
void ind_led_long_blink(void);

void ble_led_off(void);
void ble_led_on(void);
void ble_led_blink(uint16_t on_ms, uint16_t off_ms);

void enable_nreset(void);


#endif // __COMMON_H