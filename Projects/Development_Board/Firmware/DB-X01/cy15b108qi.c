#include "cy15b108qi.h"

#if CY15B108QI

static struct CY15B108QI_Status_Register status_register;
static struct CY15B108QI_Device_ID_Register device_id_register;
static struct CY15B108QI_Unique_ID_Register unique_id_register;
static struct CY15B108QI_Control_Struct control_struct;

void cy15b108qi_init(void)
{
    NRF_LOG_INFO("cy15b108qi_init");
    control_struct.start_address = CY15B108QI_FIRST_WRITE_ADDRESS;
    control_struct.write_address = control_struct.start_address;
    NRF_LOG_INFO("control_struct.write_address: %X", control_struct.write_address);
}

void cy15b108qi_uninit(void)
{
    NRF_LOG_INFO("cy15b108qi_uninit");
}


/* 
* Read Status Register to read the following values:
* D[7], WPEN = Write protect enable => Used to enable the function of the write protect pin
* D[6] - D[4], NOT USED
* D[3], BP1 = Block protect bit '0'
* D[2], BP0 = Block protect bit '1'
* D[1], WEL = Indication if the device is write enabled. This bit defaults to '0' (disabled) on power up
* D[0], NOT USED
*/
void cy15b108qi_read_status_register(void)
{
    NRF_LOG_INFO("_cy15b108qi_read_status_register");

    control_struct.command[0] = RDSR_OPCODE_COMMAND;
    uint8_t temp_data_array_length = ARRAY_LENGTH(control_struct.command) + ARRAY_LENGTH(status_register.data_array);
    uint8_t temp_data_array[temp_data_array_length];

    spim_read_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command), temp_data_array, temp_data_array_length);
    status_register.data_array[0] = temp_data_array[1];

    status_register.wel = (status_register.data_array[0] & 0b00000010) && 0b00000010;
    status_register.bp0 = (status_register.data_array[0] & 0b00000100) && 0b00000100;
    status_register.bp1 = (status_register.data_array[0] & 0b00001000) && 0b00001000;
    status_register.wpen = (status_register.data_array[0] & 0b10000000) && 0b10000000;

    NRF_LOG_INFO("status_register.wel: %u", status_register.wel);
    NRF_LOG_INFO("status_register.bp0: %u", status_register.bp0);
    NRF_LOG_INFO("status_register.bp1: %u", status_register.bp1);
    NRF_LOG_INFO("status_register.wpen: %u", status_register.wpen);
}

void cy15b108qi_write_status_register(void)
{
    NRF_LOG_INFO("_cy15b108qi_write_status_register");

    control_struct.command[0] = WRSR_OPCODE_COMMAND;

    status_register.data_array[0] = status_register.wel << 1;
    status_register.data_array[0] = (status_register.bp0 << 2) | status_register.data_array[0];
    status_register.data_array[0] = (status_register.bp1 << 3) | status_register.data_array[0];
    status_register.data_array[0] = (status_register.wpen << 7) | status_register.data_array[0];

    uint8_t temp_data_struct[ARRAY_LENGTH(control_struct.command) + ARRAY_LENGTH(status_register.data_array)];
    temp_data_struct[0] = control_struct.command[0];
    temp_data_struct[1] = status_register.data_array[0];

    spim_write_data_array(temp_data_struct, ARRAY_LENGTH(temp_data_struct));
}

/*
* Set write emable latch. The CY15X108QI will power up with writes disabled. The WRENcommand must be issued before any write operation. 
* Sending the WREN opcode allows the user to issue subsequent opcodesfor write operations.
* Read the status register afterwards and verify the WEL bit is '1'
*/
void cy15b108qi_set_write_enable_latch_command(void)
{
    NRF_LOG_INFO("cy15b108qi_set_write_enable_latch_command");

    control_struct.command[0] = WREN_OPCODE_COMMAND;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
}

/*
* The WRDI command disables all write activity by clearing the Write Enable Latch.
* Read the status register afterwards and verify the WEL bit is '0'
*/
void cy15b108qi_reset_write_enable_latch_command(void)
{
    NRF_LOG_INFO("cy15b108qi_reset_write_enable_latch_command");

    control_struct.command[0]= WRDI_OPCODE_COMMAND;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
}


