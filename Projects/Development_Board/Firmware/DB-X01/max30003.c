#include "max30003.h"


void max30003_read_device_info(void) 
{
    NRF_LOG_INFO("max30003_read_device_info");
    uint8_t data_array[3];
    spim_read_registers(MAX30003_INFO_ADDRESS, data_array, ARRAY_LENGTH(data_array));

    NRF_LOG_INFO("Device Information:");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

void max30003_read_device_status(void) 
{
    NRF_LOG_INFO("[MAX30003] Read Device Status \r\n");
    uint8_t data_array[3];
    spim_read_registers(MAX30003_STATUS_ADDRESS, data_array);

    NRF_LOG_INFO("Device Status: \r\n");
    NRF_LOG_HEXDUMP_INFO(data_array, sizeof(data_array)); // Hex Dump Output (temp)
}

void max30003_init(void) 
{
    max30003_soft_reset();        // Soft Reset
    max30003_init_interrupt();    // Initializing the interrupt pin

    // Interrupts and Dynamic Modes
    max30003_interrupt1_enable();
    max30003_interrupt2_enable();

    spim_write_single_register(MAX30003_MNGR_INT_ADDRESS, 0x78, 0x00, 0x04);    // 0x04
    spim_write_single_register(MAX30003_MNGR_DYN_ADDRESS, 0x3f, 0x00, 0x00);    // 0x05

    // Set up ECG Configurations:
    spim_write_single_register(MAX30003_CNFG_GEN_ADDRESS, 0x08, 0x10, 0x07);    // 0x10
    spim_write_single_register(MAX30003_CNFG_CAL_ADDRESS, 0x72, 0x00, 0x00);    // 0x12
    spim_write_single_register(MAX30003_CNFG_EMUX_ADDRESS, 0x0B, 0x00, 0x00);   // 0x14
    spim_write_single_register(MAX30003_CNFG_ECG_ADDRESS, 0x80, 0x50, 0x00);    // 0x15
    spim_write_single_register(MAX30003_CNFG_RTOR1_ADDRESS, 0x3f, 0x35, 0x00);  // 0x1D - RR DETECT DISABLED

    max30003_enable_interrupt();    // Enable the interrupt of the NRF52
    max30003_sync();            // Synchronize the clock frequency of the MAX30003 with the NRF52
}


void max30003_interrupt1_enable(void)
{
    interrupt1_register.en_eint = 1;        // Enable Interrupt when FIFO Buffer has been filled
    interrupt1_register.int1b_port = 3;     // Open-Drain nMOS Driver with Internal 125k Ohm pullup resistance
    max30003_write_interrupt1_register();
}

void max30003_interrupt1_disable(void)
{
    interrupt1_register.en_eint = 0;        // Disable Interrupt when FIFO Buffer has been filled
    interrupt1_register.int1b_port = 0;     // Disabled (Three-state)
    max30003_write_interrupt1_register();
}

void max30003_interrupt2_enable(void)
{
    interrupt2_register.en_eovf = 1;      // Enable Interrupt when ECG FIFO Overflow has occured
    interrupt2_register.int1b_port = 3;   // Open-Drain nMOS Driver with Internal 125k Ohm pullup resistance
    max30003_write_interrupt2_register();
}

void max30003_interrupt2_disable(void)
{
    interrupt2_register.en_eovf = 0;      // Disable Interrupt when ECG FIFO Overflow has occured
    interrupt2_register.int1b_port = 0;   // Disabled (Three-state)
    max30003_write_interrupt2_register();
}

/* 
* Read Status Register to read the following values:
* D[23], EINT = ECG FIFO Interrupt, Active High; ECG records exceed the ECG FIFO Interrupt Threshold
* D[22], EOVF = ECG FIFO Overflow, Active High; ECG FIFO has overflown and the data record has been corrupted. FIFO Reset Command to reset signal
* D[21], FSTINT = ECG Fast Recovery Mode, Active High; Issued when ECG Fast Recovery Mode is engaged
* D[20], DCLOFFINT = DC Lead-Off Detection Interrupt, Active High; Remains active as long as the leads-off condition presists, then held until cleared 
* D[19] - D[12], NOT USED 
* D[11], LONINT = Ulta-Low Power (ULP) Leads-on Detection Interrupt; The MAX30003 has determined it is in a leads-on condition. 
* D[10], RRINT = ECG R to R Detector R Event Interrupt; Issued when the R to R detector has identified a new R event.
* D[9], SAMP = Sample Synchronization Pulse; Issued on the ECG base-rate sampling instant
* D[8], PLLINT = PLL Unlocked Interrupt; Indicates that the PLL has not yet achieved or has lost its phase lock. 
* D[7] - D[4], NOT USED
* D[3], LDOFF_PH = ECGP is above the high threshold (VTHH)
* D[2], LDOFF_PL = ECGP is below the low threshold (VTHL)
* D[1], LDOFF_NH = ECGN is above the high threshold (VTHH)
* D[0], LDOFF_NL = ECGN is below the low threshold (VTHL)
*/
void max30003_read_status_register(void)
{
    uint8_t status_register_data_array[3];
    spim_read_single_register(MAX30003_STATUS_ADDRESS, status_register_data_array);

    status_register.eint = (status_register_data_array[0] & 0b10000000) && 0b10000000;
    status_register.eovf = (status_register_data_array[0] & 0b01000000) && 0b01000000;
    status_register.fstint = (status_register_data_array[0] & 0b00100000) && 0b00100000;
    status_register.dcloffint = (status_register_data_array[0] & 0b00010000) && 0b00010000;

    status_register.lonint = (status_register_data_array[1] & 0b00001000) && 0b00001000; 
    status_register.rrint = (status_register_data_array[1] & 0b00000100) && 0b00000100;
    status_register.samp = (status_register_data_array[1] & 0b00000010) && 0b00000010;
    status_register.pllint = (status_register_data_array[1] & 0b00000001) && 0b00000001;

    status_register.ldoff_ph = (status_register_data_array[2] & 0b00001000) && 0b00001000;
    status_register.ldoff_pl = (status_register_data_array[2] & 0b00000100) && 0b00000100;
    status_register.ldoff_nh = (status_register_data_array[2] & 0b00000010) && 0b00000100;
    status_register.ldoff_nl = (status_register_data_array[2] & 0b00000001) && 0b00000100;
}


/* 
* Read EN_INT1 (0x02) as an Interrupt Register to read the following values:
* D[23], EN_EINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[22], EN_EOVF = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[21], EN_FSTINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[20], EN_DCLOFFINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[19] - D[12], NOT USED 
* D[11], EN_LONINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[10], EN_RRINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[9], EN_SAMP = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[8], EN_PLLINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[7] - D[2], NOT USED
* D[1:0], INTB_TYPE1 = INT1B Port Type (EN_INT1 Selections)
*/
void max30003_read_interrupt1_register(void)
{
    uint8_t interrupt1_register_data_array[3];
    spim_read_single_register(MAX30003_EN_INT1_ADDRESS, interrupt1_register_data_array);

    interrupt1_register.en_eint = (interrupt1_register_data_array[0] & 0b10000000) && 0b10000000;
    interrupt1_register.en_eovf = (interrupt1_register_data_array[0] & 0b01000000) && 0b01000000;
    interrupt1_register.en_fstint = (interrupt1_register_data_array[0] & 0b00100000) && 0b00100000;
    interrupt1_register.en_dcloffint = (interrupt1_register_data_array[0] & 0b00010000) && 0b00010000;

    interrupt1_register.en_lonint = (interrupt1_register_data_array[1] & 0b00001000) && 0b00001000; 
    interrupt1_register.en_rrint = (interrupt1_register_data_array[1] & 0b00000100) && 0b00000100;
    interrupt1_register.en_samp = (interrupt1_register_data_array[1] & 0b00000010) && 0b00000010;
    interrupt1_register.en_pllint = (interrupt1_register_data_array[1] & 0b00000001) && 0b00000001;

    interrupt1_register.int1b_port = interrupt1_register_data_array[2] & 0b00000011;
}

void max30003_write_interrupt1_register(void)
{
    uint8_t interrupt1_register_data_array[3];

    interrupt1_register_data_array[0] = interrupt1_register.en_eint << 7;
    interrupt1_register_data_array[0] = (interrupt1_register.en_eovf << 6) | interrupt1_register_data_array[0];
    interrupt1_register_data_array[0] = (interrupt1_register.en_fstint << 5) | interrupt1_register_data_array[0];
    interrupt1_register_data_array[0] = (interrupt1_register.en_dcloffint << 4) | interrupt1_register_data_array[0];

    interrupt1_register_data_array[1] = interrupt1_register.en_lonint << 3;
    interrupt1_register_data_array[1] = (interrupt1_register.en_rrint << 2) | interrupt1_register_data_array[1];
    interrupt1_register_data_array[1] = (interrupt1_register.en_samp << 1) | interrupt1_register_data_array[1];
    interrupt1_register_data_array[1] = interrupt1_register.en_pllint | interrupt1_register_data_array[1];

    interrupt1_register_data_array[2] = interrupt1_register.int1b_port;

    spim_write_single_register(MAX30003_EN_INT1_ADDRESS, interrupt1_register_data_array[0], interrupt1_register_data_array[1], interrupt1_register_data_array[2]);
}

/*
* Read EN_INT2 (0x02) as an Interrupt Register to read the following values:
* D[23], EN_EINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[22], EN_EOVF = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[21], EN_FSTINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[20], EN_DCLOFFINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[19] - D[12], NOT USED 
* D[11], EN_LONINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[10], EN_RRINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[9], EN_SAMP = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term 
* D[8], EN_PLLINT = Interrupt Enable for interrupt terms, Active High; 1 = Individual interrupt term in included in the interrupt OR term  
* D[7] - D[2], NOT USED
* D[1:0], INTB_TYPE0 = INT2B Port Type (EN_INT2 Selections)
*/
void max30003_read_interrupt2_register(void)
{
    uint8_t interrupt2_register_data_array[3];
    spim_read_single_register(MAX30003_EN_INT2_ADDRESS, interrupt2_register_data_array);

    interrupt2_register.en_eint = (interrupt2_register_data_array[0] & 0b10000000) && 0b10000000;
    interrupt2_register.en_eovf = (interrupt2_register_data_array[0] & 0b01000000) && 0b01000000;
    interrupt2_register.en_fstint = (interrupt2_register_data_array[0] & 0b00100000) && 0b00100000;
    interrupt2_register.en_dcloffint = (interrupt2_register_data_array[0] & 0b00010000) && 0b00010000;

    interrupt2_register.en_lonint = (interrupt2_register_data_array[1] & 0b00001000) && 0b00001000; 
    interrupt2_register.en_rrint = (interrupt2_register_data_array[1] & 0b00000100) && 0b00000100;
    interrupt2_register.en_samp = (interrupt2_register_data_array[1] & 0b00000010) && 0b00000010;
    interrupt2_register.en_pllint = (interrupt2_register_data_array[1] & 0b00000001) && 0b00000001;

    interrupt2_register.int1b_port = interrupt2_register_data_array[2] & 0b00000011;
}

void max30003_write_interrupt2_register(void)
{
    uint8_t interrupt2_register_data_array[3];

    interrupt2_register_data_array[0] = interrupt2_register.en_eint << 7;
    interrupt2_register_data_array[0] = (interrupt2_register.en_eovf << 6) | interrupt2_register_data_array[0];
    interrupt2_register_data_array[0] = (interrupt2_register.en_fstint << 5) | interrupt2_register_data_array[0];
    interrupt2_register_data_array[0] = (interrupt2_register.en_dcloffint << 4) | interrupt2_register_data_array[0];

    interrupt2_register_data_array[1] = interrupt2_register.en_lonint << 3;
    interrupt2_register_data_array[1] = (interrupt2_register.en_rrint << 2) | interrupt2_register_data_array[1];
    interrupt2_register_data_array[1] = (interrupt2_register.en_samp << 1) | interrupt2_register_data_array[1];
    interrupt2_register_data_array[1] = interrupt2_register.en_pllint | interrupt2_register_data_array[1];

    interrupt2_register_data_array[2] = interrupt2_register.int1b_port;

    spim_write_single_register(MAX30003_EN_INT2_ADDRESS, interrupt2_register_data_array[0], interrupt2_register_data_array[1], interrupt2_register_data_array[2]);
}


/* 
* Read Interrupt Manager Register to read the following values:
* D[23:19], EFIT = ECG FIFO Interrupt Threshold, Active High; Issues EINT based on number of unread FIFO records
* D[19:7], NOT USED
* D[6], CLR_FAST = Fast Mode interrupt clear behavoir; 1 = FSTINT remains active until cleared by STATUS read back
* D[5:4], CLR_RRINT = RTOS R Detect Interrupt (RRINT) Clear Behavoir; many options available, see datasheet
* D[3], NOT USED 
* D[2], CLR_SAMP = Sample Synchronization Pulse clear behavoir
* D[1:0], SAMP_IT = Sample Synchronization Pulse Frequency
*/
void max30003_read_interrupt_manager_register(void)
{
    uint8_t interrupt_manager_register_data_array[3];
    spim_read_single_register(MAX30003_MNGR_INT_ADDRESS, interrupt_manager_register_data_array);

    interrupt_manager_register.efit = (interrupt_manager_register_data_array[0] & 0b11111000) >> 3;

    interrupt_manager_register.clr_fast = (interrupt_manager_register_data_array[2] & 0b01000000) && 0b01000000;
    interrupt_manager_register.clr_rrint = (interrupt_manager_register_data_array[2] & 0b00110000) >> 4;
    interrupt_manager_register.clr_samp = (interrupt_manager_register_data_array[2] & 0b00000100) && 0b00000100;
    interrupt_manager_register.samp_it = interrupt_manager_register_data_array[2] & 0b00000011; 
}

void max30003_write_interrupt_manager_register(void)
{
    uint8_t interrupt_manager_register_data_array[3];

    interrupt_manager_register_data_array[0] = interrupt_manager_register.efit << 3;

    interrupt_manager_register_data_array[2] = interrupt_manager_register.clr_fast << 6;
    interrupt_manager_register_data_array[2] = (interrupt_manager_register.clr_rrint << 4) | interrupt_manager_register_data_array[2];
    interrupt_manager_register_data_array[2] = (interrupt_manager_register.clr_samp << 2) | interrupt_manager_register_data_array[2];
    interrupt_manager_register_data_array[2] = interrupt_manager_register.samp_it | interrupt_manager_register_data_array[2];

    spim_write_single_register(MAX30003_MNGR_INT_ADDRESS, interrupt_manager_register_data_array[0], interrupt_manager_register_data_array[1], interrupt_manager_register_data_array[2]);
}

/* 
* Read Dynamic Mode Manager Register to read the following values:
* D[23:22], FAST = ECG Channel Fast Recovery Mode Selection (ECG High Pass Filter Bypass)
* D[21:16], FAST_TH = Automatic Fast Recovery Threshold
* D[15:0], NOT USED 
*/
void max30003_read_dynamic_mode_manager_register(void)
{
    uint8_t dynamic_mode_manager_register_data_array[3];
    spim_read_single_register(MAX30003_MNGR_DYN_ADDRESS, dynamic_mode_manager_register_data_array);

    dynamic_mode_manager_register.fast = (dynamic_mode_manager_register_data_array[0] & 0b11000000) >> 6;
    dynamic_mode_manager_register.fast_th = dynamic_mode_manager_register_data_array[0] & 0b00011111; 
}

void max30003_write_dynamic_mode_manager_register(void)
{
    uint8_t dynamic_mode_manager_register_data_array[3];

    dynamic_mode_manager_register_data_array[0] = dynamic_mode_manager_register.fast << 6;
    dynamic_mode_manager_register_data_array[0] = dynamic_mode_manager_register.fast_th | dynamic_mode_manager_register_data_array[0];

    spim_write_single_register(MAX30003_MNGR_DYN_ADDRESS, dynamic_mode_manager_register_data_array[0], dynamic_mode_manager_register_data_array[1], dynamic_mode_manager_register_data_array[2]);
}


/* 
* Write Software Reset Register to write the following values:
* D[23:0], SW_RST = Software Reset; Resets the MAX30003 to its original default condition at the end of the SPI SW_RST transaction
* A Software Reset is identical to power-cycling the device
* Execution occurs only if DIN[23:0] = 0x000000
*/
void max30003_write_software_reset_register(void)
{
    spim_write_single_register(MAX30003_SW_RST_ADDRESS, 0x00, 0x00, 0x00);
}

/* 
* Write Synchronize Register to write the following values:
* D[23:0], SYNCH = Synchronize; Write-only register/command that begins new ECG operations and recording, beginning on the internal MSTR clock edge
* In addition to resetting and synchronizing the operations of any active ECG and R-R circuitry, SYNCH will reset and clear the FIFO memories and the DSP filters
* Execution occurs only if DIN[23:0] = 0x000000
* If a FIFO overflow event occurs and a portion of the record is lost, it is recommended to use the SYNCH command to recover and restart the recording
* 
*/
void max30003_write_synchronize_register(void)
{
    spim_write_single_register(MAX30003_SYNCH_ADDRESS, 0x00, 0x00, 0x00);
}

/* 
* Write FIFO Reset Register to write the following values:
* D[23:0], FIFO_RST = FIFO Reset; Write-only register/command that begins a new ECG recording by resetting the FIFO memories and resuming the record
* with the next available ECG data. Operations of any active ECG and R-R circuit are not impacted by FIFO_RST. 
* FIFO_RST can be used to quickly recover from a FIFO overflow state
* Execution occurs only if DIN[23:0] = 0x000000 
*/
void max30003_write_fifo_reset_register(void)
{
    spim_write_single_register(MAX30003_SYNCH_ADDRESS, 0x00, 0x00, 0x00);
}

/* 
* Read Info Register to read the following values:
* D[23:20; 13:12] , INFO_ID = Interface Verification Code;
* D[19:16], REV_ID = Revision ID
* D[15:14; 11:0], NOT USED
*/
void max30003_read_info_register(void)
{
    uint8_t info_register_data_array[3];
    spim_read_single_register(MAX30003_INFO_ADDRESS, info_register_data_array);

    info_register.info_id = ((info_register_data_array[0] & 0b11110000) >> 2) | ((info_register_data_array[1] & 0b00110000) >> 4);
    info_register.rev_id = info_register_data_array[0] & 0b00001111; 
}

/* 
* Readand Write General Configuration Register to read and write the following values:
* D[23:22], EN_ULP_LON = Ultra-Low Power Lead-On Detection Enable; ULP mode is only active when the ECG channel is powered down/disabled
* D[21:20], FMSTR = Master Clock Frequency; These are generated from FCLK, which is always 32.768 kHz
* D[19], EN_ECG = ECG Channel Enable; 1 = Enabled, 0 = Disabled; The ECG channel must be enabled to allow R-R operation 
* D[18:14], NOT USED
* D[13:12], EN_DCLOFF = DC Lead-Off Detection Enable; Requires active selected channel, enables DCLOFF interrupt and status bit behavoir 
* D[11], DCLOFF_IPOL = DC Lead-Off current polarity (if current sources are enabled/connected)
* D[10:8], DCLOFF_IMAG = DC Lead-Off Current Magnitude Selection
* D[7:6], DCLOFF_VTH = DC Lead-Off Voltage Threshold Selection
* D[5:4], EN_RBIAS = Enable and select Resistive Lead Bias Mode
* D[3:2], RBIAS = Resistive Bias Mode Value Selection
* D[1], RBIASP = Enables Resistive Bias on Positive Input
* D[0], RBIASN = Enables Resistive Bias on Negative Input
*/
void max30003_read_general_configuration_register(void)
{
    uint8_t general_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_GEN_ADDRESS, general_configuration_register_data_array);

    general_configuration_register.en_ulp_lon = (general_configuration_register_data_array[0] & 0b11000000) >> 6;
    general_configuration_register.fmstr = (general_configuration_register_data_array[0] & 0b00110000) >> 4;
    general_configuration_register.en_ecg = (general_configuration_register_data_array[0] & 0b00001000) && 0b00001000;
    general_configuration_register.en_dcloff = (general_configuration_register_data_array[1] & 0b00110000) >> 4;
    general_configuration_register.dcloff_ipol = (general_configuration_register_data_array[1] & 0b00001000) && 0b00001000;
    general_configuration_register.dcloff_imag = general_configuration_register_data_array[1] & 0b00000111;
    general_configuration_register.dcloff_vth = (general_configuration_register_data_array[2] & 0b11000000) >> 6;
    general_configuration_register.en_rbias = (general_configuration_register_data_array[2] & 0b00110000) >> 4;
    general_configuration_register.rbiasv = (general_configuration_register_data_array[2] & 0b00001100) >> 2;
    general_configuration_register.rbiasp = general_configuration_register_data_array[2] & 0b00000010;
    general_configuration_register.rbiasn = general_configuration_register_data_array[2] & 0b00000001;
}

