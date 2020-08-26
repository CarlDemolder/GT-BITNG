#ifndef __FT201X_H__
#define __FT201X_H__

#include "common.h"
#include "i2c.h"

#define FT201X_DEVICE_ADDRESS           0X22
#define FT201X_READ_BUFFER_ADDRESS      0X73
#define FT201X_WRITE_BUFFER_ADDRESS     0X23

#define FT201X_DATA_AVAILABLE_COMMAND   0X0C
#define FT201X_FLUSH_COMMAND            0x0E
#define FT201X_SOFT_RESET_COMMAND       0X06
#define FT201X_USB_STATE_COMMAND        0X16
#define FT201X_DEVICE_ID_COMMAND        0X7C
#define FT201X_MTP_ADDR_COMMAND         0X10
#define FT201X_MTP_WRITE_COMMAND        0x12
#define FT201X_MTP_READ_COMMAND         0x14

#define FT201X_USB_NORMAL               0X03

uint8_t ft201x_available(void);
void ft201x_init(void);
void ft201x_enable_vcp(void);
void ft201x_close(void);
void ft201x_write_buffer(uint8_t* array_data, uint8_t array_size);
void ft201x_read_buffer(uint8_t* array_data, uint8_t array_size);
void ft201x_flush_buffers(void);
void ft201x_soft_reset(void);
uint8_t ft201x_data_available(void);
uint8_t ft201x_check_usb_state(void);
void ft201x_read_chip_id(uint16_t* array_data);
static void _ft201x_enable_vcp(void);
static uint8_t _ft201x_read_eeprom_register(uint8_t const* register_address_array);

#endif /*__FT201X_H__*/