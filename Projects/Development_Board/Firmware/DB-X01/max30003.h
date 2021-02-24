#ifndef MAX30003_H_
#define MAX30003_H_

#include "serial_slave.h"
#include "spi.h"

#if MAX30003

//Register Addresses:
enum MAX30003_Registers
{
    MAX30003_STATUS = 0x01,
    MAX30003_EN_INT1 = 0x02,
    MAX30003_EN_INT2 = 0x03,
    MAX30003_MNGR_INT = 0x04,
    MAX30003_MNGR_DYN = 0x05,
    MAX30003_SW_RS = 0x08,
    MAX30003_SYNCH = 0x09,
    MAX30003_FIFO_RST = 0x0A,
    MAX30003_INFO = 0x0F,
    MAX30003_CNFG_GEN = 0x10,
    MAX30003_CNFG_CAL = 0x12,
    MAX30003_CNFG_EMUX = 0x14,
    MAX30003_CNFG_ECG = 0x15,
    MAX30003_CNFG_RTOR1 = 0x1D,
    MAX30003_CNFG_RTOR2 = 0x1E,
    MAX30003_ECG_FIFO_BURST = 0x20,
    MAX30003_ECG_FIFO = 0x21,
    MAX30003_RTOR = 0x25
};

enum MAX30003_ECG_Constants
{
    MAX30003_MAX_FIFO_WORDS = 0x20,             // 32 is the max number of words available to read from the FIFO
    MAX30003_DELAY = 1,                         // Appropriate delay to let the MAX30003
    MAX30003_SPS_128 = 128,
    MAX30003_SPS_256 = 256,
    MAX30003_SPS_512 = 512, 
    MAX30003_EXTERNAL_MEMORY_START_ADDRESS = 0x000119,
    MAX30003_EXTERNAL_MEMORY_END_ADDRESS = 0x0FFFFF,
};

enum MAX30003_ECG_Data_Tags
{
    MAX30003_ETAG_VALID = 0b00000000,       // Valid Sample
    MAX30003_ETAG_FAST = 0b00000001,        // Fast Mode Sample
    MAX30003_ETAG_VALID_EOF = 0b00000010,   // Last Valid Sample
    MAX30003_ETAG_FAST_EOF = 0b00000011,    // Last Fast Mode Sample
    MAX30003_ETAG_EMPTY = 0b00000110,       // FIFO Empty (Exception)
    MAX30003_OVERFLOW = 0b00000111          // FIFO Overflow
};

/**@brief Control Structure. This structure contains all variables that are used in the MAX30003 driver.*/
struct MAX30003_Control_Struct
{
    ret_code_t error_code;                                  /**< Variable to track errors */

    uint8_t interrupt;                                      /**< Variable to record when the data interrupt flag */
    uint8_t counter;                                        /**< counter to iterate through values */
    uint8_t samples_per_interrupt;                          /**< Counter value to record the number of samples per interrupt */
    uint8_t samples_per_bluetooth_transmission;             /**< Variable to record the number of samples per bluetooth transmission */
    uint8_t bytes_per_bluetooth_transmission;               /**< Variable to record the number of bytes per bluetooth transmission */
    uint32_t samples_per_recording_session;                 /**< Variable to record the number of samples per recording session */
    uint32_t bytes_per_recording_session;                   /**< Variable to record the number of bytes per recording session */
    uint32_t current_sample_count;                          /**< Variable to record the current sample count */         
    uint32_t samples_per_second;                            /**< Variable to record the number of samples per minute */
    uint32_t bytes_left_to_transmit;                        /**< Variable to record the number of bytes left to transmit over BLE/USB */
    uint8_t bytes_per_sample;                               /**< Variable to record the number of bytes per samples */
    uint8_t bytes_per_interrupt;                            /**< Variable to record the number of bytes per interrupt */
    uint8_t data_ready_for_transmit;                        /**< Variable to record when the data is ready for transmit */

    uint8_t spi_data[3];                                    /**< Array to store SPI Data */
    uint8_t register_byte_count;                            /**< Number of bytes per register */

    uint8_t long_term_storage;                              /**< Variable to record a flag to determine if the data is going to be stored long term */

    uint32_t external_memory_write_start_address;           /**< Variable to record the start address to store values in the external memory */
    uint32_t external_memory_write_current_address;         /**< Variable to record the current address to store values in the external memory */
    
    uint32_t external_memory_start_address;                 /**< Variable to record the start address of the external memory */
    uint32_t external_memory_end_address;                   /**< Variable to record the end address of the external memory */

    uint32_t external_memory_transmit_start_address;        /**< Variable to record the start address to transmit stored values of the external memory over BLE/USB */
    uint32_t external_memory_transmit_end_address;          /**< Variable to record the end address to transmit stored values of the external memory over BLE/USB */
    uint32_t external_memory_transmit_current_address;      /**< Variable to record the current address to transmit stored values of the external memory over BLE/USB */
};

