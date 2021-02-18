#include "clocks.h"

/**@brief Function to initialize the HF clock and the NRF Clocks
 */

//struct rtc_ft201x_configuration rtc_ft201x_configuration;

static void nrfx_clock_event_handler(nrfx_clock_evt_type_t event)
{

}

/**@brief Function to initialize the NRFX Clock and Enable it
 */
void nrf52_nrfx_clock_init(void)
{
    NRF_LOG_INFO("nrf52_nrfx_clock_init");
    // Initialize Clock module.
    ret_code_t err_code = nrfx_clock_init(nrfx_clock_event_handler);
    APP_ERROR_CHECK(err_code);
//    nrfx_clock_enable();
}

void nrf52_nrfx_clock_uninit(void)
{
    NRF_LOG_INFO("nrf52_nrfx_clock_uninit");
    // Uninitialize Clock module.
    nrfx_clock_uninit();
}

/**@brief Function to start the HF clock
 */
void hfclock_start(void)
{
    if(!nrfx_clock_hfclk_is_running())
    {
        nrfx_clock_hfclk_start();
        NRF_LOG_INFO("NRFX Clock HFCLK Started");
    }
}

/**@brief Function to stop the HF clock
/**@brief Function to stop the HF clock
 */
void hfclock_stop(void)
{
    if(nrfx_clock_hfclk_is_running())
    {
        nrfx_clock_hfclk_stop();
        NRF_LOG_INFO("NRFX Clock HFCLK Stopped");
    }
}

/**@brief Function to start the LF clock
 */
void lfclock_start(void)
{
    NRF_LOG_INFO("lfclock_start");

    if(!nrfx_clock_lfclk_is_running())
    {
        NRF_LOG_INFO("NRFX Clock LFCLK Started");
        nrfx_clock_lfclk_start();
    }
}

/**@brief Function to stop the LF clock
 */
void lfclock_stop(void)
{
    NRF_LOG_INFO("lfclock_stop");
    if(nrfx_clock_lfclk_is_running())
    {
        nrfx_clock_lfclk_stop();
        NRF_LOG_INFO("NRFX Clock LFCLK Stopped");
    }
}

/**@brief Function to configure an RTC instance for the FT201X
 */
#if FT201X
void rtc_ft201x_init(void)
{
    //Initialize RTC instance for the FT201X
    NRF_LOG_INFO("rtc__ft201x_init");
    rtc_ft201x_configuration.rtc_frequency = 8;      // Setting the frequency of the RTC FT201X to 8 Hz
    rtc_ft201x_configuration.rtc_is_running = 0;     // RTC FT201X current state
    rtc_ft201x_configuration.rtc_restart = 0;        // RTC FT201X restart command

    const nrfx_rtc_t nrfx_rtc_ft201x = NRFX_RTC_INSTANCE(2); /**< Declaring an instance of nrfx_rtc using RTC2 for FT201X.*/
    rtc_ft201x_configuration.nrfx_rtc = nrfx_rtc_ft201x;
    
    rtc_ft201x_configuration.nrfx_rtc_config = (nrfx_rtc_config_t) NRFX_RTC_DEFAULT_CONFIG;
    rtc_ft201x_configuration.nrfx_rtc_config.interrupt_priority = 7;
    rtc_ft201x_configuration.nrfx_rtc_config.prescaler = RTC_TIMER_CLOCK_FREQ/rtc_ft201x_configuration.rtc_frequency - 1;
    NRF_LOG_INFO("prescaler %u", rtc_ft201x_configuration.nrfx_rtc_config.prescaler);

    nrfx_rtc_ft201x_init();
}

/**@brief Function to uninitialize the instance of the RTC for the FT201X
 */
void nrfx_rtc_ft201x_uninit(void)
{
    NRF_LOG_INFO("rtc_ft201x_uninit");
    nrfx_rtc_uninit(&rtc_ft201x_configuration.nrfx_rtc);
}

/**@brief Function to initialize the NRFX instance of the RTC for the FT201X
 */
