#include "bluetooth.h"

#define APP_ADV_INTERVAL                320                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */
#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(125, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 seconds). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

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
#define DEVICE_NAME                     "LP_ECG-X01"                                /**< Name of device. Will be included in the advertising data. */
#endif

#if BOARD_VERSION == SG_X01
#define DEVICE_NAME                     "SG-X01"                                /**< Name of device. Will be included in the advertising data. */
#endif

#if BOARD_VERSION == SG_PRESSURE_X01
#define DEVICE_NAME                     "SG-P-X01"                                /**< Name of device. Will be included in the advertising data. */
#endif

#define MANUFACTURER_NAME               "GT-BITNG"                              /**< Manufacturer. Will be passed to Device Information Service. */


NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                             /**< Advertising module instance. */

BLE_CONFIGURATION_SERVICE_DEF(m_ble_configuration_service);                     /**< Declaring Configuration Service Structure for application */

#if TMP117
BLE_TEMPERATURE_SERVICE_DEF(m_ble_temperature_service);                         /**< Declaring Temperature Service Structure for application */
ble_temperature_service_init_t ble_temperature_service_init = {0};
#endif

#if MAX30003
BLE_ECG_SERVICE_DEF(m_ble_ecg_service);                                         /**< Declaring ECG Service Structure for application */
ble_ecg_service_init_t ble_ecg_service_init = {0};
#endif

#if FDC1004
BLE_PRESSURE_SERVICE_DEF(m_ble_pressure_service);                               /**< Declaring Pressure Service Structure for application */
ble_pressure_service_init_t ble_pressure_service_init = {0};
#endif

ble_configuration_service_init_t ble_configuration_service_init = {0};

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

// YOUR_JOB: Use UUIDs for service(s) used in your application.

#if MAX30003 && !TMP117 && !FDC1004
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {ECG_SERVICE_UUID}};  /**< Universally unique service identifiers. */
#endif

#if !MAX30003 && TMP117 && !FDC1004
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {TEMPERATURE_SERVICE_UUID}};  /**< Universally unique service identifiers. */
#endif

#if MAX30003 && TMP117 && !FDC1004
/**< Universally unique service identifiers. */
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {TEMPERATURE_SERVICE_UUID}, {ECG_SERVICE_UUID}};  
#endif

#if MAX30003 && TMP117 && FDC1004
/**< Universally unique service identifiers. */
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {TEMPERATURE_SERVICE_UUID}, {ECG_SERVICE_UUID}, {PRESSURE_SERVICE_UUID}}; 
#endif

#if !MAX30003 && !TMP117 && FDC1004
/**< Universally unique service identifiers. */
ble_uuid_t m_adv_uuids[] ={{CONFIGURATION_SERVICE_UUID}, {PRESSURE_SERVICE_UUID}}; 
#endif

// Bluetooth control struct used to control common variables for the class
static struct Bluetooth_Control_Struct control;

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void _bluetooth_nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for the Peer Manager initialization.
 */
