#ifndef __CUSTOM_APP_ERROR_HANDLER_H__
#define __CUSTOM_APP_ERROR_HANDLER_H__

#include "app_error.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_util_platform.h"
#include "nrf_strerror.h"

#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info);

#endif /*__CUSTOM_APP_ERROR_HANDLER_H__*/