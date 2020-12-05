#include "ecg.h"

static struct ECG_DATA_FLOW data_flow;

void ecg_init(void)
{
    NRF_LOG_INFO("ecg_init");

    data_flow.max30003_samples_per_interrupt = max30003_get_samples_per_interrupt();
    NRF_LOG_INFO("data_flow.max30003_samples_per_interrupt: %u", data_flow.max30003_samples_per_interrupt);
    data_flow.max30003_samples_per_second = max30003_get_samples_per_second();
    NRF_LOG_INFO("data_flow.max30003_samples_per_second: %u", data_flow.max30003_samples_per_second);
    data_flow.samples_per_recording_session = 3600 * data_flow.max30003_samples_per_second;         // Each recording session is 1 hour
    NRF_LOG_INFO("data_flow.samples_per_recording_session: %u", data_flow.samples_per_recording_session);
    data_flow.bytes_per_recording_session = data_flow.samples_per_recording_session * 2;            // two bytes of ECG data stored
    NRF_LOG_INFO("data_flow.bytes_per_recording_session: %u", data_flow.bytes_per_recording_session);
    data_flow.current_sample_count = 0;

    data_flow.interrupt = 0;    // Disabling the interrupt handler for ECG Data

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    data_flow.bytes_per_bluetooth_transmission = bluetooth_get_bytes_per_transmission();
    data_flow.samples_per_bluetooth_transmission = data_flow.bytes_per_bluetooth_transmission/2;
    NRF_LOG_INFO("data_flow.samples_per_bluetooth_transmission: %u", data_flow.samples_per_bluetooth_transmission);

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    data_flow.recording_session_new_start_address = cy15b108qi_get_current_write_address();
    NRF_LOG_INFO("data_flow.recording_session_new_start_address: %X", data_flow.recording_session_new_start_address);

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
}

void ecg_interrupt_handler(void)
{
    NRF_LOG_INFO("ecg_interrupt_handler");
    
    if(data_flow.interrupt == 1)
    {
        uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
        state_handler(spim_enable_array_data); // Enable SPIM Module

        uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
        state_handler(spim_init_array_data); // Initialize SPIM Module

        uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
        state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to MAX30003 for the SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t read_status_register_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_READ_STATUS_REGISTER_COMMAND};
        state_handler(read_status_register_array_data); // Read the MAX30003 Status Register to determine if the FIFO Memory has overflown

        if(max30003_get_status_register_eovf())   // Check to see if one needs to issue a fifo reset to clear the cache
        {
            uint8_t write_fifo_memory_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_WRITE_FIFO_RESET_REGISTER_COMMAND};
            state_handler(write_fifo_memory_array_data); // Write to the MAX30003 FIFO Memory Register to Reset the FIFO Memory 
        }
        else
        {
            uint8_t read_fifo_memory_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_READ_ECG_FIFO_MEMORY_COMMAND};
            state_handler(read_fifo_memory_array_data); // Read the MAX30003 FIFO Memory Register

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            uint16_t max30003_voltage_array[data_flow.max30003_samples_per_interrupt];
            max30003_get_ecg_voltage(max30003_voltage_array, data_flow.max30003_samples_per_interrupt);
    
            uint8_t cy15b108qi_voltage_array[data_flow.max30003_samples_per_interrupt*2];
            for(uint8_t i = 0; i < data_flow.max30003_samples_per_interrupt; i++)
            {
                cy15b108qi_voltage_array[2*i] = (uint8_t) ((0xFF00 & max30003_voltage_array[i]) >> 8);
                NRF_LOG_INFO("Sample: %u, cy15b108qi_voltage_array: %u", 2*i, cy15b108qi_voltage_array[2*i]);
                cy15b108qi_voltage_array[2*i+1] = (uint8_t) (0x00FF & max30003_voltage_array[i]);
                NRF_LOG_INFO("Sample: %u, cy15b108qi_voltage_array: %u", 2*i+1, cy15b108qi_voltage_array[2*i+1]);
            }

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            spim_select_cs_pin_array_data[4] = CY15B108QI_CS_PIN;
            state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to CY15B108QI for the SPIM Module

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            uint8_t cy15b108qi_exit_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
            state_handler(cy15b108qi_exit_deep_power_down_mode_array_data); // Exit the Deep power down mode

            uint8_t cy15b108qi_set_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_SET_WRITE_ENABLE_LATCH_COMMAND};
            state_handler(cy15b108qi_set_write_enable_latch_array_data); // Set the write enable latch

            cy15b108qi_write_data_command(cy15b108qi_voltage_array, ARRAY_LENGTH(cy15b108qi_voltage_array));

            uint8_t cy15b108qi_reset_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_RESET_WRITE_ENABLE_LATCH_COMMAND};
            state_handler(cy15b108qi_reset_write_enable_latch_array_data); // Reset the write enable latch

            uint8_t cy15b108qi_enter_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
            state_handler(cy15b108qi_enter_deep_power_down_mode_array_data); // Enter the Deep power down mode

            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

            data_flow.current_sample_count += data_flow.max30003_samples_per_interrupt;
            NRF_LOG_INFO("data_flow.current_sample_count: %u", data_flow.current_sample_count);

            if(data_flow.samples >= data_flow.samples_per_recording_session)
            {
                data_flow.recording_session_old_start_address = data_flow.recording_session_new_start_address;
                NRF_LOG_INFO("data_flow.recording_session_old_start_address: %X", data_flow.recording_session_old_start_address);
                data_flow.recording_session_new_start_address = cy15b108qi_get_current_write_address();
                NRF_LOG_INFO("data_flow.recording_session_new_start_address: %X", data_flow.recording_session_new_start_address);
                data_flow.recording_session_current_read_address = data_flow.recording_session_old_start_address;
                NRF_LOG_INFO("data_flow.recording_session_current_read_address: %X", data_flow.recording_session_current_read_address);
                data_flow.bytes_left_to_transmit = data_flow.max30003_samples_per_interrupt*2;
                NRF_LOG_INFO("data_flow.bytes_left_to_transmit: %u", data_flow.bytes_left_to_transmit);
                data_flow.current_sample_count = 0;    // Resetting the sample count

                uint8_t bluetooth_transmit_recording_session_array_data[3] = {0x00, BLUETOOTH_MODULE, BLUETOOTH_TRANSMIT_RECORDING_SESSION_COMMAND};
                state_handler(bluetooth_transmit_recording_session_array_data); // Start to transmit the Recording Session of the Data
            }
        }

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

        uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
        state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

        uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
        state_handler(spim_disable_array_data); // Disable SPIM Module

        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    }
}

