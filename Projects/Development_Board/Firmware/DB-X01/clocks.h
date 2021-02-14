#ifndef __CLOCKS_H__
#define __CLOCKS_H__

#include "common.h"

#include "nrfx_clock.h"
#include "nrfx_rtc.h"
#include "nrfx_timer.h"

#include "serial_slave.h"


#define RTC_TIMER_TICKS(MS)                                \
            ((uint32_t)ROUNDED_DIV(                        \
            (MS) * (uint64_t)RTC_TIMER_CLOCK_FREQ,         \
            1000 * (NRFX_RTC_DEFAULT_CONFIG_FREQUENCY + 1)))

enum RTC_Constants
{
    RTC_TIMER_CLOCK_FREQ = 32768,                     // Clock frequency of the RTC timer used to implement the app timer module
    NRFX_RTC_AVAILABLE = 2                           // RTC0 is reserved for Soft Device, RTC1 is used by TWI, RTC2 is available for RTC
};


/**@brief RTC FT201X Structure. This structure contains all values for the RTC FT201X Configuration.*/
struct RTC_Configuration
{
    uint8_t rtc_is_running;
    uint8_t rtc_restart;
    uint8_t rtc_frequency;
    uint32_t rtc_counter;
    nrfx_rtc_t nrfx_rtc;
    nrfx_rtc_config_t nrfx_rtc_config;
};

static struct RTC_Configuration rtc_ft201x_configuration;
static struct RTC_Configuration rtc_sensor_configuration;

void nrf52_nrfx_clock_init(void);
void nrf52_nrfx_clock_uninit(void);
void hfclock_start(void);
void hfclock_stop(void);
void lfclock_start(void);
void lfclock_stop(void);

void nrfx_rtc_ft201x_init(void);
void nrfx_rtc_ft201x_uninit(void);
void rtc_ft201x_init(void);
void rtc_ft201x_handler(nrfx_rtc_int_type_t int_type);
void rtc_ft201x_set_counter(uint32_t new_sampling_counter);
void rtc_ft201x_set_frequency(uint8_t new_sampling_frequency);
void rtc_ft201x_start(void);
void rtc_ft201x_stop(void);
void rtc_ft201x_restart(void);


void nrfx_rtc_sensor_init(void);
void nrfx_rtc_sensor_uninit(void);
void rtc_sensor_init(void);
void rtc_sensor_handler(nrfx_rtc_int_type_t int_type);
void rtc_sensor_set_counter(uint32_t new_sampling_counter);
void rtc_sensor_set_frequency(uint8_t new_sampling_frequency);
uint8_t rtc_sensor_get_sampling_frequency(void);
void rtc_sensor_start(void);
void rtc_sensor_stop(void);
void rtc_sensor_restart(void);

#endif /*__CLOCKS_H__*/