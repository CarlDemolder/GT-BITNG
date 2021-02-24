#ifndef BLE_PRESSURE_SERVICE_H__
#define BLE_PRESSURE_SERVICE_H__

#include "ble_service.h"
#include "serial_slave.h"

#if FDC1004

// Macro for defining a ble instance
#define BLE_PRESSURE_SERVICE_DEF(_name) static ble_pressure_service_t _name; \
NRF_SDH_BLE_OBSERVER(_name ## _obs, BLE_HRS_BLE_OBSERVER_PRIO, ble_pressure_service_on_ble_evt, &_name)

#define PRESSURE_SERVICE_UUID_BASE {0xCE, 0x13, 0x10, 0xB0, 0xF0, 0x74, 0x3E, 0xA1, 0xEE, 0x45, 0x30, 0x38, 0x96, 0x6B, 0xAD, 0x8A}

enum PRESSURE_SERVICE_CHAR_LENGTHS
{
    PRESSURE_SERVICE_INSTANT_PRESSURE_CHAR_LENGTH = 12,
};

// Defining the custom service 16 bit UUID
enum PRESSURE_SERVICE_UUID
{
    PRESSURE_SERVICE_UUID = 0x8CD7,
    PRESSURE_SERVICE_INSTANT_PRESSURE_CHAR_UUID = 0x46B0
};

typedef struct ble_pressure_service_s ble_pressure_service_t;   // Forward declaration of the ble_cus_t type.

/**@brief Pressure Service event type. */
typedef enum
{
    PRESSURE_SERVICE_EVT_INSTANT_PRESSURE_CHAR_NOTIFICATION_ENABLED,            /**< Pressure char notification enabled event. */
    PRESSURE_SERVICE_EVT_INSTANT_PRESSURE_CHAR_NOTIFICATION_DISABLED,           /**< Pressure char notification disabled event. */
    PRESSURE_SERVICE_EVT_INSTANT_PRESSURE_CHAR_NOTIFICATION,                    /**< Pressure char notification event. */
    PRESSURE_SERVICE_EVT_DISCONNECTED,                                          /**< Pressure Service disconnected event. */
    PRESSURE_SERVICE_EVT_CONNECTED,                                             /**< Pressure Service connected event. */
    PRESSURE_SERVICE_EVT_WRITE,                                                 /**< Pressure Service write event. */
    PRESSURE_SERVICE_EVT_READ                                                   /**< Pressure Service read event. */
} pressure_service_evt_type_t;

/**@brief Custom Service event for Pressure Service. */
typedef struct
{
    pressure_service_evt_type_t evt_type;                                  /**< Type of event. */
} pressure_service_evt_t;


/**@brief Pressure Service event handler type. */
typedef void (*pressure_service_evt_handler_t) (ble_pressure_service_t *p_bas, pressure_service_evt_t *p_evt);

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_pressure_service_s
{
    pressure_service_evt_handler_t evt_handler;                                 /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t service_handle;                                                    /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t instant_pressure_char_handles;                     /**< Handles related to the Instant Pressure characteristic. */
    uint16_t conn_handle;                                                       /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type; 
};

struct BLE_Pressure_Service_Control_Struct
{
    ret_code_t error_code;
};

/**@brief Pressure Service init structure. This contains all options and data needed for initialization of the service.*/
typedef struct
{
    pressure_service_evt_handler_t evt_handler;                                       /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t instant_pressure_char[PRESSURE_SERVICE_INSTANT_PRESSURE_CHAR_LENGTH];     /**< Pressure Characteristic */
    ble_srv_cccd_security_mode_t instant_pressure_char_attr_md;                       /**< Initial security level for Instant Pressure characteristics attribute */
} ble_pressure_service_init_t;

void ble_pressure_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_pressure_service_initialize(ble_pressure_service_t *p_cus, const ble_pressure_service_init_t *p_cus_init);

uint32_t pressure_service_instant_pressure_char_add(ble_pressure_service_t *p_cus, const ble_pressure_service_init_t *p_cus_init);

uint32_t pressure_service_instant_pressure_char_write(ble_pressure_service_t *p_cus, uint8_t *new_instant_pressure_char_array);

static void pressure_service_on_connect(ble_pressure_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void pressure_service_on_disconnect(ble_pressure_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void pressure_service_on_write(ble_pressure_service_t *p_cus, ble_evt_t const *p_ble_evt);

#endif

#endif // BLE_PRESSURE_SERVICE_H__


