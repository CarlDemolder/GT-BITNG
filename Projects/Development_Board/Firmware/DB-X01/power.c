#include "power.h"

/**@brief Function for initializing power management.
 */
void power_manager_init(void)
{
    NRF_LOG_INFO("power_manager_init");
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    sd_power_mode_set(NRF_POWER_MODE_LOWPWR); // Setting the System On to low power mode with SoftDevice present
    NRF_POWER->TASKS_LOWPWR = 1;  // Setting the System On to low power mode without SoftDevice present

//    err_code = nrfx_power_init(NULL);
//    APP_ERROR_CHECK(err_code);
}

void soft_device_system_on(void)
{
    NRF_LOG_INFO("soft_device_system_on");

    // SoftDevice Method to enter System On Sleep Mode  
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void soft_device_system_off(void)
{
    NRF_LOG_INFO("soft_device_system_off");

    // SoftDevice Method to enter System OFF Sleep Mode 
    ret_code_t err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

void power_management_shutdown(void)
{
    NRF_LOG_INFO("power_management_shutdown");

    // Without SoftDevice and With SoftDevice method to enter System OFF Sleep Mode
    nrf_pwr_mgmt_shutdown_t shutdown_type = NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF;
    nrf_pwr_mgmt_shutdown(shutdown_type);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void power_manager_handler(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}
  
void enable_dcdc_converter(void)
{
    NRF_LOG_INFO("enable_dcdc_converter");
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
}

void disable_dcdc_converter(void)
{
    NRF_LOG_INFO("disable_dcdc_converter");
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_DISABLE);
}