void max30003_write_general_configuration_register(void)
{
    uint8_t general_configuration_register_data_array[3];

    general_configuration_register_data_array[0] = general_configuration_register.en_ulp_lon << 6;
    general_configuration_register_data_array[0] = (general_configuration_register.fmstr << 4) | general_configuration_register_data_array[0];
    general_configuration_register_data_array[0] = (general_configuration_register.en_ecg << 3) | general_configuration_register_data_array[0];
    
    general_configuration_register_data_array[1] = general_configuration_register.en_dcloff << 4;
    general_configuration_register_data_array[1] = (general_configuration_register.dcloff_ipol << 3) | general_configuration_register_data_array[1];
    general_configuration_register_data_array[1] = general_configuration_register.dcloff_imag | general_configuration_register_data_array[1];
    
    general_configuration_register_data_array[2] = general_configuration_register.dcloff_vth << 6;
    general_configuration_register_data_array[2] = (general_configuration_register.en_rbias << 4) | general_configuration_register_data_array[2];
    general_configuration_register_data_array[2] = (general_configuration_register.rbiasv << 2) | general_configuration_register_data_array[2];
    general_configuration_register_data_array[2] = (general_configuration_register.rbiasp << 1) | general_configuration_register_data_array[2];
    general_configuration_register_data_array[2] = general_configuration_register.rbiasn | general_configuration_register_data_array[2];

    spim_write_single_register(MAX30003_CNFG_GEN_ADDRESS, general_configuration_register_data_array[0], general_configuration_register_data_array[1], general_configuration_register_data_array[2]);
}

