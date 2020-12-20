#ifndef ECG_H_
#define ECG_H_

#include "serial_slave.h"
#include "common.h"

#if ECG

//enum ECG_CONSTANTS
//{
////    MAX_SAMPLES_PER_BLE_TRANSMISSION = 
////    BLE_PACKET_SIZE = 0XF9
//};

struct ECG_DATA_FLOW
{
    uint32_t samples;
    uint8_t samples_per_bluetooth_transmission;
    uint8_t bytes_per_bluetooth_transmission;
    uint32_t samples_per_recording_session;
    uint32_t bytes_per_recording_session;
    uint32_t current_sample_count;
    uint8_t ble_max_payload_size;
    uint8_t max30003_samples_per_interrupt;
    uint16_t max30003_samples_per_second;
    uint32_t recording_session_new_start_address;
    uint32_t recording_session_old_start_address;
    uint32_t recording_session_current_read_address;
    uint32_t bytes_left_to_transmit;
    uint8_t interrupt;
};

void ecg_init(void);
void ecg_interrupt_handler(void);
uint8_t ecg_get_data_packet(uint8_t *bluetooth_data_packet, uint8_t bluetooth_data_packet_size);
uint32_t ecg_get_bytes_left_to_transmit(void);
void ecg_start_data_recording(void);
void ecg_stop_data_recording(void);

#endif

#endif // __ECG_H

