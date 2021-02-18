#include "cy15b108qi.h"

#if CY15B108QI

static struct CY15B108QI_Status_Register_Struct status_register;
static struct CY15B108QI_Device_ID_Register_Struct device_id_register;
static struct CY15B108QI_Unique_ID_Register_Struct unique_id_register;
static struct CY15B108QI_Control_Struct control;

/* Public Functions */

void cy15b108qi_init(void)
{
    NRF_LOG_INFO("cy15b108qi_init");

    control.opcode_command[0] = 0;

    _cy15b108qi_read_device_id_register();
    _cy15b108qi_read_unique_id_register();
}

void cy15b108qi_uninit(void)
{
    NRF_LOG_INFO("cy15b108qi_uninit");
    cy15b108qi_enter_hibernation_mode();
}

/*
* A power-saving Deep Power-Down mode is implemented on the CY15X108QI device. The device enters the Deep Power-Down mode after tENTDPD
* time after the DPD opcode BAh is clocked in and a rising edge of CS is applied.
*/
void cy15b108qi_enter_deep_power_down_mode(void)
{
    NRF_LOG_INFO("cy15b108qi_enter_deep_power_down_mode_command");

    control.opcode_command[0] = CY15B108QI_DPD_OPCODE_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
}

/*
* A CS pulse-width of tCSDPD exits the DPD mode after tEXTDPD time. The CS pulse-width can be generated either by sending a dummy command 
* cycle or toggling CS alone while SCK and I/Os are don’t care.
*/
void cy15b108qi_exit_deep_power_down_mode(void)
{
    NRF_LOG_INFO("cy15b108qi_exit_deep_power_down_mode_command");

    control.opcode_command[0] = CY15B108QI_WAKEUP_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
    nrf_delay_us(DEEP_POWER_DOWN_MODE_DELAY);    // Delay 250 us to allow the CY15B108QI to wake up
}

/*
* A  lowest  power  Hibernate  mode  is  implemented  on  the CY15X108QI device. The device enters Hibernate mode after tENTHIB time after
* the HBN opcode B9h is clocked in and a rising edge of CS is applied.
*/
void cy15b108qi_enter_hibernation_mode(void)
{
    NRF_LOG_INFO("cy15b108qi_enter_hibernate_mode_command");

    control.opcode_command[0] = CY15B108QI_HBN_OPCODE_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
}

/*
* On the next falling edge of CS, the device will return to normal operation within tEXTHIB time. The SO pin remains in a Hi-Z state 
* during the wakeup from hibernate period.
*/
void cy15b108qi_exit_hibernation_mode(void)
{
    NRF_LOG_INFO("cy15b108qi_exit_hibernate_mode_command");

    control.opcode_command[0] = CY15B108QI_WAKEUP_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
    nrf_delay_ms(HIBERNATION_MODE_DELAY);    // Delay 5 ms to allow the CY15B108QI to wake up
}

/*
* All writes to the memory begin with a WREN opcode with CS being asserted and deasserted. The next opcode is WRITE. 
* The WRITE opcode is followed by a three-byte address containingthe 20-bit address (A19–A0) of the first data byte 
* to be written into the memory. The upper four bits of the three-byte addressare ignored. Subsequent bytes are data bytes, 
* which are written sequentially.
*/
void cy15b108qi_write_registers(uint8_t *write_data, uint8_t write_data_length, uint32_t start_write_address)
{
    NRF_LOG_INFO("_cy15b108qi_write_data_command");

    _cy15b108qi_set_write_enable_latch();

    control.opcode_command[0] = CY15B108QI_WRITE_OPCODE_COMMAND;
    uint32_t temp_data_length = write_data_length + ARRAY_LENGTH(control.opcode_command) + ARRAY_LENGTH(control.address);
    uint8_t temp_data[temp_data_length];
    NRF_LOG_INFO("start_write_address: %X", start_write_address);
    NRF_LOG_INFO("write_data_length: %u", write_data_length);    
    NRF_LOG_INFO("temp_data_length: %u", temp_data_length);
    _set_address_array(start_write_address);
    
    temp_data[0] = control.opcode_command[0]; 
    temp_data[1] = control.address[0];
    temp_data[2] = control.address[1];
    temp_data[3] = control.address[2];
    
    for(uint8_t i = 0; i < write_data_length; i++)
    {
        temp_data[i+4] = write_data[i];
    }
    
    spim_write_data_array(temp_data, temp_data_length);

    _cy15b108qi_reset_write_enable_latch();
}

