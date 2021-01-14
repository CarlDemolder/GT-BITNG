#ifndef BLE_DFU_SERVICE_H__
#define BLE_DFU_SERVICE_H__

#include "ble_service.h"
#include "serial_slave.h"

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "ble_dfu.h"

struct BLE_DFU_Control_Struct
{
    ret_code_t error_code;                                            /**< Variable to store the Error Code values */
};

void ble_dfu_async_svci_init(void);
void delete_bonds(void);


#endif // BLE_DFU_SERVICE_H__