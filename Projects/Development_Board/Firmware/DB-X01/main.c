#include "common.h"
#include "power.h"
#include "bluetooth.h"
#include "TMP116.h"                   
#include "bmi160.h"
#include "max30003.h"
#include "serial_slave.h"
                                                                                                                        
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
    log_init();                 // Initialize debugging through RTT not UART
    gpiote_init();              // Initializing the GPIO task scheduler and manager
    ldo_init();                 // Initialize hardware/GPIO pins
    enable_vcc_ldo();           // Enable LDO VCC, 3V
    
    power_init();               // Initialize Power Manager Unit

    init_leds();                // Initialize LEDs
    ind_led_blink(2000, 2000);    // Blink the LED for a second to wait for TWI to initialize



//    enable_max30102_power_ldo();  // Enable power to the MAX30102, 1.8V
    enable_max30003_power_ldo();
//    twim_init();                 // Initialize I2C protocol
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

    enable_dcdc_converter();    // Enabling the DC/DC Converter AFTER the BLE Stack has been Initialized
    ind_led_blink(1000,1000);   // Blink the LED for a second to wait for I2C to start


    // Start execution.
    NRF_LOG_INFO("DB-X01 started.");
    // set_hardware_version();
    // advertising_start();

    // Turn off unneccessary pheripherals before entering main loop.
//    i2c_stop();           // Turn off the TWI Pheripheral Module
//    ind_led_blink(1000,1000);   // Blink the LED for a second to wait for I2C to start
//    i2c_start();
//    ind_led_blink(1000,1000);   // Blink the LED for a second to wait for I2C to start

      spim_init();
      ind_led_blink(1000,1000);   // Blink the LED for a second to wait for SPIM to start
      max30003_soft_reset();
      max30003_init(); 

//    bmi160_init();
//    configure_tmp116(1);        // Setting the TMP116 into Shutdown Mode 
//    uint8_t chip_id = bmi160_read_chip_id();

    for (;;)
    {
//        serial_slave_handler();
//        power_handler();
        ind_led_blink(2000,2000);   // Indicator LED blink
        NRF_LOG_INFO("Blink");
        max30003_read_device_info();
//        if(max30003_read_interrupt() == 1)
//        {
//            uint8_t data_array[52];
//            max30003_read_fifo_data(data_array);
//        }
    }
}
