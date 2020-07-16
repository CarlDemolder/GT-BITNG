#include "common.h"

/**@brief Function for initializing the nrf log module.
 */
void log_init(void)
{
    NRF_LOG_DEBUG("NRF Log Initialized");
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void ldo_init(void)
{
    // Initialize LDO pins for power savings
    nrf_gpio_cfg_output(EN_VCC_LDO_PIN);                  // set EN_VCC_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30102_LED_LDO_PIN);         // set EN_MAX30102_LED_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30102_POWER_LDO_PIN);       // set EN_MAX30102_POWER_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30003_POWER_LDO_PIN);       // set EN_MAX30003_POWER_LDO pin to output
}

void enable_vcc_ldo(void)
{
    NRF_LOG_INFO("Enable VCC LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 1);     // enable the VCC LDO to keep the MCU alive
}


void disable_vcc_ldo(void)
{
    NRF_LOG_INFO("Disable VCC LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 0);     // Disabling the VCC LDO to shutdown the MCU
}

void enable_max30102_led_ldo(void)
{
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 1);     // enable the MAX30102 LED LDO to provide power to the LED supply of the MAX30102
}


void disable_max30102_led_ldo(void)
{
    NRF_LOG_INFO("disable MAX30102 LED LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 0);     // Disabling the MAX30102 LED LDO to shutdown the LDO for the MAX30102
}

void enable_max30102_power_ldo(void)
{
    NRF_LOG_INFO("enable MAX30102 POWER LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_MAX30102_POWER_LDO_PIN, 1);     // enable the MAX30102 POWER LDO to provide power to the POWER supply of the MAX30102
}


void disable_max30102_power_ldo(void)
{
    NRF_LOG_INFO("disable MAX30102 POWER LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 0);     // Disabling the MAX30102 POWER LDO to shutdown the LDO for the MAX30102
}

void enable_max30003_power_ldo(void)
{
    NRF_LOG_INFO("enable MAX30003 POWER LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_MAX30003_POWER_LDO_PIN, 1);     // enable the MAX30003 POWER LDO to provide power to the POWER supply of the MAX30003
}


void disable_max30003_power_ldo(void)
{
    NRF_LOG_INFO("disable MAX30003 POWER LDO");
    ind_led_blink(2000,0);
    nrf_gpio_pin_write(EN_MAX30003_POWER_LDO_PIN, 0);     // Disabling the MAX30003 POWER LDO to shutdown the LDO for the MAX30003
}

void init_leds(void)
{
  nrf_gpio_cfg_output(IND_LED_PIN);   //set IND LED pin to output
  nrf_gpio_cfg_output(BLE_LED_PIN);   //set BLE LED pin to output
}

void ind_led_off(void)
{
  nrf_gpio_pin_write(IND_LED_PIN, 1);
}

void ind_led_on(void)
{
  nrf_gpio_pin_write(IND_LED_PIN, 0);
}

void ind_led_blink(uint16_t on_ms, uint16_t off_ms)
{
  ind_led_on();
  nrf_delay_ms(on_ms);
  ind_led_off();
  nrf_delay_ms(off_ms);
}

void ble_led_off(void)
{
  nrf_gpio_pin_write(BLE_LED_PIN, 1);
}

void ble_led_on(void)
{
  nrf_gpio_pin_write(BLE_LED_PIN, 0);
}

void ble_led_blink(uint16_t on_ms, uint16_t off_ms)
{
  ble_led_on();
  nrf_delay_ms(on_ms);
  ble_led_off();
  nrf_delay_ms(off_ms);
}