/*
* After the falling edge of CS, the bus master can issue a READopcode. Following the READ command is a three-byte address containing 
* the 20-bit address (A19–A0) of the first byte of theread operation. The upper four bits of the address are ignored. After the opcode
* and address are issued, the device drives out the read data on the next eight clocks.
*/
uint8_t cy15b108qi_read_single_register(uint32_t read_address)
{
    NRF_LOG_INFO("_cy15b108qi_read_data_command");

    control.opcode_command[0] = CY15B108QI_READ_OPCODE_COMMAND;

    uint32_t write_data_length = ARRAY_LENGTH(control.opcode_command) + ARRAY_LENGTH(control.address);
    NRF_LOG_INFO("write_data_array_length: %u", write_data_length);
    uint8_t write_data[write_data_length];
    
    _set_address_array(read_address);
    
    write_data[0] = control.opcode_command[0]; 
    write_data[1] = control.address[0];
    write_data[2] = control.address[1];
    write_data[3] = control.address[2];
    
    uint8_t temp_read_data_length = write_data_length + 1;
    uint8_t temp_read_data[temp_read_data_length];

    spim_read_data_array(write_data, write_data_length, temp_read_data, temp_read_data_length);    // One can only read 1 byte of data at a time
    NRF_LOG_INFO("Register Address: %X; Array Value: %X", read_address, temp_read_data[temp_read_data_length]); // Debugging purposes only
    return temp_read_data[temp_read_data_length];
}

/*
* The CY15X108QI supports a FAST READ opcode (0Bh) that isprovided for opcode compatibility with serial flash devices. The FAST  READ  opcode
* is  followed  by  a  three-byte  address containing the 20-bit address (A19–A0) of the first byte of theread operation and then a dummy byte.
* The dummy byte insertsa  read  latency  of  8-clock  cycle.  The  fast  read  operation  is otherwise the same as an ordinary read operation
* except that it requires an additional dummy byte. After receiving the opcode, address, and a dummy byte, the CY15X108QI starts driving its
* SO line with data bytes, with MSb first, and continues trans-mitting  as  long  as  the  device  is  selected  and  the  clock  is available.
*/
void cy15b108qi_fast_read_registers(uint8_t *read_data, uint8_t read_data_length, uint32_t start_address)
{
    NRF_LOG_INFO("_cy15b108qi_read_data_command");

    control.opcode_command[0] = CY15B108QI_FSTRD_OPCODE_COMMAND;

    uint32_t write_data_length = ARRAY_LENGTH(control.opcode_command) + ARRAY_LENGTH(control.address) + 1;    // Extra Dummy Byte
    uint8_t write_data[write_data_length];
    NRF_LOG_INFO("write_data_length: %u", write_data_length);

    _set_address_array(start_address);
    
    write_data[0] = control.opcode_command[0]; 
    write_data[1] = control.address[0];
    write_data[2] = control.address[1];
    write_data[3] = control.address[2];
    write_data[4] = 0x00;                                       // Dummy Byte

    uint8_t temp_read_data_length = write_data_length + read_data_length;
    uint8_t temp_read_data[temp_read_data_length];

    spim_read_data_array(write_data, write_data_length, temp_read_data, temp_read_data_length);

    for(uint8_t i = 0; i < read_data_length; i++)
    {
        read_data[i] = temp_read_data[write_data_length + i];
        NRF_LOG_INFO("read_data[%u]: %u", i, read_data[i]);
    }
}

void cy15b108qi_get_manufacturer_id(uint8_t *temp_manufacturer_id)
{
    for(uint8_t i = 0; i < ARRAY_LENGTH(device_id_register.manufacturer_id); i++)
    {
        temp_manufacturer_id[i] = device_id_register.manufacturer_id[i];
    }
}

