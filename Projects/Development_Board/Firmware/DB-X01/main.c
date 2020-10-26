#include "common.h"
#include "power.h"
#include "bluetooth.h"

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
    enable_vcc_ldo();                 // Enable LDO VCC (3.3V)
    enable_serial_slave_handler();    // Enable Serial Slave Handler

    for (;;)
    {
        serial_slave_manager_handler();
        nrf_delay_ms(2000);
        bluetooth_manager_handler();
    }
}
