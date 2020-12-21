#ifndef CY15B108QI_H_
#define CY15B108QI_H_

#include "serial_slave.h"
#include "common.h"

#if CY15B108QI

enum CY15B108QI_CONSTANTS
{
    WREN_OPCODE_COMMAND = 0X06,
    WRDI_OPCODE_COMMAND = 0X04,
    
    RDSR_OPCODE_COMMAND = 0X05,
    WRSR_OPCODE_COMMAND = 0X01,
    
    WRITE_OPCODE_COMMAND = 0X02,
    
    READ_OPCODE_COMMAND = 0X03,
    FSTRD_OPCODE_COMMAND = 0X0B,
    
    SSWR_OPCODE_COMMAND = 0X42,
    SSRD_OPCODE_COMMAND = 0X4B,
    
    RDID_OPCODE_COMMAND = 0X9F,
    RUID_OPCODE_COMMAND = 0X4C,
    WRSN_OPCODE_COMMAND = 0XC2,
    RDSN_OPCODE_COMMAND = 0XC3,
    
    DPD_OPCODE_COMMAND = 0XBA,
    HBN_OPCODE_COMMAND = 0XB9,

    CY15B108QI_LAST_WRITE_ADDRESS = 0X0FFFFF,
    CY15B108QI_FIRST_WRITE_ADDRESS = 0X000000,

    HIBERNATION_MODE_DELAY = 5,
    DEEP_POWER_DOWN_MODE_DELAY = 250
};

/**@brief Status Register Structure. This structure contains all values read from the Status Register.*/
struct CY15B108QI_Status_Register
{
    uint8_t data_array[1];          /**< Register Value */
    uint8_t wpen;                   /**< Register Value */
    uint8_t bp1;                    /**< Register Value */  
    uint8_t bp0;                    /**< Register Value */
    uint8_t wel;                    /**< Register Value */
};

/**@brief Device ID Register Structure. This structure contains all values read from the Device ID Register.*/
struct CY15B108QI_Device_ID_Register
{
    uint8_t data_array[9];          /**< Register Value */
    uint8_t frequency;              /**< Register Value */
    uint8_t voltage;                /**< Register Value */  
    uint8_t revision;               /**< Register Value */
    uint8_t sub_type;               /**< Register Value */
    uint8_t inrush;                 /**< Register Value */
    uint8_t density;                /**< Register Value */
    uint8_t family;                 /**< Register Value */
    uint8_t manufacturer_id[7];     /**< Register Value */
};

/**@brief Unique ID Register Structure. This structure contains all values read from the Unique ID Register.*/
struct CY15B108QI_Unique_ID_Register
{
    uint8_t unique_id[8];     /**< Register Value */
};

struct CY15B108QI_Control_Struct
{
    uint8_t command[1];
    uint8_t address_array[3];
    
    uint32_t write_address;
    uint32_t start_address;
};

void cy15b108qi_init(void);
void cy15b108qi_uninit(void);

void cy15b108qi_read_status_register(void);
void cy15b108qi_write_status_register(void);

void cy15b108qi_set_write_enable_latch_command(void);

void cy15b108qi_reset_write_enable_latch_command(void);

void cy15b108qi_write_data_command(uint8_t *data_array, uint8_t data_array_size);

void cy15b108qi_fast_read_data_command(uint8_t *read_data_array, uint8_t read_data_array_size, uint32_t start_address);

void cy15b108qi_read_device_id_command(void);
void cy15b108qi_read_unique_id_command(void);

static void _get_address_array(uint32_t address);

uint32_t cy15b108qi_get_current_write_address(void);

void cy15b108qi_enter_deep_power_down_mode_command(void);

void cy15b108qi_exit_deep_power_down_mode_command(void);

void cy15b108qi_enter_hibernation_mode_command(void);

void cy15b108qi_exit_hibernation_mode_command(void);

#endif

#endif // __CY15B108QI_H