void cy15b108qi_get_unique_id(uint8_t *temp_unique_id)
{
    for(uint8_t i = 0; i < ARRAY_LENGTH(unique_id_register.unique_id); i++)
    {
        temp_unique_id[i] = unique_id_register.unique_id[i];
    }
}

/* Static Functions */

/* 
* Read Status Register to read the following values:
* D[7], WPEN = Write protect enable => Used to enable the function of the write protect pin
* D[6] - D[4], NOT USED
* D[3], BP1 = Block protect bit '0'
* D[2], BP0 = Block protect bit '1'
* D[1], WEL = Indication if the device is write enabled. This bit defaults to '0' (disabled) on power up
* D[0], NOT USED
*/
static void _cy15b108qi_read_status_register(void)
{
    NRF_LOG_INFO("_cy15b108qi_read_status_register");

    control.opcode_command[0] = CY15B108QI_RDSR_OPCODE_COMMAND;
    uint8_t temp_data_length = ARRAY_LENGTH(control.opcode_command) + ARRAY_LENGTH(status_register.data);
    uint8_t temp_data[temp_data_length];

    spim_read_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command), temp_data, temp_data_length);
    status_register.data[0] = temp_data[1];

    status_register.wel = (status_register.data[0] & 0b00000010) && 0b00000010;
    status_register.bp0 = (status_register.data[0] & 0b00000100) && 0b00000100;
    status_register.bp1 = (status_register.data[0] & 0b00001000) && 0b00001000;
    status_register.wpen = (status_register.data[0] & 0b10000000) && 0b10000000;

    NRF_LOG_INFO("status_register.wel: %u", status_register.wel);
    NRF_LOG_INFO("status_register.bp0: %u", status_register.bp0);
    NRF_LOG_INFO("status_register.bp1: %u", status_register.bp1);
    NRF_LOG_INFO("status_register.wpen: %u", status_register.wpen);
}

static void _cy15b108qi_write_status_register(void)
{
    NRF_LOG_INFO("_cy15b108qi_write_status_register");

    control.opcode_command[0] = CY15B108QI_WRSR_OPCODE_COMMAND;

    status_register.data[0] = status_register.wel << 1;
    status_register.data[0] = (status_register.bp0 << 2) | status_register.data[0];
    status_register.data[0] = (status_register.bp1 << 3) | status_register.data[0];
    status_register.data[0] = (status_register.wpen << 7) | status_register.data[0];

    uint8_t temp_data[ARRAY_LENGTH(control.opcode_command) + ARRAY_LENGTH(status_register.data)];
    temp_data[0] = control.opcode_command[0];
    temp_data[1] = status_register.data[0];

    spim_write_data_array(temp_data, ARRAY_LENGTH(temp_data));
}

/*
* Set write enable latch. The CY15X108QI will power up with writes disabled. The WREN command must be issued before any write operation. 
* Sending the WREN opcode allows the user to issue subsequent opcodesfor write operations.
* Read the status register afterwards and verify the WEL bit is '1'
*/
static void _cy15b108qi_set_write_enable_latch(void)
{
    NRF_LOG_INFO("cy15b108qi_set_write_enable_latch_command");

    control.opcode_command[0] = CY15B108QI_WREN_OPCODE_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
}

/*
* The WRDI command disables all write activity by clearing the Write Enable Latch.
* Read the status register afterwards and verify the WEL bit is '0'
*/
static void _cy15b108qi_reset_write_enable_latch(void)
{
    NRF_LOG_INFO("cy15b108qi_reset_write_enable_latch_command");

    control.opcode_command[0] = CY15B108QI_WRDI_OPCODE_COMMAND;

    spim_write_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command));
}

