#include "ble_temperature_service.h"

#if TMP117

static struct BLE_Temperature_Service_Control_Struct control;

/**@brief Function for initializing the Temperature Service.
 *
 * @param[out]  p_cus       Temperature Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_temperature_service_initialize(ble_temperature_service_t *p_cus, const ble_temperature_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("ble_temperature_service_initialize");
    if (p_cus == NULL || p_cus_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_cus->evt_handler = p_cus_init->evt_handler;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = TEMPERATURE_SERVICE_UUID_BASE;
    control.error_code =  sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(control.error_code);

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = TEMPERATURE_SERVICE_UUID;

    // Adding Custom Service
    control.error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle);
    if (control.error_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE SERVICE PROPERLY");
        return control.error_code;
    }

    control.error_code = temperature_service_temp_char_add(p_cus, p_cus_init);
    if (control.error_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE TEMPERATURE CHARACTERISTIC PROPERLY");
        return control.error_code;
    }

    control.error_code = temperature_service_instant_temp_char_add(p_cus, p_cus_init);
    if (control.error_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("DID NOT CREATE INSTANT TEMPERATURE CHARACTERISTIC PROPERLY");
        return control.error_code;
    }
}

/**@brief Function for adding the Temperature Value characteristic.
 *
 * @param[in]   p_cus        Temperature Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t temperature_service_temp_char_add(ble_temperature_service_t *p_cus, const ble_temperature_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("temperature_service_temp_char_add");
    
    // Local Variables to the function
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.write_perm = p_cus_init->temp_char_attr_md.cccd_write_perm;
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    // char_md is the variable used to define the properties that will be displayed to the central during service discovery
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.char_props.write = 0;
    char_md.char_props.write_wo_resp = 0;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md; 
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = TEMPERATURE_SERVICE_TEMP_CHAR_UUID;

    // attr_md is the variable that sets the properties of the attribute, or the accessability of the attribute
    // .vloc option is set to BLE_GATTS_VLOC_STACK as we want the characteristic to be stored in the SoftDevice RAM section,
    // and not in the Application RAM section
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm = p_cus_init->temp_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->temp_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t)*250;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t)*250;

    control.error_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, &p_cus->temp_char_handles);
    if (control.error_code != NRF_SUCCESS)
    {
        return control.error_code;
    }
    return NRF_SUCCESS;
}

uint32_t temperature_service_temp_char_write(ble_temperature_service_t *p_cus, uint8_t *new_temp_char_array)
{
    NRF_LOG_INFO("temperature_service_temp_char_write"); 
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    control.error_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = sizeof(uint8_t)*250;
    gatts_value.offset = 0;
    gatts_value.p_value = new_temp_char_array;

    // Update database.
    control.error_code = sd_ble_gatts_value_set(p_cus->conn_handle, p_cus->temp_char_handles.value_handle, &gatts_value);
    if (control.error_code != NRF_SUCCESS)
    {
        return control.error_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->temp_char_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        control.error_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", control.error_code); 
    }
    else
    {
        control.error_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }
    return control.error_code;
}

/**@brief Function for adding the Instant Temperature Value characteristic.
 *
 * @param[in]   p_cus        Temperature Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t temperature_service_instant_temp_char_add(ble_temperature_service_t *p_cus, const ble_temperature_service_init_t *p_cus_init)
{
    NRF_LOG_INFO("temperature_service_instant_temp_char_add");
    
    // Local Variables to the function
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t attr_char_value;
    ble_uuid_t ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.write_perm = p_cus_init->instant_temp_char_attr_md.cccd_write_perm;
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    // char_md is the variable used to define the properties that will be displayed to the central during service discovery
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read = 1;
    char_md.char_props.write = 0;
    char_md.char_props.write_wo_resp = 0;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md; 
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = TEMPERATURE_SERVICE_INSTANT_TEMP_CHAR_UUID;

    // attr_md is the variable that sets the properties of the attribute, or the accessability of the attribute
    // .vloc option is set to BLE_GATTS_VLOC_STACK as we want the characteristic to be stored in the SoftDevice RAM section,
    // and not in the Application RAM section
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm = p_cus_init->instant_temp_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->instant_temp_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t)*2;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(uint8_t)*2;

    control.error_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, &p_cus->instant_temp_char_handles);
    if (control.error_code != NRF_SUCCESS)
    {
        return control.error_code;
    }
    return NRF_SUCCESS;
}

uint32_t temperature_service_instant_temp_char_write(ble_temperature_service_t *p_cus, uint8_t *new_instant_temp_char_array)
{
    NRF_LOG_INFO("temperature_service_temp_char_write"); 
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    control.error_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = sizeof(uint8_t)*2;
    gatts_value.offset = 0;
    gatts_value.p_value = new_instant_temp_char_array;

    // Update database.
    control.error_code = sd_ble_gatts_value_set(p_cus->conn_handle, p_cus->instant_temp_char_handles.value_handle, &gatts_value);
    if (control.error_code != NRF_SUCCESS)
    {
        return control.error_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->instant_temp_char_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        control.error_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", control.error_code); 
    }
    else
    {
        control.error_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }
    return control.error_code;
}

/*
*  BLE Temperature Service Event Handler
*/
void ble_temperature_service_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    ble_temperature_service_t *p_cus = (ble_temperature_service_t *) p_context;
    
    NRF_LOG_INFO("BLE Temperature Service Event Received. Event type = %d", p_ble_evt->header.evt_id); 
    if (p_cus == NULL || p_ble_evt == NULL)
    {
        return;
    }
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE: BLE_GAP_EVT_CONNECTED");
            temperature_service_on_connect(p_cus, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("TEMPERATURE_SERVICE: BLE_GAP_EVT_DISCONNECTED");
            temperature_service_on_disconnect(p_cus, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            NRF_LOG_INFO("TEMPERATURE_SERVICE: BLE_GATTS_EVT_WRITE");
            temperature_service_on_write(p_cus, p_ble_evt);
            break;

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST: // Handling this event is not necessary
            NRF_LOG_INFO("TEMPERATURE SERVICE: BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST");
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void temperature_service_on_connect(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt)
{   
    NRF_LOG_INFO("temperature_service_on_connect");
    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    temperature_service_evt_t evt;    
    evt.evt_type = TEMPERATURE_SERVICE_EVT_CONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void temperature_service_on_disconnect(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt)
{
    NRF_LOG_INFO("temperature_service_on_disconnect");
    UNUSED_PARAMETER(p_ble_evt);
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    temperature_service_evt_t evt;

    evt.evt_type = TEMPERATURE_SERVICE_EVT_DISCONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Temperature Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void temperature_service_on_write(ble_temperature_service_t *p_cus, ble_evt_t const *p_ble_evt)
{
    NRF_LOG_INFO("temperature_service_on_write");
    ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    // Check if the Temperature value CCCD has been written and if the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_cus->temp_char_handles.cccd_handle) && (p_evt_write->len == 2))
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {
            temperature_service_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED");
                evt.evt_type = TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_ENABLED;
            }
            else
            {
                NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED");
                evt.evt_type = TEMPERATURE_SERVICE_EVT_TEMP_CHAR_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }

    // Check if the Instant Temperature value CCCD has been written and if the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_cus->instant_temp_char_handles.cccd_handle) && (p_evt_write->len == 2))
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {
            temperature_service_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_ENABLED");
                evt.evt_type = TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_ENABLED;
            }
            else
            {
                NRF_LOG_INFO("TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_DISABLED");
                evt.evt_type = TEMPERATURE_SERVICE_EVT_INSTANT_TEMP_CHAR_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }
}

#endif