void nrfx_rtc_ft201x_init(void)
{
    NRF_LOG_INFO("nrfx_rtc_ft201x_init");
    ret_code_t error_code = nrfx_rtc_init(&rtc_ft201x_configuration.nrfx_rtc, &rtc_ft201x_configuration.nrfx_rtc_config, rtc_ft201x_handler);
    APP_ERROR_CHECK(error_code);
}

/**@brief Function to configure an RTC instance for the FT201X
 */
void rtc_ft201x_handler(nrfx_rtc_int_type_t int_type)
{
    if (int_type == NRFX_RTC_INT_COMPARE2)
    {
        NRF_LOG_INFO("NRFX_RTC_INT_COMPARE2");
        nrfx_rtc_counter_clear(&rtc_ft201x_configuration.nrfx_rtc);
        nrfx_rtc_int_enable(&rtc_ft201x_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);

        usb_handler();
    }
}

/**@brief Function to set the counter of the RTC FT201X
* The counter is a function of the frequency. For example, if the frequency = 8 Hz, a counter value of 80 would be 10 seconds...
 */
void rtc_ft201x_set_counter(uint32_t new_sampling_counter)
{
    NRF_LOG_INFO("rtc_ft201x_set_counter");
    rtc_ft201x_configuration.rtc_counter = new_sampling_counter;
    ret_code_t error_code = nrfx_rtc_cc_set(&rtc_ft201x_configuration.nrfx_rtc, 2, rtc_ft201x_configuration.rtc_counter, true);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_INFO("counter %u", rtc_ft201x_configuration.rtc_counter);
}

/**@brief Function to set the counter of the RTC FT201X
* The frequency is when the RTC timer is triggered
 */
void rtc_ft201x_set_frequency(uint8_t new_sampling_frequency)
{
    NRF_LOG_INFO("rtc_ft201x_set_frequency");
    
    nrfx_rtc_ft201x_uninit();
    
    rtc_ft201x_configuration.rtc_frequency = new_sampling_frequency;
    rtc_ft201x_configuration.nrfx_rtc_config.prescaler =  RTC_TIMER_CLOCK_FREQ/rtc_ft201x_configuration.rtc_frequency - 1;
    NRF_LOG_INFO("frequency %u", rtc_ft201x_configuration.rtc_frequency);
    nrfx_rtc_ft201x_init();
}

/**@brief Function to stop the RTC FT201X
 */
void rtc_ft201x_stop(void)
{
    NRF_LOG_INFO("rtc_ft201x_stop");
    if(rtc_ft201x_configuration.rtc_is_running)
    {
        nrfx_rtc_disable(&rtc_ft201x_configuration.nrfx_rtc);        // Disable the RTC Instance for the FT201X
        rtc_ft201x_configuration.rtc_is_running = 0;
        rtc_ft201x_configuration.rtc_restart = 1;
        NRF_LOG_INFO("RTC FT201X is stopped and disabled");
    }
}

/**@brief Function to start the RTC FT201X
 */
void rtc_ft201x_start(void)
{
    NRF_LOG_INFO("rtc_ft201x_start");
    if(rtc_ft201x_configuration.rtc_is_running != 0x01)
    {
        nrfx_rtc_enable(&rtc_ft201x_configuration.nrfx_rtc);        // Enable the RTC Instance for the FT201X
        nrfx_rtc_int_enable(&rtc_ft201x_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);
        nrfx_rtc_tick_enable(&rtc_ft201x_configuration.nrfx_rtc, true);
        rtc_ft201x_configuration.rtc_is_running = 1;
        NRF_LOG_INFO("RTC FT201X is enabled and has started");
    }
}

/**@brief Function to restart the RTC FT201X
 */