/*
* The  CY15X108QI  device  can  be  interrogated  for  its manufacturer, product identification, and die revision. The RDID opcode 9Fh allows
* the user to read the 9-byte manufacturer ID and  product  ID,  both  of  which  are  read-only  bytes.  The JEDEC-assigned  manufacturer  ID
* places  the  Cypress(Ramtron) identifier in bank 7; therefore, there are six bytes ofthe continuation code 7Fh followed by the single byte 
* C2h. Thereare two bytes of product ID, which includes a family code, adensity code, a sub code, and the product revision code.
*/
static void _cy15b108qi_read_device_id_register(void)
{
    NRF_LOG_INFO("cy15b108qi_read_device_id_command");

    control.opcode_command[0] = CY15B108QI_RDID_OPCODE_COMMAND;
    uint8_t temp_data_length = ARRAY_LENGTH(device_id_register.data) + ARRAY_LENGTH(control.opcode_command);
    uint8_t temp_data[temp_data_length];
    spim_read_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command), temp_data, temp_data_length);

    for(uint8_t i = 0; i < ARRAY_LENGTH(device_id_register.data); i++)
    {
        device_id_register.data[i] = temp_data[i+1];
    }

    device_id_register.frequency = device_id_register.data[0] & 0b00000011;
    device_id_register.voltage = (device_id_register.data[0] & 0b00000100) >> 2;
    device_id_register.revision = (device_id_register.data[0] & 0b00011000) >> 3;
    device_id_register.sub_type = (device_id_register.data[0] & 0b11100000) >> 5;

    device_id_register.inrush = device_id_register.data[1] & 0b00000001;
    device_id_register.density = (device_id_register.data[1] & 0b00011110) >> 1;
    device_id_register.family = (device_id_register.data[1] & 0b11100000) >> 5;

    NRF_LOG_INFO("device_id_register.frequency: %u", device_id_register.frequency);
    NRF_LOG_INFO("device_id_register.voltage: %u", device_id_register.voltage);
    NRF_LOG_INFO("device_id_register.revision: %u", device_id_register.revision);
    NRF_LOG_INFO("device_id_register.sub_type: %u", device_id_register.sub_type);

    NRF_LOG_INFO("device_id_register.inrush: %u", device_id_register.inrush);
    NRF_LOG_INFO("device_id_register.density: %u", device_id_register.density);
    NRF_LOG_INFO("device_id_register.family: %u", device_id_register.family);

    for(uint8_t i = 0; i < 7; i++)
    {
        device_id_register.manufacturer_id[i] = device_id_register.data[2+i]; 
    }
    NRF_LOG_INFO("device_id_register.manufacturer_id:");
    NRF_LOG_HEXDUMP_INFO(device_id_register.manufacturer_id, ARRAY_LENGTH(device_id_register.manufacturer_id));
}

/*
* The  CY15X102QN  device  can  be  interrogated  for  unique  ID which is a factory programmed, 64-bit number unique to each device. 
* The RUID opcode, 4Ch allows to read the 8-byte, readonly unique ID. 
*/
static void _cy15b108qi_read_unique_id_register(void)
{
    NRF_LOG_INFO("cy15b108qi_read_unique_id_command");

    control.opcode_command[0] = CY15B108QI_RUID_OPCODE_COMMAND;

    uint8_t temp_data_length = ARRAY_LENGTH(unique_id_register.unique_id) + ARRAY_LENGTH(control.opcode_command);
    uint8_t temp_data[temp_data_length];

    spim_read_data_array(control.opcode_command, ARRAY_LENGTH(control.opcode_command), temp_data, temp_data_length);

    for(uint8_t i = 0; i < ARRAY_LENGTH(unique_id_register.unique_id); i++)
    {
        unique_id_register.unique_id[i] = temp_data[i+1];
    }
    NRF_LOG_INFO("unique_id_register.unique_id:");
    NRF_LOG_HEXDUMP_INFO(unique_id_register.unique_id, ARRAY_LENGTH(unique_id_register.unique_id));
}

static void _set_address_array(uint32_t address)
{
    NRF_LOG_INFO("_get_address_array");
    control.address[0] = (uint8_t)((address & 0b00000000000011110000000000000000) >> 16);
    control.address[1] = (uint8_t)((address & 0b00000000000000001111111100000000) >> 8);
    control.address[2] = (uint8_t)(address & 0b000000000000000000000000011111111);
}

#endif