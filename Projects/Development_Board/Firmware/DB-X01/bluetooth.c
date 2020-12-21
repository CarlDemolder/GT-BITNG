#include "bluetooth.h"


#define APP_ADV_INTERVAL                320                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                36000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(10, UNIT_1_25_MS)         /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)         /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(8000, UNIT_10_MS)         /**< Connection supervisory timeout (10 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  RTC_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   RTC_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    1                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#if BOARD_VERSION == LP_ECG_X01
#define DEVICE_NAME                     "LP_ECG-X01"                                /**< Name of device. Will be included in the advertising data. */
#endif

#if BOARD_VERSION == DB_X02
#define DEVICE_NAME                     "DB-X02"                                /**< Name of device. Will be included in the advertising data. */
#endif

#if BOARD_VERSION == SG_X01
#define DEVICE_NAME                     "SG-X01"                                /**< Name of device. Will be included in the advertising data. */
#endif

#define MANUFACTURER_NAME               "GT-BITNG"                              /**< Manufacturer. Will be passed to Device Information Service. */


NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                             /**< Advertising module instance. */

BLE_CONFIGURATION_SERVICE_DEF(m_ble_configuration_service);                     /**< Declaring Configuration Service Structure for application */

#if TMP117
BLE_TEMPERATURE_SERVICE_DEF(m_ble_temperature_service);                         /**< Declaring Temperature Service Structure for application */
#endif

#if ECG
BLE_ECG_SERVICE_DEF(m_ble_ecg_service);                                         /**< Declaring ECG Service Structure for application */
#endif

ble_configuration_service_init_t ble_configuration_service_init = {0};

#if TMP117
ble_temperature_service_init_t ble_temperature_service_init = {0};
#endif

#if ECG
ble_ecg_service_init_t ble_ecg_service_init = {0};
#endif

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

// YOUR_JOB: Use UUIDs for service(s) used in your application.

#if ECG && !TMP117
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {ECG_SERVICE_UUID}};  /**< Universally unique service identifiers. */
#endif

#if !ECG && TMP117
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {TEMPERATURE_SERVICE_UUID}};  /**< Universally unique service identifiers. */
#endif

#if ECG && TMP117
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {TEMPERATURE_SERVICE_UUID}, {ECG_SERVICE_UUID}};  /**< Universally unique service identifiers. */
#endif


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for the Peer Manager initialization.
 */
void peer_manager_init(void)
{
    NRF_LOG_INFO("peer_manager_init");
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
void pm_evt_handler(pm_evt_t const * p_evt)
{
    NRF_LOG_INFO("pm_evt_handler");

    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
            NRF_LOG_INFO("Connected to a previously bonded device.");
            break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
            NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
            ble_conn_state_role(p_evt->conn_handle), p_evt->conn_handle, p_evt->params.conn_sec_succeeded.procedure);
            NRF_LOG_INFO("PM_EVT_CONN_SEC_SUCCEEDED");
            break;

        case PM_EVT_CONN_SEC_FAILED:
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
            NRF_LOG_INFO("PM_EVT_CONN_SEC_FAILED");
            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
            // Reject pairing request from an already bonded peer.
//            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
//            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
            NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
            break;

        case PM_EVT_STORAGE_FULL:
            // Run garbage collection on the flash.
            NRF_LOG_INFO("PM_EVT_STORAGE_FULL");
            err_code = fds_gc();
            if (err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            NRF_LOG_INFO("PM_EVT_PEERS_DELETE_SUCCEEDED");
            advertising_start();
            break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_FAILED");
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
            break;

        case PM_EVT_PEER_DELETE_FAILED:
            NRF_LOG_INFO("PM_EVT_PEER_DELETE_FAILED");
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
            break;

        case PM_EVT_ERROR_UNEXPECTED:
            NRF_LOG_INFO("PM_EVT_PEER_UNEXPECTED");
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
            break;

        case PM_EVT_CONN_SEC_START:
            NRF_LOG_INFO("PM_EVT_CONN_SEC_START");
            break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_SUCCEEDED");
            break;

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            NRF_LOG_INFO("PM_EVT_PEER_DELETE_SUCCEEDED");
            break;

        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
            NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLIED");
            break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
            // This can happen when the local DB has changed.
            NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED");
            break;

        case PM_EVT_SERVICE_CHANGED_IND_SENT:
            NRF_LOG_INFO("PM_EVT_SERVICE_CHANGED_IND_SENT");
            break;

        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
            NRF_LOG_INFO("PM_EVT_SERVICE_CHANGED_IND_CONFIRMED");
            break;

        default:
          break;
    }
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    NRF_LOG_INFO("gap_params_init");
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
    if(err_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("Procedure request succeeded. Connection parameters will be negotiated as requested.");
    }
    else
    {
        NRF_LOG_INFO("Procedure request failed: %d", err_code);
    }
}