uint8_t ecg_get_data_packet(uint8_t *bluetooth_data_packet, uint8_t bluetooth_data_packet_size)
{
    NRF_LOG_INFO("ecg_get_data_packet"); 

    uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_array_data); // Initialize SPIM Module

    uint8_t spim_set_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_set_cs_pin_array_data); // Set Chip Select Pin to CY15B108QI for the SPIM Module

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
    uint8_t cy15b108qi_exit_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_DEEP_POWER_DOWN_MODE_COMMAND};
    state_handler(cy15b108qi_exit_deep_power_down_mode_array_data); // Exit the Deep power down mode

    uint8_t cy15b108qi_set_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_SET_WRITE_ENABLE_LATCH_COMMAND};
    state_handler(cy15b108qi_set_write_enable_latch_array_data); // Set the write enable latch

    uint8_t empty_values = 0;
    if(data_flow.bytes_left_to_transmit < data_flow.bytes_per_bluetooth_transmission)
    {
        NRF_LOG_INFO("data_flow.bytes_left_to_transmit: %u", data_flow.bytes_left_to_transmit);
        NRF_LOG_INFO("data_flow.bytes_per_bluetooth_transmission: %u", data_flow.bytes_per_bluetooth_transmission);
        empty_values = (data_flow.bytes_per_bluetooth_transmission - data_flow.bytes_left_to_transmit);
        NRF_LOG_INFO("empty_value: %u", empty_values);
        bluetooth_data_packet_size = data_flow.bytes_left_to_transmit;
        NRF_LOG_INFO("bluetooth_data_packet_size: %u", bluetooth_data_packet_size);
        cy15b108qi_fast_read_data_command(bluetooth_data_packet, bluetooth_data_packet_size, data_flow.recording_session_current_read_address);
    }
    else
    {
        cy15b108qi_fast_read_data_command(bluetooth_data_packet, bluetooth_data_packet_size, data_flow.recording_session_current_read_address);
    }

    uint8_t cy15b108qi_reset_write_enable_latch_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_RESET_WRITE_ENABLE_LATCH_COMMAND};
    state_handler(cy15b108qi_reset_write_enable_latch_array_data); // Reset the write enable latch

    uint8_t cy15b108qi_enter_deep_power_down_mode_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
    state_handler(cy15b108qi_enter_deep_power_down_mode_array_data); // Enter the Deep power down mode

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    
    uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_array_data); // Disable SPIM Module  

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    data_flow.recording_session_current_read_address += bluetooth_data_packet_size;
    NRF_LOG_INFO("data_flow.recording_session_current_read_address: %X", data_flow.recording_session_current_read_address);
    data_flow.bytes_left_to_transmit -= bluetooth_data_packet_size;
    NRF_LOG_INFO("data_flow.bytes_left_to_transmit: %u", data_flow.bytes_left_to_transmit);
    return empty_values;
}

