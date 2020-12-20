#ifndef BLE_ECG_SERVICE_H__
#define BLE_ECG_SERVICE_H__

#include "ble_service.h"
#include "serial_slave.h"

#if ECG

// Macro for defining a ble instance
#define BLE_ECG_SERVICE_DEF(_name) static ble_ecg_service_t _name; \
NRF_SDH_BLE_OBSERVER(_name ## _obs, BLE_HRS_BLE_OBSERVER_PRIO, ble_ecg_service_on_ble_evt, &_name)

#define ECG_SERVICE_UUID_BASE {0xCE, 0x13, 0x10, 0xB0, 0xF0, 0x74, 0x3E, 0xA1, 0xEE, 0x45, 0x30, 0x38, 0x96, 0x6B, 0xAD, 0x8A}

enum ECG_SERVICE_CHAR_LENGTHS
{
    ECG_SERVICE_ECG_CHAR_LENGTH = 250
};

// Defining the custom service 16 bit UUID
enum ECG_SERVICE_UUID
{
    ECG_SERVICE_UUID = 0x8CD6,
    ECG_ECG_CHAR_UUID = 0x46AE
};

/**@brief ECG Service event type. */
typedef enum
{
    ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED,            /**< ECG Characteristic notification enabled event. */
    ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_DISABLED,           /**< ECG Characteristic notification disabled event. */
    ECG_SERVICE_EVT_NOTIFICATION,                             /**< ECG Service notification event. */
    ECG_SERVICE_EVT_DISCONNECTED,                             /**< ECG Service disconnected event. */
    ECG_SERVICE_EVT_CONNECTED,                                /**< ECG Service connected event. */
    ECG_SERVICE_EVT_WRITE,                                    /**< ECG Service write event. */
    ECG_SERVICE_EVT_READ                                      /**< ECG Service read event. */
} ecg_service_evt_type_t;

/**@brief Custom Service event for ECG Service */
typedef struct
{
    ecg_service_evt_type_t evt_type;                          /**< Type of event. */
} ecg_service_evt_t;

typedef struct ble_ecg_service_s ble_ecg_service_t;           /**< Forward declaration of the ble_cus_t type */

/**@brief ECG Service event handler type */
typedef void (*ecg_service_evt_handler_t) (ble_ecg_service_t *p_bas, ecg_service_evt_t *p_evt);

/**@brief ECG Service structure. This contains various status information for the service. */
struct ble_ecg_service_s
{
    ecg_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the ECG Service. */
    uint16_t service_handle;                                  /**< Handle of ECG Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t ecg_char_handles;                /**< Handles related to the ECG characteristic. */
    uint16_t conn_handle;                                     /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type; 
};

/**@brief ECG Service init structure. This contains all options and data needed for initialization of the service.*/
typedef struct
{
    ecg_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the ECG Service. */
    uint8_t ecg_char[ECG_SERVICE_ECG_CHAR_LENGTH];            /**< Initial custom value array to store the ECG voltage data */
    ble_srv_cccd_security_mode_t ecg_char_attr_md;            /**< Initial security level for ECG characteristics attribute */
} ble_ecg_service_init_t;

uint32_t ble_ecg_service_initialize(ble_ecg_service_t *p_cus, const ble_ecg_service_init_t *p_cus_init);

uint32_t ecg_service_ecg_char_add(ble_ecg_service_t *p_cus, const ble_ecg_service_init_t *p_cus_init);

uint32_t ecg_service_ecg_char_write(ble_ecg_service_t *p_cus, uint8_t *new_ecg_char_array);

void ble_ecg_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);

static void _ecg_service_on_connect(ble_ecg_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void _ecg_service_on_disconnect(ble_ecg_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void _ecg_service_on_write(ble_ecg_service_t *p_cus, ble_evt_t const *p_ble_evt);

#endif

#endif // BLE_ECG_SERVICE_H__