void gap_params_update(uint16_t m_conn_handle)
{
    NRF_LOG_INFO("gap_params_update");
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = ble_conn_params_change_conn_params(m_conn_handle, &gap_conn_params);
    APP_ERROR_CHECK(err_code);
    if(err_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("GAP connection parameters updated.");
    }
    else
    {
        NRF_LOG_INFO("Procedure request failed: %d", err_code);
    }
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    NRF_LOG_INFO("on_conn_params_evt");
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        NRF_LOG_INFO("On Connection Parameters Failed");
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    NRF_LOG_INFO("conn_params_init");
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    NRF_LOG_INFO("on_adv_evt");
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            break;

        case BLE_ADV_EVT_IDLE:
            NRF_LOG_INFO("BLE advertising idle.");
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
    NRF_LOG_INFO("advertising_init");
    ret_code_t err_code;
    ble_advertising_init_t adv_init;

    memset(&adv_init, 0, sizeof(adv_init));

    adv_init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    adv_init.advdata.include_appearance = true;
    adv_init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//    adv_init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
//    adv_init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    adv_init.config.ble_adv_fast_enabled = true;
    adv_init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    adv_init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

    adv_init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &adv_init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for setting the power level of the advertising
 */
void set_advertising_power(void)
{
    NRF_LOG_INFO("set_advertising_power");
    ret_code_t err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, 4);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
void advertising_start(void)
{
    NRF_LOG_INFO("advertising_start");
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function to stop advertising.
 */
void advertising_stop(void)
{
    NRF_LOG_INFO("advertising_stop");
    ret_code_t err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
void gatt_init(void)
{
    NRF_LOG_INFO("gatt_init");
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the BLE stack
 *
 * @details Initializes the SoftDevice and the BLE event interrupt
 */
void ble_stack_init(void)
{
    NRF_LOG_INFO("ble_stack_init");
    ret_code_t err_code;
    
    if(nrf_sdh_is_enabled())
    {
        NRF_LOG_INFO("SOFT DEVICE IS ENABLED");
    }
    else
    {
        NRF_LOG_INFO("SOFT DEVICE IS DISABLED");
    }

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    NRF_LOG_INFO("ble_evt_handler");
    ret_code_t err_code = NRF_SUCCESS;
    NRF_LOG_INFO("BLE Configuration Service Event Received. Event type = %d", p_ble_evt->header.evt_id); 

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED");
            break;

        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            gap_params_update(m_conn_handle); // Once all Services have been discovered, change the GAP Connection Parameters
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_PHY_UPDATE_REQUEST");
            ble_gap_phys_t const phys = {.rx_phys = BLE_GAP_PHY_AUTO, .tx_phys = BLE_GAP_PHY_AUTO};
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_INFO("BLE_GATTC_EVT_TIMEOUT");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_INFO("BLE_GATTS_EVT_TIMEOUT");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_WRITE:
            NRF_LOG_INFO("BLE_GATTS_EVT_WRITE");
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling the Configuration Service events.
 *
 * @details This function will be called for all Configuration Service events which are passed to
 *          the application.
 *
 * @param[in]   p_cus_service  Configuration Service structure
 * @param[in]   p_evt          Event received from the Configuration Service
 *
 */
static void on_configuration_service_evt(ble_configuration_service_t *p_cus_service, configuration_service_evt_t *p_evt)
{
    ret_code_t err_code;
    NRF_LOG_INFO("on_configuration_service_evt");
    switch(p_evt->evt_type)
    {
        case CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_ENABLED");
            break;

        case CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_DISABLED");
            break;

        case CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_ENABLED");
            break;

        case CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_DISABLED");
            break;
        
        case CONFIGURATION_SERVICE_EVT_SETTINGS_CHAR_WRITE:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_SETTINGS_CHAR_WRITE");
            configuration_service_settings_char_read(p_cus_service, ble_configuration_service_init.settings_char);
            bluetooth_configuration_service_settings_char_read(ble_configuration_service_init.settings_char);
            uint8_t response_char_ack_data_array[10] = {BLUETOOTH_COMMAND_HEADER, BLUETOOTH_MODULE, BLUETOOTH_WRITE_RESPONSE_CHAR_COMMAND, 
                0X00, BLUETOOTH_RESPONSE_CHAR_MESSAGE_RECEIVED, 0X00, 0X00, 0X00, 0X00, BLUETOOTH_COMMAND_FOOTER};
            bluetooth_handler(response_char_ack_data_array);
            break;

        default:
              break;
    }
}

/**@brief Function for handling the Temperature Service Service events.
 *
 * @details This function will be called for all Temperature Service events which are passed to
 *          the application.
 *
 * @param[in]   p_cus_service  Temperature Service structure.
 * @param[in]   p_evt          Event received from the Temperature Service.
 *
 */
 #if TMP117
static void on_temperature_service_evt(ble_temperature_service_t *p_cus_service, temperature_service_evt_t *p_evt)
{
    ret_code_t err_code;
    NRF_LOG_INFO("on_temperature_service_evt");
    switch(p_evt->evt_type)
    {
        case TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_CONNECTED");
            break;

        case TEMPERATURE_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_DISCONNECTED");
            break;
        
        case TEMPERATURE_SERVICE_EVT_WRITE:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_WRITE");
            break;

        default:
              break;
    }
}
#endif

/**@brief Function for handling the ECG Service Service events.
 *
 * @details This function will be called for all ECG Service events which are passed to
 *          the application.
 *
 * @param[in]   p_cus_service  ECG Service structure
 * @param[in]   p_evt          Event received from the ECG Service
 *
 */
#if ECG
static void on_ecg_service_evt(ble_ecg_service_t *p_cus_service, ecg_service_evt_t *p_evt)
{
    ret_code_t err_code;
    NRF_LOG_INFO("on_ecg_service_evt");
    switch(p_evt->evt_type)
    {
        case ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED");
            break;

        case ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_DISABLED");
            break;

        case ECG_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_CONNECTED");
            break;

        case ECG_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_DISCONNECTED");
            break;
        
        case ECG_SERVICE_EVT_WRITE:
            NRF_LOG_INFO("ECG_SERVICE_EVT_WRITE");
            break;

        default:
              break;
    }
}
#endif

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    NRF_LOG_INFO("services_init");
    ret_code_t err_code;
    nrf_ble_qwr_init_t qwr_init = {0};
    
    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    ble_configuration_service_init.evt_handler = on_configuration_service_evt;    // Initialize Configuration Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.crc_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.crc_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.write_perm);

    err_code = ble_configuration_service_initialize(&m_ble_configuration_service, &ble_configuration_service_init);
    APP_ERROR_CHECK(err_code);

    #if TMP117
    ble_temperature_service_init.evt_handler = on_temperature_service_evt;        // Initialize Temperature Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.temp_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.temp_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.write_perm);

    err_code = ble_temperature_service_initialize(&m_ble_temperature_service, &ble_temperature_service_init);
    APP_ERROR_CHECK(err_code);
    #endif

    #if ECG
    ble_ecg_service_init.evt_handler = on_ecg_service_evt;                        // Initialize ECG Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.ecg_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.ecg_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.write_perm);

    err_code = ble_ecg_service_initialize(&m_ble_ecg_service, &ble_ecg_service_init);
    APP_ERROR_CHECK(err_code);
    #endif
}

void bluetooth_configuration_service_settings_char_read(uint8_t *settings_char_data_array)
{
    NRF_LOG_INFO("bluetooth_configuration_service_settings_char_read");
    bluetooth_handler(settings_char_data_array);
}

void bluetooth_configuration_service_response_char_write(uint8_t *response_char_data_array)
{
    NRF_LOG_INFO("bluetooth_configuration_service_response_char_write");
    uint32_t err_code;
    memcpy(ble_configuration_service_init.response_char, response_char_data_array, CONFIGURATION_SERVICE_RESPONSE_CHAR_LENGTH);
    err_code = configuration_service_response_char_write(&m_ble_configuration_service, response_char_data_array);   // Update the Response Characteristic
    APP_ERROR_CHECK(err_code);
}

void bluetooth_configuration_service_crc_char_write(uint8_t *crc_char_data_array)
{
    NRF_LOG_INFO("bluetooth_configuration_service_crc_char_write");
    uint32_t err_code;
    memcpy(ble_configuration_service_init.crc_char, crc_char_data_array, CONFIGURATION_SERVICE_CRC_CHAR_LENGTH);
    err_code = configuration_service_crc_char_write(&m_ble_configuration_service, crc_char_data_array);   // Update the CRC Characteristic
    APP_ERROR_CHECK(err_code);
}

#if TMP117
void bluetooth_temperature_service_temp_char_write(uint8_t *temp_char_data_array)
{
    NRF_LOG_INFO("bluetooth_temperature_service_temp_char_write");
    uint32_t err_code;
    memcpy(ble_temperature_service_init.temp_char, temp_char_data_array, TEMPERATURE_SERVICE_TEMP_CHAR_LENGTH);
    err_code = temperature_service_temp_char_write(&m_ble_temperature_service, temp_char_data_array);   // Update the Temp Characteristic
    APP_ERROR_CHECK(err_code);
}
#endif

#if ECG
void bluetooth_ecg_service_ecg_char_write(uint8_t *ecg_char_data_array)
{
    NRF_LOG_INFO("bluetooth_ecg_service_ecg_char_write");
    uint32_t err_code;
    memcpy(ble_ecg_service_init.ecg_char, ecg_char_data_array, ECG_SERVICE_ECG_CHAR_LENGTH);
    err_code = ecg_service_ecg_char_write(&m_ble_ecg_service, ecg_char_data_array);   // Update the ECG Characteristic
    APP_ERROR_CHECK(err_code);
}
#endif

#if ECG
void bluetooth_transmit_ecg_recording_session(void)
{
    NRF_LOG_INFO("bluetooth_transmit_recording_session");
    uint8_t temp_data_array[64];
    ecg_get_data_packet(temp_data_array, 64);

    ecg_stop_recording_session();
}
#endif

#if TMP117
void bluetooth_transmit_temperature_recording_session(void)
{
//    NRF_LOG_INFO("bluetooth_transmit_recording_session");
//    uint8_t temp_data_array[64];
//    ecg_get_data_packet(temp_data_array, 64);
//
//    ecg_stop_recording_session();
}
#endif

uint8_t bluetooth_get_bytes_per_transmission(void)
{
    NRF_LOG_INFO("bluetooth_get_bytes_per_transmission");
    uint8_t temp = 250;
    return temp;
}

void bluetooth_transmit_firmware_version(void)
{
    NRF_LOG_INFO("bluetooth_transmit_firmware_version");
    uint8_t response_char_data_array[2] = {0x00, FIRMWARE_VERSION};
    bluetooth_configuration_service_response_char_write(response_char_data_array);
}

void bluetooth_transmit_hardware_board_version(void)
{
    NRF_LOG_INFO("bluetooth_transmit_hardware_board_version");
    uint8_t response_char_data_array[2] = {0x00, BOARD_VERSION};
    bluetooth_configuration_service_response_char_write(response_char_data_array);
}