/* 
* Read and Write Calibration Configuration Register to read and write the following values:
* D[23], NOT USED
* D[22], EN_VCAL = Calibration Source (VCALP and VCALN) Enable
* D[21], VMODE = Calibration Source Mode Selection
* D[20], VMAG = Calibration Source Magnitude Selection (VMAG)
* D[19:15], NOT USED
* D[14:12], FCAL = Calibration Source Frequency Selection (FCAL)
* D[11], FIFTY = Calibration Soruce Duty Cycle Mode Selection
* D[10:0], THIGH = Calibration Source Time High Selection
*/
void max30003_read_calibration_configuration_register(void)
{
    uint8_t calibration_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_CAL_ADDRESS, calibration_configuration_register_data_array);

    calibration_configuration_register.en_vcal = (calibration_configuration_register_data_array[0] & 0b01000000) && 0b01000000;
    calibration_configuration_register.vmode = (calibration_configuration_register_data_array[0] & 0b00100000) && 0b00100000;
    calibration_configuration_register.vmag = (calibration_configuration_register_data_array[0] & 0b00010000) && 0b00010000;
    calibration_configuration_register.fcal = (calibration_configuration_register_data_array[1] & 0b01110000) >> 4;
    calibration_configuration_register.fifty = (calibration_configuration_register_data_array[1] & 0b00001000) && 0b00001000;
    calibration_configuration_register.thigh = (calibration_configuration_register_data_array[1] & 0b00000111) | (calibration_configuration_register_data_array[2] & 0b11111111);
}

