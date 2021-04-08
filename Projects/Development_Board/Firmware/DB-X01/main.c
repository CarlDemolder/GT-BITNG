#include "common.h"
#include "power.h"
#include "bluetooth.h"

#include "serial_slave.h"
                                                                                                                        
/**@brief Function for application main entry.
 */
int main(void)
{
    startup_initialization();

    #if FT201X
    enable_usb_handler();    // Enable USB Handler for Serial Debugging through I2C
    #endif

    enable_bluetooth_handler();
    for (;;)
    {
        power_manager_handler();        
    }
}