/*
* The  CY15X108QI  device  can  be  interrogated  for  its manufacturer, product identification, and die revision. The RDID opcode 9Fh allows
* the user to read the 9-byte manufacturer ID and  product  ID,  both  of  which  are  read-only  bytes.  The JEDEC-assigned  manufacturer  ID
* places  the  Cypress(Ramtron) identifier in bank 7; therefore, there are six bytes ofthe continuation code 7Fh followed by the single byte 
* C2h. Thereare two bytes of product ID, which includes a family code, adensity code, a sub code, and the product revision code.
*/
void cy15b108qi_read_device_id_command(void)
{
    NRF_LOG_INFO("cy15b108qi_read_device_id_command");

    control_struct.command[0] = RDID_OPCODE_COMMAND;
    uint8_t temp_data_array_length = ARRAY_LENGTH(device_id_register.data_array) + ARRAY_LENGTH(control_struct.command);
    uint8_t temp_data_array[temp_data_array_length];
    spim_read_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command), temp_data_array, temp_data_array_length);

    for(uint8_t i = 0; i < ARRAY_LENGTH(device_id_register.data_array); i++)
    {
        device_id_register.data_array[i] = temp_data_array[i+1];
    }

    device_id_register.frequency = device_id_register.data_array[0] & 0b00000011;
    device_id_register.voltage = (device_id_register.data_array[0] & 0b00000100) >> 2;
    device_id_register.revision = (device_id_register.data_array[0] & 0b00011000) >> 3;
    device_id_register.sub_type = (device_id_register.data_array[0] & 0b11100000) >> 5;

    device_id_register.inrush = device_id_register.data_array[1] & 0b00000001;
    device_id_register.density = (device_id_register.data_array[1] & 0b00011110) >> 1;
    device_id_register.family = (device_id_register.data_array[1] & 0b11100000) >> 5;

    NRF_LOG_INFO("device_id_register.frequency: %u", device_id_register.frequency);
    NRF_LOG_INFO("device_id_register.voltage: %u", device_id_register.voltage);
    NRF_LOG_INFO("device_id_register.revision: %u", device_id_register.revision);
    NRF_LOG_INFO("device_id_register.sub_type: %u", device_id_register.sub_type);

    NRF_LOG_INFO("device_id_register.inrush: %u", device_id_register.inrush);
    NRF_LOG_INFO("device_id_register.density: %u", device_id_register.density);
    NRF_LOG_INFO("device_id_register.family: %u", device_id_register.family);

    for(uint8_t i = 0; i < 7; i++)
    {
        device_id_register.manufacturer_id[i] = device_id_register.data_array[2+i]; 
    }
    NRF_LOG_INFO("device_id_register.manufacturer_id:");
    NRF_LOG_HEXDUMP_INFO(device_id_register.manufacturer_id, ARRAY_LENGTH(device_id_register.manufacturer_id));
}

/*
* The  CY15X102QN  device  can  be  interrogated  for  unique  ID which is a factory programmed, 64-bit number unique to each device. 
* The RUID opcode, 4Ch allows to read the 8-byte, readonly unique ID. 
*/
void cy15b108qi_read_unique_id_command(void)
{
    NRF_LOG_INFO("cy15b108qi_read_unique_id_command");

    control_struct.command[0] = RUID_OPCODE_COMMAND;

    uint8_t temp_data_array_length = ARRAY_LENGTH(unique_id_register.unique_id) + ARRAY_LENGTH(control_struct.command);
    uint8_t temp_data_array[temp_data_array_length];

    spim_read_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command), temp_data_array, temp_data_array_length);

    for(uint8_t i = 0; i < ARRAY_LENGTH(unique_id_register.unique_id); i++)
    {
        unique_id_register.unique_id[i] = temp_data_array[i+1];
    }
    NRF_LOG_INFO("unique_id_register.unique_id:");
    NRF_LOG_HEXDUMP_INFO(unique_id_register.unique_id, ARRAY_LENGTH(unique_id_register.unique_id));
}

