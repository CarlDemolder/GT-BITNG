#include "power.h"

/**@brief Function for initializing power management.
 */
void power_manager_init(void)
{
    NRF_LOG_INFO("Power Management Initialized");
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

//    err_code = nrfx_power_init(NULL);
//    APP_ERROR_CHECK(err_code);
}

void sleep_mode_enter(void)
{
    NRF_LOG_INFO("Entering Sleep Mode via System On");

    // Without SoftDevice and With SoftDevice method to enter System On Sleep Mode
    nrf_pwr_mgmt_run();

    // SoftDevice Method to enter System On Sleep Mode  
//    ret_code_t err_code = sd_power_system_on();
//    APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void deep_sleep_mode_enter(void)
{
    NRF_LOG_INFO("deep_sleep_mode_enter");

    // Without SoftDevice and With SoftDevice method to enter System OFF Sleep Mode
    nrf_pwr_mgmt_shutdown_t shutdown_type = NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF;
    nrf_pwr_mgmt_shutdown(shutdown_type);

    // SoftDevice Method to enter System OFF Sleep Mode 
//    ret_code_t err_code = sd_power_system_off();
//    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void power_manager_handler(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        sleep_mode_enter();
    }
}
  
void enable_dcdc_converter(void)
{
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
}

void disable_dcdc_converter(void)
{
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_DISABLE);
}