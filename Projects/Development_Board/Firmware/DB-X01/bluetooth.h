#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "common.h"

#if TMP117
#include "ble_temperature_service.h"
#endif

#include "ble_configuration_service.h"

#if MAX30003
#include "ble_ecg_service.h"
#endif

#if FDC1004
#include "ble_pressure_service.h"
#endif

#include "serial_slave.h"

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "ble_dfu.h"

/**@brief Status Register Structure. This structure contains all values read from the Status Register.*/
struct Bluetooth_Control_Struct
{
    ret_code_t error_code;          /**< Variable to track errors */
};

static void _bluetooth_nrf_qwr_error_handler(uint32_t nrf_error);
static void _bluetooth_pm_evt_handler(pm_evt_t const *p_evt);
void bluetooth_gap_params_init(void);
void bluetooth_gap_params_update(uint16_t m_conn_handle);
void bluetooth_gatt_init(void);

void bluetooth_services_init(void);
static void _bluetooth_on_conn_params_evt(ble_conn_params_evt_t *p_evt);
static void _bluetooth_conn_params_error_handler(uint32_t nrf_error);
void bluetooth_conn_params_init(void);
static void _bluetooth_on_adv_evt(ble_adv_evt_t ble_adv_evt);
static void _bluetooth_ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
void bluetooth_ble_stack_init(void);
void bluetooth_peer_manager_init(void);
void bluetooth_advertising_init(void);
void bluetooth_set_advertising_power(void);
void bluetooth_advertising_start(void);
void bluetooth_advertising_stop(void);
void bluetooth_advertising_restart(void);

void bluetooth_configuration_service_settings_char_read(uint8_t *settings_char_data);
void bluetooth_configuration_service_response_char_write(uint8_t *response_char_data);
void bluetooth_configuration_service_crc_char_write(uint8_t *crc_char_data);

#if TMP117
void bluetooth_temperature_service_temp_char_write(uint8_t *temp_char_data);
void bluetooth_temperature_service_instant_temp_char_write(uint8_t *instant_temp_char_data);
void bluetooth_transmit_temperature_recording_session(void);
#endif

#if MAX30003
void bluetooth_ecg_service_ecg_char_write(uint8_t *ecg_char_data);
void bluetooth_ecg_service_instant_ecg_char_write(uint8_t *ecg_char_data);
void bluetooth_transmit_ecg_recording_session(void);
#endif

#if FDC1004
void bluetooth_pressure_service_instant_pressure_char_write(uint8_t *instant_pressure_char_data);
#endif

void bluetooth_transmit_firmware_version(void);
void bluetooth_transmit_hardware_board_version(void);

uint8_t bluetooth_get_bytes_per_transmission(void);
void bluetooth_disconnect(void);

// DFU Related Functions
void bluetooth_ble_dfu_async_svci_init(void);
void bluetooth_delete_bonds(void);
void bluetooth_ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event);


#endif /*__BLUETOOTH_H__*/