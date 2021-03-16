#ifndef __POWER_H__
#define __POWER_H__

#include "common.h"
#include "nrfx_power.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_power_clock.h"

void power_manager_init(void);
void soft_device_system_on(void);
void soft_device_system_off(void);
void power_management_shutdown(void);
void power_manager_handler(void);
void enable_dcdc_converter(void);
void disable_dcdc_converter(void);

#endif /*__POWER_H__*/