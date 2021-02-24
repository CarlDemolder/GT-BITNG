#include "ble_configuration_service.h"

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_cus       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_configuration_service_initialize(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("ble_configuration_service_initialize");
    if (p_cus == NULL || p_cus_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_cus->evt_handler = p_cus_init->evt_handler;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = CONFIGURATION_SERVICE_UUID_BASE;
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CONFIGURATION_SERVICE_UUID;

    // Adding Configuration Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE CONFIGURATION SERVICE PROPERLY");
        return err_code;
    }

    err_code = configuration_service_settings_char_add(p_cus, p_cus_init);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE SETTINGS CHARACTERISTIC PROPERLY");
        return err_code;
    }

    err_code = configuration_service_response_char_add(p_cus, p_cus_init);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE RESPONSE CHARACTERISTIC PROPERLY");
        return err_code;
    }

    err_code = configuration_service_crc_char_add(p_cus, p_cus_init);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE CRC CHARACTERISTIC PROPERLY");
        return err_code;
    }
}

/**@brief Function for adding the Settings characteristic.
 *
 * @param[in]   p_cus        Configuration Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t configuration_service_settings_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("configuration_service_settings_char_add");
    // Local Variables to the function
    uint32_t err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // char_md is the variable used to define the properties that will be displayed to the central during service discovery
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.char_props.write = 0;
    char_md.char_props.write_wo_resp = 1;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL; 
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CONFIGURATION_SETTINGS_CHAR_UUID;

    // attr_md is the variable that sets the properties of the attribute, or the accessability of the attribute
    // .vloc option is set to BLE_GATTS_VLOC_STACK as we want the characteristic to be stored in the SoftDevice RAM section,
    // and not in the Application RAM section
    memset(&attr_md, 0, sizeof(attr_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.read_perm = p_cus_init->settings_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->settings_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->settings_char);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->settings_char);

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, &p_cus->settings_char_handles);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}

/**@brief Function for adding the Response characteristic.
 *
 * @param[in]   p_cus        Configuration Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t configuration_service_response_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("configuration_service_response_char_add");
    // Local Variables to the function
    uint32_t err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // char_md is the variable used to define the properties that will be displayed to the central during service discovery
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.char_props.write = 0;
    char_md.char_props.write_wo_resp = 0;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL; 
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CONFIGURATION_RESPONSE_CHAR_UUID;

    // attr_md is the variable that sets the properties of the attribute, or the accessability of the attribute
    // .vloc option is set to BLE_GATTS_VLOC_STACK as we want the characteristic to be stored in the SoftDevice RAM section,
    // and not in the Application RAM section
    memset(&attr_md, 0, sizeof(attr_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    attr_md.read_perm = p_cus_init->response_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->response_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->response_char);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->response_char);

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, &p_cus->response_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}

/**@brief Function for adding the CRC characteristic.
 *
 * @param[in]   p_cus        Configuration Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t configuration_service_crc_char_add(ble_configuration_service_t *p_cus, const ble_configuration_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("configuration_service_crc_char_add");
    // Local Variables to the function
    uint32_t err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // char_md is the variable used to define the properties that will be displayed to the central during service discovery
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.char_props.write = 0;
    char_md.char_props.write_wo_resp = 0;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL; 
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CONFIGURATION_CRC_CHAR_UUID;

    // attr_md is the variable that sets the properties of the attribute, or the accessability of the attribute
    // .vloc option is set to BLE_GATTS_VLOC_STACK as we want the characteristic to be stored in the SoftDevice RAM section,
    // and not in the Application RAM section
    memset(&attr_md, 0, sizeof(attr_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    attr_md.read_perm  = p_cus_init->crc_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->crc_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->crc_char);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t)*ARRAY_LENGTH(p_cus_init->crc_char);

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, &p_cus->crc_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}

uint32_t configuration_service_settings_char_read(ble_configuration_service_t *p_cus, uint8_t *new_settings_char)
{
    NRF_LOG_INFO("configuration_service_settings_char_read"); 
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = sizeof(uint8_t)*CONFIGURATION_SERVICE_SETTINGS_CHAR_LENGTH;
    gatts_value.offset = 0;
    gatts_value.p_value = new_settings_char;

    // Get Characteristic from Database
    err_code = sd_ble_gatts_value_get(p_cus->conn_handle, p_cus->settings_char_handles.value_handle, &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Characteristic could not be read from database");
        return err_code;
    }
}

uint32_t configuration_service_response_char_write(ble_configuration_service_t *p_cus, uint8_t *new_response_char)
{
    NRF_LOG_INFO("configuration_service_response_char_write"); 
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = sizeof(uint8_t)*CONFIGURATION_SERVICE_RESPONSE_CHAR_LENGTH;
    gatts_value.offset = 0;
    gatts_value.p_value = new_response_char;

    // Update GATT Database
    err_code = sd_ble_gatts_value_set(p_cus->conn_handle, p_cus->response_char_handles.value_handle, &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Characteristic could not be set");
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->response_char_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
        while(err_code != 0x00)
        {
            err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
        }
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x", err_code);
        NRF_LOG_INFO("Response Char Written: %X %X %X %X", new_response_char[0], new_response_char[1], new_response_char[2], new_response_char[3]);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE"); 
    }
    return err_code;
}

uint32_t configuration_service_crc_char_write(ble_configuration_service_t *p_cus, uint8_t *new_crc_char)
{
    NRF_LOG_INFO("configuration_service_crc_char_write"); 
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = sizeof(uint8_t)*CONFIGURATION_SERVICE_CRC_CHAR_LENGTH;
    gatts_value.offset = 0;
    gatts_value.p_value = new_crc_char;

    // Update GATT Database
    err_code = sd_ble_gatts_value_set(p_cus->conn_handle, p_cus->crc_char_handles.value_handle, &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Characteristic could not be set");
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->crc_char_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x", err_code);
        NRF_LOG_INFO("CRC Char Written: %X %X", new_crc_char[0], new_crc_char[1]);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE"); 
    }
    return err_code;
}

/*
*  BLE Configuration Service Event Handler
*/
void ble_configuration_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    NRF_LOG_INFO("ble_configuration_service_on_ble_evt");
    ble_configuration_service_t *p_cus = (ble_configuration_service_t *) p_context;
    if (p_cus == NULL || p_ble_evt == NULL)
    {
        return;
    }

    NRF_LOG_INFO("BLE Configuration Service Event Received. Event type = %d", p_ble_evt->header.evt_id); 

    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE: BLE_GAP_EVT_CONNECTED");
            _configuration_service_on_connect(p_cus, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("CONFIGURATION_SERVICE: BLE_GAP_EVT_DISCONNECTED");
            _configuration_service_on_disconnect(p_cus, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            NRF_LOG_INFO("CONFIGURATION_SERVICE: BLE_GATTS_EVT_WRITE");
            _configuration_service_on_write(p_cus, p_ble_evt);
            break;

        // Handling this event is not necessary
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            NRF_LOG_INFO("CONFIGURATION SERVICE: BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST");
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Configuration Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void _configuration_service_on_connect(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt)
{   
    NRF_LOG_INFO("configuration_service_on_connect");
    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    configuration_service_evt_t evt;    
    evt.evt_type = CONFIGURATION_SERVICE_EVT_CONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Configuration Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void _configuration_service_on_disconnect(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt)
{
    NRF_LOG_INFO("configuration_service_on_disconnect");
    UNUSED_PARAMETER(p_ble_evt);
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    configuration_service_evt_t evt;

    evt.evt_type = CONFIGURATION_SERVICE_EVT_DISCONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Configuration Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void _configuration_service_on_write(ble_configuration_service_t *p_cus, ble_evt_t const *p_ble_evt)
{
    NRF_LOG_INFO("_configuration_service_on_write");
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    // Settings Value Characteristic Has Changed
    if (p_evt_write->handle == p_cus->settings_char_handles.value_handle)
    {
        configuration_service_evt_t evt;
        evt.evt_type = CONFIGURATION_SERVICE_EVT_SETTINGS_CHAR_WRITE;

        NRF_LOG_INFO("CONFIGURATION_SERVICE_ON_WRITE: SETTINGS_CHAR");
        p_cus->evt_handler(p_cus, &evt);
    }

    // Check if the Response value CCCD is written and that the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_cus->response_char_handles.cccd_handle) && (p_evt_write->len == 2))
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {
            configuration_service_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT: RESPONSE_NOTIFICATION_ENABLED");
                evt.evt_type = CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_ENABLED;
            }
            else
            {
                NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT: RESPONSE_NOTIFICATION_DISABLED");
                evt.evt_type = CONFIGURATION_SERVICE_EVT_RESPONSE_CHAR_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }

    // Check if the CRC value CCCD is written and that the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_cus->crc_char_handles.cccd_handle) && (p_evt_write->len == 2))
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {
            configuration_service_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT: CRC_NOTIFICATION_ENABLED");
                evt.evt_type = CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_ENABLED;
            }
            else
            {
                NRF_LOG_INFO("CONFIGURATION_SERVICE_EVT: CRC_NOTIFICATION_DISABLED");
                evt.evt_type = CONFIGURATION_SERVICE_EVT_CRC_CHAR_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }
}

