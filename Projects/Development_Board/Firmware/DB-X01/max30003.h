#ifndef MAX30003_H_
#define MAX30003_H_

#include "spi.h"

// Write command (0b[ADDR(6:0)]|1):
#define MAX30003_READ_ADDR(x) ((x << 1) | 1) // x is the address  
#define MAX30003_WRITE_ADDR(x) ((x << 1) | 0) // x is the address  

//Register Addresses:
#define MAX30003_REGADDR_STAT               0x01
#define MAX30003_REGADDR_EN_INT             0x02
#define MAX30003_REGADDR_EN_INT2            0x03
#define MAX30003_REGADDR_MNGR_INT           0x04
#define MAX30003_REGADDR_MNGR_DYN           0x05
#define MAX30003_REGADDR_SW_RST             0x08
#define MAX30003_REGADDR_SYNCH              0x09
#define MAX30003_REGADDR_FIFO_RST           0x0A
#define MAX30003_REGADDR_INFO               0x0F
#define MAX30003_REGADDR_CNFG_GEN           0x10
#define MAX30003_REGADDR_CNFG_CAL           0x12
#define MAX30003_REGADDR_CNFG_EMUX          0x14
#define MAX30003_REGADDR_CNFG_ECG           0x15
#define MAX30003_REGADDR_CNFG_RTOR1         0x1D
#define MAX30003_REGADDR_CNFG_RTOR2         0x1E
#define MAX30003_REGADDR_ECG_FIFO_BURST     0x20
#define MAX30003_REGADDR_ECG_FIFO           0x21
#define MAX30003_REGADDR_RTOR               0x25

//Function Prototypes:
void max30003_read_device_info(void);

void max30003_read_device_status(void);

void max30003_init_registers(void);

void max30003_readback_registers(void);

void max30003_soft_reset(void);

void max30003_sync(void);

void max30003_read_fifo_data(uint8_t* data_array);

#endif //MAX30003_H_