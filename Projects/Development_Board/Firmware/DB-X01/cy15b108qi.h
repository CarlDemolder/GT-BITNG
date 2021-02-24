#ifndef CY15B108QI_H_
#define CY15B108QI_H_

#include "serial_slave.h"
#include "spi.h"

#if CY15B108QI

enum CY15B108QI_CONSTANTS
{
    CY15B108QI_WREN_OPCODE_COMMAND = 0X06,
    CY15B108QI_WRDI_OPCODE_COMMAND = 0X04,
    
    CY15B108QI_RDSR_OPCODE_COMMAND = 0X05,
    CY15B108QI_WRSR_OPCODE_COMMAND = 0X01,
    
    CY15B108QI_WRITE_OPCODE_COMMAND = 0X02,
    
    CY15B108QI_READ_OPCODE_COMMAND = 0X03,
    CY15B108QI_FSTRD_OPCODE_COMMAND = 0X0B,
    
    CY15B108QI_SSWR_OPCODE_COMMAND = 0X42,
    CY15B108QI_SSRD_OPCODE_COMMAND = 0X4B,
    
    CY15B108QI_RDID_OPCODE_COMMAND = 0X9F,
    CY15B108QI_RUID_OPCODE_COMMAND = 0X4C,
    CY15B108QI_WRSN_OPCODE_COMMAND = 0XC2,
    CY15B108QI_RDSN_OPCODE_COMMAND = 0XC3,
    
    CY15B108QI_DPD_OPCODE_COMMAND = 0XBA,
    CY15B108QI_HBN_OPCODE_COMMAND = 0XB9,

    CY15B108QI_WAKEUP_COMMAND = 0X00,

    CY15B108QI_LAST_WRITE_ADDRESS = 0X0FFFFF,
    CY15B108QI_FIRST_WRITE_ADDRESS = 0X000000,

    HIBERNATION_MODE_DELAY = 5,
    DEEP_POWER_DOWN_MODE_DELAY = 250
};

/**@brief Status Register Structure. This structure contains all values read from the Status Register.*/
struct CY15B108QI_Status_Register_Struct
{
    uint8_t data[1];                /**< Register Value */
    uint8_t wpen;                   /**< Register Value */
    uint8_t bp1;                    /**< Register Value */  
    uint8_t bp0;                    /**< Register Value */
    uint8_t wel;                    /**< Register Value */
};

/**@brief Device ID Register Structure. This structure contains all values read from the Device ID Register.*/
struct CY15B108QI_Device_ID_Register_Struct
{
    uint8_t data[9];                /**< Register Value */
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
struct CY15B108QI_Unique_ID_Register_Struct
{
    uint8_t unique_id[8];     /**< Register Value */
};

struct CY15B108QI_Control_Struct
{
    uint8_t opcode_command[1];
    uint8_t address[3];
};

/* Public Functions */

void cy15b108qi_init(void);

void cy15b108qi_uninit(void);

void cy15b108qi_enter_deep_power_down_mode(void);

void cy15b108qi_exit_deep_power_down_mode(void);

void cy15b108qi_enter_hibernation_mode(void);

void cy15b108qi_exit_hibernation_mode(void);

void cy15b108qi_write_registers(uint8_t *write_data, uint8_t write_data_length, uint32_t start_write_address);

uint8_t cy15b108qi_read_single_register(uint32_t read_address);

void cy15b108qi_fast_read_registers(uint8_t *read_data, uint8_t read_data_length, uint32_t start_read_address);

void cy15b108qi_get_manufacturer_id(uint8_t *temp_manufacturer_id);

void cy15b108qi_get_unique_id(uint8_t *temp_unique_id);

/* Static Functions */

static void _cy15b108qi_read_status_register(void);

static void _cy15b108qi_write_status_register(void);

static void _cy15b108qi_set_write_enable_latch(void);

static void _cy15b108qi_reset_write_enable_latch(void);

static void _cy15b108qi_read_device_id_register(void);

static void _cy15b108qi_read_unique_id_register(void);

static void _set_address_array(uint32_t address);


#endif

#endif // __CY15B108QI_H