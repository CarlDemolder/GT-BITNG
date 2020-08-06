#include "ft201x.h"
//
//void ft201x_init()
//{
//    
//}
//
//void ft201x_i2c_read_buffer()
//{
//
//    NRF_LOG_INFO("TMP116_get_uint16_t");
//    uint8_t device_address = 0x48;                      // Slave Address for TMP116, ADD0 = GND
//    uint8_t register_address = 0x00;                    // Temperature values stored in this Address of TMP116
//    uint8_t register_byte_count = 2;                    // Number of bytes of buffer to read
//    uint8_t i2c_temp_counts[2];
//    
//    i2c_read(tmp116_gnd_address, tmp116_temp_address, i2c_temp_counts, temp_register_byte_count);
//    uint16_t uint16_i2c_counts =  (i2c_temp_counts[0] << 8) | i2c_temp_counts[1];
//    return uint16_i2c_counts;
//}
//
//void ft201x_i2c_write_buffer()
//{
//
//}
//
//void ft201x_flush_buffers()
//{
//    uint8_t register_byte_count = 1;                       // General Call address + Register Write Value
//    uint8_t general_call_address = 0x00;                   // General call address
//    
//    /* General call address plus flush command to flush transmit and receive buffers */
//    uint8_t ft201x_flush_command = 0x0E;                   // Flush command to flush transmit and receive buffers
//
//    uint8_t ft201x_flush_command_array[1]= {0};
//    ft201x_flush_command_array[0] = ft201x_flush_command;
//    uint8_t *ft201x_flush_command_array_pointer = ft201x_flush_command_array;
//    
//    i2c_write(general_call_address, ft201x_flush_command_array_pointer, register_byte_count);
//
//    NRF_LOG_INFO("FT201X transmit and receive buffers have been flushed.");
//}
//
//uint32_t ft201x_data_available()
//{
//    uint8_t register_byte_count = 1;                       // General Call address + Register Write Value
//    uint8_t general_call_address = 0x00;                   // General call address
//    
//    /* General call address plus data available command to see if data is available for download */
//    uint8_t ft201x_data_available_command = 0x0C;                   // Flush command to flush transmit and receive buffers
//
//    uint8_t ft201x_data_available_command_array[1]= {0};
//    ft201x_data_available_command_array[0] = ft201x_data_available_command;
//    uint8_t *ft201x_data_available_command_pointer = ft201x_data_available_command_array;
//    
//    i2c_write(general_call_address, ft201x_data_available_command_pointer, register_byte_count);
//
//    /*Read back the data available register */
//    uint8_t data_available_bytes[1];
//    uint8_t register_byte_count = 1; //Number of bytes of buffer to read
//    
//    i2c_read(FT201X_DEVICE_ADDRESS, tmp116_config_address, data_available_bytes, register_byte_count);
//       
//    if(data_available_bytes > 0)
//    {  
//      NRF_LOG_INFO("FT201X has data available for reading.");
//      return (uint32_t) data_available_bytes[0];
//    }
//    nrf_delay_ms(100); 
//}
//
//uint32_t ft201x_check_usb_state()
//{
//    uint8_t register_byte_count = 1;                       // General Call address + Register Write Value
//    uint8_t general_call_address = 0x00;                   // General call address
//    
//    /* General call address plus data available command to see if data is available for download */
//    uint8_t ft201x_usb_state_command = 0x16;                   // Flush command to flush transmit and receive buffers
//
//    uint8_t ft201x_usb_status_command_array[1]= {0};
//    ft201x_usb_status_command_array[0] = ft201x_usb_status_command;
//    uint8_t *ft201x_usb_status_command_pointer = ft201x_usb_status_command_array;
//    
//    i2c_write(general_call_address, ft201x_usb_status_command_pointer, register_byte_count);
//
//    /*Read back the data available register */
//    uint8_t usb_status_bytes[1];
//    uint8_t register_byte_count = 1; //Number of bytes of buffer to read
//    
//    i2c_read(FT201X_DEVICE_ADDRESS, tmp116_config_address, usb_status_bytes, register_byte_count);
//       
//    if(usb_status_bytes == 0x03)
//    {  
//      NRF_LOG_INFO("FT201X is in its normal operating state. USB cable and computer are plugged in.");
//      return (uint32_t) usb_status_bytes[0];
//    }
//
//    else 
//    {
//        return 0;
//    }
//}