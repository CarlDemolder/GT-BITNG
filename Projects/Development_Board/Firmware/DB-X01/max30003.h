#ifndef MAX30003_H_
#define MAX30003_H_

#include "spi.h"
#include "common.h"
#include "ecg.h"

//Register Addresses:
enum MAX30003_Registers
{
    MAX30003_STATUS_ADDRESS = 0x01,
    MAX30003_EN_INT1_ADDRESS = 0x02,
    MAX30003_EN_INT2_ADDRESS = 0x03,
    MAX30003_MNGR_INT_ADDRESS = 0x04,
    MAX30003_MNGR_DYN_ADDRESS = 0x05,
    MAX30003_SW_RST_ADDRESS = 0x08,
    MAX30003_SYNCH_ADDRESS = 0x09,
    MAX30003_FIFO_RST_ADDRESS = 0x0A,
    MAX30003_INFO_ADDRESS = 0x0F,
    MAX30003_CNFG_GEN_ADDRESS = 0x10,
    MAX30003_CNFG_CAL_ADDRESS = 0x12,
    MAX30003_CNFG_EMUX_ADDRESS = 0x14,
    MAX30003_CNFG_ECG_ADDRESS = 0x15,
    MAX30003_CNFG_RTOR1_ADDRESS = 0x1D,
    MAX30003_CNFG_RTOR2_ADDRESS = 0x1E,
    MAX30003_ECG_FIFO_BURST_ADDRESS = 0x20,
    MAX30003_ECG_FIFO_ADDRESS = 0x21,
    MAX30003_RTOR_ADDRESS = 0x25
};

/**@brief Status Register Structure. This structure contains all values read from the Status Register.*/
extern struct Status_Register
{
    uint8_t eint;             /**< Register Value */
    uint8_t eovf;             /**< Register Value */  
    uint8_t fstint;           /**< Register Value */
    uint8_t dcloffint;        /**< Register Value */
    uint8_t lonint;           /**< Register Value */
    uint8_t rrint;            /**< Register Value */
    uint8_t samp;             /**< Register Value */
    uint8_t pllint;           /**< Register Value */
    uint8_t ldoff_ph;         /**< Register Value */
    uint8_t ldoff_pl;         /**< Register Value */
    uint8_t ldoff_nh;         /**< Register Value */
    uint8_t ldoff_nl;         /**< Register Value */
} status_register;

/**@brief Interrupt Register Structure. This structure contains all values to read/write from the Interrupt Register.*/
struct Interrupt_Register
{
    uint8_t en_eint;          /**< Register Value */
    uint8_t en_eovf;          /**< Register Value */  
    uint8_t en_fstint;        /**< Register Value */
    uint8_t en_dcloffint;     /**< Register Value */
    uint8_t en_lonint;        /**< Register Value */
    uint8_t en_rrint;         /**< Register Value */
    uint8_t en_samp;          /**< Register Value */
    uint8_t en_pllint;        /**< Register Value */
    uint8_t int1b_port;       /**< Register Value */
    uint8_t int2b_port;       /**< Register Value */
};

extern struct Interrupt_Register interrupt1_register;
extern struct Interrupt_Register interrupt2_register;

/**@brief Interrupt Manager Register Structure. This structure contains all values read from the Interrupt Manager Register.*/
extern struct Interrupt_Manager_Register
{
    uint8_t efit;                 /**< Register Value */
    uint8_t clr_fast;             /**< Register Value */  
    uint8_t clr_rrint;            /**< Register Value */
    uint8_t clr_samp;             /**< Register Value */
    uint8_t samp_it;              /**< Register Value */
} interrupt_manager_register;

/**@brief Dynamic Mode Manager Register Structure. This structure contains all values read from the Dynamic Mode Manager Register.*/
extern struct Dynamic_Mode_Manager_Register
{
    uint8_t fast;                 /**< Register Value */
    uint8_t fast_th;              /**< Register Value */  
} dynamic_mode_manager_register;

/**@brief Info Register Structure. This contains all values read from the Info Register.*/
extern struct Info_Register
{
    uint8_t info_id;             /**< Register Value */
    uint8_t rev_id;              /**< Register Value */  
} info_register;

/**@brief General Configuration Register Structure. This structure contains all values read from the General Configuration Register.*/
extern struct General_Configuration_Register
{
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
} general_configuration_register;

/**@brief Calibration Configuration Register Structure. This structure contains all values read from the Calibration Configuration Register.*/
extern struct Calibration_Configuration_Register
{
    uint8_t en_vcal;                /**< Register Value */
    uint8_t vmode;                  /**< Register Value */  
    uint8_t vmag;                   /**< Register Value */
    uint8_t fcal;                   /**< Register Value */  
    uint8_t fifty;                  /**< Register Value */
    uint8_t thigh;                  /**< Register Value */  
} calibration_configuration_register;

//Function Prototypes:
void max30003_read_device_info(void);

void max30003_read_device_status(void);

void max30003_init(void);

void max30003_init_interrupt(void);

void max30003_enable_interrupt(void);

void max30003_readback_registers(void);

void max30003_soft_reset(void);

void max30003_sync(void);

void max30003_read_fifo_data(uint8_t* data_array);

static void _max30003_init_interrupt(void);

static void _max30003_interrupt_handler();

uint8_t max30003_read_interrupt();

void max30003_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void max30003_interrupt1_enable(void);

void max30003_interrupt1_disable(void);

void max30003_interrupt2_enable(void);

void max30003_interrupt2_disable(void);

void max30003_read_status_register(void);

void max30003_read_interrupt1_register(void);

void max30003_write_interrupt1_register(void);

void max30003_read_interrupt2_register(void);

void max30003_write_interrupt2_register(void);

void max30003_read_interrupt_manager_register(void);

void max30003_write_interrupt_manager_register(void);

void max30003_read_dynamic_mode_manager_register(void);

void max30003_write_dynamic_mode_manager_register(void);

void max30003_write_software_reset_register(void);

void max30003_write_synchronize_register(void);

void max30003_write_fifo_reset_register(void);

void max30003_read_info_register(void);

#endif //MAX30003_H_