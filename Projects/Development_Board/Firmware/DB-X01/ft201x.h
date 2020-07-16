#ifndef __FT201X_H__
#define __FT201X_H__

#include "common.h"
#include "i2c.h"

#define FT201X_DEVICE_ADDRESS 0X22
#define FT201X_READ_BUFFER_ADDRESS 0X73
#define FT201X_WRITE_BUFFER_ADDRESS 0X23


void ft201x_init();
void ft201x_close();
void ft201x_i2c_write_buffer();
uint32_t ft201x_i2c_read_buffer();
uint32_t ft201x_flush_buffers();
void ft201x_set_usb_callback(void (*callback)(void));
void usb_in_event();
uint32_t usb_out_event();
uint32_t ft201x_data_available();
uint32_t ft201x_check_usb_state();

#endif /*__FT201X_H__*/