void rtc_ft201x_restart(void)
{
    NRF_LOG_INFO("rtc_ft201x_restart");
    if(rtc_ft201x_configuration.rtc_is_running == 0x00 && rtc_ft201x_configuration.rtc_restart == 0x01)
    {
        nrfx_rtc_enable(&rtc_ft201x_configuration.nrfx_rtc);        // Enable an RTC instance for the FT201X
        nrfx_rtc_int_enable(&rtc_ft201x_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);
        nrfx_rtc_tick_enable(&rtc_ft201x_configuration.nrfx_rtc, true);
        rtc_ft201x_configuration.rtc_is_running = 1;
        rtc_ft201x_configuration.rtc_restart = 0;
        NRF_LOG_INFO("RTC FT201X has been Restarted");
    }
}
#endif


/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


/**@brief Function to configure an RTC instance for all available sensors at a set sampling frequency
 */
void rtc_sensor_init(void)
{
    //Initialize a RTC instance for all available sensors
    NRF_LOG_INFO("rtc_sensor_init");
    rtc_sensor_configuration.rtc_frequency = 8;      // Setting the frequency of the RTC sensor to 8 Hz. No value lower than 8 will work.
    rtc_sensor_configuration.rtc_is_running = 0;     // RTC sensor current state
    rtc_sensor_configuration.rtc_restart = 0;        // RTC sensor restart command

    const nrfx_rtc_t nrfx_rtc_sensor = NRFX_RTC_INSTANCE(2); // Declaring an instance of nrfx_rtc using RTC2 for all available sensors
    rtc_sensor_configuration.nrfx_rtc = nrfx_rtc_sensor;
    
    rtc_sensor_configuration.nrfx_rtc_config = (nrfx_rtc_config_t) NRFX_RTC_DEFAULT_CONFIG;
    rtc_sensor_configuration.nrfx_rtc_config.interrupt_priority = 7;
    rtc_sensor_configuration.nrfx_rtc_config.prescaler = RTC_TIMER_CLOCK_FREQ/rtc_sensor_configuration.rtc_frequency-1;
    NRF_LOG_INFO("prescaler %u", rtc_sensor_configuration.nrfx_rtc_config.prescaler);

    nrfx_rtc_sensor_init();

    rtc_sensor_configuration.rtc_counter = 240;  // Setting the counter of the RTC sensor to trigger every 30 seconds
    rtc_sensor_set_counter(rtc_sensor_configuration.rtc_counter);
}

/**@brief Function to uninitialize the instance of the RTC sensor
 */
void nrfx_rtc_sensor_uninit(void)
{
    NRF_LOG_INFO("rtc_sensor_uninit");
    nrfx_rtc_uninit(&rtc_sensor_configuration.nrfx_rtc);
}

/**@brief Function to initialize the NRFX instance of the RTC sensor
 */
void nrfx_rtc_sensor_init(void)
{
    NRF_LOG_INFO("nrfx_rtc_sensor_init");
    ret_code_t error_code = nrfx_rtc_init(&rtc_sensor_configuration.nrfx_rtc, &rtc_sensor_configuration.nrfx_rtc_config, rtc_sensor_handler);
    APP_ERROR_CHECK(error_code);
}

/**@brief Handler for the RTC sensor instance
 */
void rtc_sensor_handler(nrfx_rtc_int_type_t interrupt_type)
{
    if (interrupt_type == NRFX_RTC_INT_COMPARE2)
    {
        NRF_LOG_INFO("rtc_sensor_handler: NRFX_RTC_INT_COMPARE2");
        nrfx_rtc_counter_clear(&rtc_sensor_configuration.nrfx_rtc);
        nrfx_rtc_int_enable(&rtc_sensor_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);

        #if TMP117
        if(tmp117_is_enabled() == 1)
        {
            tmp117_interrupt_handler();
        }
        #endif

        #if FDC1004
        if(fdc1004_is_enabled() == 1)
        {
            fdc1004_interrupt_handler();
        }
        #endif

        #if STRAIN_GAUGE
//        if(strain_gauge_is_enabled() == 1)
//        {
//            strain_gauge_handler();
//        }
        #endif
    }
}

/**@brief Function to set the counter of the RTC sensor
* The counter is a function of the frequency. For example, if the frequency = 8 Hz, a counter value of 80 would trigger the interrupt every 10 seconds...
 */