/**@brief Status Register Structure. This structure contains all values read from the Status Register.*/
struct MAX30003_Status_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t eint;                   /**< Register Value */
    uint8_t eovf;                   /**< Register Value */  
    uint8_t fstint;                 /**< Register Value */
    uint8_t dcloffint;              /**< Register Value */
    uint8_t lonint;                 /**< Register Value */
    uint8_t rrint;                  /**< Register Value */
    uint8_t samp;                   /**< Register Value */
    uint8_t pllint;                 /**< Register Value */
    uint8_t ldoff_ph;               /**< Register Value */
    uint8_t ldoff_pl;               /**< Register Value */
    uint8_t ldoff_nh;               /**< Register Value */
    uint8_t ldoff_nl;               /**< Register Value */
};

/**@brief Interrupt Register Structure. This structure contains all values to read/write from the Interrupt Register.*/
struct MAX30003_Interrupt_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t en_eint;                /**< Register Value */
    uint8_t en_eovf;                /**< Register Value */  
    uint8_t en_fstint;              /**< Register Value */
    uint8_t en_dcloffint;           /**< Register Value */
    uint8_t en_lonint;              /**< Register Value */
    uint8_t en_rrint;               /**< Register Value */
    uint8_t en_samp;                /**< Register Value */
    uint8_t en_pllint;              /**< Register Value */
    uint8_t int1b_port;             /**< Register Value */
    uint8_t int2b_port;             /**< Register Value */
};

/**@brief Interrupt Manager Register Structure. This structure contains all values read from the Interrupt Manager Register.*/
struct MAX30003_Interrupt_Manager_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t efit;                   /**< Register Value */
    uint8_t clr_fast;               /**< Register Value */  
    uint8_t clr_rrint;              /**< Register Value */
    uint8_t clr_samp;               /**< Register Value */
    uint8_t samp_it;                /**< Register Value */
};

/**@brief Dynamic Mode Manager Register Structure. This structure contains all values read from the Dynamic Mode Manager Register.*/
struct MAX30003_Dynamic_Mode_Manager_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t fast;                   /**< Register Value */
    uint8_t fast_th;                /**< Register Value */  
};

/**@brief Info Register Structure. This contains all values read from the Info Register.*/
struct MAX30003_Info_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t info_id;                /**< Register Value */
    uint8_t rev_id;                 /**< Register Value */  
};

/**@brief General Configuration Register Structure. This structure contains all values read from the General Configuration Register.*/
struct MAX30003_General_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t en_ulp_lon;             /**< Register Value */
    uint8_t fmstr;                  /**< Register Value */  
    uint8_t en_ecg;                 /**< Register Value */
    uint8_t en_dcloff;              /**< Register Value */  
    uint8_t dcloff_ipol;            /**< Register Value */
    uint8_t dcloff_imag;            /**< Register Value */  
    uint8_t dcloff_vth;             /**< Register Value */
    uint8_t en_rbias;               /**< Register Value */  
    uint8_t rbiasv;                 /**< Register Value */  
    uint8_t rbiasp;                 /**< Register Value */
    uint8_t rbiasn;                 /**< Register Value */  
};

/**@brief Calibration Configuration Register Structure. This structure contains all values read from the Calibration Configuration Register.*/
struct MAX30003_Calibration_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t en_vcal;                /**< Register Value */
    uint8_t vmode;                  /**< Register Value */  
    uint8_t vmag;                   /**< Register Value */
    uint8_t fcal;                   /**< Register Value */  
    uint8_t fifty;                  /**< Register Value */
    uint16_t thigh;                 /**< Register Value */  
};

/**@brief Input Multiplexer Configuration Register Structure. This structure contains all values read from the Input Multiplexer Configuration Register.*/
struct MAX30003_Input_Multiplexer_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t pol;                    /**< Register Value */
    uint8_t openp;                  /**< Register Value */  
    uint8_t openn;                  /**< Register Value */
    uint8_t calp_sel;               /**< Register Value */  
    uint8_t caln_sel;               /**< Register Value */
};

/**@brief ECG Configuration Register Structure. This structure contains all values read from the ECG Configuration Register.*/
struct MAX30003_ECG_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t rate;                   /**< Register Value */
    uint8_t gain;                   /**< Register Value */  
    uint8_t dhpf;                   /**< Register Value */
    uint8_t dlpf;                   /**< Register Value */  
}; 

/**@brief RTOR1 Configuration Register Structure. This structure contains all values read from the RTOR1 Configuration Register.*/
struct MAX30003_RTOR1_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t wndw;                   /**< Register Value */
    uint8_t gain;                   /**< Register Value */  
    uint8_t en_rtor;                /**< Register Value */
    uint8_t pavg;                   /**< Register Value */ 
    uint8_t ptsf;                   /**< Register Value */ 
};

