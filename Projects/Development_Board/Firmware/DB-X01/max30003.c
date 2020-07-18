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

#include "max30003.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"

#include "ble_eeg.h"

uint8_t max30003_default_registers[] = {};

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0);
static volatile bool spi_xfer_done;

void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
  spi_xfer_done = true;
}

void max30003_spi_init(void) {
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
  spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
  spi_config.irq_priority = APP_IRQ_PRIORITY_HIGHEST;
  spi_config.mode = NRF_DRV_SPI_MODE_0;
  spi_config.miso_pin = MAX30003_PIN_SDO;
  spi_config.sck_pin = MAX30003_PIN_SCLK;
  spi_config.mosi_pin = MAX30003_PIN_SDI;
  spi_config.ss_pin = MAX30003_PIN_CSB;
  spi_config.orc = 0x55;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));
  NRF_LOG_INFO("[MAX30003] SPI initialized \r\n");
}

void max30003_spi_uninit(void) {
  nrf_drv_spi_uninit(&spi);
  NRF_LOG_INFO("[MAX30003] SPI uninitialized \r\n");
}

void max30003_read_register(uint8_t reg_addr, uint8_t *reg_array) {
  uint8_t tx_data[1] = {MAX30003_READ_ADDR(reg_addr)};
  uint8_t rx_data[4] = {0, 0, 0, 0};
  spi_xfer_done = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, sizeof(tx_data), rx_data, sizeof(rx_data)));
  while (!spi_xfer_done) {
    __WFE();
  }
  // Dump:
  NRF_LOG_INFO("[MAX30003] [RX]READ DATA, REGISTER 0x%x: \r\n", reg_addr);
  NRF_LOG_HEXDUMP_INFO(rx_data, sizeof(rx_data)); // Hex Dump Output (temp)
  // Copy results to array: TEMP
  memcpy_fast(reg_array, &rx_data[1], 3);
}

void max30003_write_register(uint8_t reg_addr, uint8_t b0, uint8_t b1, uint8_t b2) {
  uint8_t tx_data[4];
  tx_data[0] = MAX30003_WRITE_ADDR(reg_addr);
  tx_data[1] = b0;
  tx_data[2] = b1;
  tx_data[3] = b2;
  NRF_LOG_INFO("[MAX30003] Writing 0x[%X|%X|%X] to Address 0x%X\r\n", tx_data[1], tx_data[2], tx_data[3], reg_addr);
  NRF_LOG_HEXDUMP_INFO(tx_data, sizeof(tx_data));
  // SPI Command
  spi_xfer_done = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, sizeof(tx_data), NULL, NULL));
  while (!spi_xfer_done) {
    __WFE();
  }
  nrf_delay_ms(5);
}

void max30003_read_device_info(void) {
  NRF_LOG_INFO("[MAX30003] Read Device Information \r\n");
  uint8_t dummy_array[3];
  max30003_read_register(MAX30003_REGADDR_INFO, dummy_array);
}

void max30003_read_device_status(void) {
  NRF_LOG_INFO("[MAX30003] Read Device Status \r\n");
  uint8_t dummy_array[3];
  max30003_read_register(MAX30003_REGADDR_STAT, dummy_array);
}

// Control functions>
void max30003_init_regs(void) {
  // Interrupts and Dynamic Modes
  max30003_write_register(MAX30003_REGADDR_EN_INT, 0x80, 0x00, 0x03);   //0x02
  max30003_write_register(MAX30003_REGADDR_EN_INT2, 0x00, 0x00, 0x03);  //0x03
  max30003_write_register(MAX30003_REGADDR_MNGR_INT, 0x78, 0x00, 0x04); //0x04
  max30003_write_register(MAX30003_REGADDR_MNGR_DYN, 0x3f, 0x00, 0x00); //0x05

  // Set up ECG Configurations:
  max30003_write_register(MAX30003_REGADDR_CNFG_GEN, 0x08, 0x10, 0x07);   //0x10
  max30003_write_register(MAX30003_REGADDR_CNFG_CAL, 0x72, 0x00, 0x00);   //0x12
  max30003_write_register(MAX30003_REGADDR_CNFG_EMUX, 0x0B, 0x00, 0x00);  //0x14
  max30003_write_register(MAX30003_REGADDR_CNFG_ECG, 0x80, 0x50, 0x00);   //0x15
  max30003_write_register(MAX30003_REGADDR_CNFG_RTOR1, 0x3f, 0x35, 0x00); //0x1D - RR DETECT DISABLED
}

void max30003_readback_registers(void) {
  uint8_t dummy_array[3];
  max30003_read_register(MAX30003_REGADDR_EN_INT, dummy_array);
  max30003_read_register(MAX30003_REGADDR_EN_INT2, dummy_array);
  max30003_read_register(MAX30003_REGADDR_MNGR_INT, dummy_array);

  max30003_read_register(MAX30003_REGADDR_CNFG_GEN, dummy_array);
  max30003_read_register(MAX30003_REGADDR_CNFG_CAL, dummy_array);
  max30003_read_register(MAX30003_REGADDR_CNFG_EMUX, dummy_array);
  max30003_read_register(MAX30003_REGADDR_CNFG_ECG, dummy_array);
  max30003_read_register(MAX30003_REGADDR_CNFG_RTOR1, dummy_array);
}

void max30003_soft_reset(void) {
  NRF_LOG_INFO("[MAX30003] Running software reset \r\n");
  max30003_write_register(MAX30003_REGADDR_SW_RST, 0x00, 0x00, 0x00);
  nrf_delay_ms(5); // Give time to start up.
}

void max30003_sync(void) {
  NRF_LOG_INFO("[MAX30003] Running Sync/Start \r\n");
  max30003_write_register(MAX30003_REGADDR_SYNCH, 0x00, 0x00, 0x00);
}

void max30003_read_fifo_data(ble_eeg_t *p_eeg) {
  uint8_t tx_data[1] = {MAX30003_READ_ADDR(MAX30003_REGADDR_ECG_FIFO_BURST)};
  uint8_t rx_data[51+1];
  spi_xfer_done = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_data, sizeof(tx_data), rx_data, sizeof(rx_data)));
  while (!spi_xfer_done) {
    __WFE();
  }
  // Dump:
//  NRF_LOG_INFO("[MAX30003] [RX] FIFO READ DATA \r\n");
//  NRF_LOG_HEXDUMP_INFO(rx_data, sizeof(rx_data)); // Hex Dump Output (temp)
  //TODO: Copy results to p_eeg: TEMP
  memcpy_fast(&p_eeg->eeg_ch1_buffer[p_eeg->eeg_ch1_count], &rx_data[1], 48);
  p_eeg->eeg_ch1_count += 48;
}