void bluetooth_peer_manager_init(void)
{
    NRF_LOG_INFO("peer_manager_init");
    ble_gap_sec_params_t sec_param;

    control.error_code = pm_init();
    APP_ERROR_CHECK(control.error_code);

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

    control.error_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(control.error_code);

    control.error_code = pm_register(_bluetooth_pm_evt_handler);
    APP_ERROR_CHECK(control.error_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void _bluetooth_pm_evt_handler(pm_evt_t const * p_evt)
{
    NRF_LOG_INFO("pm_evt_handler");

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
            control.error_code = fds_gc();
            if (control.error_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(control.error_code);
            }
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            NRF_LOG_INFO("PM_EVT_PEERS_DELETE_SUCCEEDED");
            bluetooth_advertising_start();
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

/** @brief Clear bonding information from persistent storage.
 */
static void _bluetooth_delete_bonds(void)
{
    NRF_LOG_INFO("_bluetooth_delete_bonds");

    control.error_code = pm_peers_delete();
    APP_ERROR_CHECK(control.error_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void bluetooth_gap_params_init(void)
{
    NRF_LOG_INFO("gap_params_init");
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    control.error_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(control.error_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       control.error_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(control.error_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    control.error_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(control.error_code);
    if(control.error_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("Procedure request succeeded. Connection parameters will be negotiated as requested.");
    }
    else
    {
        NRF_LOG_INFO("Procedure request failed: %d", control.error_code);
    }
}

void bluetooth_gap_params_update(uint16_t m_conn_handle)
{
    NRF_LOG_INFO("gap_params_update");
    ble_gap_conn_params_t gap_conn_params;

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    control.error_code = ble_conn_params_change_conn_params(m_conn_handle, &gap_conn_params);
    APP_ERROR_CHECK(control.error_code);
    if(control.error_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("GAP connection parameters updated.");
    }
    else
    {
        NRF_LOG_INFO("Procedure request failed: %d", control.error_code);
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
static void _bluetooth_on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    NRF_LOG_INFO("on_conn_params_evt");

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        NRF_LOG_INFO("BLE_CONN_PARAMS_EVT_FAILED");
        control.error_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(control.error_code);
    }
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_SUCCEEDED)
    {
        NRF_LOG_INFO("BLE_CONN_PARAMS_EVT_SUCCEEDED");
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void _bluetooth_conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
void bluetooth_conn_params_init(void)
{
    NRF_LOG_INFO("conn_params_init");
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = _bluetooth_on_conn_params_evt;
    cp_init.error_handler = _bluetooth_conn_params_error_handler;

    control.error_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(control.error_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void _bluetooth_on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    NRF_LOG_INFO("_bluetooth_on_adv_evt");

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
        {
            NRF_LOG_INFO("BLE_ADV_EVT_FAST");

            #if DEBUG_MODE
            uint8_t ble_led_on_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_BLE_LED_ON};  
            state_handler(ble_led_on_command); // BLE LED on during advertising 
            #endif

            break;
        }

        case BLE_ADV_EVT_IDLE:
        {
            NRF_LOG_INFO("BLE_ADV_EVT_IDLE");

            #if DEBUG_MODE
            uint8_t ble_led_off_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_BLE_LED_OFF};  
            state_handler(ble_led_off_command); // BLE LED off after advertising timeout
            #endif

            disable_vcc_ldo();
            break;
        }

        default:
            break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 */
void bluetooth_advertising_init(void)
{
    NRF_LOG_INFO("advertising_init");
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

    adv_init.config.ble_adv_on_disconnect_disabled = true;      //ADD THIS LINE TO PREVENT ADVERTISING ON THE DISCONNECTED EVENT

    adv_init.evt_handler = _bluetooth_on_adv_evt;

    control.error_code = ble_advertising_init(&m_advertising, &adv_init);
    APP_ERROR_CHECK(control.error_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for setting the power level of the advertising
 */
void bluetooth_set_transmitting_power(uint8_t power_level)
{
    NRF_LOG_INFO("bluetooth_set_transmitting_power");
    int8_t tx_power_values[9] = {-40, -20, -16, -12, -8, -4, 0, 3, 4}; // -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
    control.error_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, tx_power_values[power_level]);
    APP_ERROR_CHECK(control.error_code);
}

/**@brief Function for starting advertising.
 */
void bluetooth_advertising_start(void)
{
    NRF_LOG_INFO("bluetooth_advertising_start");
    control.error_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(control.error_code);
}

/**@brief Function to stop advertising.
 */
void bluetooth_advertising_stop(void)
{
    NRF_LOG_INFO("bluetooth_advertising_stop");
    control.error_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
    APP_ERROR_CHECK(control.error_code);
}

void bluetooth_advertising_restart(void)
{
    NRF_LOG_INFO("bluetooth_advertising_restart");
    control.error_code = sd_ble_gap_adv_start(m_advertising.adv_handle, APP_BLE_CONN_CFG_TAG);

    #if DEBUG_MODE
    uint8_t ble_led_on_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_BLE_LED_ON};  
    state_handler(ble_led_on_command); // BLE LED on during advertising 
    #endif
    
    APP_ERROR_CHECK(control.error_code);
}

void bluetooth_enable_advertising_after_disconnection(void)
{
    NRF_LOG_INFO("bluetooth_enable_advertising_after_disconnection");
    control.advertising_after_disconnection_flag = true;
}

/**@brief Function for initializing the GATT module.
 */
void bluetooth_gatt_init(void)
{
    NRF_LOG_INFO("bluetooth_gatt_init");
    control.error_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(control.error_code);
}

/**@brief Function for initializing the BLE stack
 *
 * @details Initializes the SoftDevice and the BLE event interrupt
 */
void bluetooth_ble_stack_init(void)
{
    NRF_LOG_INFO("bluetooth_ble_stack_init");
    
    if(nrf_sdh_is_enabled())
    {
        NRF_LOG_INFO("SOFT DEVICE IS ENABLED");
    }
    else
    {
        NRF_LOG_INFO("SOFT DEVICE IS DISABLED");
    }

    control.error_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(control.error_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    control.error_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(control.error_code);

    // Enable BLE stack.
    control.error_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(control.error_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, _bluetooth_ble_evt_handler, NULL);
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void _bluetooth_ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    NRF_LOG_INFO("ble_evt_handler");
    control.error_code = NRF_SUCCESS;
    NRF_LOG_INFO("BLE Configuration Service Event Received. Event type = %d", p_ble_evt->header.evt_id); 

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED");
            if(control.advertising_after_disconnection_flag)
            {
                bluetooth_advertising_restart();
            }
            else
            {
                uint8_t medium_blink_ind_led_command[5] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_IND_BLINK, NRF52_LED_IND_MEDIUM_BLINK};  
                state_handler(medium_blink_ind_led_command); // BLE LED Medium Blink 
                disable_vcc_ldo();
            }

            control.connection_flag = false;
            
            #if DEBUG_MODE
            uint8_t ind_led_off_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_IND_LED_OFF};  
            state_handler(ind_led_off_command); // IND LED off once disconnected 
            #endif

            break;
        }

        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            control.error_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(control.error_code);
            
            bluetooth_gap_params_update(m_conn_handle); // Once all Services have been discovered, change the GAP Connection Parameters
            
            control.connection_flag = true;
            
            #if DEBUG_MODE
            uint8_t ble_led_off_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_BLE_LED_OFF};  
            state_handler(ble_led_off_command); // BLE LED off once connected
             
            uint8_t ind_led_on_command[4] = {0x00, NRF52_MODULE, NRF52_COMMON_COMMAND, NRF52_LED_IND_LED_ON};  
            state_handler(ind_led_on_command); // IND LED on once connected 
            #endif
            
            break;
        }

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_PHY_UPDATE_REQUEST");
            ble_gap_phys_t const phys = {.rx_phys = BLE_GAP_PHY_AUTO, .tx_phys = BLE_GAP_PHY_AUTO};
            control.error_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(control.error_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_INFO("BLE_GATTC_EVT_TIMEOUT");
            control.error_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(control.error_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_INFO("BLE_GATTS_EVT_TIMEOUT");
            control.error_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(control.error_code);
            break;

       case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            NRF_LOG_INFO("BLE_GATTS_EVT_SYS_ATTR_MISSING");
            control.error_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gatts_evt.conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(control.error_code);
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
static void _bluetooth_on_configuration_service_evt(ble_configuration_service_t *p_cus_service, configuration_service_evt_t *p_evt)
{
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
            break;

        case CONFIGURATION_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_CONNECTED");
            break;

        case CONFIGURATION_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT_DISCONNECTED");
            control.connection_flag = false;
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
static void _bluetooth_on_temperature_service_evt(ble_temperature_service_t *p_cus_service, temperature_service_evt_t *p_evt)
{
    NRF_LOG_INFO("on_temperature_service_evt");
    switch(p_evt->evt_type)
    {
        case TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_ENABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_DISABLED");
            break;

        case TEMPERATURE_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_CONNECTED");
            break;

        case TEMPERATURE_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_DISCONNECTED");
            control.connection_flag = false;

            uint8_t stop_tmp117_data_collection_command[3] = {0X00, TMP117_MODULE, TMP117_STOP_DATA_COLLECTION_COMMAND};
            state_handler(stop_tmp117_data_collection_command); // If the phone disconnects from the patch, stop collecting data

            uint8_t stop_rtc_timer_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_SENSOR_STOP};
            state_handler(stop_rtc_timer_command); // If the phone disconnects from the patch, stop the RTC to collect data from sensors
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
#if MAX30003
static void _bluetooth_on_ecg_service_evt(ble_ecg_service_t *p_cus_service, ecg_service_evt_t *p_evt)
{
    NRF_LOG_INFO("on_ecg_service_evt");
    switch(p_evt->evt_type)
    {
        case ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED");
            break;

        case ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_DISABLED");
            break;

        case ECG_SERVICE_EVT_INSTANT_ECG_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_ECG_CHAR_NOTIFICATION_ENABLED");
            break;

        case ECG_SERVICE_EVT_INSTANT_ECG_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_INSTANT_ECG_CHAR_NOTIFICATION_DISABLED");
            break;

        case ECG_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_CONNECTED");
            break;

        case ECG_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("ECG_SERVICE_EVT_DISCONNECTED");
            control.connection_flag = false;

            uint8_t stop_max30003_data_collection_command[3] = {0X00, MAX30003_MODULE, MAX30003_STOP_DATA_COLLECTION_COMMAND};
            state_handler(stop_max30003_data_collection_command); // If the phone disconnects from the patch, stop collecting data
            break;
        
        case ECG_SERVICE_EVT_WRITE:
            NRF_LOG_INFO("ECG_SERVICE_EVT_WRITE");
            break;

        default:
              break;
    }
}
#endif

/**@brief Function for handling the Pressure Service Service events.
 *
 * @details This function will be called for all Pressure Service events which are passed to
 *          the application.
 *
 * @param[in]   p_cus_service  Pressure Service structure.
 * @param[in]   p_evt          Event received from the Pressure Service.
 *
 */
 #if FDC1004
static void _bluetooth_on_pressure_service_evt(ble_pressure_service_t *p_cus_service, pressure_service_evt_t *p_evt)
{
    NRF_LOG_INFO("on_pressure_service_evt");
    switch(p_evt->evt_type)
    {
        case PRESSURE_SERVICE_EVT_INSTANT_PRESSURE_CHAR_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("PRESSURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED");
            break;

        case PRESSURE_SERVICE_EVT_INSTANT_PRESSURE_CHAR_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("PRESSURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED");
            break;

        case PRESSURE_SERVICE_EVT_CONNECTED:
            NRF_LOG_INFO("PRESSURE_SERVICE_EVT_CONNECTED");
            break;

        case PRESSURE_SERVICE_EVT_DISCONNECTED:
            NRF_LOG_INFO("PRESSURE_SERVICE_EVT_DISCONNECTED");
            control.connection_flag = false;

            break;
        
        case PRESSURE_SERVICE_EVT_WRITE:
            NRF_LOG_INFO("PRESSURE_SERVICE_EVT_WRITE");
            break;

        default:
              break;
    }
}
#endif



/**@brief Function for initializing services that will be used by the application.
 */
void bluetooth_services_init(void)
{
    NRF_LOG_INFO("services_init");
    nrf_ble_qwr_init_t qwr_init = {0};
    ble_dfu_buttonless_init_t dfus_init = {0};
    
    // Initialize Queued Write Module.
    qwr_init.error_handler = _bluetooth_nrf_qwr_error_handler;

    control.error_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(control.error_code);

    // DFU Related Event Handlers
    dfus_init.evt_handler = _bluetooth_dfu_service_evt_handler;

    control.error_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(control.error_code);

    ble_configuration_service_init.evt_handler = _bluetooth_on_configuration_service_evt;    // Initialize Configuration Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.settings_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.response_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.crc_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.crc_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_configuration_service_init.crc_char_attr_md.write_perm);

    control.error_code = ble_configuration_service_initialize(&m_ble_configuration_service, &ble_configuration_service_init);
    APP_ERROR_CHECK(control.error_code);

    #if TMP117
    ble_temperature_service_init.evt_handler = _bluetooth_on_temperature_service_evt;        // Initialize Temperature Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.temp_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.temp_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.temp_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.instant_temp_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.instant_temp_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_temperature_service_init.instant_temp_char_attr_md.write_perm);

    control.error_code = ble_temperature_service_initialize(&m_ble_temperature_service, &ble_temperature_service_init);
    APP_ERROR_CHECK(control.error_code);
    #endif

    #if MAX30003
    ble_ecg_service_init.evt_handler = _bluetooth_on_ecg_service_evt;                        // Initialize ECG Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.ecg_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.ecg_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.ecg_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.instant_ecg_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.instant_ecg_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_ecg_service_init.instant_ecg_char_attr_md.write_perm);

    control.error_code = ble_ecg_service_initialize(&m_ble_ecg_service, &ble_ecg_service_init);
    APP_ERROR_CHECK(control.error_code);
    #endif

    #if FDC1004
    ble_pressure_service_init.evt_handler = _bluetooth_on_pressure_service_evt;        // Initialize Pressure Service

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_pressure_service_init.instant_pressure_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_pressure_service_init.instant_pressure_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&ble_pressure_service_init.instant_pressure_char_attr_md.write_perm);

    control.error_code = ble_pressure_service_initialize(&m_ble_pressure_service, &ble_pressure_service_init);
    APP_ERROR_CHECK(control.error_code);
    #endif
}

void bluetooth_configuration_service_settings_char_read(uint8_t *settings_char_data)
{
    NRF_LOG_INFO("bluetooth_configuration_service_settings_char_read");
    bluetooth_handler(settings_char_data);
}

void bluetooth_configuration_service_response_char_write(uint8_t *response_char_data)
{
    NRF_LOG_INFO("bluetooth_configuration_service_response_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_configuration_service_init.response_char, response_char_data, CONFIGURATION_SERVICE_RESPONSE_CHAR_LENGTH);
        control.error_code = configuration_service_response_char_write(&m_ble_configuration_service, response_char_data);   // Update the Response Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}

void bluetooth_configuration_service_crc_char_write(uint8_t *crc_char_data)
{
    NRF_LOG_INFO("bluetooth_configuration_service_crc_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_configuration_service_init.crc_char, crc_char_data, CONFIGURATION_SERVICE_CRC_CHAR_LENGTH);
        control.error_code = configuration_service_crc_char_write(&m_ble_configuration_service, crc_char_data);   // Update the CRC Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}

#if TMP117
void bluetooth_temperature_service_temp_char_write(uint8_t *temp_char_data)
{
    NRF_LOG_INFO("bluetooth_temperature_service_temp_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_temperature_service_init.temp_char, temp_char_data, TEMPERATURE_SERVICE_TEMP_CHAR_LENGTH);
        control.error_code = temperature_service_temp_char_write(&m_ble_temperature_service, temp_char_data);   // Update the Temp Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}

void bluetooth_temperature_service_instant_temp_char_write(uint8_t *instant_temp_char_data)
{
    NRF_LOG_INFO("bluetooth_temperature_service_instant_temp_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_temperature_service_init.instant_temp_char, instant_temp_char_data, TEMPERATURE_SERVICE_INSTANT_TEMP_CHAR_LENGTH);
        control.error_code = temperature_service_instant_temp_char_write(&m_ble_temperature_service, instant_temp_char_data);   // Update the Temp Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}
#endif

#if MAX30003
void bluetooth_ecg_service_ecg_char_write(uint8_t *ecg_char_data)
{
    NRF_LOG_INFO("bluetooth_ecg_service_ecg_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_ecg_service_init.ecg_char, ecg_char_data, ECG_SERVICE_ECG_CHAR_LENGTH);
        control.error_code = ecg_service_ecg_char_write(&m_ble_ecg_service, ecg_char_data);   // Update the ECG Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}

void bluetooth_ecg_service_instant_ecg_char_write(uint8_t *instant_ecg_char_data)
{
    NRF_LOG_INFO("bluetooth_ecg_service_instant_ecg_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_ecg_service_init.instant_ecg_char, instant_ecg_char_data, ECG_SERVICE_INSTANT_ECG_CHAR_LENGTH);
        control.error_code = ecg_service_instant_ecg_char_write(&m_ble_ecg_service, instant_ecg_char_data);   // Update the INSTANT ECG Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
}
#endif

#if FDC1004
void bluetooth_pressure_service_instant_pressure_char_write(uint8_t *instant_pressure_char_data)
{
    NRF_LOG_INFO("bluetooth_pressure_service_instant_pressure_char_write");
    if(control.connection_flag)
    {
        memcpy(ble_pressure_service_init.instant_pressure_char, instant_pressure_char_data, PRESSURE_SERVICE_INSTANT_PRESSURE_CHAR_LENGTH);
        control.error_code = pressure_service_instant_pressure_char_write(&m_ble_pressure_service, instant_pressure_char_data);   // Update the Temp Characteristic
        APP_ERROR_CHECK(control.error_code);
    }
    else
    {
        uint8_t stop_fdc1004_data_collection_command[3] = {0X00, FDC1004_MODULE, FDC1004_STOP_DATA_COLLECTION_COMMAND};
        state_handler(stop_fdc1004_data_collection_command); // If the phone disconnects from the patch, stop collecting data

        uint8_t stop_rtc_timer_command[4] = {0x00, NRF52_MODULE, NRF52_RTC_CLOCK_COMMAND, NRF52_RTC_SENSOR_STOP};
        state_handler(stop_rtc_timer_command); // If the phone disconnects from the patch, stop the RTC to collect data from sensors
    }
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
    uint8_t response_char_command[4] = {0x00, 0x00, 0x00, FIRMWARE_VERSION};
    bluetooth_configuration_service_response_char_write(response_char_command);
}

void bluetooth_transmit_hardware_board_version(void)
{
    NRF_LOG_INFO("bluetooth_transmit_hardware_board_version");
    uint8_t response_char_command[4] = {0x00,0x00, 0x00, BOARD_VERSION};
    bluetooth_configuration_service_response_char_write(response_char_command);
}

void bluetooth_disconnect(void)
{
    NRF_LOG_INFO("bluetooth_disconnect");
    control.error_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(control.error_code);
    if (control.error_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Failed to disconnect connection. Connection handle: %d Error: %d", m_conn_handle, control.error_code);
    }
    else
    {
        NRF_LOG_INFO("Disconnected connection handle %d", m_conn_handle);
    }
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void _bluetooth_soft_device_sleep_mode_enter(void)
{
    NRF_LOG_INFO("_bluetooth_soft_device_sleep_mode_enter");
    //Disable SoftDevice. It is required to be able to write to GPREGRET2 register (SoftDevice API blocks it).
    //GPREGRET2 register holds the information about skipping CRC check on next boot.
    control.error_code = nrf_sdh_disable_request();
    APP_ERROR_CHECK(control.error_code);
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = _buttonless_dfu_sdh_state_observer,
};

//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(_bluetooth_app_shutdown_handler, 0);

/** @brief handler for buttonless dfu sdh state observer.
 */
static void _buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool _bluetooth_app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    NRF_LOG_INFO("app_shutdown_handler");
    switch(event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("NRF_PWR_MGMT_EVT_PREPARE_DFU");
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            control.error_code = nrf_sdh_disable_request();
            APP_ERROR_CHECK(control.error_code);
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

/**@brief This function must be called before any interrupts are enabled. It can be called after the log module is initialized.
 */
void bluetooth_dfu_async_svci_init(void)
{
    NRF_LOG_INFO("ble_dfu_async_svci_init");
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    control.error_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(control.error_code);
}

// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void _bluetooth_dfu_service_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    NRF_LOG_INFO("_bluetooth_dfu_service_evt_handler");
    switch(event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
            NRF_LOG_INFO("BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE");
            bluetooth_disconnect();
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            NRF_LOG_INFO("BLE_DFU_EVT_BOOTLOADER_ENTER");
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_INFO("BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED");
            NRF_LOG_INFO("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_INFO("BLE_DFU_EVT_RESPONSE_SEND_ERROR");
            NRF_LOG_INFO("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_INFO("Unknown event from ble_dfu_buttonless.");
            break;
    }
}