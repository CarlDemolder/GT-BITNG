#include "ft201x.h"


void ft201x_init(void)
{
    NRF_LOG_INFO("ft201x_init");
    ft201x_flush_buffers();
}

void ft201x_enable_vcp(void)
{
    NRF_LOG_INFO("ft201x_enable_vcp");
    // Enable the Virtual Com Port Functionality
}

uint8_t ft201x_available(void)
{
    NRF_LOG_INFO("ft201x_available");

    ft201x_data.usb_connection = ft201x_check_usb_state();

    if(ft201x_data.usb_connection == FT201X_USB_NORMAL)
    {
        ft201x_data.data_available = ft201x_data_available();
        return ft201x_data.data_available;
    }
    return 0;
}

void ft201x_read_buffer(uint8_t* array_data, uint8_t array_size)
{
    NRF_LOG_INFO("ft201x_read_buffer");
    uint8_t device_address = FT201X_DEVICE_ADDRESS;                     // Slave Address for FT201X
    uint8_t register_byte_count = array_size;                           // Number of bytes of buffer to read
//    uint8_t register_data[register_byte_count];                         // Store data in array
    
    if( register_byte_count > 0)
    {
        i2c_read_device(device_address, array_data, register_byte_count);
    }
}

void ft201x_write_buffer(uint8_t* array_data_pointer, uint8_t array_size)
{   
    NRF_LOG_INFO("ft201x_write_buffer");
    i2c_write_registers(FT201X_DEVICE_ADDRESS, array_data_pointer, array_size);        // Write array data to FT201X

    NRF_LOG_INFO("FT201X: writing data to transmitting buffer");
}

void ft201x_flush_buffers()
{
    NRF_LOG_INFO("ft201x_flush_buffer");    
    uint8_t register_byte_count = 1;
    uint8_t register_data = FT201X_FLUSH_COMMAND;    
    i2c_write_registers(GENERAL_CALL_ADDRESS, &register_data, register_byte_count);

    NRF_LOG_INFO("FT201X transmit and receive buffers have been flushed.");
}

uint8_t ft201x_data_available(void)
{
    NRF_LOG_INFO("ft201x_data_available");
    uint8_t register_byte_count = 1;
    uint8_t register_value = FT201X_DATA_AVAILABLE_COMMAND;    
    uint8_t register_data[register_byte_count];

    i2c_separate_write_read_registers(GENERAL_CALL_ADDRESS, FT201X_DEVICE_ADDRESS, &register_value, register_byte_count, register_data, register_byte_count);
    
    ft201x_data.data_available_flag = register_data[0];
       
    if(ft201x_data.data_available_flag > 0)
    {  
        NRF_LOG_INFO("FT201X has data available for reading.");
        NRF_LOG_INFO("Number of bytes available to read: %hhu", ft201x_data.data_available_flag);
        return ft201x_data.data_available_flag;
    }
    NRF_LOG_INFO("FT201X does not have data available for reading.");
    return 0;
}

void ft201x_soft_reset(void)
{    
    uint8_t register_byte_count = 1;    
    uint8_t register_data = FT201X_SOFT_RESET_COMMAND; 
    i2c_write_registers(GENERAL_CALL_ADDRESS, &register_data, register_byte_count);

    NRF_LOG_INFO("FT201X Soft Reset.");
}

uint8_t ft201x_check_usb_state(void)
{
    NRF_LOG_INFO("ft201x_check_usb_state");

    uint8_t register_byte_count = 1;    
    uint8_t register_value = FT201X_USB_STATE_COMMAND; 
    uint8_t register_data[register_byte_count];
    
    i2c_separate_write_read_registers(GENERAL_CALL_ADDRESS, FT201X_DEVICE_ADDRESS, &register_value, register_byte_count, register_data, register_byte_count); 
    if(get_i2c_timeout() == 0x01)
    {
        ft201x_data.ft201x_usb_state == FT201X_DISCONNECTED;
    }

    ft201x_data.ft201x_usb_state = register_data[0];   

    if(ft201x_data.ft201x_usb_state == FT201X_USB_NORMAL)
    {
        NRF_LOG_INFO("FT201X USB State: 0x%X (expected: 0x03)\r\n", ft201x_data.ft201x_usb_state);
        NRF_LOG_INFO("FT201X is in its normal operating state. USB cable and computer are plugged in.");
        return ft201x_data.ft201x_usb_state;
    }

    NRF_LOG_INFO("FT201X USB State: 0x%X (expected: 0x03)\r\n", ft201x_data.ft201x_usb_state);
    return 0;
}

void ft201x_read_chip_id(uint16_t* array_data)
{

//    uint16_t device_id_data[3] = {0};
//    ft201x_read_chip_id(device_id_data);
//    NRF_LOG_INFO("FT201X Manufacturer ID: %hu", device_id_data[0]);
//    NRF_LOG_INFO("FT201X Part ID: %hu", device_id_data[1]);
//    NRF_LOG_INFO("FT201X Revision: %hu", device_id_data[2]);
//

    NRF_LOG_INFO("FT201X read the chip ID");

    uint8_t register_byte_count = 1;
    uint8_t register_value = FT201X_DEVICE_ADDRESS; 
    uint8_t register_data_byte_count = 3;                                       // Number of bytes to read from registers  
    uint8_t register_data[register_data_byte_count];

    i2c_separate_write_read_registers(FT201X_DEVICE_ID_COMMAND, FT201X_DEVICE_ID_COMMAND, &register_value, register_byte_count, register_data, register_data_byte_count);
    
    
    array_data[0] = (register_data[0] << 4) | ((register_data[1] & 0xF0) >> 4);               // Manufacturer ID (12 bits)
    array_data[1] = ((register_data[1] & 0x0F) << 5) | (register_data[2] & 0xF8);             // Part ID (9 bits)
    array_data[2] = register_data[2] & 0x07;                                                  // Revision (3 bits)
}