void max30003_write_calibration_configuration_register(void)
{
    uint8_t calibration_configuration_register_data_array[3];

    calibration_configuration_register_data_array[0] = calibration_configuration_register.en_vcal << 6;
    calibration_configuration_register_data_array[0] = (calibration_configuration_register.vmode << 5) | calibration_configuration_register_data_array[0];
    calibration_configuration_register_data_array[0] = (calibration_configuration_register.vmag << 4) | calibration_configuration_register_data_array[0];
    
    calibration_configuration_register_data_array[1] = calibration_configuration_register.fcal << 4;
    calibration_configuration_register_data_array[1] = (calibration_configuration_register.fifty << 3) | calibration_configuration_register_data_array[1];
    calibration_configuration_register_data_array[1] = (uint8_t)((calibration_configuration_register.thigh & 0b0000011100000000) >> 8 )  | calibration_configuration_register_data_array[1];
    
    calibration_configuration_register_data_array[2] = (uint8_t)(calibration_configuration_register.thigh & 0b0000000011111111);

    spim_write_single_register(MAX30003_CNFG_CAL_ADDRESS, calibration_configuration_register_data_array[0], calibration_configuration_register_data_array[1], calibration_configuration_register_data_array[2]);
}

/* 
* Read and Write Input Multiplexer Configuration Register to read and write the following values:
* D[23], POL = ECG Input Polarity Selection
* D[22], NOT USED
* D[21], OPENP = Open the ECGP Input Switch
* D[20], OPENN = Open the ECGN Input Switch
* D[19:18], CALP_SEL = ECGP Calibration Selection
* D[17:16], CALN_SEL = ECGN Calibration Selection
* D[15:0], NOT USED
*/
void max30003_read_input_multiplexer_configuration_register(void)
{
    uint8_t input_multiplexer_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_EMUX_ADDRESS, input_multiplexer_configuration_register_data_array);

    input_multiplexer_configuration_register.pol = (input_multiplexer_configuration_register_data_array[0] & 0b10000000) && 0b10000000;
    input_multiplexer_configuration_register.openp = (input_multiplexer_configuration_register_data_array[0] & 0b00100000) && 0b00100000;
    input_multiplexer_configuration_register.openn = (input_multiplexer_configuration_register_data_array[0] & 0b00010000) && 0b00010000;
    input_multiplexer_configuration_register.calp_sel = (input_multiplexer_configuration_register_data_array[0] & 0b00001100) >> 2;
    input_multiplexer_configuration_register.caln_sel = input_multiplexer_configuration_register_data_array[0] & 0b00000011;
}

