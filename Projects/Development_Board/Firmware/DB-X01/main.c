#include "common.h"
#include "power.h"
#include "bluetooth.h"
#include "TMP116.h"                   
#include "bmi160.h"
#include "max30003.h"
                                                                                                                        
#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    clocks_init();              // Initialize Clocks: HF & LF
    log_init();                 // Initialize debugging through RTT not UART
    init_leds();                // Initialize LEDs
    ldo_init();                 // Initialize hardware/GPIO pins
    power_init();               // Initialize Power Manager Unit
    enable_vcc_ldo();           // Enable LDO VCC, 3V
    enable_max30102_power_ldo();  // Enable power to the MAX30102, 1.8V
    twi_init();                 // Initialize I2C protocol
    ind_led_blink(1000, 1000);    // Blink the LED for a second to wait for TWI to initialize
//    rtc_config();                 // Initialize RTC Clock with the RTC Handler

    // ble_stack_init();
    // gap_params_init();
    // gatt_init();
    // services_init();
    // advertising_init();
    // set_advertising_power();
    // conn_params_init();
    // peer_manager_init();

//    enable_dcdc_converter();    // Enabling the DC/DC Converter after the BLE Stack has been Initialized
    i2c_start();      // Starting the I2C module
    ind_led_blink(1000,1000);   // Blink the LED for a second to wait for I2C to start
//    configure_tmp116(0);    // Setting the TMP116 into Shutdown Mode 

    // Start execution.
    NRF_LOG_INFO("Nordic Butterfly started.");
    // set_hardware_version();
    // advertising_start();

    // Turn off unneccessary pheripherals before entering main loop.
//    i2c_stop();           // Turn off the TWI Pheripheral Module

//    bmi160_init_registers();

    for (;;)
    {
//        power_handler();
        ind_led_blink(1000,1000);   // Indicator LED blink
        NRF_LOG_INFO("Blink");
//        tmp116_get_celsius();

//        uint8_t *data_array_pointer;
//        bmi160_read_accel_gyro(data_array_pointer); 
    }
}