static void _ft201x_enable_vcp(void)
{
    // Read register value first
    // if not enabled, enable virtual com port
    
    /*
    Register Address: 0x00
    Binary address always points to a single byte only
    A word is just a group of bytes - depending upon the data bus size of the CPU
    Miscellanous Configuration Descriptor Data (Page 12 of 25; section 7)

    Bit 0: BCD Enable, battery charge detection. Allow the device to detect a dedicated charging board
    1 = Enabled (Battery Charge Detection is on)
    0 = Disabled (Battery Charge Detection is off)

    Bit 1: Force Power Enable, When BCD is enabled and a BCD port is detected, force power enable asserts the power enable signal on CBUS
    1 = Enabled (Force Power Enable is on)
    0 = Disabled (Force Power Enable is off)

    Bit 2: De-activate Sleep, When BCD is enabled and a BCD port is detected, de-activate sleep forces the device never to go into sleep mode
    1 = Enabled (De-Activate Sleep function is on)
    0 = Disabled (De-Activate Sleep function is off)

    Bit 3: RS485 Echo Suppression, RS485 Echo suppression enable
    1 = Enabled (Echo is suppressed)
    0 = Disabled (Echo is suppressed)

    Bit 4: Ext. OSC, External Oscillator
    1 = Enabled (Use External oscillator)
    0 = Disabled (Use Internal oscillator)

    Bit 5: Ext. OSC feedback resistor enable, oscillator feedback
    1 = Disabled (External Oscillator feedback resistor disabled)
    0 = Enabled (External Oscillator feedback resistor enabled)

    Bit 6: CBUS pin set for VBUS sense. CBUS has been allocated to VBUS sense mode. When self powered, use this mode to determine when the
    device is connected to a powered host. Used when operating in a self powered mode and is used to prevent forcing current down the USB
    lines when the host or hub is powered off. 
    1 = Enabled (CBUS pin has been allocated to VBUS sense mode)
    0 = Disabled (CBUS pin has NOT been allocated to VBUS sense mode)

    Bit 7: Load D2XX or VCP Driver, Enables software to select which driver to load
    1 = Load VCP (Load VCP layer on top of D2xx)
    0 = Disabled (i.e. do not load VCP layer)

    Bit 15:8 unused     */

    NRF_LOG_INFO("FT201X enabling a Virtual COM Port on the FT201X");
    uint8_t ft201x_mtp_register_address[2] = {0x00, 0x00};
    uint8_t ft201x_mtp_register_value = _ft201x_read_eeprom_register(ft201x_mtp_register_address);  
    NRF_LOG_INFO("MTP Register 0x00 0x00: %hhu", ft201x_mtp_register_value);
}

static uint8_t _ft201x_read_eeprom_register(uint8_t const* register_address_array)
{

    // General call address plus MTP address command

    uint8_t register_byte_count = 1;
    uint8_t register_value_1[register_byte_count];

    register_value_1[0] = FT201X_MTP_ADDR_COMMAND;

    i2c_no_stop_write_registers(GENERAL_CALL_ADDRESS, register_value_1, register_byte_count);

    // Slave address plus MTP address to read from

    register_byte_count = 2;
    uint8_t register_value_2[register_byte_count];

    register_value_2[0] = register_address_array[0];
    register_value_2[1] = register_address_array[1];

    i2c_no_stop_write_registers(FT201X_DEVICE_ADDRESS, register_value_2, register_byte_count);

    // General call address plus MTP read command

    register_byte_count = 1;
    uint8_t register_value_3[register_byte_count];

    register_value_3[0] = FT201X_MTP_READ_COMMAND;

    i2c_no_stop_write_registers(GENERAL_CALL_ADDRESS, register_value_3, register_byte_count);

    // Call out Device Address, then read the data stored at that register

    uint8_t array_data[1];
    i2c_read_device(FT201X_DEVICE_ADDRESS, array_data, 1);       // Read 1 byte from the EEPROM
    return array_data[0];
} 


static void _ft201x_write_eeprom_register(uint8_t const* register_address_array, uint8_t register_write_value)
{

    // General call address plus MTP address command

    uint8_t register_byte_count = 1;
    uint8_t register_value_1[register_byte_count];

    register_value_1[0] = FT201X_MTP_ADDR_COMMAND;

    i2c_no_stop_write_registers(GENERAL_CALL_ADDRESS, register_value_1, register_byte_count);

    // Slave address plus MTP address to read from

    register_byte_count = 2;
    uint8_t register_value_2[register_byte_count];

    register_value_2[0] = register_address_array[0];
    register_value_2[1] = register_address_array[1];

    i2c_no_stop_write_registers(FT201X_DEVICE_ADDRESS, register_value_2, register_byte_count);

    // General call address plus MTP read command

    register_byte_count = 1;
    uint8_t register_value_3[register_byte_count];

    register_value_3[0] = FT201X_MTP_WRITE_COMMAND;

    i2c_no_stop_write_registers(GENERAL_CALL_ADDRESS, register_value_3, register_byte_count);

    // Call out Device Address, then read the data stored at that register

    uint8_t register_data[1];
    register_data[1] = register_write_value;
    i2c_write_registers(FT201X_DEVICE_ADDRESS, register_data, register_byte_count); 
}     