void rtc_sensor_set_counter(uint32_t new_sampling_counter)
{
    NRF_LOG_INFO("rtc_sensor_set_counter");

    rtc_sensor_configuration.rtc_counter = new_sampling_counter;
    NRF_LOG_INFO("counter %u", rtc_sensor_configuration.rtc_counter);
    ret_code_t error_code = nrfx_rtc_cc_set(&rtc_sensor_configuration.nrfx_rtc, 2, rtc_sensor_configuration.rtc_counter, true);
    APP_ERROR_CHECK(error_code);
}

/**@brief Function to set the frequency of the RTC sensor
* The frequency is when the RTC timer is triggered
 */
void rtc_sensor_set_frequency(uint8_t new_sampling_frequency)
{
    NRF_LOG_INFO("rtc_sensor_set_frequency");
    
    nrfx_rtc_sensor_uninit();
    
    rtc_sensor_configuration.rtc_frequency = new_sampling_frequency;
    rtc_sensor_configuration.nrfx_rtc_config.prescaler =  RTC_TIMER_CLOCK_FREQ/rtc_sensor_configuration.rtc_frequency - 1;
    NRF_LOG_INFO("frequency %u", rtc_sensor_configuration.rtc_frequency);

    nrfx_rtc_sensor_init();
}

/** @brief Function to return the number of samples per minute */
uint8_t rtc_sensor_get_sampling_frequency(void)
{
    NRF_LOG_INFO("rtc_sensor_get_sampling_frequency");
    uint8_t sampling_frequency = (uint8_t) (60/(rtc_sensor_configuration.rtc_counter/rtc_sensor_configuration.rtc_frequency));
    NRF_LOG_INFO("RTC Sensor Sampling Frequency: %u", sampling_frequency);
    return sampling_frequency;
}

/**@brief Function to stop the RTC sensor
 */
void rtc_sensor_stop(void)
{
    NRF_LOG_INFO("rtc_sensor_stop");
    if(rtc_sensor_configuration.rtc_is_running)
    {
        nrfx_rtc_disable(&rtc_sensor_configuration.nrfx_rtc);        // Disable the RTC Sensor Instance
        rtc_sensor_configuration.rtc_is_running = 0;
        rtc_sensor_configuration.rtc_restart = 1;
        NRF_LOG_INFO("RTC sensor is stopped and disabled");
    }
}

/**@brief Function to start the RTC sensor
 */
void rtc_sensor_start(void)
{
    NRF_LOG_INFO("rtc_sensor_start");
    if(rtc_sensor_configuration.rtc_is_running != 0x01)
    {
        nrfx_rtc_enable(&rtc_sensor_configuration.nrfx_rtc);        // Enable the RTC sensor Instance
        nrfx_rtc_int_enable(&rtc_sensor_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);
        nrfx_rtc_tick_enable(&rtc_sensor_configuration.nrfx_rtc, true);
        rtc_sensor_configuration.rtc_is_running = 1;
        NRF_LOG_INFO("RTC sensor is enabled and has started");
    }
}

/**@brief Function to restart the RTC sensor
 */
void rtc_sensor_restart(void)
{
    NRF_LOG_INFO("rtc_sensor_restart");
    if(rtc_sensor_configuration.rtc_is_running == 0x00 && rtc_sensor_configuration.rtc_restart == 0x01)
    {
        nrfx_rtc_enable(&rtc_sensor_configuration.nrfx_rtc);        // Enable an RTC sensor instance
        nrfx_rtc_int_enable(&rtc_sensor_configuration.nrfx_rtc, NRF_RTC_INT_COMPARE2_MASK);
        nrfx_rtc_tick_enable(&rtc_sensor_configuration.nrfx_rtc, true);
        rtc_sensor_configuration.rtc_is_running = 1;
        rtc_sensor_configuration.rtc_restart = 0;
        NRF_LOG_INFO("RTC sensor has been restarted");
    }
}