/*
* A power-saving Deep Power-Down mode is implemented on the CY15X108QI device. The device enters the Deep Power-Down mode after tENTDPD
* time after the DPD opcode BAh is clocked in and a rising edge of CS is applied.
*/
void cy15b108qi_enter_deep_power_down_mode_command(void)
{
    NRF_LOG_INFO("cy15b108qi_enter_deep_power_down_mode_command");

    control_struct.command[0]= DPD_OPCODE_COMMAND;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
}

/*
* A CS pulse-width of tCSDPD exits the DPD mode after tEXTDPD time. The CS pulse-width can be generated either by sending a dummy command 
* cycle or toggling CS alone while SCK and I/Os are don’t care.
*/
void cy15b108qi_exit_deep_power_down_mode_command(void)
{
    NRF_LOG_INFO("cy15b108qi_exit_deep_power_down_mode_command");

    control_struct.command[0]= 0x00;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
    nrf_delay_us(DEEP_POWER_DOWN_MODE_DELAY);    // Delay 250 us to allow the CY15B108QI to wake up
}

/*
* A  lowest  power  Hibernate  mode  is  implemented  on  the CY15X108QI device. The device enters Hibernate mode after tENTHIB time after
* the HBN opcode B9h is clocked in and a rising edge of CS is applied.
*/
void cy15b108qi_enter_hibernation_mode_command(void)
{
    NRF_LOG_INFO("cy15b108qi_enter_hibernate_mode_command");

    control_struct.command[0]= HBN_OPCODE_COMMAND;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
}

/*
* On the next falling edge of CS, the device will return to normal operation within tEXTHIB time. The SO pin remains in a Hi-Z state 
* during the wakeup from hibernate period.
*/
void cy15b108qi_exit_hibernation_mode_command(void)
{
    NRF_LOG_INFO("cy15b108qi_exit_hibernate_mode_command");

    control_struct.command[0]= 0x00;

    spim_write_data_array(control_struct.command, ARRAY_LENGTH(control_struct.command));
    nrf_delay_ms(HIBERNATION_MODE_DELAY);    // Delay 5 ms to allow the CY15B108QI to wake up
}

/*
* All writes to the memory begin with a WREN opcode with CS being asserted and deasserted. The next opcode is WRITE. 
* The WRITE opcode is followed by a three-byte address containingthe 20-bit address (A19–A0) of the first data byte 
* to be written into the memory. The upper four bits of the three-byte addressare ignored. Subsequent bytes are data bytes, 
* which are written sequentially.
*/
void cy15b108qi_write_data_command(uint8_t *data_array, uint8_t data_array_size)
{
    NRF_LOG_INFO("_cy15b108qi_write_data_command");

    control_struct.command[0] = WRITE_OPCODE_COMMAND;

    uint32_t temp_data_array_size = data_array_size + ARRAY_LENGTH(control_struct.command) + ARRAY_LENGTH(control_struct.address_array);
    uint8_t temp_data_array[temp_data_array_size];
    NRF_LOG_INFO("control_struct.write_address: %X", control_struct.write_address);
    NRF_LOG_INFO("data_array_size: %u", data_array_size);    
    NRF_LOG_INFO("temp_data_array_size: %u", temp_data_array_size);
    _get_address_array(control_struct.write_address);
    
    temp_data_array[0] = control_struct.command[0]; 
    temp_data_array[1] = control_struct.address_array[0];
    temp_data_array[2] = control_struct.address_array[1];
    temp_data_array[3] = control_struct.address_array[2];
    
    for(uint8_t i = 0; i < data_array_size; i++)
    {
        temp_data_array[i+4] = data_array[i];
    }
    
    spim_write_data_array(temp_data_array, temp_data_array_size);
    control_struct.write_address += data_array_size;
    NRF_LOG_INFO("new control_struct.write_address: %X", control_struct.write_address);
}

