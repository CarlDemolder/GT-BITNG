#include "common.h"

/**@brief Function for initializing the nrf log module.
 */

void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_FLUSH();
    NRF_LOG_INFO("NRF Log Initialized");
}

void gpiote_init(void)
{
    if(!nrfx_gpiote_is_init())
    {
        ret_code_t error_code = nrfx_gpiote_init();
        APP_ERROR_CHECK(error_code);
    }
}

void ram_retention(void)
{
//    sd_power_ram_power_set()
}

void ldo_init(void)
{
    // Initialize LDO pins for power savings
    disable_max30003_power_ldo();     // Disable MAX30003 Power LDO on startup
    disable_max30102_led_ldo();       // Disable MAX30102 LED LDO on startup
    disable_max30102_power_ldo();     // Disable MAX30102 Power LDO on startup
}

void input_output_init(void)
{
    nrf_gpio_cfg_output(EN_VCC_LDO_PIN);                            // set EN_VCC_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30102_LED_LDO_PIN);                   // set EN_MAX30102_LED_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30102_POWER_LDO_PIN);                 // set EN_MAX30102_POWER_LDO pin to output
    nrf_gpio_cfg_output(EN_MAX30003_POWER_LDO_PIN);                 // set EN_MAX30003_POWER_LDO pin to output
    
    nrf_gpio_cfg_input(WB_V_OUT, NRF_GPIO_PIN_NOPULL);              // set WB_V_OUT input pin to no pull up
}

void enable_vcc_ldo(void)
{
    // enable the VCC LDO to keep the MCU alive. This is performed by toggling the Flip Flop to remember the magnetic input
    NRF_LOG_INFO("Enable VCC LDO");
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 0);
    nrf_delay_ms(100);
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 1);
    nrf_delay_ms(100);
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 0); 
}


void disable_vcc_ldo(void)
{
    NRF_LOG_INFO("Disable VCC LDO");
    nrf_gpio_pin_write(EN_VCC_LDO_PIN, 0);     // Disabling the VCC LDO to shutdown the MCU
}

void enable_max30102_led_ldo(void)
{
    NRF_LOG_INFO("enable MAX30102 LED LDO");
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 1);     // enable the MAX30102 LED LDO to provide power to the LED supply of the MAX30102
}


void disable_max30102_led_ldo(void)
{
    NRF_LOG_INFO("disable MAX30102 LED LDO");
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 0);     // Disabling the MAX30102 LED LDO to shutdown the LDO for the MAX30102
}

void enable_max30102_power_ldo(void)
{
    NRF_LOG_INFO("enable MAX30102 POWER LDO");
    nrf_gpio_pin_write(EN_MAX30102_POWER_LDO_PIN, 1);     // enable the MAX30102 POWER LDO to provide power to the POWER supply of the MAX30102
}


void disable_max30102_power_ldo(void)
{
    NRF_LOG_INFO("disable MAX30102 POWER LDO");
    nrf_gpio_pin_write(EN_MAX30102_LED_LDO_PIN, 0);     // Disabling the MAX30102 POWER LDO to shutdown the LDO for the MAX30102
}

void enable_max30003_power_ldo(void)
{
    NRF_LOG_INFO("enable MAX30003 POWER LDO");
    nrf_gpio_pin_write(EN_MAX30003_POWER_LDO_PIN, 1);     // enable the MAX30003 POWER LDO to provide power to the POWER supply of the MAX30003
}


void disable_max30003_power_ldo(void)
{
    NRF_LOG_INFO("disable MAX30003 POWER LDO");
    nrf_gpio_pin_write(EN_MAX30003_POWER_LDO_PIN, 0);     // Disabling the MAX30003 POWER LDO to shutdown the LDO for the MAX30003
}

void init_leds(void)
{
    NRF_LOG_INFO("Init LEDS");
    nrf_gpio_cfg_output(IND_LED_PIN);   //set IND LED pin to output
    nrf_gpio_cfg_output(BLE_LED_PIN);   //set BLE LED pin to output

    ind_led_off();    // Turn off Indicator LED
    ble_led_off();    // Turn off BLE LED
}

void ind_led_off(void)
{
    NRF_LOG_INFO("Indicator LED OFF");
    nrf_gpio_pin_write(IND_LED_PIN, 1);
}

void ind_led_on(void)
{
    NRF_LOG_INFO("Indicator LED ON");
    nrf_gpio_pin_write(IND_LED_PIN, 0);
}

void ind_led_blink(uint16_t on_ms, uint16_t off_ms)
{
    NRF_LOG_INFO("Indicator LED Blink Custom");
    ind_led_on();
    nrf_delay_ms(on_ms);
    ind_led_off();
    nrf_delay_ms(off_ms);
}

void ind_led_short_blink(void)
{
    NRF_LOG_INFO("Indicator LED Short Blink");
    ind_led_on();
    nrf_delay_ms((uint32_t) 100);
    ind_led_off();
    nrf_delay_ms((uint32_t) 100);
}

void ind_led_medium_blink(void)
{
    NRF_LOG_INFO("Indicator LED Medium Blink");
    ind_led_on();
    nrf_delay_ms((uint32_t) 1000);
    ind_led_off();
    nrf_delay_ms((uint32_t) 1000);
}

void ind_led_long_blink(void)
{
    NRF_LOG_INFO("Indicator LED Long Blink");
    ind_led_on();
    nrf_delay_ms((uint32_t) 3000);
    ind_led_off();
    nrf_delay_ms((uint32_t) 3000);
}

void ble_led_off(void)
{
    NRF_LOG_INFO("BLE LED OFF");
    nrf_gpio_pin_write(BLE_LED_PIN, 1);
}

void ble_led_on(void)
{
    NRF_LOG_INFO("BLE LED ON");
    nrf_gpio_pin_write(BLE_LED_PIN, 0);
}

void ble_led_blink(uint16_t on_ms, uint16_t off_ms)
{
    NRF_LOG_INFO("BLE LED Custom Blink");
    ble_led_on();
    nrf_delay_ms(on_ms);
    ble_led_off();
    nrf_delay_ms(off_ms);
}

