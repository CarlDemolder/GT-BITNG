#ifndef MAX30003_H_
#define MAX30003_H_

#include "spi.h"
#include "common.h"

// Write command (0b[ADDR(6:0)]|1):
#define MAX30003_READ_ADDR(x) ((x << 1) | 1) // x is the address  
#define MAX30003_WRITE_ADDR(x) ((x << 1) | 0) // x is the address  

//Register Addresses:
#define MAX30003_STAT_ADDRESS               0x01
#define MAX30003_EN_INT_ADDRESS             0x02
#define MAX30003_EN_INT2_ADDRESS            0x03
#define MAX30003_MNGR_INT_ADDRESS           0x04
#define MAX30003_MNGR_DYN_ADDRESS           0x05
#define MAX30003_SW_RST_ADDRESS             0x08
#define MAX30003_SYNCH_ADDRESS              0x09
#define MAX30003_FIFO_RST_ADDRESS           0x0A
#define MAX30003_INFO_ADDRESS               0x0F
#define MAX30003_CNFG_GEN_ADDRESS           0x10
#define MAX30003_CNFG_CAL_ADDRESS           0x12
#define MAX30003_CNFG_EMUX_ADDRESS          0x14
#define MAX30003_CNFG_ECG_ADDRESS           0x15
#define MAX30003_CNFG_RTOR1_ADDRESS         0x1D
#define MAX30003_CNFG_RTOR2_ADDRESS         0x1E
#define MAX30003_ECG_FIFO_BURST_ADDRESS     0x20
#define MAX30003_ECG_FIFO_ADDRESS           0x21
#define MAX30003_RTOR_ADDRESS               0x25

//Function Prototypes:
void max30003_read_device_info(void);

void max30003_read_device_status(void);

void max30003_init(void);

void max30003_readback_registers(void);

void max30003_soft_reset(void);

void max30003_sync(void);

void max30003_read_fifo_data(uint8_t* data_array);

static void _max30003_init_interrupt(void);

static void _max30003_interrupt_handler();

uint8_t max30003_read_interrupt();

void max30003_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif //MAX30003_H_