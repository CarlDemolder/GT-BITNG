#ifndef ECG_H_
#define ECG_H_

#include "serial_slave.h"
#include "common.h"

#if ECG

enum ECG_CONSTANTS
{
    TEMPERATURE_DATA_REGISTER_SIZE = 0X0001F4,
};

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
    uint32_t recording_session_start_address;
    uint32_t recording_session_old_start_address;
    uint32_t recording_session_current_read_address;
    uint32_t bytes_left_to_transmit;
    uint8_t bytes_per_sample;
    uint8_t interrupt;
};

void ecg_init(void);
void ecg_interrupt_handler(void);
uint8_t ecg_get_data_packet(uint8_t *bluetooth_data_packet, uint8_t bluetooth_data_packet_size);
uint32_t ecg_get_bytes_left_to_transmit(void);
void ecg_start_recording_session(void);
void ecg_stop_recording_session(void);

#endif

#endif // __ECG_H