void max30003_write_input_multiplexer_configuration_register(void)
{
    uint8_t input_multiplexer_configuration_register_data_array[3];

    input_multiplexer_configuration_register_data_array[0] = input_multiplexer_configuration_register.pol << 7;
    input_multiplexer_configuration_register_data_array[0] = (input_multiplexer_configuration_register.openp << 5) | input_multiplexer_configuration_register_data_array[0];
    input_multiplexer_configuration_register_data_array[0] = (input_multiplexer_configuration_register.openn << 4) | input_multiplexer_configuration_register_data_array[0];
    input_multiplexer_configuration_register_data_array[0] = (input_multiplexer_configuration_register.calp_sel << 2) | input_multiplexer_configuration_register_data_array[0];
    input_multiplexer_configuration_register_data_array[0] = input_multiplexer_configuration_register.caln_sel | input_multiplexer_configuration_register_data_array[0];
    
    spim_write_single_register(MAX30003_CNFG_EMUX_ADDRESS, input_multiplexer_configuration_register_data_array[0], input_multiplexer_configuration_register_data_array[1], input_multiplexer_configuration_register_data_array[2]);
}

/* 
* Read and Write ECG Configuration Register to read and write the following values:
* D[23:22], RATE = ECG Data Rate Dependent on the F_MSTR
* D[21:18], NOT USED
* D[17:16], GAIN = ECG Channel Gain Setting
* D[15], NOT USED
* D[14], DHPF = ECG Channel Digital High-Pass Filter Cutoff Frequency
* D[13:12], DLPF = ECGP Channel Digital Low-Pass Filter Cutoff Frequency
* D[11:0], NOT USED
*/
void max30003_read_ecg_configuration_register(void)
{
    uint8_t ecg_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_ECG_ADDRESS, ecg_configuration_register_data_array);

    ecg_configuration_register.rate = (ecg_configuration_register_data_array[0] & 0b11000000) >> 6;
    ecg_configuration_register.gain = ecg_configuration_register_data_array[0] & 0b00000011;

    ecg_configuration_register.dhpf = (ecg_configuration_register_data_array[1] & 0b01000000) && 0b01000000;
    ecg_configuration_register.dlpf = (ecg_configuration_register_data_array[1] & 0b00110000) >> 4;
}

