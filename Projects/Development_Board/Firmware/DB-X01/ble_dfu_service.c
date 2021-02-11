//static struct BLE_DFU_Control_Struct control_struct;
//
//static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
//{
//    if (state == NRF_SDH_EVT_STATE_DISABLED)
//    {
//        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
//        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);
//
//        //Go to system off.
//        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
//    }
//}
//
///* nrf_sdh state observer. */
//NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
//{
//    .handler = buttonless_dfu_sdh_state_observer,
//};
//
///**@brief Handler for shutdown preparation.
// *
// * @details During shutdown procedures, this function will be called at a 1 second interval
// *          untill the function returns true. When the function returns true, it means that the
// *          app is ready to reset to DFU mode.
// *
// * @param[in]   event   Power manager event.
// *
// * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
// */
//static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
//{
//    switch (event)
//    {
//        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
//            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
//            // YOUR_JOB: Get ready to reset into DFU mode
//            //
//            // If you aren't finished with any ongoing tasks, return "false" to
//            // signal to the system that reset is impossible at this stage.
//            //
//            // Here is an example using a variable to delay resetting the device.
//            //
//            // if (!m_ready_for_reset)
//            // {
//            //      return false;
//            // }
//            // else
//            //{
//            //
//            //    // Device ready to enter
//            //    uint32_t err_code;
//            //    err_code = sd_softdevice_disable();
//            //    APP_ERROR_CHECK(err_code);
//            //    err_code = app_timer_stop_all();
//            //    APP_ERROR_CHECK(err_code);
//            //}
//            break;
//
//        default:
//            // YOUR_JOB: Implement any of the other events available from the power management module:
//            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
//            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
//            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
//            return true;
//    }
//
//    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
//    return true;
//}
//
////lint -esym(528, m_app_shutdown_handler)
///**@brief Register application shutdown handler with priority 0.
// */
//NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);
//
//
///**@brief Function for putting the chip into sleep mode.
// *
// * @note This function will not return.
// */
//static void sleep_mode_enter(void)
//{
//    //Disable SoftDevice. It is required to be able to write to GPREGRET2 register (SoftDevice API blocks it).
//    //GPREGRET2 register holds the information about skipping CRC check on next boot.
//    err_code = nrf_sdh_disable_request();
//    APP_ERROR_CHECK(err_code);
//}