uint32_t ecg_get_bytes_left_to_transmit(void)
{
    NRF_LOG_INFO("ecg_get_bytes_left_to_transmit");
    return data_flow.bytes_left_to_transmit;
}

void ecg_start_data_recording(void)
{
    NRF_LOG_INFO("ecg_start_data_recording");
    data_flow.interrupt = 1;    // Enabling the interrupt

    uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_array_data); // Initialize SPIM Module

    uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, CY15B108QI_CS_PIN};
    state_handler(spim_select_cs_pin_array_data); // Select Chip Select Pin to CY15B108QI for the SPIM Module

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t cy15b108qi_exit_hibernation_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_EXIT_HIBERNATION_MODE_COMMAND};
    state_handler(cy15b108qi_exit_hibernation_array_data); // Exit the Hibernation power state

    uint8_t cy15b108qi_enter_deep_power_down_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_DEEP_POWER_DOWN_MODE_COMMAND};
    state_handler(cy15b108qi_enter_deep_power_down_array_data); // Enter the deep-power-down power state

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    spim_select_cs_pin_array_data[4] = MAX30003_CS_PIN;
    state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to MAX30003 for the SPIM Module

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t max30003_start_recording_array_data[4] = {0x00, MAX30003_MODULE, MAX30003_START_RECORDING_COMMAND};
    state_handler(max30003_start_recording_array_data); // Set the MAX30003 to start recording ECG data

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_array_data); // Disable SPIM Module
}

void ecg_stop_data_recording(void)
{
    NRF_LOG_INFO("ecg_stop_data_recording");

    data_flow.interrupt = 0;    // Disabling the interrupt

    uint8_t spim_enable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_ENABLE};
    state_handler(spim_enable_array_data); // Enable SPIM Module

    uint8_t spim_init_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_INIT};
    state_handler(spim_init_array_data); // Initialize SPIM Module

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t max30003_disable_pin_interrupt_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_DISABLE_PIN_INTERRUPT_COMMAND}; 
    state_handler(max30003_disable_pin_interrupt_array_data); // MAX30003: Disable Pin Interrupt Command

    uint8_t spim_select_cs_pin_array_data[5] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_SELECT_CS_PIN, MAX30003_CS_PIN};
    state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to MAX30003 for the SPIM Module

    uint8_t max30003_interrupt1_disable_array_data[3] = {0x00, MAX30003_MODULE, MAX30003_INTERRUPT1_DISABLE_COMMAND};
    state_handler(max30003_interrupt1_disable_array_data); // MAX30003: Disable Interrupt Command

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    spim_select_cs_pin_array_data[4] = CY15B108QI_CS_PIN;
    state_handler(spim_select_cs_pin_array_data); // Set Chip Select Pin to CY15B108QI for the SPIM Module

    uint8_t cy15b108qi_enter_hibernation_array_data[3] = {0x00, CY15B108QI_MODULE, CY15B108QI_ENTER_HIBERNATION_MODE_COMMAND};
    state_handler(cy15b108qi_enter_hibernation_array_data);

    /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    uint8_t spim_uninit_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_UNINIT};
    state_handler(spim_uninit_array_data); // Uninitialize SPIM Module

    uint8_t spim_disable_array_data[4] = {0x00, NRF52_MODULE, NRF52_SPI_COMMAND, NRF52_SPI_SPIM_DISABLE};
    state_handler(spim_disable_array_data); // Disable SPIM Module

}