void max30003_write_ecg_configuration_register(void)
{
    uint8_t ecg_configuration_register_data_array[3];

    ecg_configuration_register_data_array[0] = ecg_configuration_register.rate << 6;
    ecg_configuration_register_data_array[0] = ecg_configuration_register.gain | ecg_configuration_register_data_array[0];
    ecg_configuration_register_data_array[1] = (ecg_configuration_register.dhpf << 6) | ecg_configuration_register_data_array[1];
    ecg_configuration_register_data_array[1] = (ecg_configuration_register.dlpf << 4) | ecg_configuration_register_data_array[1];
    
    spim_write_single_register(MAX30003_CNFG_ECG_ADDRESS, ecg_configuration_register_data_array[0], ecg_configuration_register_data_array[1], ecg_configuration_register_data_array[2]);
}

/* 
* Read and Write R-R #1 Configuration Register to read and write the following values:
* D[23:20], WNDW = This is the Width of the Averaging Window; Adjusts the Algorithm Sensitivity to the Width of the QRS Complex
* D[29:16], GAIN = R to R Gain; This is Used to Maximize the Dynamic Range of the Algorithm
* D[15], EN_RTOR = ECG RTOR Detection Enable
* D[14], NOT USED
* D[13:12], PACG = R to R Peak Averaging Weight Factor
* D[11:8], PTSF = R to R Peak Threshold Scaling Factor
* D[7:0], NOT USED
*/
void max30003_read_rtor1_configuration_register(void)
{
    uint8_t rtor1_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_RTOR1_ADDRESS, rtor1_configuration_register_data_array);

    rtor1_configuration_register.wndw = (rtor1_configuration_register_data_array[0] & 0b11110000) >> 4;
    rtor1_configuration_register.gain = rtor1_configuration_register_data_array[0] & 0b00001111;

    rtor1_configuration_register.en_rtor = (rtor1_configuration_register_data_array[1] & 0b10000000) && 0b10000000;
    rtor1_configuration_register.pavg = (rtor1_configuration_register_data_array[1] & 0b00110000) >> 4;
    rtor1_configuration_register.ptsf = rtor1_configuration_register_data_array[1] & 0b00001111;
}

