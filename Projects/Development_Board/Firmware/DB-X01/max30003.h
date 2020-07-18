/* Copyright (c) 2019 Musa Mahmood
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MAX30003_H_
#define MAX30003_H_

#include <stdint.h>
#include "ble_eeg.h"

#define MAX30003_PIN_INTB 11
#define MAX30003_PIN_INT2B 12
#define MAX30003_PIN_SDO 13
#define MAX30003_PIN_SDI 14
#define MAX30003_PIN_SCLK 15
#define MAX30003_PIN_CSB 16

//TODO: Default registers
//#define MAX30003_REGDEF_EN_INT 0x00
//#define MAX30003_REGDEF_EN_INT2 0x00
//#define MAX30003_REGDEF_

// Write command (0b[ADDR(6:0)]|1):
#define MAX30003_READ_ADDR(x) ((x << 1) | 1) // x is the address  
#define MAX30003_WRITE_ADDR(x) ((x << 1) | 0) // x is the address  

//Register Addresses:
#define MAX30003_REGADDR_STAT 0x01
#define MAX30003_REGADDR_EN_INT 0x02
#define MAX30003_REGADDR_EN_INT2 0x03
#define MAX30003_REGADDR_MNGR_INT 0x04
#define MAX30003_REGADDR_MNGR_DYN 0x05
#define MAX30003_REGADDR_SW_RST 0x08
#define MAX30003_REGADDR_SYNCH 0x09
#define MAX30003_REGADDR_FIFO_RST 0x0A
#define MAX30003_REGADDR_INFO 0x0F
#define MAX30003_REGADDR_CNFG_GEN 0x10
#define MAX30003_REGADDR_CNFG_CAL 0x12
#define MAX30003_REGADDR_CNFG_EMUX 0x14
#define MAX30003_REGADDR_CNFG_ECG 0x15
#define MAX30003_REGADDR_CNFG_RTOR1 0x1D
#define MAX30003_REGADDR_CNFG_RTOR2 0x1E
#define MAX30003_REGADDR_ECG_FIFO_BURST 0x20
#define MAX30003_REGADDR_ECG_FIFO 0x21
#define MAX30003_REGADDR_RTOR 0x25

//Function Prototypes:
void max30003_spi_init(void);

void max30003_spi_uninit(void);

void max30003_read_register(uint8_t reg_addr, uint8_t *reg_array);

void max30003_write_register(uint8_t reg_addr, uint8_t b0, uint8_t b1, uint8_t b2);

void max30003_read_device_info(void);

void max30003_read_device_status(void);

void max30003_init_regs(void);

void max30003_readback_registers(void);

void max30003_soft_reset(void);

void max30003_sync(void);

void max30003_read_fifo_data(ble_eeg_t *p_eeg);

#endif //MAX30003_H_