#ifndef BLE_CONFIGURATION_SERVICE_H__
#define BLE_CONFIGURATION_SERVICE_H__

#include "ble_service.h"
#include "serial_slave.h"

// Macro for defining a ble instance
#define BLE_CONFIGURATION_SERVICE_DEF(_name) static ble_configuration_service_t _name; \
NRF_SDH_BLE_OBSERVER(_name ## _obs, BLE_HRS_BLE_OBSERVER_PRIO, ble_configuration_service_on_ble_evt, &_name)

#define CONFIGURATION_SERVICE_UUID_BASE {0xCE, 0x13, 0x10, 0xB0, 0xF0, 0x74, 0x3E, 0xA1, 0xEE, 0x45, 0x30, 0x38, 0x96, 0x6B, 0xAD, 0x8A}

enum CONFIGURATION_SERVICE_CHAR_LENGTHS
{
    CONFIGURATION_SERVICE_SETTINGS_CHAR_LENGTH = 10,
    CONFIGURATION_SERVICE_RESPONSE_CHAR_LENGTH = 2,
    CONFIGURATION_SERVICE_CRC_CHAR_LENGTH = 2,
};

// Defining the custom service 16 bit UUID
enum CONFIGURATION_SERVICE_UUID
{
    CONFIGURATION_SERVICE_UUID = 0x8CD4,
    CONFIGURATION_SETTINGS_CHAR_UUID = 0x46AA,
    CONFIGURATION_RESPONSE_CHAR_UUID = 0x46AB,
    CONFIGURATION_CRC_CHAR_UUID = 0x46AC
};

typedef struct ble_configuration_service_s ble_configuration_service_t; // Forward declaration of the ble_cus_t type.

/**@brief Configuration Service event type. */
typedef enum
{
    CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_ENABLED,       /**< Response notification enabled event. */
    CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_DISABLED,      /**< Response notification disabled event. */
    CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION,               /**< Response notification event. */
    CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_ENABLED,            /**< CRC notification enabled event. */
    CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_DISABLED,           /**< CRC notification disabled event. */
    CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION,                    /**< CRC notification event. */
    CONFIGURATION_SERVICE_EVT_DISCONNECTED,                             /**< Configuration Service disconnected event. */
    CONFIGURATION_SERVICE_EVT_CONNECTED,                                /**< Configuration Service connected event. */
    CONFIGURATION_SERVICE_EVT_SETTINGS_CHAR_WRITE,                      /**< Configuration Service write event. */
    CONFIGURATION_SERVICE_EVT_READ                                      /**< Configuration Service read event. */
} configuration_service_evt_type_t;

/**@brief Configuration Service event for the Configuration Service. */
typedef struct
{
    configuration_service_evt_type_t evt_type;                          /**< Type of event. */
} configuration_service_evt_t;


/**@brief Configuration Service event handler type. */
typedef void (*configuration_service_evt_handler_t) (ble_configuration_service_t *p_bas, configuration_service_evt_t *p_evt);

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_configuration_service_s
{
    configuration_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t service_handle;                                            /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t settings_char_handles;                     /**< Handles related to the Settings characteristic. */
    ble_gatts_char_handles_t response_char_handles;                     /**< Handles related to the Response characteristic. */
    ble_gatts_char_handles_t crc_char_handles;                          /**< Handles related to the CRC characteristic. */
    uint16_t conn_handle;                                               /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type; 
};

/**@brief Configuration Service init structure. This contains all options and data needed for initialization of the service.*/
typedef struct
{
    configuration_service_evt_handler_t evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t settings_char[CONFIGURATION_SERVICE_SETTINGS_CHAR_LENGTH];  /**< Characteristic to communicate the settings for the patch */
    uint8_t response_char[CONFIGURATION_SERVICE_RESPONSE_CHAR_LENGTH];  /**< Characteristic to communicate the response from the patch */
    uint8_t crc_char[CONFIGURATION_SERVICE_CRC_CHAR_LENGTH];            /**< Characteristic to communicate the crc value to ensure data integrity*/
    ble_srv_cccd_security_mode_t settings_char_attr_md;                 /**< Initial security level for Settings characteristics attribute */
    ble_srv_cccd_security_mode_t response_char_attr_md;                 /**< Initial security level for Response characteristics attribute */
    ble_srv_cccd_security_mode_t crc_char_attr_md;                      /**< Initial security level for CRC characteristics attribute */
} ble_configuration_service_init_t;

uint32_t ble_configuration_service_initialize(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init);

uint32_t configuration_service_settings_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init);
uint32_t configuration_service_response_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init);
uint32_t configuration_service_crc_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init);

uint32_t configuration_service_settings_char_read(ble_configuration_service_t *p_cus, uint8_t *new_setting_char_array);
uint32_t configuration_service_response_char_write(ble_configuration_service_t *p_cus, uint8_t *new_response_char_array);
uint32_t configuration_service_crc_char_write(ble_configuration_service_t *p_cus, uint8_t *new_crc_char_array);

void ble_configuration_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);

static void _configuration_service_on_connect(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt);
static void _configuration_service_on_disconnect(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt);
static void _configuration_service_on_write(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt);

#endif // BLE_CONFIGURATION_H__


