#include "max30003.h"

uint8_t max30003_default_registers[] = {};


void max30003_read_device_info(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Information \r\n");
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_REGADDR_INFO, data_array);

    NRF_LOG_INFO("Device Information: \r\n");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

void max30003_read_device_status(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Status \r\n");
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_REGADDR_STAT, data_array);

    NRF_LOG_INFO("Device Status: \r\n");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

// Control functions>
void max30003_init_registers(void) 
{
    // Interrupts and Dynamic Modes
    spim_write_single_register(MAX30003_REGADDR_EN_INT, 0x80, 0x00, 0x03);   //0x02
    spim_write_single_register(MAX30003_REGADDR_EN_INT2, 0x00, 0x00, 0x03);  //0x03
    spim_write_single_register(MAX30003_REGADDR_MNGR_INT, 0x78, 0x00, 0x04); //0x04
    spim_write_single_register(MAX30003_REGADDR_MNGR_DYN, 0x3f, 0x00, 0x00); //0x05

    // Set up ECG Configurations:
    spim_write_single_register(MAX30003_REGADDR_CNFG_GEN, 0x08, 0x10, 0x07);   //0x10
    spim_write_single_register(MAX30003_REGADDR_CNFG_CAL, 0x72, 0x00, 0x00);   //0x12
    spim_write_single_register(MAX30003_REGADDR_CNFG_EMUX, 0x0B, 0x00, 0x00);  //0x14
    spim_write_single_register(MAX30003_REGADDR_CNFG_ECG, 0x80, 0x50, 0x00);   //0x15
    spim_write_single_register(MAX30003_REGADDR_CNFG_RTOR1, 0x3f, 0x35, 0x00); //0x1D - RR DETECT DISABLED
}

void max30003_readback_registers(void) 
{
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_REGADDR_EN_INT, data_array);
    spim_read_single_register(MAX30003_REGADDR_EN_INT2, data_array);
    spim_read_single_register(MAX30003_REGADDR_MNGR_INT, data_array);

    spim_read_single_register(MAX30003_REGADDR_CNFG_GEN, data_array);
    spim_read_single_register(MAX30003_REGADDR_CNFG_CAL, data_array);
    spim_read_single_register(MAX30003_REGADDR_CNFG_EMUX, data_array);
    spim_read_single_register(MAX30003_REGADDR_CNFG_ECG, data_array);
    spim_read_single_register(MAX30003_REGADDR_CNFG_RTOR1, data_array);
}

void max30003_soft_reset(void) 
{
    NRF_LOG_INFO("[MAX30003] Running software reset \r\n");
    spim_write_single_register(MAX30003_REGADDR_SW_RST, 0x00, 0x00, 0x00);
    nrf_delay_ms(5); // Give time to start up.
}

void max30003_sync(void) 
{
    NRF_LOG_INFO("[MAX30003] Running Sync/Start \r\n");
    spim_write_single_register(MAX30003_REGADDR_SYNCH, 0x00, 0x00, 0x00);
}

void max30003_read_fifo_data(uint8_t* data_array) 
{
    uint8_t register_address = MAX30003_REGADDR_ECG_FIFO_BURST;
    uint8_t register_data[51+1];

    spim_read_registers(register_address, register_data, sizeof(register_data)); 

    NRF_LOG_INFO("[MAX30003] FIFO READ DATA \r\n");
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data)); // Hex Dump Output (temp)
    //TODO: Copy results to p_eeg: TEMP
//    memcpy_fast(data_array, &register_data[1], 48);
//    p_eeg->eeg_ch1_count += 48;
}