void max30003_write_rtor1_configuration_register(void)
{
    uint8_t rtor1_configuration_register_data_array[3];

    rtor1_configuration_register_data_array[0] = rtor1_configuration_register.wndw << 4;
    rtor1_configuration_register_data_array[0] = rtor1_configuration_register.gain | rtor1_configuration_register_data_array[0];
    rtor1_configuration_register_data_array[1] = (rtor1_configuration_register.en_rtor << 7) | rtor1_configuration_register_data_array[1];
    rtor1_configuration_register_data_array[1] = (rtor1_configuration_register.pavg << 4) | rtor1_configuration_register_data_array[1];
    rtor1_configuration_register_data_array[1] = rtor1_configuration_register.ptsf | rtor1_configuration_register_data_array[1];
    
    spim_write_single_register(MAX30003_CNFG_RTOR1_ADDRESS, rtor1_configuration_register_data_array[0], rtor1_configuration_register_data_array[1], rtor1_configuration_register_data_array[2]);
}

/* 
* Read and Write R-R #2 Configuration Register to read and write the following values:
* D[23:22], NOT USED
* D[21:16], HOFF = R to R Minimum Hold Off; Sets the Absolute Minimum Interval Used for the Static Portion of the Hold Off Criteria
* D[15:14], NOT USED
* D[13:12], GAIN = R to R Interval Averaging Weight Factor; Weighting Factor for the Current R to R interval observation
* D[11], NOT USED
* D[10:8], RHSF = R to R Interval Hold Off Scaling Factor; Fraction of the R to R Average Interval Used for the Dynamic Portion of the Holdoff Criteria
* D[7:0], NOT USED
*/
void max30003_read_rtor2_configuration_register(void)
{
    uint8_t rtor2_configuration_register_data_array[3];
    spim_read_single_register(MAX30003_CNFG_RTOR2_ADDRESS, rtor2_configuration_register_data_array);

    rtor2_configuration_register.hoff = rtor2_configuration_register_data_array[0] & 0b00111111;

    rtor2_configuration_register.ravg = (rtor2_configuration_register_data_array[1] & 0b00110000) >> 6;
    rtor2_configuration_register.rhsf = rtor2_configuration_register_data_array[1] & 0b00000011;
}

