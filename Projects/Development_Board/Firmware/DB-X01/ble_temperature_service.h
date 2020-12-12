#ifndef BLE_TEMPERATURE_SERVICE_H__
#define BLE_TEMPERATURE_SERVICE_H__

#include "ble_service.h"
#include "serial_slave.h"

// Macro for defining a ble instance
#define BLE_TEMPERATURE_SERVICE_DEF(_name) static ble_temperature_service_t _name; \
NRF_SDH_BLE_OBSERVER(_name ## _obs, BLE_HRS_BLE_OBSERVER_PRIO, ble_temperature_service_on_ble_evt, &_name)

#define TEMPERATURE_SERVICE_UUID_BASE {0xCE, 0x13, 0x10, 0xB0, 0xF0, 0x74, 0x3E, 0xA1, 0xEE, 0x45, 0x30, 0x38, 0x96, 0x6B, 0xAD, 0x8A}

enum TEMPERATURE_SERVICE_CHAR_LENGTHS
{
    TEMPERATURE_SERVICE_TEMP_CHAR_LENGTH = 250
};

// Defining the custom service 16 bit UUID
enum TEMPERATURE_SERVICE_UUID
{
    TEMPERATURE_SERVICE_UUID = 0x8CD5,
    TEMPERATURE_SERVICE_TEMP_CHAR_UUID = 0x46AD
};

typedef struct ble_temperature_service_s ble_temperature_service_t;   // Forward declaration of the ble_cus_t type.

/**@brief Temperature Service event type. */
typedef enum
{
    TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED,           /**< Temp char notification enabled event. */
    TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED,          /**< Temp char notification disabled event. */
    TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION,                   /**< Temp char notification event. */
    TEMPERATURE_SERVICE_EVT_DISCONNECTED,                             /**< Temperature Service disconnected event. */
    TEMPERATURE_SERVICE_EVT_CONNECTED,                                /**< Temperature Service connected event. */
    TEMPERATURE_SERVICE_EVT_WRITE,                                    /**< Temperature Service write event. */
    TEMPERATURE_SERVICE_EVT_READ                                      /**< Temperature Service read event. */
} temperature_service_evt_type_t;

/**@brief Custom Service event for Temperature Service. */
typedef struct
{
    temperature_service_evt_type_t evt_type;                                  /**< Type of event. */
} temperature_service_evt_t;


/**@brief Temperature Service event handler type. */
typedef void (*temperature_service_evt_handler_t) (ble_temperature_service_t *p_bas, temperature_service_evt_t *p_evt);

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_temperature_service_s
{
    temperature_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t service_handle;                                          /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t temp_char_handles;                       /**< Handles related to the Temp characteristic. */
    uint16_t conn_handle;                                             /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type; 
};

/**@brief Temperature Service init structure. This contains all options and data needed for initialization of the service.*/
typedef struct
{
    temperature_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t temp_char[TEMPERATURE_SERVICE_TEMP_CHAR_LENGTH];          /**< Temp Characteristic */
    ble_srv_cccd_security_mode_t temp_char_attr_md;                   /**< Initial security level for Temp characteristics attribute */
} ble_temperature_service_init_t;

void ble_temperature_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_temperature_service_initialize(ble_temperature_service_t *p_cus, const ble_temperature_service_init_t *p_cus_init);

uint32_t temperature_service_temp_char_add(ble_temperature_service_t *p_cus, const ble_temperature_service_init_t *p_cus_init);

uint32_t temperature_service_temp_char_write(ble_temperature_service_t *p_cus, uint8_t *new_temp_char_array);

static void temperature_service_on_connect(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void temperature_service_on_disconnect(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt);

static void temperature_service_on_write(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt);



#endif // BLE_TEMPERATURE_SERVICE_H__