/*
* After the falling edge of CS, the bus master can issue a READopcode. Following the READ command is a three-byte address containing 
* the 20-bit address (A19–A0) of the first byte of theread operation. The upper four bits of the address are ignored. After the opcode
* and address are issued, the device drives out the read data on the next eight clocks.
*/
uint8_t cy15b108qi_read_data_command(uint32_t address)
{
    NRF_LOG_INFO("_cy15b108qi_read_data_command");

    control_struct.command[0] = READ_OPCODE_COMMAND;

    uint32_t write_data_array_size = ARRAY_LENGTH(control_struct.command) + ARRAY_LENGTH(control_struct.address_array);
    NRF_LOG_INFO("write_data_array_size: %u", write_data_array_size);
    uint8_t write_data_array[write_data_array_size];
    
    _get_address_array(address);
    
    write_data_array[0] = control_struct.command[0]; 
    write_data_array[1] = control_struct.address_array[0];
    write_data_array[2] = control_struct.address_array[1];
    write_data_array[3] = control_struct.address_array[2];
    
    uint8_t temp_read_data_array_length = write_data_array_size + 1;
    uint8_t temp_read_data_array[temp_read_data_array_length];

    spim_read_data_array(write_data_array, write_data_array_size, temp_read_data_array, temp_read_data_array_length);    // One can only read 1 byte of data at a time
    NRF_LOG_INFO("Register Address: %X; Array Value: %X", address, temp_read_data_array[temp_read_data_array_length]); // Debugging purposes only
    return temp_read_data_array[temp_read_data_array_length];
}

/*
* The CY15X108QI supports a FAST READ opcode (0Bh) that isprovided for opcode compatibility with serial flash devices. The FAST  READ  opcode
* is  followed  by  a  three-byte  address containing the 20-bit address (A19–A0) of the first byte of theread operation and then a dummy byte.
* The dummy byte insertsa  read  latency  of  8-clock  cycle.  The  fast  read  operation  is otherwise the same as an ordinary read operation
* except that it requires an additional dummy byte. After receiving the opcode, address, and a dummy byte, the CY15X108QI starts driving its
* SO line with data bytes, with MSb first, and continues trans-mitting  as  long  as  the  device  is  selected  and  the  clock  is available.
*/
void cy15b108qi_fast_read_data_command(uint8_t *read_data_array, uint8_t read_data_array_size, uint32_t start_address)
{
    NRF_LOG_INFO("_cy15b108qi_read_data_command");

    control_struct.command[0] = FSTRD_OPCODE_COMMAND;

    uint32_t write_data_array_size = ARRAY_LENGTH(control_struct.command) + ARRAY_LENGTH(control_struct.address_array) + 1;    // Extra Dummy Byte
    uint8_t write_data_array[write_data_array_size];
    NRF_LOG_INFO("write_data_array_size: %u", write_data_array_size);

    _get_address_array(start_address);
    
    write_data_array[0] = control_struct.command[0]; 
    write_data_array[1] = control_struct.address_array[0];
    write_data_array[2] = control_struct.address_array[1];
    write_data_array[3] = control_struct.address_array[2];
    write_data_array[4] = 0x00;                                       // Dummy Byte

    uint8_t temp_read_data_array_length = write_data_array_size + read_data_array_size;
    uint8_t temp_read_data_array[temp_read_data_array_length];

    spim_read_data_array(write_data_array, write_data_array_size, temp_read_data_array, temp_read_data_array_length);

    for(uint8_t i = 0; i < read_data_array_size; i++)
    {
        read_data_array[i] = temp_read_data_array[write_data_array_size + i];
        NRF_LOG_INFO("read_data_array[%u]: %u", i, read_data_array[i]);
    }
}

static void _get_address_array(uint32_t address)
{
    NRF_LOG_INFO("_get_address_array");
    control_struct.address_array[0] = (uint8_t)((address & 0b00000000000011110000000000000000) >> 16);
    control_struct.address_array[1] = (uint8_t)((address & 0b00000000000000001111111100000000) >> 8);
    control_struct.address_array[2] = (uint8_t)(address & 0b000000000000000000000000011111111);
}

uint32_t cy15b108qi_get_current_write_address(void)
{
    NRF_LOG_INFO("cy15b108qi_get_current_write_address");
    return control_struct.write_address;
}
#endif