void max30003_write_rtor2_configuration_register(void)
{
    uint8_t rtor2_configuration_register_data_array[3];

    rtor2_configuration_register_data_array[0] = rtor2_configuration_register.hoff;
    rtor2_configuration_register_data_array[1] = rtor2_configuration_register.ravg << 4;
    rtor2_configuration_register_data_array[1] = rtor2_configuration_register.rhsf | rtor2_configuration_register_data_array[1];
    
    spim_write_single_register(MAX30003_CNFG_RTOR2_ADDRESS, rtor2_configuration_register_data_array[0], rtor2_configuration_register_data_array[1], rtor2_configuration_register_data_array[2]);
}

/* 
* Read ECG FIFO Memory Register to read the following values:
* D[23:6], ECG_VOLTAGE = ECG Sample Voltage Data; 18 Bit ECG Voltage Information at the Requested Sample Rate in Left Justified Two's Complement Format
* D[5:3], ETAG = DATA Tag; Data condition Tag
* D[2:0], PTAG = ???
*/
void max30003_read_ecg_fifo_memory_register(void)
{
    uint8_t ecg_fifo_memory_register_data_array[interrupt_manager_register.efit*3];    // Number of words to read from the FIFO, 3 bytes per word
    spim_read_registers(MAX30003_ECG_FIFO_BURST_ADDRESS, ecg_fifo_memory_register_data_array, interrupt_manager_register.efit*3);
    uint32_t temp_ecg_voltage = 0;
    uint8_t counter = 0;
    for(uint8_t i = 0; i < ((interrupt_manager_register.efit*3)-2); i = i + 3)
    {
        temp_ecg_voltage = (ecg_fifo_memory_register_data_array[i] << 10) | (ecg_fifo_memory_register_data_array[i+1] << 2) | ((ecg_fifo_memory_register_data_array[i+2] & 0b11000000) >> 6); 
        ecg_fifo_memory_register.ecg_voltage[counter] = (uint16_t)(temp_ecg_voltage & 0b00000000000000001111111111111111);
        ecg_fifo_memory_register.etag[counter] = (ecg_fifo_memory_register_data_array[i+2] & 0b00111000) >> 3;
        ecg_fifo_memory_register.ptag[counter] = ecg_fifo_memory_register_data_array[i+2] & 0b00000111;
        counter++;
    }
}


void max30003_readback_registers(void) 
{
    uint8_t data_array[3];
    spim_read_single_register(MAX30003_EN_INT1_ADDRESS, data_array);
    spim_read_single_register(MAX30003_EN_INT2_ADDRESS, data_array);
    spim_read_single_register(MAX30003_MNGR_INT_ADDRESS, data_array);

    spim_read_single_register(MAX30003_CNFG_GEN_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_CAL_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_EMUX_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_ECG_ADDRESS, data_array);
    spim_read_single_register(MAX30003_CNFG_RTOR1_ADDRESS, data_array);
}

void max30003_soft_reset(void) 
{
    NRF_LOG_INFO("max_30003_soft_reset");
    spim_write_single_register(MAX30003_SW_RST_ADDRESS, 0x00, 0x00, 0x00);
    nrf_delay_ms(5); // Delay the MCU to allow the MAX30003 to go through a soft reset
}

void max30003_sync(void) 
{
    NRF_LOG_INFO("max30003_sync");
    spim_write_single_register(MAX30003_SYNCH_ADDRESS, 0x00, 0x00, 0x00);
}

void max30003_read_fifo_data(uint8_t* data_array) 
{
    NRF_LOG_INFO("max30003_read_fifo_data");
    uint8_t register_address = MAX30003_ECG_FIFO_BURST_ADDRESS;
    uint8_t register_data[52];

    spim_read_registers(register_address, register_data, sizeof(register_data)); 

    NRF_LOG_INFO("[MAX30003] FIFO READ DATA: \r\n");
    NRF_LOG_HEXDUMP_INFO(register_data, sizeof(register_data)); // Hex Dump Output (temp)
}

void max30003_init_interrupt(void)
{
    NRF_LOG_INFO("max30003_init_interrupt");
    nrf_gpio_pin_dir_set(MAX30003_INT1_PIN, NRF_GPIO_PIN_DIR_INPUT);
    nrfx_gpiote_in_config_t max30003_interrupt_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    ret_code_t error_code = nrfx_gpiote_in_init(MAX30003_INT1_PIN, &max30003_interrupt_config, max30003_interrupt_handler);
    
    NRF_LOG_INFO(" nrfx_gpiote_in_init: %d: \r\n", error_code);
    APP_ERROR_CHECK(error_code);
}

void max30003_enable_interrupt(void)
{
    NRF_LOG_INFO("max30003_enable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, true);
}

void max30003_disable_interrupt(void)
{
    NRF_LOG_INFO("max30003_disable_interrupt");
    nrfx_gpiote_in_event_enable(MAX30003_INT1_PIN, true);
}

void max30003_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("max30003_interrupt_handler");
    UNUSED_PARAMETER(pin);
    UNUSED_PARAMETER(action);

    ecg_interrupt_handler();    // Call to Overall ECG Manager to manage ECG workflow
}