/**@brief RTOR2 Configuration Register Structure. This structure contains all values read from the RTOR2 Configuration Register.*/
struct MAX30003_RTOR2_Configuration_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
    uint8_t hoff;                   /**< Register Value */
    uint8_t ravg;                   /**< Register Value */  
    uint8_t rhsf;                   /**< Register Value */
};

/**@brief FIFO Memory Register Structure. This structure contains all values read from the RTO Configuration Register.*/
struct MAX30003_ECG_FIFO_Memory_Register_Struct
{
    uint8_t register_pointer;                                       /**< Register Value */    
    uint8_t data[(MAX30003_MAX_FIFO_WORDS+1)*3];                    /**< Register Value, Number of words to read from the FIFO, 3 bytes per word */
    uint16_t ecg_voltage[(MAX30003_MAX_FIFO_WORDS+1)];              /**< Register Value */
    uint8_t etag[(MAX30003_MAX_FIFO_WORDS+1)];                      /**< Register Value */  
    uint8_t ptag[(MAX30003_MAX_FIFO_WORDS+1)];                      /**< Register Value */ 
};

struct MAX30003_Software_Reset_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
};

struct MAX30003_Synchronization_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
};

struct MAX30003_FIFO_Reset_Register_Struct
{
    uint8_t register_pointer;       /**< Register Value */
};

/* Public Functions */
void max30003_read_device_info(void);

void max30003_read_device_status(void);

void max30003_set_samples_per_interrupt(uint8_t temp_samples_per_interrupt);

void max30003_set_samples_per_second(uint16_t temp_samples_per_second);

uint8_t max30003_get_status_register_eovf(void);

void max30003_init(void);

void max30003_start_recording(void);

uint8_t max30003_get_samples_per_interrupt(void);

uint8_t max30003_get_samples_per_second(void);

uint8_t max30003_get_bytes_per_sample(void);

uint8_t max30003_get_bytes_per_interrupt(void);

void max30003_interrupt1_enable(void);

void max30003_interrupt1_disable(void);

void max30003_interrupt2_enable(void);

void max30003_interrupt2_disable(void);

void max30003_get_ecg_voltage(uint8_t* data, uint8_t data_length);

void max30003_soft_reset(void);

void max30003_sync(void);

void max30003_init_pin_interrupt(void);

void max30003_enable_pin_interrupt(void);

void max30003_disable_pin_interrupt(void);

void max30003_pin_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void max30003_fifo_reset(void);

void max30003_read_ecg_fifo_memory(void);

void max30003_interrupt_handler(void);

void max30003_transmit_ecg_recording_session(void);

uint32_t max30003_get_bytes_left_to_transmit(void);

void max30003_enable_long_term_storage(void);

void max30003_disable_long_term_storage(void);

void max30003_start_data_collection(void);

void max30003_stop_data_collection(void);

/* Static Functions */

static void _max30003_read_status_register(void);

static void _max30003_read_interrupt1_register(void);

static void _max30003_write_interrupt1_register(void);

static void _max30003_read_interrupt2_register(void);

static void _max30003_write_interrupt2_register(void);

static void _max30003_read_interrupt_manager_register(void);

static void _max30003_write_interrupt_manager_register(void);

static void _max30003_read_dynamic_mode_manager_register(void);

static void _max30003_write_dynamic_mode_manager_register(void);

static void _max30003_write_software_reset_register(void);

static void _max30003_write_synchronize_register(void);

static void _max30003_write_fifo_reset_register(void);

static void _max30003_read_info_register(void);

static void _max30003_read_general_configuration_register(void);

static void _max30003_write_general_configuration_register(void);

static void _max30003_read_calibration_configuration_register(void);

static void _max30003_write_calibration_configuration_register(void);

static void _max30003_read_input_multiplexer_configuration_register(void);

static void _max30003_write_input_multiplexer_configuration_register(void);

static void _max30003_read_ecg_configuration_register(void);

static void _max30003_write_ecg_configuration_register(void);

static void _max30003_read_rtor1_configuration_register(void);

static void _max30003_write_rtor1_configuration_register(void);

static void _max30003_read_rtor2_configuration_register(void);

static void _max30003_write_rtor2_configuration_register(void);

static void _max30003_read_ecg_fifo_memory_register(void);

static void _max30003_spim_read_register(uint8_t register_address, uint8_t *data, uint8_t data_length);

static void _max30003_spim_read_registers(uint8_t start_register_address, uint8_t *data, uint8_t data_length);

static void _max30003_spim_write_registers(uint8_t start_register_address, uint8_t *data, uint8_t data_length);

#endif

#endif //MAX30003_H_